#ifndef SHERPA_HELPER_H
#define SHERPA_HELPER_H

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
   #include "sherpa_ncnn_shell.h"
}

#include <QtCore>

class Sherpa_Helper: public QThread
{
    Q_OBJECT

public:
    Sherpa_Helper(QObject *parent);

    ~Sherpa_Helper();

    void tryStart();

    void tryStop();

    void putdata(int channel_count,AVSampleFormat sample_fmt,int sample_rate,unsigned char* data[8],int nb_samples);//float sampling_rate,const float *waveform, int frames_per_buffer);

protected:
    static void RecognizeCallback(const char * text, int size);

    /*线程处理函数*/
    virtual void run() override;

private:
    bool isRunning = false;

    char *feature_extractor = nullptr;
    //转码相关
    struct SwrContext *audio_convert_ctx = nullptr;



};

#endif // SHERPA_HELPER_H
