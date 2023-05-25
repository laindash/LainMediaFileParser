#ifndef CHECKER_H
#define CHECKEH_H
#include <QString>

bool checkUrl(const QString &url);

enum MEDIA_LIST
{
	IMAGES,
	AUDIO,
	VIDEOS,
	DOCS,
	ALL
};

#endif