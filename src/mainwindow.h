#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "inc/update.h"

#include <QMainWindow>
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow * getUi(void) const { return ui; };

signals:

public slots:
    void updateState(const QString state);
    void updateProgress(const QString filename, const int val, const int max);

private slots:
    void on_cancelPushButton_clicked();

private:
    Ui::MainWindow *ui;
    QThread * updateThread;
    Update * update;
};

#endif // MAINWINDOW_H
