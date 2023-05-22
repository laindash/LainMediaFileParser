#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QMovie>
#include "download_worker.h"
#include <QThread>

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
   // void retranslateUi();


private:
    Ui::MainWindow *_ui{};
    QMovie *_movie{};
    DownloadWorker *_downloader{};
    QThread *_downloadThread{};
};

#endif //MAIN_WINDOW_H
