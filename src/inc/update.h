#ifndef UPDATE_H
#define UPDATE_H

#include "inc/httpfile.h"

#include <QTimer>
#include <QObject>

class Update : public QObject
{
    Q_OBJECT
public:
    explicit Update(QObject *parent = 0);
    ~Update(void);

private:
    void fetchVersionXML(const QString urlVersionXML);

signals:
    void updateStateSignal(const QString state);
    void updateProgressSignal(const QString filename, const int val, const int max);

public slots:
    void check(void);
    void update(void);
    void cancel(void);
    void run(void);

private:
    QTimer * runtimer;
    QString state;
    HttpFile * httpFile;
};

#endif // UPDATE_H
