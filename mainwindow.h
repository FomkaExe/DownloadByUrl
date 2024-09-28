#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ImageDownloader;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFileWithUrls();
    void openDirectory();
    void aboutQt();
    void startDownload();
    void newFileDownloaded();
    void timerUpdate();
    void finished();

private:
    ImageDownloader *m_downloader;
    Ui::MainWindow *m_ui;
    QTimer *m_timer;
    quint16 m_time;
};
#endif // MAINWINDOW_H
