#ifndef CLIENTPREVIEWWIDGET_H
#define CLIENTPREVIEWWIDGET_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class ClientPreviewWidget;
}

class ClientPreviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClientPreviewWidget(QWidget *parent = nullptr, QImage frame = QImage(), QString hostname = QString());
    void update_preview_frame(QImage frame);
    void update_client_hostname(QString hostname);
    ~ClientPreviewWidget();

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    Ui::ClientPreviewWidget *ui;
    QImage frame;
    QString client_hostname;
    bool b_selected;
};

#endif // CLIENTPREVIEWWIDGET_H
