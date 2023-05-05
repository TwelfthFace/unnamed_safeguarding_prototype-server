#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent, std::vector<u_char>* client_logs) :
    QDialog(parent),
    ui(new Ui::Dialog), client_logs(client_logs)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update_ui()));
    timer->start(1000);

    update_ui();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::update_ui()
{
    QString text;
    for (const auto &ch : *client_logs) {
        if(ch == '\0')
            break;
        text.append(static_cast<char>(ch));
    }

    ui->client_logs->setHtml(text);
}
