#include "ClientConnection.h"

#include "Server.h"

ClientConnection::ClientConnection(boost::asio::io_context& context, boost::asio::ssl::context& ssl_context, Server& server)
: server_(server), socket_(context, ssl_context) {
    keyStrokes = new std::vector<u_char>{};
    blacklistDictionary = new std::array<u_char, 1024>{};
}

ClientConnection::Pointer ClientConnection::create(boost::asio::io_context &context, boost::asio::ssl::context& ssl_context, Server& server) {
    return Pointer(new ClientConnection(context, ssl_context, server));
}

void ClientConnection::readFromSocket(const boost::asio::mutable_buffer& buffer) {
    try {
        boost::asio::read(socket_, buffer);
    } catch(std::exception& ex) {
        std::cerr << "ERROR: Read from Socket: " << ex.what() << std::endl;
        disconnect();
        throw;
    }
}

void ClientConnection::writeToSocket(const boost::asio::const_buffer& buffer) {
    try {
        boost::asio::write(socket_, buffer);
    } catch(std::exception& ex) {
        std::cerr << "ERROR: Write to Socket: " << ex.what() << std::endl;
        disconnect();
        throw;
    }
}

boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& ClientConnection::socket() {
  return socket_;
}

void ClientConnection::start() {
    requestScreenshot();
}

void ClientConnection::sendText(const std::string& text) {
    Header header;
    header.type = MessageType::TEXT;
    header.size = text.size();

    std::cout << "Sending Message" << std::endl;
    writeToSocket(boost::asio::buffer(&header, sizeof(header)));
    writeToSocket(boost::asio::buffer(text));
}

void ClientConnection::checkQueue() {
    std::lock_guard<std::mutex> lock(queue_mutex_);

    if (!msg_queue_.empty()) {
        for (auto& msg : msg_queue_) {
            header_ = std::get<0>(msg);
            text_buffer_ = std::get<1>(msg);

            std::cout << "SENDING MESSAGE " << "WITH SIZE " << header_.size << std::endl;
            writeToSocket(boost::asio::buffer(&header_, sizeof(header_)));

            if (header_.size == 0) {
                std::cout << "Skipping empty message" << std::endl;
                continue;
            }

            writeToSocket(boost::asio::buffer(&text_buffer_, sizeof(text_buffer_)));
        }
        msg_queue_.clear();
    }
}

void ClientConnection::lockScreen()
{
    Header header = {LOCK_SCREEN, 0, 0};
    header.meta_length = 0;
    std::array<char, 1024> dead_beef{};

    addToQueue(header, dead_beef);
}

void ClientConnection::unlockScreen()
{
    Header header = {UNLOCK_SCREEN, 0, 0};
    std::array<char, 1024> dead_beef{};

    addToQueue(header, dead_beef);
}

void ClientConnection::addToWhitelist(const std::string& word)
{
    const char* cword = word.c_str();
    auto cword_len = strlen(cword);

    Header header = {ADD_TO_WHITELIST, cword_len, 0};

    std::array<char, 1024> metadata{};
    std::copy(cword, cword + cword_len, metadata.data());

    addToQueue(header, metadata);
}

void ClientConnection::removeFromWhitelist(const std::string& word)
{
    const char* cword = word.c_str();
    auto cword_len = strlen(cword);

    Header header = {REMOVE_FROM_WHITELIST, cword_len, 0};

    std::array<char, 1024> metadata{};
    std::copy(cword, cword + cword_len, metadata.data());

    addToQueue(header, metadata);

}


void ClientConnection::readHeader() {
    std::cout << "WAIT FOR PACKET" << std::endl;
    readFromSocket(boost::asio::buffer(&header_, sizeof(Header)));
    std::cout << "PACKET RECEIVED: " << header_.type << std::endl;

    switch (header_.type) {
        case TEXT:
            readTextData();
            break;
        case SCREENSHOT:
            readScreenshotData();
            break;
        default:
            std::cerr << "Unknown data type received" << std::endl;
            disconnect();
            break;
    }
}

void ClientConnection::readTextData() {
    readFromSocket(boost::asio::buffer(&text_buffer_, header_.size));
    std::string received_text(text_buffer_.data(), header_.size);
    std::cout << "Text received from client: " << socket_.lowest_layer().remote_endpoint() << ": " << received_text << std::endl;
}

void ClientConnection::readScreenshotData() {
    const size_t chunk_size = 1024; // adjust this
    const auto screenshot_size = header_.size;

    screenshot_data_.resize(screenshot_size);
    size_t bytes_received = 0;

    std::cout << "SCREENSHOT SIZE: " << (float)(screenshot_size / 1000) / 1000 << "MB" << std::endl;

    while (bytes_received < screenshot_size) {
        size_t bytes_to_receive = std::min(chunk_size, screenshot_size - bytes_received);
        readFromSocket(boost::asio::buffer(&screenshot_data_[bytes_received], bytes_to_receive));
        bytes_received += bytes_to_receive;
    }

    //receive metadata
    MetaData metadata;
    readFromSocket(boost::asio::buffer(&metadata, sizeof(metadata)));

    isLocked = metadata.is_locked;

    for(auto& ch : metadata.keyData){
        if(ch == '\0')
            break;
        keyStrokes->emplace_back(ch);
    }

    std::copy(metadata.blacklistData.data(), metadata.blacklistData.data() + 1024, blacklistDictionary->data());

    // check png header if valid
    const std::array<short, 8> png_header = {137, 80, 78, 71, 13, 10, 26, 10};
    Ack ack;

    for(long unsigned int i = 0; i < png_header.size() - 1; i++){
        short byte = (short)screenshot_data_[i];

        if (byte != png_header[i]){
            ack.status = ERRORR;
            break;
        }else{
            ack.status = RECEIVED;
        }
    }

    // Send acknowledgement back to the client
    writeToSocket(boost::asio::buffer(&ack, sizeof(ack)));

    cv::cvtColor(cv::imdecode(screenshot_data_, cv::IMREAD_COLOR), uncompressed_screenshot_data, cv::COLOR_BGR2RGB);
}


void ClientConnection::requestScreenshot() {
    while(true) {
        sleep(1);
        checkQueue();

        header_.type = SCREENSHOT_REQ;
        header_.size = 0;
        header_.meta_length = 0;
        try{
            writeToSocket(boost::asio::buffer(&header_, sizeof(header_)));

            readHeader();
        }catch (...){
            break;
        }
    }
}

bool ClientConnection::isConnected()
{
    std::cout << "Checking" << std::endl;
    return socket_.lowest_layer().is_open();
}

void ClientConnection::stop()
{
    this->disconnect();
}

void ClientConnection::disconnect() {
    if(preview_ui)
        delete preview_ui;
    auto self(shared_from_this());
    socket_.lowest_layer().close();
    server_.on_session_stopped(self, remote_endpoint_);
}

void ClientConnection::setRemoteEndpoint(const boost::asio::ip::tcp::endpoint& endpoint) {
    remote_endpoint_ = endpoint;
}

std::string ClientConnection::getRemoteEndpointAsString()
{
    return remote_endpoint_.address().to_string();
}

void ClientConnection::handle_timeout(const boost::system::error_code &error)
{
    if(!error){
        std::cout << "Timeout Achieved" << std::endl;
        disconnect();
    }
}

void ClientConnection::addToQueue(const Header& header, const std::array<char, 1024>& message) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    msg_queue_.emplace_back(header, message);
}

ClientConnection::~ClientConnection(){
    delete keyStrokes;
    delete blacklistDictionary;
    std::cout << "DESTRUCTOR CALLED" << std::endl;
}
