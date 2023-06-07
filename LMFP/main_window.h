#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QMovie>
#include <QThread>
#include <QPixmap>
#include <QCheckBox>
#include <vector>
#include "download_worker.h"
#include "settings.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void btnStart_clicked();
    void btnStop_clicked();
    void btnSettings_clicked();
    void btnSave_clicked();
    void readyToStart();
    void closeEvent(QCloseEvent *event);
    void updateImagesCounter();
    void updateAudioCounter();
    void updateVideosCounter();
    void updateDocsCounter();
    void updateAllCounter();
    void updateImagesAllCounter();
    void updateAudioAllCounter();
    void updateVideosAllCounter();
    void updateDocsAllCounter();
    void updateAllAllCounter();
   // void retranslateUi();
signals:
    void startDownload(QString url, std::vector<QListWidget*> lists); 
private:
    Ui::MainWindow *_ui{};
    QMovie *_movie{};
    QPixmap *_pixExpected{}, *_pixCompleted{};
    QThread *_downloadThread{};
    DownloadWorker *_downloader{};
    Settings *_dialog{};
};

#endif //MAIN_WINDOW_H
