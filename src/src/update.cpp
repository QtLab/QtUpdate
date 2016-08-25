#include "inc/update.h"

#include <QUrl>
#include <QThread>
#include <QDebug>

const QString defaultRepository("http://127.0.0.1/repository/");
const QString defaultVersionXML("version.xml");

Update::Update(QObject *parent)
    : QObject(parent)
{
    httpFile = new HttpFile(this);

    runtimer = new QTimer(this);
    connect(runtimer, SIGNAL(timeout()), this, SLOT(run()));
    runtimer->start(0);
}

Update::~Update(void)
{

}
void Update::fetchVersionXML(const QString urlVersionXML)
{

}

void Update::run(void)
{

}

void Update::check(void)
{
    qDebug() << QThread::currentThread() << "check";
    QString url("http://127.0.0.1/repository/EHdebuger_qt 1.3.exe");
    //QString url("https://ss0.bdstatic.com/5aV1bjqh_Q23odCf/static/superman/img/logo/bd_logo1_31bdc765.png");
    httpFile->downloadFile(url);
}

void Update::update(void)
{

}

void Update::cancel(void)
{

}
