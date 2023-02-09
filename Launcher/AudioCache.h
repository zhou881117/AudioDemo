#ifndef AUDIOCACHE_H
#define AUDIOCACHE_H
#include <QtCore>

extern "C" {
 #include "libavformat/avformat.h"
}

class AudioCache
{
public:
    AudioCache();

public:

    bool isRunning = false;
    int readIndex = 0;
    int writeIndex = 0;
    static const int bufferSize = 1024 * 500; //扩大缓存，有助于播放mp4的音频时，不卡顿
    char buffer[AudioCache::bufferSize];

    int channelCount = 1;
    int sample_rate = 0;
    AVSampleFormat sample_fmt = AVSampleFormat::AV_SAMPLE_FMT_FLT;

};

#endif // AUDIOCACHE_H
