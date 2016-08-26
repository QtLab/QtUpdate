#ifndef UPDATE_H
#define UPDATE_H

#include "inc/httpfile.h"

#include <QTimer>
#include <QObject>
#include <QDomDocument>

class Update : public QObject
{
    Q_OBJECT
public:
    explicit Update(QObject *parent = 0);
    ~Update(void);

private:
    void fetchVersionXML(const QString urlVersionXML);
    // analyse xml
    QDomDocument * openXML(const QString xml);
    QString getPublishTime(const QDomDocument * doc);
    QString getVersion(const QDomDocument * doc);
    QString getVersionDescription(const QDomDocument * doc);
    QString currentVersion(void);
    QDomElement getFileList(const QDomDocument *doc);
    QDomElement findFileInList(const QString filename, const QDomElement fileList);
    int versionCmp(const QString version1, const QString version2, bool * ok);
    void findAddedFile(const QDomDocument * docNew, const QDomDocument * docOld);
    void findUpdatedFile(const QDomDocument * docNew, const QDomDocument * docOld);
    void findRemovedFile(const QDomDocument * docNew, const QDomDocument * docOld);
    void addFile(void);
    void coverFile(void);
    void removeFile(void);

signals:
    void newVersionSignal(const QString version, const QString timer, const QString descrip);
    void updateStateSignal(const QString state);
    void updateProgressSignal(const QString filename, const int val, const int max);

public slots:
    void check(void);
    void update(void);
    void cancel(void);
    void run(void);
    void httpFileFinished(const QString filename, bool isSuccess);

private:
    enum Status_M {
        IDLE,
        CHECK_VERSION,
        ANALYSE_XML,
        DOWNLOAD_FILE,
        UPDATE_FILE,
        UPDATE_XML,
    };

private:
    QTimer * runtimer;
    bool isBusy;
    Status_M status;
    bool error;
    HttpFile * httpFile;

    QString currentDir;
    QString downloadDir;
    QString remoteXML;
    QString localXML;

    QDomDocument * remoteDoc;
    QDomDocument * localDoc;

    QList<QString> * addedFileList;
    QList<QString> * updatedFileList;
    QList<QString> * removedFileList;
};

#endif // UPDATE_H
