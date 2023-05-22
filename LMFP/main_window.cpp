#include "main_window.h"
#include "ui_main_window.h"
#include <QLabel>
#include <QRadioButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , _ui(new Ui::MainWindow) {
    _ui->setupUi(this);
    _movie = new QMovie(":/imageResource/img/lain.gif");
    _downloader = new DownloadWorker;
    _downloadThread = new QThread(this);
    QString styleSheet = "background-color: #404040;";
    this->setStyleSheet(styleSheet);
    _ui->parsingAnimation->setMovie(_movie);
    _ui->parsingAnimation->show();
    connect(_ui->btnStart, &QPushButton::clicked, this, &MainWindow::btnStart_clicked);
    connect(_ui->btnStop, &QPushButton::clicked, this, &MainWindow::btnStop_clicked);

    QGroupBox *groupBox = new QGroupBox;
    QRadioButton *radioButton1 = new QRadioButton(tr("Images"));
    QRadioButton *radioButton2 = new QRadioButton(tr("Audio"));
    QRadioButton *radioButton3 = new QRadioButton(tr("Videos"));
    QRadioButton *radioButton4 = new QRadioButton(tr("Docs"));
    QVBoxLayout *vbox = new QVBoxLayout;

    vbox->addWidget(radioButton1);
    vbox->addWidget(radioButton2);
    vbox->addWidget(radioButton3);
    vbox->addWidget(radioButton4);
    groupBox->setLayout(vbox);
    _ui->switchesGroup->layout()->addWidget(groupBox);

    
    QPixmap pixSave(":/imageResource/img/btnSave.png");
    int width = _ui->btnSave->width();
    int height = _ui->btnSave->height();
    _ui->btnSave->setPixmap(pixSave.scaled(width, height, Qt::KeepAspectRatio));

    QPixmap pixSettings(":/imageResource/img/btnSettings.png");
    width = _ui->btnSettings->width();
    height = _ui->btnSettings->height();
    _ui->btnSettings->setPixmap(pixSettings.scaled(width, height, Qt::KeepAspectRatio));


}

MainWindow::~MainWindow() {
    delete _ui;
}


void MainWindow::btnStart_clicked() { 
    _downloader->setUrl(_ui->plainTextEdit->toPlainText());
    _downloader->setAudioList(_ui->audioList);
    _movie->start();
    _downloader->moveToThread(_downloadThread);
    connect(_downloadThread, &QThread::started, _downloader, &DownloadWorker::startDownload);
    connect(_downloader, &DownloadWorker::finished, _downloadThread, &QThread::quit);
    connect(_downloader, &DownloadWorker::finished, _downloader, &DownloadWorker::deleteLater);

    _downloadThread->start();
}

void MainWindow::btnStop_clicked() {
    _movie->stop();
}

void MainWindow::btnSettings_clicked() {

}

void MainWindow::btnSave_clicked() {

}