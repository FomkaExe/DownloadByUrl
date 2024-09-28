#ifndef IMAGEDOWNLOADER_H
#define IMAGEDOWNLOADER_H

#include <QObject>
#include <QNetworkRequest>
#include <QQueue>

class QNetworkAccessManager;
class QNetworkReply;

class ImageDownloader : public QObject
{
    Q_OBJECT
public:
    ImageDownloader(const QStringList &urls = QStringList(), const QString &directory = QString());

    void startDownloading(quint16 streamsCount);
    void setUrls(const QStringList &urls);
    void setDirectory(const QString &directory);

private slots:
    void onDownloadFinished(QNetworkReply *reply);
signals:
    void newFileDownloaded();
    void finished();
private:
    void sendNextRequest();

private:
    QStringList m_urls;
    QString m_directory;
    QNetworkAccessManager *m_manager;
    quint8 m_streamLimit;
    quint8 m_currentStreams;
    bool m_progress;
    QQueue<QNetworkRequest> m_request_queue;
};

#endif // IMAGEDOWNLOADER_H
