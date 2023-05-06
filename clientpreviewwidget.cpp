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

    QString stylesheet("background-color: red");

    b_locked = client_ptr->isLocked;

    if(!b_locked){
        this->ui->btn_Lock->setText(QString("Lock"));
        this->setStyleSheet(QString(""));
    }else{
        this->ui->btn_Lock->setText(QString("Unlock"));
        this->setStyleSheet(stylesheet);
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
    disconnect_client();
    delete ui;
}

void ClientPreviewWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if(!big_view){
            big_view = new Dialog(nullptr, client_ptr);
            big_view->setAttribute(Qt::WA_DeleteOnClose);
            big_view->setWindowTitle(QString::fromStdString(client_ptr->getRemoteEndpointAsString()));
            // Set the Qt::WA_DeleteOnClose attribute
            big_view->setAttribute(Qt::WA_DeleteOnClose);

            // Connect the destroyed signal to set the m_dialog pointer to nullptr
            connect(big_view, &QDialog::destroyed, [this]() {
                big_view = nullptr;
            });

            // Configure and show the QDialog
            big_view->show();
        } else {
            // If the dialog is already open, raise and activate it
            big_view->raise();
            big_view->activateWindow();
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

