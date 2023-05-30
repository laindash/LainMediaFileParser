#include "settings.h"

Settings::Settings(QWidget *parent) : QMainWindow(parent), _uiSettings(new Ui::Settings) {
	_uiSettings->setupUi(this);
	setWindowIcon(QIcon(":/imageResource/img/maxresdefault.ico"));
}

Settings::~Settings() {
	delete _uiSettings;
}

QString Settings::getImagesExtensions() {
	return _uiSettings->imagesExt->toPlainText();
}

QString Settings::getAudioExtensions() {
	return _uiSettings->audioExt->toPlainText();
}

QString Settings::getVideosExtensions() {
	return _uiSettings->videosExt->toPlainText();
}

QString Settings::getDocsExtensions() {
	return _uiSettings->docsExt->toPlainText();
}

