#include "clientpreviewwidget.h"
#include "ui_clientpreviewwidget.h"

ClientPreviewWidget::ClientPreviewWidget(QWidget *parent, QImage frame, QString hostname, std::shared_ptr<ClientConnection> ptr_to_client)
    : QWidget(parent), ui(new Ui::ClientPreviewWidget), frame(frame), client_hostname(hostname), big_view(nullptr) { // Initialize the member variable
    ui->setupUi(this);
    ui->client_hostname->setText(hostname);
    client_ptr = ptr_to_client;

    ui->frame_out->setPixmap(QPixmap::fromImage(frame, Qt::AutoColor).scaled(ui->frame_out->width(),ui->frame_out->height(),Qt::KeepAspectRatio));
}

void ClientPreviewWidget::update_preview_frame(QImage frame)
{
    ClientPreviewWidget::frame = frame;
    ui->frame_out->setPixmap(QPixmap::fromImage(frame, Qt::AutoColor).scaled(ui->frame_out->width(),ui->frame_out->height(),Qt::KeepAspectRatio));

    b_locked = client_ptr->isLocked;

    if(!b_locked){
        this->ui->btn_Lock->setText(QString("Lock"));
    }else{
        this->ui->btn_Lock->setText(QString("Unlock"));
    }
}

void ClientPreviewWidget::update_client_hostname(QString hostname)
{
    ClientPreviewWidget::client_hostname = hostname;
    ui->client_hostname->setText(hostname);
}

void ClientPreviewWidget::disconnect_client()
{
    client_ptr = nullptr;
}


ClientPreviewWidget::~ClientPreviewWidget()
{
    delete ui;
}

void ClientPreviewWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if(!big_view){
            big_view = new Dialog(nullptr, client_ptr->keyStrokes);
            big_view->show();
        }else{
            big_view->show();
        }
    }
}

void ClientPreviewWidget::on_btn_Lock_clicked()
{

    if(!b_locked){
        client_ptr->lockScreen();
    }else{
        client_ptr->unlockScreen();
    }

}

