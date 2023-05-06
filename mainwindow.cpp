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
    for (auto &c : server->getClientConnections()) {
        QImage stream = QImage(c->uncompressed_screenshot_data.data, c->uncompressed_screenshot_data.cols, c->uncompressed_screenshot_data.rows, c->uncompressed_screenshot_data.step, QImage::Format_RGB888).copy();

        if (c->preview_ui == nullptr) {
            c->preview_ui = new ClientPreviewWidget(this, stream, QString::fromStdString(c->remote_endpoint_.address().to_string()), c);
            preview_layout->add_widget(c->preview_ui);
        } else {
            c->preview_ui->update_preview_frame(stream);
        }
    }

    char connected_clients[16];
    snprintf(connected_clients, sizeof(connected_clients), "%zu", server->getClientConnections().size());

    QString client_connected_message("Clients Connected: ");
    client_connected_message.append(connected_clients);

    if(server->getClientConnections().size() > 0){
        ui->statusbar->showMessage(client_connected_message);
    }else{
        ui->statusbar->showMessage(tr("No clients connected."));
    }

}


void MainWindow::on_btn_lock_all_clicked()
{
    for (auto &c : server->getClientConnections()) {
        c->lockScreen();
    }
}

void MainWindow::on_btn_unlock_all_clicked()
{
    for (auto &c : server->getClientConnections()) {
        c->unlockScreen();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
}
