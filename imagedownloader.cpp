#include "imagedownloader.h"

#include <QFile>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDir>
#include <QTextStream>

ImageDownloader::ImageDownloader(const QStringList &urls, const QString &directory):
    m_urls(urls),
    m_directory(directory),
    m_manager(new QNetworkAccessManager(this)),
    m_streamLimit(50),
    m_currentStreams(0),
    m_progress(false),
    m_request_queue(QQueue<QNetworkRequest>()) {
    connect(m_manager, &QNetworkAccessManager::finished, this, &ImageDownloader::onDownloadFinished);
}

void ImageDownloader::startDownloading(quint16 streamsCount) {
    m_streamLimit = streamsCount;
    for (int i = 0; i < m_urls.size(); ++i) {
        QNetworkRequest request((QUrl(m_urls[i])));
        m_request_queue.enqueue(request);
    }
    sendNextRequest();
}

void ImageDownloader::setUrls(const QStringList &urls) {
    m_urls = urls;
}

void ImageDownloader::setDirectory(const QString &directory) {
    m_directory = directory;
}

void ImageDownloader::onDownloadFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray imageData = reply->readAll();
        QString fileName = reply->url().fileName(); // Получаем имя файла из URL
        QFile file(QDir(m_directory).filePath(fileName));


        QString uniqueName = file.fileName();
        int index = 1;

        // Получаем базовое имя и расширение файла
        QFileInfo fileInfo(file.fileName());
        QString nameWithoutExtension = fileInfo.completeBaseName();
        QString extension = fileInfo.suffix();
        while (QFile::exists(QDir(m_directory).filePath(uniqueName))) {
            // Если расширение есть, добавляем индекс перед ним
            if (!extension.isEmpty()) {
                uniqueName = QString("%1(%2).%3")
                .arg(nameWithoutExtension)
                    .arg(index++)
                    .arg(extension);
              } else {
                uniqueName = QString("%1(%2)").arg(file.fileName()).arg(index++);
            }
        }
        file.setFileName(QDir(m_directory).filePath(uniqueName));

        if (file.open(QIODevice::WriteOnly)) {
            file.write(imageData);
            file.close();
            emit newFileDownloaded();
        } else {
            qDebug() << "ERROR SAVING: " << file.errorString();
        }
    } else {
        qDebug() << reply->errorString();
    }
    reply->deleteLater(); // Удаляем reply после завершения
    m_currentStreams--;
    if (!m_request_queue.isEmpty()) {
        sendNextRequest();
    } else if (!m_currentStreams) {
        emit finished();
    }
}

void ImageDownloader::sendNextRequest() {
    if (m_request_queue.isEmpty()) {
        return;
    }
    while (m_currentStreams < m_streamLimit) {
        m_manager->get(m_request_queue.head());
        ++m_currentStreams;
        m_request_queue.dequeue();
    }
}
