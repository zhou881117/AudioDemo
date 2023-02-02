#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(),this->height());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_selectFileButton_clicked()
{
    QString  srcDirPath = QFileDialog::getOpenFileName(
                this,QStringLiteral("选择音频文件"),QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                "音频频文件 (*.mp3 *.wav *.aac *.wma);; 所有文件 (*.*);; ");

    if (srcDirPath.isEmpty())
    {
        return;
    }
    else
    {
        ui->lineEdit->setText(srcDirPath);
    }
}


void MainWindow::on_playButton_clicked()
{
    this->audio_Player = new Audio_Player(this,this->ui->lineEdit->text());
    this->audio_Player->tryStart();
    this->audio_Player->start();
}

void MainWindow::on_pauseButton_clicked()
{
    this->audio_Player->tryStop();
    delete  this->audio_Player;
}
