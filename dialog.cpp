#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent, std::shared_ptr<ClientConnection> client_ptr) :
    QDialog(parent),
    ui(new Ui::Dialog), client_ptr(client_ptr)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update_ui()));
    timer->start(1000);

    update_ui();
}

void Dialog::update_ui()
{
    QString text;
    for (const auto &ch : *client_ptr->keyStrokes) {
        if(ch == '\0')
            break;
        text.append(static_cast<char>(ch));
    }

    ui->client_logs->setHtml(text);
}

void Dialog::get_blacklist(){
    QString blacklistedWord;
    ui->cmb_blacklisted_words->clear();

    for (auto& ch : *client_ptr->blacklistDictionary){
        if(ch == '\0')
            break;

        if(ch == ' '){
            ui->cmb_blacklisted_words->addItem(blacklistedWord);
            blacklistedWord.clear();
        }else{
            blacklistedWord.append(ch);
        }
    }
}

void Dialog::on_btn_get_blacklist_clicked()
{
    get_blacklist();
}

void Dialog::on_btn_add_to_blacklist_clicked()
{
    auto word = ui->txt_add_blacklisted->text().toUpper().trimmed().replace(" ", "");
    if(!word.isEmpty())
        client_ptr->addToWhitelist(word.toStdString());
    get_blacklist();
}

void Dialog::on_btn_remove_from_blacklist_clicked()
{
    auto selection = ui->cmb_blacklisted_words->currentText();
    if(!selection.isEmpty())
        client_ptr->removeFromWhitelist(selection.toStdString());
    get_blacklist();
}

Dialog::~Dialog()
{
    client_ptr = nullptr;
    delete ui;
}
