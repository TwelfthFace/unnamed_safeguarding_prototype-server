#include "ClientConnection.h"

#include "Server.h"

ClientConnection::ClientConnection(boost::asio::io_context& context, Server& server) : server_(server), socket_(context) {}

ClientConnection::Pointer ClientConnection::create(boost::asio::io_context &context, Server& server) {
    return Pointer(new ClientConnection(context, server));
}

boost::asio::ip::tcp::socket& ClientConnection::socket() {
  return socket_;
}

void ClientConnection::start() {
    requestScreenshot();
}

void ClientConnection::sendText(const std::string& text) {
    Header header;
    header.type = MessageType::TEXT;
    header.size = text.size();

    try{
        std::cout << "Sending Message" << std::endl;
        boost::asio::write(socket_, boost::asio::buffer(&header, sizeof(header)));
        boost::asio::write(socket_, boost::asio::buffer(text));
    }catch(std::exception& ec){
        std::cerr << "Ungraceful disconnect: " << ec.what() << std::endl;
        disconnect();
    }

}

void ClientConnection::checkQueue()
{
    if(!msg_queue_.empty()){
        for(auto& msg : msg_queue_){
            header_ = std::get<0>(msg);
            text_buffer_ = std::get<1>(msg);

            std::cout << "SENDING MESSAGE" << "WITH SIZE " << header_.size << std::endl;
            boost::asio::write(socket_, boost::asio::buffer(&header_, sizeof(header_)));
            boost::asio::write(socket_, boost::asio::buffer(&text_buffer_, header_.size));

        }
        msg_queue_.clear();
    }
}

void ClientConnection::readHeader() {
    std::cout << "WAIT FOR PACKET" << std::endl;
    try {
        boost::asio::read(socket_, boost::asio::buffer(&header_, sizeof(Header)));
        std::cout << "PACKET RECIEVED: " << header_.type << std::endl;

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
    }catch(std::exception& ex){
        std::cerr << "ERROR: Read Header: " << ex.what() << std::endl;
        disconnect();
    }
}

void ClientConnection::readTextData() {
    try{
        boost::asio::read(socket_, boost::asio::buffer(&text_buffer_, header_.size));
        std::string received_text(text_buffer_.data(), header_.size);
        std::cout << "Text received from client: " << socket_.remote_endpoint() << ": " << received_text << std::endl;

    }catch(std::exception &ex){
        std::cerr << ex.what() << std::endl;
        disconnect();
    }
}

void ClientConnection::readScreenshotData() {
    try {
        const size_t chunk_size = 1024; // adjust this
        const auto screenshot_size = header_.size;

        screenshot_data_.resize(screenshot_size);
        size_t bytes_received = 0;

        std::cout << "SCREENSHOT SIZE: " << (float)(screenshot_size / 1000) / 1000 << "MB" << std::endl;

        while (bytes_received < screenshot_size) {
            size_t bytes_to_receive = std::min(chunk_size, screenshot_size - bytes_received);
            boost::asio::read(socket_, boost::asio::buffer(&screenshot_data_[bytes_received], bytes_to_receive));
            bytes_received += bytes_to_receive;
        }

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
        boost::asio::write(socket_, boost::asio::buffer(&ack, sizeof(ack)));

        cv::cvtColor(cv::imdecode(screenshot_data_, cv::IMREAD_COLOR), uncompressed_screenshot_data, cv::COLOR_BGR2RGB);

    }catch(std::exception &ex){
        std::cerr << "ERROR: Read Screenshot: " << ex.what() << std::endl;
        disconnect();
    }

}

void ClientConnection::requestScreenshot(){
    while(true){
        //sleep(5);
        checkQueue();

        header_.type = SCREENSHOT_REQ;
        header_.size = 0;

        try {
            boost::asio::write(socket_, boost::asio::buffer(&header_, sizeof(header_)));
        }catch(std::exception& ex){
            std::cerr << "ERROR: Request Screenshot: " << ex.what() << std::endl;
            //disconnect();
            break;
        }

        readHeader();
    }
}

bool ClientConnection::isConnected()
{
    std::cout << "Checking" << std::endl;
    return socket_.is_open();
}

void ClientConnection::stop()
{
    this->disconnect();
}

void ClientConnection::disconnect() {
    auto self(shared_from_this());
    socket_.close();
    server_.on_session_stopped(self, remote_endpoint_);
}

void ClientConnection::setRemoteEndpoint(const boost::asio::ip::tcp::endpoint& endpoint) {
    remote_endpoint_ = endpoint;
}

void ClientConnection::handle_timeout(const boost::system::error_code &error)
{
    if(!error){
        std::cout << "Timeout Achieved" << std::endl;
        disconnect();
    }
}

ClientConnection::~ClientConnection(){
    std::cout << "DESTRUCTOR CALLED" << std::endl;
}
