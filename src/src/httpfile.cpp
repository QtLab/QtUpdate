#include "inc/httpfile.h"

#include <QUrl>
#include <QtNetwork>
#include <QDebug>

HttpFile::HttpFile(QObject *parent)
    : QObject(parent)
    , reply(Q_NULLPTR)
    , file(Q_NULLPTR)
    , httpRequestAborted(false)
{
    qnam = new QNetworkAccessManager(this);
    connect(qnam, &QNetworkAccessManager::authenticationRequired,
            this, &HttpFile::slotAuthenticationRequired);
#ifndef QT_NO_SSL
    connect(qnam, &QNetworkAccessManager::sslErrors,
            this, &HttpFile::sslErrors);
#endif
}

HttpFile::~HttpFile(void)
{

}

void HttpFile::startRequest(const QUrl &requestedUrl)
{
    qDebug() << QString("%1")
                .arg(__func__);

    url = requestedUrl;
    httpRequestAborted = false;

    reply = qnam->get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, &HttpFile::httpFinished);
    connect(reply, &QIODevice::readyRead, this, &HttpFile::httpReadyRead);
}

void HttpFile::downloadFile(const QString urlSpec, const QString directory)
{
    qDebug() << QString("%1")
                .arg(__func__);
    qDebug() << urlSpec;

    if (urlSpec.isEmpty())
        return;

    const QUrl newUrl = QUrl::fromUserInput(urlSpec);
    if (!newUrl.isValid()) {
        return;
    }

    QString fileName = newUrl.fileName();
    if (fileName.isEmpty()) {
        return;
    }

    QString downloadDirectory = QDir::cleanPath(directory);
    if (!downloadDirectory.isEmpty() && QFileInfo(downloadDirectory).isDir()) {
        fileName.prepend(downloadDirectory + '/');
    }

    if (QFile::exists(fileName)) {
#if 0
        if (QMessageBox::question(Q_NULLPTR, tr("Overwrite Existing File"),
                                  tr("There already exists a file called %1 in "
                                     "the current directory. Overwrite?").arg(fileName),
                                  QMessageBox::Yes|QMessageBox::No, QMessageBox::No)
            == QMessageBox::No) {
            return;
        }
#endif
        QFile::remove(fileName);
    }

    file = openFileForWrite(fileName);
    if (!file)
        return;

    // schedule the request
    startRequest(newUrl);
}

QFile *HttpFile::openFileForWrite(const QString &fileName)
{
    QScopedPointer<QFile> file(new QFile(fileName));
    if (!file->open(QIODevice::WriteOnly)) {
#if 0
        QMessageBox::information(this, tr("Error"),
                                 tr("Unable to save the file %1: %2.")
                                 .arg(QDir::toNativeSeparators(fileName),
                                      file->errorString()));
#endif
        return Q_NULLPTR;
    }
    return file.take();
}

void HttpFile::cancelDownload()
{
    httpRequestAborted = true;
    reply->abort();
}

void HttpFile::httpFinished()
{
    qDebug() << QString("%1")
                .arg(__func__);

    QFileInfo fi;
    if (file) {
        qDebug() << QString("%1 %2")
                    .arg(__func__)
                    .arg(__LINE__);

        fi.setFile(file->fileName());
        file->close();
        delete file;
        file = Q_NULLPTR;
    }

    if (httpRequestAborted) {
        qDebug() << QString("%1 %2")
                    .arg(__func__)
                    .arg(__LINE__);

        reply->deleteLater();
        reply = Q_NULLPTR;
        return;
    }

    if (reply->error()) {
        qDebug() << QString("%1 %2")
                    .arg(__func__)
                    .arg(__LINE__);

        QFile::remove(fi.absoluteFilePath());
        reply->deleteLater();
        reply = Q_NULLPTR;
        return;
    }

    const QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    reply->deleteLater();
    reply = Q_NULLPTR;

    if (!redirectionTarget.isNull()) {
        const QUrl redirectedUrl = url.resolved(redirectionTarget.toUrl());
#if 0
        if (QMessageBox::question(this, tr("Redirect"),
                                  tr("Redirect to %1 ?").arg(redirectedUrl.toString()),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
            downloadButton->setEnabled(true);
            return;
        }
#endif
        file = openFileForWrite(fi.absoluteFilePath());
        if (!file) {
            return;
        }
        startRequest(redirectedUrl);
        return;
    }

    qDebug() << QString("%1 %2")
                .arg(__func__)
                .arg("finished~");
#if 0
    statusLabel->setText(tr("Downloaded %1 bytes to %2\nin\n%3")
                         .arg(fi.size()).arg(fi.fileName(), QDir::toNativeSeparators(fi.absolutePath())));
#endif
}

void HttpFile::httpReadyRead()
{
    qDebug() << QString("%1")
                .arg(__func__);

    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file)
        file->write(reply->readAll());
}

void HttpFile::slotAuthenticationRequired(QNetworkReply*,QAuthenticator *authenticator)
{
    qDebug() << "authentication required";
#if 0
    QDialog authenticationDialog;
    Ui::Dialog ui;
    ui.setupUi(&authenticationDialog);
    authenticationDialog.adjustSize();
    ui.siteDescription->setText(tr("%1 at %2").arg(authenticator->realm(), url.host()));

    // Did the URL have information? Fill the UI
    // This is only relevant if the URL-supplied credentials were wrong
    ui.userEdit->setText(url.userName());
    ui.passwordEdit->setText(url.password());

    if (authenticationDialog.exec() == QDialog::Accepted) {
        authenticator->setUser(ui.userEdit->text());
        authenticator->setPassword(ui.passwordEdit->text());
    }
#endif
}

#ifndef QT_NO_SSL
void HttpFile::sslErrors(QNetworkReply*,const QList<QSslError> &errors)
{
    QString errorString;
    foreach (const QSslError &error, errors) {
        if (!errorString.isEmpty())
            errorString += '\n';
        errorString += error.errorString();
    }

#if 0
    if (QMessageBox::warning(this, tr("SSL Errors"),
                             tr("One or more SSL errors has occurred:\n%1").arg(errorString),
                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
        reply->ignoreSslErrors();
    }
#endif
}
#endif
