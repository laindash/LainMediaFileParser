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
    
    connect(_downloader, &DownloadWorker::downloadFinished, this, &MainWindow::readyToStart);

    connect(_ui->btnStart, &QPushButton::clicked, this, &MainWindow::btnStart_clicked);
    connect(_ui->btnStop, &QPushButton::clicked, this, &MainWindow::btnStop_clicked);
    connect(_ui->btnSave, &QPushButton::clicked, this, &MainWindow::btnSave_clicked);
    connect(_ui->btnSettings, &QPushButton::clicked, this, &MainWindow::btnSettings_clicked);

    connect(_downloader, &DownloadWorker::imagesCountChanged, this, &MainWindow::updateImagesCounter);
    connect(_downloader, &DownloadWorker::audioCountChanged, this, &MainWindow::updateAudioCounter);
    connect(_downloader, &DownloadWorker::videosCountChanged, this, &MainWindow::updateVideosCounter);
    connect(_downloader, &DownloadWorker::docsCountChanged, this, &MainWindow::updateDocsCounter);
    connect(_downloader, &DownloadWorker::allCountChanged, this, &MainWindow::updateAllCounter);
    connect(_downloader, &DownloadWorker::imagesAllCountChanged, this, &MainWindow::updateImagesAllCounter);
    connect(_downloader, &DownloadWorker::audioAllCountChanged, this, &MainWindow::updateAudioAllCounter);
    connect(_downloader, &DownloadWorker::videosAllCountChanged, this, &MainWindow::updateVideosAllCounter);
    connect(_downloader, &DownloadWorker::docsAllCountChanged, this, &MainWindow::updateDocsAllCounter);
    connect(_downloader, &DownloadWorker::allAllCountChanged, this, &MainWindow::updateAllAllCounter);

    QPixmap pixSave(":/imageResource/img/btnSave.png");
    _ui->btnSave->setIcon(pixSave);
    _ui->btnSave->setStyleSheet("QPushButton { background-color: transparent; border: none; }");
    _ui->btnSave->setIconSize(_ui->btnSave->size());

    _ui->imagesAllCount->setAlignment(Qt::AlignCenter);
    _ui->audioAllCount->setAlignment(Qt::AlignCenter);
    _ui->videosAllCount->setAlignment(Qt::AlignCenter);
    _ui->docsAllCount->setAlignment(Qt::AlignCenter);
    _ui->allAllCount->setAlignment(Qt::AlignCenter);
    _ui->imagesCount->setAlignment(Qt::AlignCenter);
    _ui->audioCount->setAlignment(Qt::AlignCenter);
    _ui->videosCount->setAlignment(Qt::AlignCenter);
    _ui->docsCount->setAlignment(Qt::AlignCenter);
    _ui->allCount->setAlignment(Qt::AlignCenter);
    _ui->separator1->setAlignment(Qt::AlignCenter);
    _ui->separator2->setAlignment(Qt::AlignCenter);
    _ui->separator3->setAlignment(Qt::AlignCenter);
    _ui->separator4->setAlignment(Qt::AlignCenter);
    _ui->separator5->setAlignment(Qt::AlignCenter);

    _dialog = new Settings(this);
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
    _ui->btnStart->setEnabled(false);
    _downloader->clearCounters();
    _ui->imagesList->clear();
    _ui->audioList->clear();
    _ui->videosList->clear();
    _ui->docsList->clear();
    _ui->allList->clear();
    _downloader->_stopDownload = false;
    _downloader->_parsingIsGood = false;
    _downloader->_htmlIsBad = false;  

    std::vector<QListWidget*> mediaLists{};
        mediaLists.push_back(_ui->imagesList);
        mediaLists.push_back(_ui->audioList);
        mediaLists.push_back(_ui->videosList);
        mediaLists.push_back(_ui->docsList);
        mediaLists.push_back(_ui->allList);

    QString url = _ui->inputUrl->text();
    bool validUrl = true, readyToStart = true;
    validUrl = checkUrl(url);

    if (!url.isEmpty() && validUrl) {
        if (_ui->imagesCBox->isChecked()) {
            _downloader->_imagesSelected = true;
            std::string ext = _dialog->getImagesExtensions().toStdString();
            if (!(ext.empty())) {
                if (checkExtensions(ext)) {
                    _downloader->_imagesExtensions = '(' + ext + ')';
                }
                else {
                    QMessageBox::critical(this, "Error", "Image extensions is incorrect!");
                    readyToStart = false;
                }
            }
            else {
                _downloader->setDefaultImages();
            }
        }
        else {
            _downloader->_imagesSelected = false;
        }

        if (_ui->audioCBox->isChecked()) {
            _downloader->_audioSelected = true;
            std::string ext = _dialog->getAudioExtensions().toStdString();
            if (!(ext.empty())) {
                if (checkExtensions(ext)) {
                    _downloader->_audioExtensions = '(' + ext + ')';
                }
                else {
                    QMessageBox::critical(this, "Error", "Audio extensions is incorrect!");
                    readyToStart = false;
                }
            }
            else {
                _downloader->setDefaultAudio();
            }
        }
        else {
            _downloader->_audioSelected = false;
        }

        if (_ui->videosCBox->isChecked()) {
            _downloader->_videosSelected = true;
            std::string ext = _dialog->getVideosExtensions().toStdString();
            if (!(ext.empty())) {
                if (checkExtensions(ext)) {
                    _downloader->_videosExtensions = '(' + ext + ')';
                }
                else {
                    QMessageBox::critical(this, "Error", "Videos extensions is incorrect!");
                    readyToStart = false;
                }
            }
            else {
                _downloader->setDefaultVideos();
            }
        }
        else {
            _downloader->_videosSelected = false;
        }

        if (_ui->docsCBox->isChecked()) {
            _downloader->_docsSelected = true;
            std::string ext = _dialog->getDocsExtensions().toStdString();
            if (!(ext.empty())) {
                if (checkExtensions(ext)) {
                    _downloader->_docsExtensions = '(' + ext + ')';
                }
                else {
                    QMessageBox::critical(this, "Error", "Docs extensions is incorrect!");
                    readyToStart = false;
                }
            }
            else {
                _downloader->setDefaultDocs();
            }
        }
        else {
            _downloader->_docsSelected = false;
        }

        if (!(_downloader->_imagesSelected || _downloader->_audioSelected || _downloader->_videosSelected || _downloader->_docsSelected)) {
            QMessageBox::warning(this, "Warning", "Select at least one of the filters to start parsing!");
            readyToStart = false;
        }
        
    } 
    else {
        QMessageBox::critical(this, "Error", "URL not entered or invalid!");
        readyToStart = false;
        _ui->parsingAnimation->clear();
        _ui->parsingAnimation->setPixmap(*_pixExpected);
    }

    if (readyToStart) {
        _ui->parsingAnimation->setMovie(_movie);
        _movie->start();     
        _downloadThread->start();
        emit startDownload(url, mediaLists);
    }
    else {
        _ui->btnStart->setEnabled(true);
    }
}

void MainWindow::btnStop_clicked() {
    _movie->stop();
    _ui->parsingAnimation->clear();
    if (_downloader->_parsingIsGood) {
        _ui->parsingAnimation->setPixmap(*_pixCompleted);
    } 
    else {
        _ui->parsingAnimation->setPixmap(*_pixExpected);
    }
    _downloader->_stopDownload = true;
    _downloadThread->quit(); // stop thread
    _downloadThread->wait(); // waiting cancelling thread
    _ui->btnStart->setEnabled(true);
}

void MainWindow::btnSettings_clicked() {
    _dialog->setWindowFlags(_dialog->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    _dialog->setWindowFlags(_dialog->windowFlags() & ~(Qt::WindowFullscreenButtonHint | Qt::WindowMaximizeButtonHint));
    _dialog->setWindowModality(Qt::ApplicationModal);
    //_dialog->setAttribute(Qt::WA_DeleteOnClose); //clear memory
    _dialog->show();
}

void MainWindow::btnSave_clicked() {
    _downloader->_directory = QFileDialog::getExistingDirectory(this, tr("Select a directory to save"), QDir::homePath());
}

void MainWindow::readyToStart() { 
    _movie->stop();
    _ui->parsingAnimation->clear();
    if (_downloader->_parsingIsGood) {
        _ui->parsingAnimation->setPixmap(*_pixCompleted);
    } 
    else {
        _ui->parsingAnimation->setPixmap(*_pixExpected);
        if (_downloader->_htmlIsBad) {
            QMessageBox::warning(this, "Warning", "HTML could not be retrieved from the specified url!");
        }
        else if (!_downloader->_parsingIsGood) {
            QMessageBox::information(this, "Information", "The specified filters could not find media files");
        }
    }
    _ui->btnStart->setEnabled(true);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    _downloader->_stopDownload = true;
    _downloadThread->quit(); // stop thread
    _downloadThread->wait(); // waiting cancelling thread
    QMainWindow::closeEvent(event);
}

void MainWindow::updateImagesCounter() {
    _ui->imagesCount->setText(QString::number(_downloader->_imagesCount));
}

void MainWindow::updateAudioCounter() {
    _ui->audioCount->setText(QString::number(_downloader->_audioCount));
}

void MainWindow::updateVideosCounter() {
    _ui->videosCount->setText(QString::number(_downloader->_videosCount));
}

void MainWindow::updateDocsCounter() {
    _ui->docsCount->setText(QString::number(_downloader->_docsCount));
}

void MainWindow::updateAllCounter() {
    _ui->allCount->setText(QString::number(_downloader->_allCount));
}

void MainWindow::updateImagesAllCounter() {
    _ui->imagesAllCount->setText(QString::number(_downloader->_imagesAllCount));
}

void MainWindow::updateAudioAllCounter() {
    _ui->audioAllCount->setText(QString::number(_downloader->_audioAllCount));
}

void MainWindow::updateVideosAllCounter() {
    _ui->videosAllCount->setText(QString::number(_downloader->_videosAllCount));
}

void MainWindow::updateDocsAllCounter() {
    _ui->docsAllCount->setText(QString::number(_downloader->_docsAllCount));
}

void MainWindow::updateAllAllCounter() {
    _ui->allAllCount->setText(QString::number(_downloader->_allAllCount));
}
