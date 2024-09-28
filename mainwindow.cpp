#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imagedownloader.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    m_downloader(new ImageDownloader()),
    m_ui(new Ui::MainWindow),
    m_time(0) {
    m_ui->setupUi(this);

    setWindowTitle("File Downloader");

    connect(m_ui->button_open_file, &QPushButton::clicked,
            this, &MainWindow::openFileWithUrls);
    connect(m_ui->button_open_directory, &QPushButton::clicked,
            this, &MainWindow::openDirectory);
    connect(m_ui->button_start_download, &QPushButton::clicked,
            this, &MainWindow::startDownload);
    connect(m_ui->action_setFile, &QAction::triggered,
            this, &MainWindow::openFileWithUrls);
    connect(m_ui->action_setDir, &QAction::triggered,
            this, &MainWindow::openDirectory);
    connect(m_ui->action_aboutQt, &QAction::triggered,
            this, &MainWindow::aboutQt);
    connect(m_downloader, &ImageDownloader::newFileDownloaded, this,
            &MainWindow::newFileDownloaded);
    connect(m_downloader, &ImageDownloader::finished,
            this, &MainWindow::finished);


    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout,
            this, &MainWindow::timerUpdate);
    m_ui->button_start_download->setDisabled(true);
    m_ui->action_start_download->setEnabled(false);
}

MainWindow::~MainWindow() {
    delete m_ui;
    m_ui = nullptr;
}

void MainWindow::openFileWithUrls() {
    QString fileName = QFileDialog::getOpenFileName(nullptr,
                                                    "Выберите файл",
                                                    QString(),
                                                    "Text Files (*.txt)");
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл: " << fileName;
        return;
    }

    QStringList urls;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            urls << line;
        }
    }

    file.close();
    m_downloader->setUrls(urls);
    m_ui->label_file_path->setText(fileName);
    m_ui->label_downloaded->setText("0");
    m_ui->label_total->setText(QString::number(urls.size()));
    m_ui->progressBar->setMaximum(urls.size());

    if (!m_ui->label_directory_path->text().isEmpty()) {
        m_ui->button_start_download->setDisabled(false);
        m_ui->action_start_download->setEnabled(true);
    }
}

void MainWindow::openDirectory() {
    QString directory = QFileDialog::getExistingDirectory(nullptr,
                                                          "Выберите директорию",
                                                          QString());
    m_downloader->setDirectory(directory);
    m_ui->label_directory_path->setText(directory);
    if (!m_ui->label_file_path->text().isEmpty()) {
        m_ui->button_start_download->setDisabled(false);
        m_ui->action_start_download->setEnabled(true);
    }
}

void MainWindow::aboutQt() {
    QMessageBox::aboutQt(this);
}

void MainWindow::startDownload() {
    m_ui->button_open_directory->setDisabled(true);
    m_ui->button_open_file->setDisabled(true);
    m_ui->action_setFile->setEnabled(false);
    m_ui->action_setDir->setEnabled(false);
    m_ui->action_aboutQt->setEnabled(false);
    m_ui->label_downloaded->setText("0");
    m_ui->label_time->setText("0 seconds");
    m_time = 0;
    m_downloader->startDownloading(m_ui->spinbox_streams->value());
    m_timer->start();
    m_ui->button_start_download->setDisabled(true);
    m_ui->action_start_download->setEnabled(false);
}

void MainWindow::newFileDownloaded() {
    int number = m_ui->label_downloaded->text().toInt();
    m_ui->label_downloaded->setText(QString::number(number + 1));
    m_ui->progressBar->setValue(number + 1);
}

void MainWindow::timerUpdate() {
    ++m_time;
    m_ui->label_time->setText(QString("%1 seconds").arg(m_time));
}

void MainWindow::finished() {
    m_timer->stop();
    QMessageBox::information(this,
                             "Загрузка завершена",
                             "Загружено " + m_ui->label_total->text() + " файлов");
    m_ui->button_open_directory->setDisabled(false);
    m_ui->button_open_file->setDisabled(false);
    m_ui->button_start_download->setDisabled(false);
    m_ui->action_setDir->setEnabled(true);
    m_ui->action_setFile->setEnabled(true);
    m_ui->action_start_download->setEnabled(true);
    m_ui->action_aboutQt->setEnabled(true);
}
