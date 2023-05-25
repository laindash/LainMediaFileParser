#include "settings.h"

Settings::Settings(QWidget *parent) : QMainWindow(parent), _uiSettings(new Ui::Settings) {
	_uiSettings->setupUi(this);
	setWindowIcon(QIcon(":/imageResource/img/maxresdefault.ico"));
}

Settings::~Settings() {
	delete _uiSettings;
}
