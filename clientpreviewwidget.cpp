#include "clientpreviewwidget.h"
#include "ui_clientpreviewwidget.h"

ClientPreviewWidget::ClientPreviewWidget(QWidget *parent, QImage frame, QString hostname, std::shared_ptr<ClientConnection> ptr_to_client) :
    QWidget(parent),
    ui(new Ui::ClientPreviewWidget), frame(frame), client_hostname(hostname)
{
    ui->setupUi(this);

    ui->frame_out->setPixmap(QPixmap::fromImage(frame, Qt::AutoColor).scaled(ui->frame_out->width(),ui->frame_out->height(),Qt::KeepAspectRatio));
    ui->client_hostname->setText(hostname);
    client_ptr = ptr_to_client;
}

void ClientPreviewWidget::update_preview_frame(QImage frame)
{
    ClientPreviewWidget::frame = frame;
    ui->frame_out->setPixmap(QPixmap::fromImage(frame, Qt::AutoColor).scaled(ui->frame_out->width(),ui->frame_out->height(),Qt::KeepAspectRatio));
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

void ClientPreviewWidget::ClientPreviewWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        if(!b_selected){
            b_selected = true;
            this->setStyleSheet("QLabel{Background-color: rgb(255,247,191); color: black;}");
        }else{
            b_selected = false;
            this->setStyleSheet("");
        }
    }
}

void ClientPreviewWidget::on_btn_Lock_clicked()
{
    if(!b_locked){
        client_ptr->lockScreen();
        this->ui->btn_Lock->setText(QString("Unlock"));
        b_locked = true;
    }else{
        client_ptr->unlockScreen();
        this->ui->btn_Lock->setText(QString("Lock"));
        b_locked = false;
    }

}

