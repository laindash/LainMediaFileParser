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
public slots:
	void downloadMedia(QString url, QListWidget *list);
signals:

private:
	size_t writeToFile(void* contents, size_t size);
	void downloadFile(QString &text, QListWidget *audioList);
	QString saveHtml(QString& url);
	QString getLinksFromHtml(QString &savedPath);
	void downloadMusic(QString &url, QListWidget *audioList);
};

size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);

#endif //DOWNLOAD_WORKER_H