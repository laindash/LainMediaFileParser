// download functions
#include "download_worker.h"
#include "checker.h"
#include <iostream>
#include <fstream>
#include <string>
#include <QMessageBox>
#include <QFileInfo>
#include <QUrl>
#include <QDir>
#include <regex>

size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    // callback function for writing data to a file
    std::ofstream *file = static_cast<std::ofstream*>(userp);
    file->write(static_cast<const char*>(contents), size * nmemb);
    return size * nmemb;
}

void DownloadWorker::downloadFile(QString &text) {
	CURL* curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        // setting the file download URL
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
        } 
        else {
            fullPath = _directory + QDir::separator() + fileName;
        }
        int counter = 1;
        while (QFile::exists(fullPath)) {
            QString uniqueFileName = '(' + QString::number(counter) + ')' + fileName;
            fullPath = (_directory.isEmpty() ? "grab" : _directory) + QDir::separator() + uniqueFileName;
            counter++;
        }
        // open file for writing
        std::ofstream file(fullPath.toStdString(), std::ofstream::binary);
        if (file.is_open()) {
            // setting a callback to write data to a file
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);

            // making a request and downloading a file
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "Failed to download file: " << curl_easy_strerror(res) << std::endl;
            }
            file.close();
        }
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
    } 
    else {
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
                _htmlIsBad = true;
                std::cerr << "Failed to retrieve HTML: " << curl_easy_strerror(res) << std::endl;
            }
            file.close();
        } 
        else {
            std::cerr << "Failed to open file for writing: " << savedPath.toStdString() << std::endl;
        }
        curl_easy_cleanup(curl);

    }
    else {
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

    std::ifstream fileCount(savedPath.toStdString());
    std::string lineForCount{};
    std::regex patternImages{}, patternAudio{}, patternVideos{}, patternDocs{};
    int imagesAllCount(0), audioAllCount(0), videosAllCount(0), docsAllCount(0), allAllCount(0); 
    patternImages = "(https?://[\\w\\-\\.]+\\.\\w+\\.\\w+/[\\w\\d\\/\\-\\.]+\\." + _imagesExtensions + ')';
    patternAudio = "(https?://[\\w\\-\\.]+\\.\\w+\\.\\w+/[\\w\\d\\/\\-\\.]+\\." + _audioExtensions + ')';
    patternVideos = "(https?://[\\w\\-\\.]+\\.\\w+\\.\\w+/[\\w\\d\\/\\-\\.]+\\." + _videosExtensions + ')';
    patternDocs = "(https?://[\\w\\-\\.]+\\.\\w+\\.\\w+/[\\w\\d\\/\\-\\.]+\\." + _docsExtensions + ')';

    while (std::getline(fileCount, lineForCount) && !fileCount.eof()) {
        if (std::regex_match(lineForCount, patternImages)) {
            imagesAllCount++;
            allAllCount++;
            if (_imagesSelected) {
                _parsingIsGood = true;
            }
        }
        else if (std::regex_match(lineForCount, patternAudio)) {
            audioAllCount++;
            allAllCount++;
            if (_audioSelected) {
                _parsingIsGood = true;
            }
        }
        else if (std::regex_match(lineForCount, patternVideos)) {
            videosAllCount++;
            allAllCount++;
            if (_videosSelected) {
                _parsingIsGood = true;
            }
        }
        else if (std::regex_match(lineForCount, patternDocs)) {
            docsAllCount++;
            allAllCount++;
            if (_docsSelected) {
                _parsingIsGood = true;
            }
        }
    }
    fileCount.close();
    _imagesAllCount = imagesAllCount;
    _audioAllCount = audioAllCount;
    _videosAllCount = videosAllCount;
    _docsAllCount = docsAllCount;
    _allAllCount = allAllCount;
    emit imagesAllCountChanged();
    emit audioAllCountChanged();
    emit videosAllCountChanged();
    emit docsAllCountChanged();
    emit allAllCountChanged();

    std::ifstream fileResult(savedPath.toStdString());
    std::string line{};
    QString urlFromResult{};
    bool fileIsDownloaded{};

    while (std::getline(fileResult, line) && !fileResult.eof()) {
        if (!_stopDownload) {
            fileIsDownloaded = false;
            urlFromResult = QString::fromStdString(line);
            if (_imagesSelected && !fileIsDownloaded) {
                if (std::regex_match(urlFromResult.toStdString(), patternImages)) {
                    downloadFile(urlFromResult);
                    fileIsDownloaded = true;
                    QUrl urlChecker(urlFromResult);
                    QString fileName = QFileInfo(urlChecker.path()).fileName(); 
                    QListWidgetItem *itemImages = new QListWidgetItem(fileName);
                    QListWidgetItem *itemAll = new QListWidgetItem(fileName); 
                    lists[IMAGES]->addItem(itemImages);
                    lists[ALL]->addItem(itemAll);
                    _imagesCount++;
                    _allCount++;
                    emit imagesCountChanged();
                    emit allCountChanged();
                }
            }
            if (_audioSelected && !fileIsDownloaded) {
                if (std::regex_match(urlFromResult.toStdString(), patternAudio)) {
                    downloadFile(urlFromResult);
                    fileIsDownloaded = true;
                    QUrl urlChecker(urlFromResult);
                    QString fileName = QFileInfo(urlChecker.path()).fileName();
                    QListWidgetItem *itemAudio = new QListWidgetItem(fileName);
                    QListWidgetItem *itemAll = new QListWidgetItem(fileName);                     
                    lists[AUDIO]->addItem(itemAudio);
                    lists[ALL]->addItem(itemAll);
                    _audioCount++;
                    _allCount++;
                    emit audioCountChanged();
                    emit allCountChanged();
                }
            }
            if (_videosSelected && !fileIsDownloaded) {
                if (std::regex_match(urlFromResult.toStdString(), patternVideos)) {
                    downloadFile(urlFromResult);
                    fileIsDownloaded = true;
                    QUrl urlChecker(urlFromResult);
                    QString fileName = QFileInfo(urlChecker.path()).fileName(); 
                    QListWidgetItem *itemVideos = new QListWidgetItem(fileName);
                    QListWidgetItem *itemAll = new QListWidgetItem(fileName); 
                    lists[VIDEOS]->addItem(itemVideos);
                    lists[ALL]->addItem(itemAll);
                    _videosCount++;
                    _allCount++;
                    emit videosCountChanged();
                    emit allCountChanged();
                }
            }
            if (_docsSelected && !fileIsDownloaded) {
                if (std::regex_match(urlFromResult.toStdString(), patternDocs)) {
                    downloadFile(urlFromResult);
                    fileIsDownloaded = true;
                    QUrl urlChecker(urlFromResult);
                    QString fileName = QFileInfo(urlChecker.path()).fileName(); 
                    QListWidgetItem *itemDocs= new QListWidgetItem(fileName);
                    QListWidgetItem *itemAll = new QListWidgetItem(fileName); 
                    lists[DOCS]->addItem(itemDocs);
                    lists[ALL]->addItem(itemAll);
                    _docsCount++;
                    _allCount++;
                    emit docsCountChanged();
                    emit allCountChanged();
                }
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

void DownloadWorker::clearCounters() {
    _imagesCount = 0;
    _audioCount = 0;
    _videosCount = 0;
    _docsCount = 0;
    _allCount = 0;
    _imagesAllCount = 0;
    _audioAllCount = 0;
    _videosAllCount = 0;
    _docsAllCount = 0;
    _allAllCount = 0;
    emit imagesCountChanged();
    emit audioCountChanged();
    emit videosCountChanged();
    emit docsCountChanged();
    emit allCountChanged();
    emit imagesAllCountChanged();
    emit audioAllCountChanged();
    emit videosAllCountChanged();
    emit docsAllCountChanged();
    emit allAllCountChanged();
}