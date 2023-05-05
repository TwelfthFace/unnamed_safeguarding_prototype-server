#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTimer>
#include "clientpreviewwidget.h"

class ClientPreviewWidget;

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr, std::vector<u_char>* client_logs = nullptr);
    ~Dialog();
public slots:
    void update_ui();
private:
    Ui::Dialog *ui;
    std::vector<u_char>* client_logs;
    QTimer* timer;
};

#endif // DIALOG_H
