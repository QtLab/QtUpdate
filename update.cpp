#include "update.h"

Update::Update(QObject *parent) : QObject(parent)
{
    runtimer = new QTimer(this);
    connect(runtimer, SIGNAL(timeout()), this, SLOT(run()));
    runtimer->start(0);
}

void Update::run(void)
{

}

void Update::check(void)
{

}

void Update::update(void)
{

}

void Update::cancel(void)
{

}
