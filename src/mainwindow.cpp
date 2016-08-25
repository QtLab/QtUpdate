#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->updatePushButton->hide();
    ui->updateProgressBar->setDisabled(true);

    // update thread
    updateThread = new QThread(this);
    updateThread->start();
    update = new Update;
    update->moveToThread(updateThread);

    // signal-slot
    connect(update, SIGNAL(updateStateSignal(QString)),
            this, SLOT(updateState(QString)));
    connect(update, SIGNAL(updateProgressSignal(QString,int,int)),
            this, SLOT(updateProgress(QString,int,int)));
    connect(ui->checkPushButton, SIGNAL(clicked(bool)),
            update, SLOT(check()));
    connect(ui->updatePushButton, SIGNAL(clicked(bool)),
            update, SLOT(update()));
    connect(ui->cancelPushButton, SIGNAL(clicked(bool)),
            update, SLOT(cancel()));
}

MainWindow::~MainWindow()
{
    updateThread->exit(0);
    delete ui;
}

void MainWindow::updateState(const QString state)
{

}

void MainWindow::updateProgress(const QString filename, const int val, const int max)
{

}
