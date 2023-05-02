#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    server = new Server(context, server_port);

    context.run();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update_ui()));
    timer->start(1000);

    preview_container = new QWidget(this);
    preview_layout = new my_grid_layout(this, 2);
    preview_container->setLayout(preview_layout);

    ui->scrollArea->setWidget(preview_container);

    ui->statusbar->showMessage(tr("No clients connected."));
}

void MainWindow::update_ui()
{
    for(auto &c : server->getClientConnections()){
        if(c != nullptr){
            // Convert the cv::Mat to a QImage.
            QImage stream = QImage(c->uncompressed_screenshot_data.data, c->uncompressed_screenshot_data.cols, c->uncompressed_screenshot_data.rows, c->uncompressed_screenshot_data.step, QImage::Format_RGB888).copy();

            if(c->preview_ui == nullptr){
                c->preview_ui = new ClientPreviewWidget(this, stream, QString::fromStdString(c->remote_endpoint_.address().to_string()));
                preview_layout->add_widget(c->preview_ui);
            }else{
                c->preview_ui->update_preview_frame(stream);
            }
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
}

