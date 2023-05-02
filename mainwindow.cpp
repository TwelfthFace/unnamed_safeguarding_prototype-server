#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update_ui()));
    timer->start(1000);

    preview_container = new QWidget(this);
    preview_layout = new my_grid_layout(this, 2);
    preview_container->setLayout(preview_layout);

    ui->scrollArea->setWidget(preview_container);

    ui->statusbar->showMessage(tr("No clients connected."));
}

void MainWindow::update_ui()
{

}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
}

