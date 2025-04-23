#include <QApplication>
#include "mainwindow.h"
#include "signalhandler.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Create signal handler instance
    SignalHandler signalHandler;
    
    MainWindow w;
    w.show();
    
    return app.exec();
}
