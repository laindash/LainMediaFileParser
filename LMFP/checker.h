#ifndef CHECKER_H
#define CHECKEH_H
#include <QString>

bool checkUrl(const QString &url);
bool checkExtensions(const std::string &ext);

enum MEDIA_LIST
{
	IMAGES,
	AUDIO,
	VIDEOS,
	DOCS,
	ALL
};

#endif