#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtCore>
#include <QScreen>
#include <QApplication>

#include "Audio_Player.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void handleMessage(const QString& message)
    {
        if(message.size()>0)
        {
            this->showNormal();
            QScreen *screen = QGuiApplication::primaryScreen();
            QSize screenSize = screen->size();
            this->move((screenSize.width()-this->width()) / 2, (screenSize.height()-this->height()) / 2);
            this->activateWindow();
        }
    }

private slots:
    void on_selectFileButton_clicked();

    void on_playButton_clicked();

    void on_pauseButton_clicked();

private:
    Ui::MainWindow *ui;

    Audio_Player *audio_Player = nullptr;
    Sherpa_Helper *sherpa_Helper = nullptr;
};
#endif // MAINWINDOW_H
