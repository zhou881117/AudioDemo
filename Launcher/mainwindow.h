#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private slots:
    void on_selectFileButton_clicked();

    void on_playButton_clicked();

    void on_pauseButton_clicked();

private:
    Ui::MainWindow *ui;

    Audio_Player *audio_Player = nullptr;
};
#endif // MAINWINDOW_H
