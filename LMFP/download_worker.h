#ifndef DOWNLOAD_WORKER_H
#define DOWNLOAD_WORKER_H

#define CURL_STATICLIB
#include "curl/curl.h"
#include <QString>
#include <QListwidget>
#include <fstream>

#ifdef _DEBUG
#pragma comment (lib, "curl/libcurl_a_debug.lib")
#else
#pragma comment (lib, "curl/libcurl_a_debug.lib")
#endif

#pragma comment (lib, "Normaliz.lib")
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Wldap32.lib")
#pragma comment (lib, "Crypt32.lib")
#pragma comment (lib, "advapi32.lib")


class DownloadWorker : public QObject {
	Q_OBJECT
public:
	QString _directory{};
	bool _parsingIsGood{}, _stopDownload{}, _imagesSelected{}, _audioSelected{}, _videosSelected{}, _docsSelected{};
	std::string _imagesExtensions{}, _audioExtensions{}, _videosExtensions{}, _docsExtensions{};
public:
	void setDefaultImages();
	void setDefaultAudio();
	void setDefaultVideos();
	void setDefaultDocs();
public slots:
	void downloadMedia(QString url, std::vector<QListWidget*> lists);
signals:
	void downloadFinished();
private:
	void downloadFile(QString &text, QListWidget *list);
	QString saveHtml(QString &url);
	QString getLinksFromHtml(QString &savedPath);
	
};


#endif //DOWNLOAD_WORKER_H