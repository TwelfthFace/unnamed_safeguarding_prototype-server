#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include <iostream>
#include <thread>
#include "Server.h"

int main(int argc, char *argv[])
{


    QApplication a(argc, argv);

    const unsigned short server_port = 12345;

    boost::asio::io_context context;
    Server server(context, server_port);

    //std::thread context_thread([&](){context.run();});

    context.run();

//    auto io_context = std::make_shared<boost::asio::io_context>();
//    auto server = std::make_shared<Server>(*io_context, server_port);


//    std::cout << "Server running on port " << server_port << std::endl;


   //  Example: sending a text command to clients
//    std::thread command_thread([server]() {
//        while(true){
//            for (const auto& client_connection : server->getClientConnections()) {
//                std::string command = "HELLOOOO";
//                //std::getline(std::cin, command);
//                std::this_thread::sleep_for(std::chrono::seconds(5));
//                client_connection->sendText(command);
//            }
//        }
//    });

    // Run io_context on a detached thread
    //std::thread io_context_thread([&]() { io_context->run(); });

//    for (const auto& client_connection : server->getClientConnections()) {

//        client_connection->requestScreenshot();
//        //client_connection->checkQueue();

//        //std::this_thread::sleep_for(std::chrono::seconds(2));
//        Header header;
//        header.type = TEXT;
//        header.size = sizeof("abcd") + 1;
//        //client_connection->msg_queue_.emplace_back(header, std::array<char, 1024>{'a', 'b', 'c', 'd', 'e'});

//    }


//    while(true){

//        for (const auto& client_connection : server->getClientConnections()) {

//            client_connection->requestScreenshot();
//            //client_connection->checkQueue();

//            //std::this_thread::sleep_for(std::chrono::seconds(2));
//            Header header;
//            header.type = TEXT;
//            header.size = sizeof("abcd") + 1;
//            //client_connection->msg_queue_.emplace_back(header, std::array<char, 1024>{'a', 'b', 'c', 'd', 'e'});

//        }

//        std::this_thread::sleep_for(std::chrono::seconds(5));

//        //server->RemoveClosedConnections();
//    }

//    // Join the command_thread and wait for it to complete
//    command_thread.join();

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Dissertation_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

 //   MainWindow w(nullptr);
 //   w.show();

    return a.exec();
}
