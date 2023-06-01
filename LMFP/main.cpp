#include "main_window.h"
#include <QApplication>
#include <QCoreApplication>
#include <qdebug.h>
#include <windows.h>

int main(int argc, char *argv[]) {
    //system("chcp 65001");
    //SetConsoleCP(CP_UTF8);
    //SetConsoleOutputCP(CP_UTF8);
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
