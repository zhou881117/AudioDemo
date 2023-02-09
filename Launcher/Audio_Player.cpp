#include "Audio_Player.h"

Audio_Player::Audio_Player(QObject *parent,  QString _filePath,Sherpa_Helper *_sherpa_Helper): QThread(parent)
{
    this->filePath = _filePath;
    this->sherpa_Helper = _sherpa_Helper;
}

Audio_Player::~Audio_Player()
{

}

void Audio_Player::tryStart()
{
    this->audioCache.isRunning = true;


}

void Audio_Player::tryStop()
{
    this->audioCache.isRunning  = false;
    this->quit();
    this->wait();
}


int  Audio_Player::RecordCallback(const void * /*input_buffer*/, void  *output_buffer,
                                  unsigned long frames_per_buffer,  // NOLINT
                                  const PaStreamCallbackTimeInfo */*time_info*/,
                                  PaStreamCallbackFlags /*status_flags*/,
                                  void *user_data) {
    AudioCache *audioCache = reinterpret_cast<AudioCache *>(user_data);


    //qDebug()<<"writeIndex"<<audioCache->writeIndex<<"readIndex"<<audioCache->readIndex;
    int  out_size = av_samples_get_buffer_size(NULL, audioCache->channelCount, frames_per_buffer, audioCache->sample_fmt,0);
    //qDebug()<<"rr out_size"<<frames_per_buffer<<out_size;
    bool getData = false;
    if(audioCache->writeIndex > audioCache->readIndex)
    {
        if(audioCache->writeIndex >= audioCache->readIndex+out_size)
        {
            memcpy(output_buffer,audioCache->buffer+audioCache->readIndex ,out_size);
            audioCache->readIndex += out_size;
            getData=true;
        }
    }
    else if(audioCache->writeIndex < audioCache->readIndex)
    {
        if(audioCache->writeIndex >= audioCache->readIndex + out_size - AudioCache::bufferSize)
        {
            if(audioCache->readIndex + out_size - AudioCache::bufferSize <= 0)
            {
                memcpy(output_buffer,audioCache->buffer + audioCache->readIndex ,out_size);
                audioCache->readIndex += out_size;
                getData=true;
            }
            else
            {
                int len1 = AudioCache::bufferSize - audioCache->readIndex;
                int len2 = out_size - len1;
                //qDebug()<<"len"<<out_size<<len1<<len2;

                if(len1>0)
                {
                    memcpy(output_buffer,audioCache->buffer+audioCache->readIndex ,len1);
                }
                memcpy((unsigned char*)output_buffer +len1,audioCache->buffer ,len2);
                audioCache->readIndex = len2;
                getData = true;
            }
        }
    }

    if(!getData)
    {
        memset(output_buffer,0,out_size);//没有输入时赋值0
    }
    return audioCache->isRunning ? paContinue: paComplete;
}



void Audio_Player::run()
{

    //PaStreamParameters inputParameters, outputParameters;
    PaError err;

    //初始化
    err = Pa_Initialize();
    if( err != paNoError )
    {
        //cout<<"Pa_Initialize error\r\n";
    }

    //获得设备数量
    int  iNumDevices = Pa_GetDeviceCount();
    if (iNumDevices < 0)
    {
        qDebug()<<"Get device count error\r\n";
    }

    //默认音频设备
    PaDeviceIndex outputDeviceIndex = Pa_GetDefaultOutputDevice();
    const  PaDeviceInfo *outputDeviceInfo = Pa_GetDeviceInfo(outputDeviceIndex);
    //枚举音频设备
    for (int i = 0;i<iNumDevices;i++)
    {
        const  PaDeviceInfo* paDeviceInfo = Pa_GetDeviceInfo(i);
        qDebug()<<"input count"<<paDeviceInfo->maxInputChannels
               <<"output count"<<paDeviceInfo->maxOutputChannels
              <<"hostApi"<<paDeviceInfo->hostApi
             <<"apiType"<<Pa_GetHostApiInfo(paDeviceInfo->hostApi)->type
            <<"name"<<paDeviceInfo->name<<endl;		//打印设备名
        if(outputDeviceIndex == -1 && paDeviceInfo->maxOutputChannels > 0)
        {
            outputDeviceIndex = i;
            outputDeviceInfo = paDeviceInfo;
        }

        PaHostApiTypeId typeId = PaHostApiTypeId::paASIO;
    }

    //av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO)
    //目的输出参数
    this->audioCache.channelCount = 1;// av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
    this->audioCache.sample_rate =  outputDeviceInfo->defaultSampleRate;//32000;// outputDeviceInfo->defaultSampleRate;// 48000;//采样率最好和音频文件的一致
    this->audioCache.sample_fmt = AVSampleFormat::AV_SAMPLE_FMT_FLT;//AV_SAMPLE_FMT_FLT;//AV_SAMPLE_FMT_S16;//    AV_SAMPLE_FMT_FLT;

    qDebug()<<"sample_rate"<<this->audioCache.sample_rate;

    this->audioCache.writeIndex = 0;
    this->audioCache.readIndex = 0;


    outputParameters.device = outputDeviceIndex;
    outputParameters.channelCount = this->audioCache.channelCount;
    outputParameters.sampleFormat = paFloat32;// paFloat32;// paFloat32;// paInt16;// paFloat32;// paFloat32;
    outputParameters.suggestedLatency = outputDeviceInfo->defaultLowOutputLatency;// outputDeviceInfo->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    //打开音频文件
    bool res = this->Open(this->filePath.toUtf8().data());

    if(res)
    {
        int _frame_size = 0;
        int framesPerBuffer = this->pCodecCtx->frame_size;//this->pCodecCtx->frame_size;// av_samples_get_buffer_size(NULL, this->channelCount, this->pCodecCtx->frame_size, this->sample_fmt,0);

        int max_size = av_samples_get_buffer_size(NULL, this->audioCache.channelCount, 1024 * 8, this->audioCache.sample_fmt,0);

        //paFramesPerBufferUnspecified
        //paClipOff
        //paDitherOff有效降低播放噪音
        PaStream *out_stream = nullptr;

        while (this->audioCache.isRunning) {
            if(this->audioCache.writeIndex>=this->audioCache.readIndex)
            {
                if(this->audioCache.writeIndex + max_size <= AudioCache::bufferSize)
                {
                    int out_size = this->ReadData(_frame_size);
                    if(out_size > 0)
                    {
                        memcpy(this->audioCache.buffer + this->audioCache.writeIndex,this->temp_buffer,out_size);
                        this->audioCache.writeIndex += out_size;
                    }
                }
                else
                {
                    int ll = this->audioCache.writeIndex + max_size - AudioCache::bufferSize;
                    if(ll <= this->audioCache.readIndex)
                    {
                        int out_size  =  this->ReadData(_frame_size);
                        if(out_size>0)
                        {

                            if(this->audioCache.writeIndex + out_size <= AudioCache::bufferSize)
                            {
                                memcpy(this->audioCache.buffer + this->audioCache.writeIndex,this->temp_buffer,out_size);
                                this->audioCache.writeIndex += out_size;
                            }
                            else
                            {
                                int len1 = AudioCache::bufferSize-this->audioCache.writeIndex;
                                int len2 = out_size - len1;
                                memcpy(this->audioCache.buffer + this->audioCache.writeIndex,this->temp_buffer,len1);
                                memcpy(this->audioCache.buffer,this->temp_buffer+len1,len2);
                                this->audioCache.writeIndex =len2;
                            }
                        }
                    }
                }
            }
            else
            {
                int ll = this->audioCache.readIndex - this->audioCache.writeIndex;
                if(ll >= max_size)
                {

                    int out_size = this->ReadData(_frame_size);
                    if(out_size>0)
                    {

                        memcpy(this->audioCache.buffer + this->audioCache.writeIndex,this->temp_buffer,out_size);
                        this->audioCache.writeIndex += out_size;
                    }
                }
            }

            if(_frame_size>0 && framesPerBuffer <= 0)
            {
                framesPerBuffer = _frame_size;//有时只有读一帧数据之后才知道帧长
            }
            if(out_stream == nullptr)
            {
                qDebug()<< "framesPerBuffer"<< framesPerBuffer;
                err = Pa_OpenStream(&out_stream, NULL, &outputParameters, this->audioCache.sample_rate, framesPerBuffer, paClipOff, RecordCallback, &this->audioCache);
                if (err != paNoError) {

                }

                err = Pa_StartStream(out_stream);
                if (err != paNoError) {

                }
            }

            msleep(1);
        }

        qDebug()<<"stop 1";
        Pa_StopStream(out_stream);
        Pa_AbortStream(out_stream);
        Pa_CloseStream(out_stream);
        Pa_Terminate();
    }
    else
    {
        exec();//和quit是一对
    }

    this->Close();
}

bool Audio_Player::Open(char *filePath)
{
    bool res = false;
    this->pFormatCtx = avformat_alloc_context();

    if(avformat_open_input(&this->pFormatCtx,filePath,nullptr,nullptr) != 0) //&opts
    {
        return res;
    }


    //查找流信息
    if(avformat_find_stream_info(this->pFormatCtx, NULL)!=0)
    {
        return res;
    }



    //查找流index
    this->audioIndex = -1;
    AVCodec **codec=nullptr;
    this->audioIndex = av_find_best_stream(this->pFormatCtx,AVMediaType::AVMEDIA_TYPE_AUDIO,-1,-1,codec,0);


    if(this->audioIndex == -1)
    {
        return res;
    }

    //初始化解码
    this->pCodec = avcodec_find_decoder(this->pFormatCtx->streams[this->audioIndex]->codecpar->codec_id);//avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL)
    {
        qDebug()<<"find_decoder -1";
        return res;
    }

    this->pCodecCtx = avcodec_alloc_context3(this->pCodec);
    avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[this->audioIndex]->codecpar);

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        qDebug()<<"avcodec_open2 -1";
        return res;
    }

    //av_get_bytes_per_sample(pCodecCtx->sample_fmt) 每一个采样数据的byte长度
    qDebug()<<"frame_size"<<this->pCodecCtx->frame_size;//有些有，有些又没有
    qDebug()<<"sample_rate"<<pCodecCtx->sample_rate;


    //设备支持音频文件的采样率
    PaError err = Pa_IsFormatSupported(NULL,&outputParameters,pCodecCtx->sample_rate);
    if( err == paFormatIsSupported )
    {
        this->audioCache.sample_rate = pCodecCtx->sample_rate;
    }

    this->packet = av_packet_alloc();// (AVPacket *)av_malloc(sizeof(AVPacket));
    //this->packet->size=1000;
    this->pFrame = av_frame_alloc();
    //this->pFrame->nb_samples = 1000;
    //Out Buffer Size 每个采样值所占的字节数
    this->temp_buffer  = (unsigned char *)av_malloc(this->temp_buffer_size);
    //转码
    //Swr
    this->audio_convert_ctx = swr_alloc();
    this->audio_convert_ctx = swr_alloc_set_opts(this->audio_convert_ctx, av_get_default_channel_layout(this->audioCache.channelCount), this->audioCache.sample_fmt, this->audioCache.sample_rate,
                                                 av_get_default_channel_layout(pCodecCtx->channels), pCodecCtx->sample_fmt, pCodecCtx->sample_rate, NULL, NULL);
    swr_init(this->audio_convert_ctx);

    //qDebug()<<"pFrame->nb_samples"<<pFrame->nb_samples;


    res = true;

    return  res;
}

void Audio_Player::Close()
{
    swr_free(&this->audio_convert_ctx);
    this->audio_convert_ctx = NULL;

    av_packet_free(&packet);

    if (pFrame != NULL)
    {
        av_free(pFrame);
        pFrame = NULL;
    }

    if (pCodecCtx != NULL)
    {
        avcodec_close(pCodecCtx);
        pCodecCtx = NULL;
    }

    if (pFormatCtx != NULL)
    {
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
        pFormatCtx = NULL;
    }

    if (this->temp_buffer != NULL)
    {
        av_free(this->temp_buffer);
        this->temp_buffer = NULL;
    }


}

int Audio_Player::ReadData(int &frame_size)
{
    int out_size = 0;
    if (av_read_frame(pFormatCtx, packet) >= 0)
    {
        if (packet->stream_index == this->audioIndex)
        {
            int  ret = avcodec_send_packet(pCodecCtx, packet);
            av_packet_unref(packet);
            if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
            {
                qDebug()<<111;
                return out_size;
            }
            //从解码器返回解码输出数据
            ret = avcodec_receive_frame(pCodecCtx, pFrame);
            if (ret < 0 && ret != AVERROR_EOF)
            {
                //av_packet_unref(packet);
                qDebug()<<222;
                return out_size;
            }

            if (ret < 0)
            {
                printf("Error in decoding audio frame.\n");
                qDebug()<<333;
                return out_size;
            }

            frame_size = pFrame->nb_samples;

            this->sherpa_Helper->putdata(pCodecCtx->channels,pCodecCtx->sample_fmt,pCodecCtx->sample_rate,pFrame->data,pFrame->nb_samples);

            int len = swr_convert(this->audio_convert_ctx, &this->temp_buffer, this->temp_buffer_size, (const uint8_t **)pFrame->data, pFrame->nb_samples);


            out_size = av_samples_get_buffer_size(NULL, this->audioCache.channelCount, len, this->audioCache.sample_fmt,0);

            //qDebug()<< "swr_convert"<<pFrame->nb_samples<<len<<out_size;

        }
        //av_packet_unref(packet);
    }
    else
    {
        //qDebug()<<444;
    }

    return out_size;
}

