#include "main_window.h"
#include "ui_main_window.h"
#include "checker.h"
#include "settings.h"
#include <QLabel>
#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , _ui(new Ui::MainWindow) {
    _ui->setupUi(this);
    setWindowIcon(QIcon(":/imageResource/img/maxresdefault.ico"));
    setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    setWindowFlags(windowFlags() & ~(Qt::WindowFullscreenButtonHint | Qt::WindowMaximizeButtonHint));
    _pixExpected = new QPixmap(":/imageResource/img/parsingExpected.jpg");
    _pixCompleted = new QPixmap(":/imageResource/img/parsingCompleted.jpg");
    _ui->parsingAnimation->setPixmap(*_pixExpected);
    _movie = new QMovie(":/imageResource/img/parsingInProcess.gif");

    _downloadThread = new QThread(this);
    connect(this, &MainWindow::destroyed, _downloadThread, &QThread::quit);
    _downloader = new DownloadWorker;
    connect(this, &MainWindow::startDownload, _downloader, &DownloadWorker::downloadMedia);
    _downloader->moveToThread(_downloadThread);
    _downloadThread->start();
    connect(_downloader, &DownloadWorker::downloadFinished, this, &MainWindow::readyToStart);

    connect(_ui->btnStart, &QPushButton::clicked, this, &MainWindow::btnStart_clicked);
    connect(_ui->btnStop, &QPushButton::clicked, this, &MainWindow::btnStop_clicked);
    connect(_ui->btnSave, &QPushButton::clicked, this, &MainWindow::btnSave_clicked);
    connect(_ui->btnSettings, &QPushButton::clicked, this, &MainWindow::btnSettings_clicked);
    QGroupBox *groupBox = new QGroupBox(this);
    _imagesCBox = new QCheckBox(tr("Images"));
    _audioCBox = new QCheckBox(tr("Audio"));
    _videosCBox = new QCheckBox(tr("Videos"));
    _docsCBox = new QCheckBox(tr("Docs"));
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(_imagesCBox);
    vbox->addWidget(_audioCBox);
    vbox->addWidget(_videosCBox);
    vbox->addWidget(_docsCBox);
    groupBox->setLayout(vbox);
    _ui->switchesGroup->layout()->addWidget(groupBox);

    QPixmap pixSave(":/imageResource/img/btnSave.png");
    _ui->btnSave->setIcon(pixSave);
    _ui->btnSave->setStyleSheet("QPushButton { background-color: transparent; border: none; }");
    _ui->btnSave->setIconSize(_ui->btnSave->size());

    QPixmap pixSettings(":/imageResource/img/btnSettings.png");
    _ui->btnSettings->setIcon(pixSettings);
    _ui->btnSettings->setStyleSheet("QPushButton { background-color: transparent; border: none; }");
    _ui->btnSettings->setIconSize(_ui->btnSettings->size());
    setWindowState(windowState() & ~Qt::WindowFullScreen);
    setWindowFlags(windowFlags() & ~Qt::WindowFullscreenButtonHint);
}

MainWindow::~MainWindow() {
    delete _ui;
    delete _downloader;
}

void MainWindow::btnStart_clicked() {
    _downloader->_parsingIsGood = false;
    _ui->audioList->clear();

    QString url = _ui->inputUrl->text();
    bool validUrl = true;
    validUrl = checkUrl(url);

    if (!url.isEmpty() && validUrl) {
        _ui->parsingAnimation->setMovie(_movie);
        _movie->start();
        _ui->btnStart->setEnabled(false);

        std::vector<QListWidget*> mediaLists{};
        mediaLists.push_back(_ui->imagesList);
        mediaLists.push_back(_ui->audioList);
        mediaLists.push_back(_ui->videosList);
        mediaLists.push_back(_ui->docsList);
        mediaLists.push_back(_ui->allList);

        if (_imagesCBox->isChecked()) {
            _downloader->_imagesSelected = true;
        }
        if (_audioCBox->isChecked()) {
            _downloader->_audioSelected = true;
        }
        if (_videosCBox->isChecked()) {
            _downloader->_videosSelected = true;
        }
        if (_docsCBox->isChecked()) {
            _downloader->_docsSelected = true;
        }
        emit startDownload(url, mediaLists);
    } else {
        QMessageBox::critical(this, "Error", "URL not entered or invalid!");
    }
}

void MainWindow::btnStop_clicked() {
    _movie->stop();
    _ui->parsingAnimation->clear();
    if (_downloader->_parsingIsGood) {
        _ui->parsingAnimation->setPixmap(*_pixCompleted);
    } else {
        _ui->parsingAnimation->setPixmap(*_pixExpected);
    }
    _ui->btnStart->setEnabled(true);
}

void MainWindow::btnSettings_clicked() {
    Settings* dialog = new Settings;
    dialog->setWindowFlags(dialog->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    dialog->setWindowFlags(dialog->windowFlags() & ~(Qt::WindowFullscreenButtonHint | Qt::WindowMaximizeButtonHint));
    dialog->setWindowModality(Qt::WindowModal);
    dialog->setAttribute(Qt::WA_DeleteOnClose); //clear memory
    dialog->show();
}

void MainWindow::btnSave_clicked() {
    _downloader->_directory = QFileDialog::getExistingDirectory(this, tr("Select a directory to save"), QDir::homePath());
}

void MainWindow::readyToStart() { 
    _movie->stop();
    _ui->parsingAnimation->clear();
    if (_downloader->_parsingIsGood) {
        _ui->parsingAnimation->setPixmap(*_pixCompleted);
    } else {
        _ui->parsingAnimation->setPixmap(*_pixExpected);
    }
    _ui->btnStart->setEnabled(true);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    _downloadThread->quit(); // Остановка работы потока
    _downloadThread->wait(); // Ожидание завершения потока
    QMainWindow::closeEvent(event);
}