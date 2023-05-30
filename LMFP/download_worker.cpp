// download functions
#include "download_worker.h"
#include "checker.h"
#include <iostream>
#include <fstream>
#include <string>
#include <QFileInfo>
#include <QUrl>
#include <QDir>
#include <regex>

size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    // Функция обратного вызова для записи данных в файл
    std::ofstream *file = static_cast<std::ofstream*>(userp);
    file->write(static_cast<const char*>(contents), size * nmemb);
    return size * nmemb;
}

void DownloadWorker::downloadFile(QString &text, QListWidget *list) {
	CURL* curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        // Установка URL-адреса для скачивания файла
        curl_easy_setopt(curl, CURLOPT_URL, text.toUtf8().constData());
        QUrl url(text);
        QString fileName = QFileInfo(url.path()).fileName();
        QString fullPath{};

        if (_directory.isEmpty()) {
            QString savedPath = "grab";
            QDir dir(savedPath);
            if (!dir.exists()) {
                dir.mkpath(".");
            }         
            fullPath = savedPath + QDir::separator() + fileName;
        } else {
            fullPath = _directory + QDir::separator() + fileName;
        }
        int counter = 1;
        while (QFile::exists(fullPath)) {
            QString uniqueFileName = QString::number(counter) + "_" + fileName;
            fullPath = (_directory.isEmpty() ? "grab" : _directory) + QDir::separator() + uniqueFileName;
            counter++;
        }
        // Открытие файла для записи
        std::ofstream file(fullPath.toStdString(), std::ofstream::binary);
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
            _parsingIsGood = true;
        }
        QListWidgetItem *item = new QListWidgetItem(fileName);
        list->addItem(item);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

QString DownloadWorker::saveHtml(QString &url) {
    CURL *curl = curl_easy_init();
    QString fullPath{}, savedPath{};

    if (_directory.isEmpty()) {
        QString temp = "grab";
        QDir dir(temp);
        if (!dir.exists()) {
            dir.mkpath(".");
        }         
        fullPath = temp + QDir::separator();
    } else {
        fullPath = _directory + QDir::separator();
    }
    savedPath = fullPath + "output.txt";
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.toUtf8().constData());
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
        curl_easy_cleanup(curl);

    } else {
        std::cerr << "Failed to initialize libcurl" << std::endl;
    }
    return fullPath;
}

QString DownloadWorker::getLinksFromHtml(QString &savedPath) {
    QString resultPath = savedPath + "result.txt";
    savedPath += "output.txt";
    std::ifstream file(savedPath.toStdString());
    std::ofstream outFile(resultPath.toStdString());

    std::regex pattern("(https?://[\\w\\-\\.]+\\.\\w+\\.\\w+/[\\w\\d\\/\\-\\.]+\\.\\w+)");
    
    std::string line{};
    while (std::getline(file, line)) {
        std::smatch match;
        std::regex_search(line, match, pattern);

        while (std::regex_search(line, match, pattern)) {
            outFile << match[1].str() << std::endl;
            line = match.suffix();
        }
    }
    file.close();
    return resultPath;
}

void DownloadWorker::downloadMedia(QString url, std::vector<QListWidget*> lists) {
    QString savedPath = saveHtml(url);
            
    savedPath = getLinksFromHtml(savedPath);
    std::ifstream fileResult(savedPath.toStdString());
    std::string line{};
    std::regex pattern{};
    QString urlFromResult{};
    bool fileIsDownloaded{};

    while (std::getline(fileResult, line) && !fileResult.eof()) {
        if (!_stopDownload) {
            fileIsDownloaded = false;
            urlFromResult = QString::fromStdString(line);
            if (_imagesSelected && !fileIsDownloaded) {
                pattern = "(https?://[\\w\\-\\.]+\\.\\w+\\.\\w+/[\\w\\d\\/\\-\\.]+\\." + _imagesExtensions + ')';
                if (std::regex_match(urlFromResult.toStdString(), pattern)) {
                    downloadFile(urlFromResult, lists[IMAGES]);
                    fileIsDownloaded = true;
                }
            }
            if (_audioSelected && !fileIsDownloaded) {
                pattern = "(https?://[\\w\\-\\.]+\\.\\w+\\.\\w+/[\\w\\d\\/\\-\\.]+\\." + _audioExtensions + ')';
                if (std::regex_match(urlFromResult.toStdString(), pattern)) {
                    downloadFile(urlFromResult, lists[AUDIO]);
                    fileIsDownloaded = true;
                }
            }
            if (_videosSelected && !fileIsDownloaded) {
                pattern = "(https?://[\\w\\-\\.]+\\.\\w+\\.\\w+/[\\w\\d\\/\\-\\.]+\\." + _videosExtensions + ')';
                if (std::regex_match(urlFromResult.toStdString(), pattern)) {
                    downloadFile(urlFromResult, lists[VIDEOS]);
                    fileIsDownloaded = true;
                }
            }
            if (_docsSelected && !fileIsDownloaded) {
                pattern = "(https?://[\\w\\-\\.]+\\.\\w+\\.\\w+/[\\w\\d\\/\\-\\.]+\\." + _docsExtensions + ')';
                if (std::regex_match(urlFromResult.toStdString(), pattern)) {
                    downloadFile(urlFromResult, lists[DOCS]);
                    fileIsDownloaded = true;
                }
            }
            else if (!(_imagesSelected || _audioSelected || _videosSelected || _docsSelected)) {
                downloadFile(urlFromResult, lists[ALL]);
            }

        }   
    }
    fileResult.close();
    emit downloadFinished();
}

void DownloadWorker::setDefaultImages() {
    _imagesExtensions = "(gif|bmp|jpg|png|jpeg|ico|tiff|tif|webp|eps|psd|cdr|raw)";
}

void DownloadWorker::setDefaultAudio() {
    _audioExtensions = "(aac|ac3|aif|aiff|amr|aob|ape|asf|aud|awb|bin|bwg|cdr|flac|gpx|ics|iff\
						|m|m3u|m3u8|m4a|m4b|m4p|m4r|mid|midi|mod|mp3|mpa|mpp|msc|msv|mts|nkc\
						|ogg|ps|ra|ram|sdf|sib|sln|spl|srt|temp|vb|wav|wave|wm|wma|wpd|xsb|xwb)";
}

void DownloadWorker::setDefaultVideos() {
    _videosExtensions = "(3g2|3gp|3gp2|3gpp|3gpp2|asf|asx|avi|bin|dat|drv|f4v|flv|gtp|h264|m4v\
						|mkv|mod|moov|mov|mp4|mpeg|mpg|mts|rm|rmvb|spl|srt|stl|swf|ts|vcd|vid|vob|webm|wm|wmv|yuv)";
}

void DownloadWorker::setDefaultDocs() {
    _docsExtensions = "(asp|cdd|cpp|doc|docm|docx|dot|dotx|epub|fb2|gpx|ibooks|indd|kdc|key|kml\
						|mdb|mdf|mobi|mso|ods|odt|one|oxps|pages|pdf|pkg|pl|pot|potm|potx|pps|ppsm\
						|ppsx|ppt|pptm|pptx|ps|pub|rtf|sdf|sgml|sldm|snb|wpd|wps|xar|xlr|xls|xlsb\
						|xlsm|xlsx|xlt|xltm|xltx|xps|html|js|css)";
}