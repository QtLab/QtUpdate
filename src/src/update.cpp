#include "inc/update.h"

#include <QUrl>
#include <QThread>
#include <QDebug>
#include <QDir>

const QString defaultRepository("http://127.0.0.1/repository/");
const QString defaultVersionXML("version.xml");

Update::Update(QObject *parent)
    : QObject(parent)
{
    isBusy = false;
    status = Update::IDLE;
    remoteDoc = Q_NULLPTR;
    localDoc = Q_NULLPTR;

    currentDir = QDir::currentPath();
    qDebug() << "currentDir" << currentDir;
    downloadDir = currentDir + "/download";
    QDir dir(downloadDir);
    if (!dir.exists()) {
        dir.mkdir(downloadDir);
    }
    qDebug() << "downloadDir" << downloadDir;

    httpFile = new HttpFile(this);
    connect(httpFile, &HttpFile::httpFinishedSignal, this, &Update::httpFileFinished);

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

QDomDocument * Update::openXML(const QString xml)
{
    qDebug() << __func__ << __LINE__;
    QDomDocument * doc = new QDomDocument("version");

    QFile file(xml);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << __func__ << __LINE__;
        return Q_NULLPTR;
    }
    if (!doc->setContent(&file)) {
        file.close();
        qDebug() << __func__ << __LINE__;
        return Q_NULLPTR;
    }
    file.close();
    qDebug() << __func__ << __LINE__;
    return doc;
}

QString Update::getPublishTime(const QDomDocument *doc)
{
    QDomElement docElem = doc->documentElement();

    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            if (QString("time") == e.tagName()) {
                return e.text();
            }
        }
        n = n.nextSibling();
    }
    return QString("");
}

QString Update::getVersion(const QDomDocument * doc)
{
    QDomElement docElem = doc->documentElement();

    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            if (QString("version") == e.tagName()) {
                return e.text();
            }
        }
        n = n.nextSibling();
    }
    return QString("");
#if 0
    qDebug() << __func__ << __LINE__ << xml;
    QDomDocument doc("mydocument");
    QFile file(xml);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << __func__ << __LINE__;
        return QString("");
    }
    if (!doc.setContent(&file)) {
        file.close();
        qDebug() << __func__ << __LINE__;
        return QString("");
    }
    file.close();

    // print out the element names of all elements that are direct children
    // of the outermost element.
    QDomElement docElem = doc.documentElement();

    qDebug() << docElem.tagName();

    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            qDebug() << e.tagName() << e.text(); // the node really is an element.
        }
        n = n.nextSibling();
    }
#endif
}

QString Update::getVersionDescription(const QDomDocument *doc)
{
    QDomElement docElem = doc->documentElement();

    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            if (QString("description") == e.tagName()) {
                return e.text();
            }
        }
        n = n.nextSibling();
    }
    return QString("");
}

QString Update::currentVersion(void)
{
    if (Q_NULLPTR == localDoc) {
        return QString("");
    }
    return (getVersion(localDoc) + getPublishTime(localDoc));
}

QDomElement Update::getFileList(const QDomDocument *doc)
{
    QDomElement docElem = doc->documentElement();

    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            if (QString("filelist") == e.tagName()) {
                return e;
            }
        }
        n = n.nextSibling();
    }
    return QDomElement();
}

QDomElement Update::findFileInList(const QString filename, const QDomElement fileList)
{
    QDomNode n = fileList.firstChild();
    while (!n.isNull())  {
        QDomElement e = n.toElement();
        if (!e.isNull()) {
            if (filename == e.attribute("filename")) {
                return e;
            }
        }
        n = n.nextSibling();
    }
    return QDomElement();
}

int Update::versionCmp(const QString version1, const QString version2, bool * ok)
{
    QStringList versionList1 = version1.split('.', QString::SkipEmptyParts);
    QStringList versionList2 = version2.split('.', QString::SkipEmptyParts);

    int size1 = versionList1.size();
    int size2 = versionList2.size();
    int size = (size1 < size2) ? size1 : size2;
    bool isOk;
    int v1, v2;
    for (int i = 0; i < size; ++i) {
        v1 = QString(versionList1.at(i)).toInt(&isOk);
        if (!isOk) {
            *ok = false;
            return 0;
        }
        v2 = QString(versionList2.at(i)).toInt(&isOk);
        if (!isOk) {
            *ok = false;
            return 0;
        }
        if (v1 > v2) {
            *ok = true;
            return 1;
        }
    }
    *ok = true;
    if (v1 == v2) {
        if (size1 > size2) {
            return 1;
        } else if (size1 == size2){
            return 0;
        }
    }
    return -1;
}

void Update::findAddedFile(const QDomDocument *docNew, const QDomDocument *docOld)
{
    QDomElement fileListNew = getFileList(docNew);
    QDomElement fileListOld = getFileList(docNew);

    if (QString("") == fileListNew.tagName()
        ||QString("") == fileListOld.tagName()) {
        return;
    }

    QDomNode n = fileListNew.firstChild();
    while (!n.isNull()) {
        QDomElement fileNew = n.toElement();
        if (!fileNew.isNull()) {
            QDomElement fileOld = findFileInList(fileNew.attribute("filename"), fileListOld);
            if (fileOld.isNull()) {

            } else {

            }
        }
        n = n.nextSibling();
    }
}

void Update::findUpdatedFile(const QDomDocument *docNew, const QDomDocument *docOld)
{

}

void Update::findRemovedFile(const QDomDocument *docNew, const QDomDocument *docOld)
{

}

void Update::addFile(void)
{

}

void Update::coverFile(void)
{

}

void Update::removeFile(void)
{

}

void Update::run(void)
{
    if (IDLE == status) {

    } else if (CHECK_VERSION == status) {
        if (!isBusy) {
            qDebug() << __func__ << __LINE__;
            remoteXML = QDir::cleanPath(downloadDir + "/" + defaultVersionXML);
            qDebug() << remoteXML;
            localXML = QDir::cleanPath(currentDir + "/" + defaultVersionXML);
            qDebug() << localXML;
            remoteDoc = openXML(remoteXML);
            localDoc = openXML(localXML);
            if (Q_NULLPTR == remoteDoc || Q_NULLPTR == localDoc) {
                status = IDLE;
                return;
            }
            qDebug() << getVersion(remoteDoc);
            qDebug() << getVersion(localDoc);
            status = IDLE;
        }
    }
}

void Update::httpFileFinished(const QString filename, bool isSuccess) {
    if (!isSuccess) {

        qDebug() << filename << "Failed";
        return;
    }
    qDebug() << filename << "Success";
    isBusy = false;
}

void Update::check(void)
{
    qDebug() << QThread::currentThread() << "check";
    QString url(defaultRepository + defaultVersionXML);
    httpFile->downloadFile(url, downloadDir);
    status = CHECK_VERSION;
    isBusy = true;
}

void Update::update(void)
{

}

void Update::cancel(void)
{

}
