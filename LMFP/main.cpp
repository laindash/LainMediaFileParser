#include "main_window.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, ".UTF8");
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
