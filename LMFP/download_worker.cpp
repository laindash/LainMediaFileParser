// download functions
#include "download_worker.h"
#include <iostream>
#include <fstream>
#include <string>
#include <QFileInfo>
#include <QUrl>
#include <regex>

size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    // Функция обратного вызова для записи данных в файл
    std::ofstream *file = static_cast<std::ofstream*>(userp);
    file->write(static_cast<const char*>(contents), size * nmemb);
    return size * nmemb;
}

void DownloadWorker::downloadFile(QString &text, QListWidget *audioList) {
	CURL* curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        // Установка URL-адреса для скачивания файла
        curl_easy_setopt(curl, CURLOPT_URL, text.toUtf8().constData());
        QString savedImagesPath = "C:\\Users\\User\\Desktop\\grab\\music\\";
        // Получение имени файла из URL-адреса
        QUrl url(text);
        QString fileName = QFileInfo(url.path()).fileName();
        QString fullImagesPath = savedImagesPath + fileName;

        // Открытие файла для записи
        std::ofstream file(fullImagesPath.toStdString(), std::ofstream::binary);
        if (file.is_open()) {
            // Установка функции обратного вызова для записи данных в файл
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);

            // Выполнение запроса и скачивание файла
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "Failed to download file: " << curl_easy_strerror(res) << std::endl;
            }
            // Закрытие файла
            file.close();
            
        }
        QListWidgetItem* item = new QListWidgetItem(fileName);
        audioList->addItem(item);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

QString DownloadWorker::saveHtml(QString &url) {
    CURL* curl = curl_easy_init();
    QString savedPath = "C:\\Users\\User\\Desktop\\grab\\";
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.toUtf8().constData());
        savedPath += "output.txt";
        std::ofstream file(savedPath.toStdString(), std::ofstream::out);
        if (file.is_open()) {
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "Failed to retrieve HTML: " << curl_easy_strerror(res) << std::endl;
            }

            file.close();
        } else {
            std::cerr << "Failed to open file for writing: " << savedPath.toStdString() << std::endl;
        }
        //savedPath = getLinksFromHtml(savedPath);
        //downloadMusic(savedPath);
        curl_easy_cleanup(curl);

    } else {
        std::cerr << "Failed to initialize libcurl" << std::endl;
    }
    return savedPath;
}

QString DownloadWorker::getLinksFromHtml(QString &savedPath) {
    std::ifstream file(savedPath.toStdString());
    QString resultPath = "C:\\Users\\User\\Desktop\\grab\\result.txt";
    std::ofstream outFile(resultPath.toStdString());
    //std::regex pattern("\"(https://.*\\.mp3)\".*");
    std::regex pattern("\"(https://[^\"]+\\.mp3[^\"]*)\"");
    std::string line;

    while (std::getline(file, line)) {
        std::sregex_iterator iter(line.begin(), line.end(), pattern);
        std::sregex_iterator end;

        while (iter != end) {
            outFile << (*iter)[1].str() << std::endl;
            ++iter;
        }
    }
    file.close();
    return resultPath;
}

void DownloadWorker::downloadMusic(QString &url, QListWidget *audioList) {
    QString savedPath = saveHtml(url);
    
    savedPath = getLinksFromHtml(savedPath);
    std::ifstream fileResult(savedPath.toStdString());
    std::string line{};

    while (std::getline(fileResult, line) && !fileResult.eof()) {
        QString urlFromResult = QString::fromStdString(line);
        downloadFile(urlFromResult, audioList);
    }

    fileResult.close();
}

void DownloadWorker::downloadMedia(QString url, QListWidget *list) {
    //downloadFile(url, list);
    downloadMusic(url, list);
}
