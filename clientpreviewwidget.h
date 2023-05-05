#ifndef CLIENTPREVIEWWIDGET_H
#define CLIENTPREVIEWWIDGET_H

#include <QWidget>
#include <QMouseEvent>

#include "my_grid_layout.h"
#include "ClientConnection.h"
#include "dialog.h"


class ClientConnection;
class Dialog;


namespace Ui {
class ClientPreviewWidget;
}

class ClientPreviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClientPreviewWidget(QWidget *parent, QImage frame, QString hostname, std::shared_ptr<ClientConnection> ptr_to_client);

    void update_preview_frame(QImage frame);
    void update_client_hostname(QString hostname);
    void disconnect_client();

    ~ClientPreviewWidget();

protected:
    void mousePressEvent(QMouseEvent *event);

private slots:
    void on_btn_Lock_clicked();

private:
    Ui::ClientPreviewWidget *ui;
    QImage frame;
    QString client_hostname;
    std::shared_ptr<ClientConnection> client_ptr = nullptr;
    Dialog* big_view;
    my_grid_layout *preview_layout;
    bool b_selected;
    bool b_locked;

};

#endif // CLIENTPREVIEWWIDGET_H
