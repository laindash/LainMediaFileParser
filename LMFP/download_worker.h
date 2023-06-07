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
	bool _parsingIsGood{}, _htmlIsBad{}, _stopDownload{}, _imagesSelected{}, _audioSelected{}, _videosSelected{}, _docsSelected{};
	std::string 
		_imagesExtensions = "(gif|bmp|jpg|png|jpeg|ico|tiff|tif|webp|eps|psd|cdr|raw)",

		_audioExtensions = "(aac|ac3|aif|aiff|amr|aob|ape|asf|aud|awb|bin|bwg|cdr|flac|gpx|ics|iff\
						|m|m3u|m3u8|m4a|m4b|m4p|m4r|mid|midi|mod|mp3|mpa|mpp|msc|msv|mts|nkc\
						|ogg|ps|ra|ram|sdf|sib|sln|spl|srt|temp|vb|wav|wave|wm|wma|wpd|xsb|xwb)",

		_videosExtensions = "(3g2|3gp|3gp2|3gpp|3gpp2|asf|asx|avi|bin|dat|drv|f4v|flv|gtp|h264|m4v\
						|mkv|mod|moov|mov|mp4|mpeg|mpg|mts|rm|rmvb|spl|srt|stl|swf|ts|vcd|vid|vob|webm|wm|wmv|yuv)",

		_docsExtensions = "(asp|cdd|cpp|doc|docm|docx|dot|dotx|epub|fb2|gpx|ibooks|indd|kdc|key|kml\
						|mdb|mdf|mobi|mso|ods|odt|one|oxps|pages|pdf|pkg|pl|pot|potm|potx|pps|ppsm\
						|ppsx|ppt|pptm|pptx|ps|pub|rtf|sdf|sgml|sldm|snb|wpd|wps|xar|xlr|xls|xlsb\
						|xlsm|xlsx|xlt|xltm|xltx|xps|html|js|css)";

	int _imagesCount{}, _audioCount{}, _videosCount{}, _docsCount{}, _allCount{},
		_imagesAllCount{}, _audioAllCount{}, _videosAllCount{}, _docsAllCount{}, _allAllCount{};
public:
	void setDefaultImages();
	void setDefaultAudio();
	void setDefaultVideos();
	void setDefaultDocs();
	void clearCounters();
public slots:
	void downloadMedia(QString url, std::vector<QListWidget*> lists);
signals:
	void downloadFinished();
	void imagesCountChanged();
	void audioCountChanged();
	void videosCountChanged();
	void docsCountChanged();
	void allCountChanged();
	void imagesAllCountChanged();
	void audioAllCountChanged();
	void videosAllCountChanged();
	void docsAllCountChanged();
	void allAllCountChanged();
private:
	void downloadFile(QString &text);
	QString saveHtml(QString &url);
	QString getLinksFromHtml(QString &savedPath);
	
};


#endif //DOWNLOAD_WORKER_H