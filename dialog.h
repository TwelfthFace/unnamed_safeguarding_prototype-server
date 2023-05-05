#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTimer>
#include "clientpreviewwidget.h"

class ClientPreviewWidget;
class ClientConnection;

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr, std::shared_ptr<ClientConnection> client_ptr = nullptr);
    ~Dialog();

public slots:
    void update_ui();
private slots:
    void on_btn_get_blacklist_clicked();

    void on_btn_add_to_blacklist_clicked();

    void on_btn_remove_from_blacklist_clicked();

private:
    void get_blacklist();

private:
    Ui::Dialog *ui;
    std::shared_ptr<ClientConnection> client_ptr;
    QTimer* timer;
};

#endif // DIALOG_H
