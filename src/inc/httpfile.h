#ifndef HTTPFILE_H
#define HTTPFILE_H

#include <QUrl>
#include <QNetworkAccessManager>

QT_BEGIN_NAMESPACE
class QFile;
class QLabel;
class QLineEdit;
class QPushButton;
class QSslError;
class QAuthenticator;
class QNetworkReply;
class QCheckBox;

QT_END_NAMESPACE

//#define QT_NO_SSL

class HttpFile : public QObject
{
    Q_OBJECT

public:
    explicit HttpFile(QObject *paretn = Q_NULLPTR);

    void startRequest(const QUrl &requestedUrl);
    void downloadFile(const QString urlSpec, const QString directory = QString("./"));

private slots:
    void cancelDownload();
    void httpFinished();
    void httpReadyRead();
    void slotAuthenticationRequired(QNetworkReply*,QAuthenticator *);
#ifndef QT_NO_SSL
    void sslErrors(QNetworkReply*,const QList<QSslError> &errors);
#endif

private:
    QFile *openFileForWrite(const QString &fileName);

    QUrl url;
    QNetworkAccessManager *qnam;
    QNetworkReply *reply;
    QFile *file;
    bool httpRequestAborted;
};

#endif
