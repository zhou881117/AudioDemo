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
    static const int bufferSize = 1024 * 50;
    char buffer[AudioCache::bufferSize];

    int channelCount = 1;
    int sample_rate = 0;
    AVSampleFormat sample_fmt = AVSampleFormat::AV_SAMPLE_FMT_FLT;

};

#endif // AUDIOCACHE_H
