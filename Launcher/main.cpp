#include "mainwindow.h"

#include <QtCore>
#include <QApplication>

#include "qtsingleapplication.h"


int main(int argc, char *argv[])
{
    //    QApplication a(argc, argv);
    //    MainWindow w;
    //    w.show();
    //    return a.exec();
    QtSingleApplication app(argc, argv);

    if (app.isRunning()) {
        QString msg(QString("Hi master, I am %1.").arg(QCoreApplication::applicationPid()));
        bool sentok = app.sendMessage(msg, 2000);
        QString rep("Another instance is running, so I will exit.");
        rep += sentok ? " Message sent ok." : " Message sending failed; the other instance may be frozen.";

        return 0;
    }
    else
    {
        MainWindow w;
        QObject::connect(&app, SIGNAL(messageReceived(const QString&)),
                         &w, SLOT(handleMessage(const QString&)));
        w.show();

        return app.exec();
    }
}
