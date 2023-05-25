#include "settings.h"

Settings::Settings(QWidget *parent) : QMainWindow(parent), _uiSettings(new Ui::Settings) {
	_uiSettings->setupUi(this);
}

Settings::~Settings() {
	delete _uiSettings;
}
