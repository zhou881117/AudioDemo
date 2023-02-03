#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

extern "C" {
 #include "libavcodec/avcodec.h"
 #include "libavdevice/avdevice.h"
 #include "libavformat/avformat.h"
 #include "libavfilter/avfilter.h"
 #include "libavutil/avutil.h"
 #include "libswscale/swscale.h"
 #include "libavutil/pixdesc.h"
 #include "libavutil/imgutils.h"
 #include "libavutil/dict.h"
 #include "libavutil/opt.h"
 #include "libavutil/time.h"
 #include "libswresample/swresample.h"
}

extern "C" {
  #include "portaudio.h"
}

#include <QtCore>
#include <QThread>
#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include <QtMath>

#include "AudioCache.h"

class Audio_Player: public QThread
{
    Q_OBJECT

public:
    Audio_Player(QObject *parent,  QString filePath);

    ~Audio_Player();

    void tryStart();

    void tryStop();



protected:
    bool Open(char *filePath);

    void Close();

     int ReadData();

protected:
    /*线程处理函数*/
    virtual void run() override;


    static int RecordCallback(const void *input_buffer, void * /*output_buffer*/,
                              unsigned long frames_per_buffer,  // NOLINT
                              const PaStreamCallbackTimeInfo * /*time_info*/,
                              PaStreamCallbackFlags /*status_flags*/,
                              void *user_data);



private:
    //设备相关
    //QAudioOutput *audioOutput = nullptr;
    //QIODevice *ioDevice = nullptr;


    QString filePath;
    AudioCache   audioCache;
    PaStreamParameters outputParameters;

private:

    AVFormatContext	*pFormatCtx = NULL;
    int				 audioIndex = -1;

    //解码相关
    AVCodecContext	*pCodecCtx = NULL;
    AVCodec			*pCodec = NULL;

    //读取相关
    AVPacket *packet = NULL;
    AVFrame	*pFrame = NULL;

    //转码相关
    struct SwrContext *audio_convert_ctx = NULL;

    const   int temp_buffer_size = 192000;
    unsigned char *temp_buffer = NULL;
};

#endif // AUDIO_PLAYER_H
