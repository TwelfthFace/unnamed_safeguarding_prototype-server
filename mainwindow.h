#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QWidget>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

#include "clientpreviewwidget.h"
#include "my_grid_layout.h"

#include "Server.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void update_ui();
private:
    Ui::MainWindow *ui;
    my_grid_layout* preview_layout;
    QWidget* preview_container;
    QTimer* timer;
    Server* server;

    const unsigned short server_port = 12345;
    boost::asio::io_context context;

};
#endif // MAINWINDOW_H
