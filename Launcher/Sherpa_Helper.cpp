#include "Sherpa_Helper.h"

Sherpa_Helper::Sherpa_Helper(QObject *parent): QThread(parent)
{

}

Sherpa_Helper::~Sherpa_Helper()
{

}

void Sherpa_Helper::RecognizeCallback(const char * text, int size)
{
    QString s = QString(text);
    qDebug()<<s;
}


void Sherpa_Helper::tryStart()
{
    this->isRunning = true;

    this->feature_extractor = sherpa_init_feature();
}

void Sherpa_Helper::tryStop()
{
    this->isRunning  = false;
    this->quit();
    this->wait();

    sherpa_release_feature(this->feature_extractor);

    if(this->audio_convert_ctx != nullptr)
    {
        swr_free(&this->audio_convert_ctx);
        this->audio_convert_ctx = nullptr;
    }
}


void Sherpa_Helper::putdata(int channel_count,AVSampleFormat sample_fmt,int sample_rate,unsigned char* data[8],int nb_samples)//float sampling_rate,const float *waveform, int frames_per_buffer)
{
    int fixed_sample_rate = 16000;
    if(this->audio_convert_ctx == nullptr)// && sample_rate != fixed_sample_rate)
    {
        this->audio_convert_ctx = swr_alloc();
        this->audio_convert_ctx = swr_alloc_set_opts(this->audio_convert_ctx, av_get_default_channel_layout(1), AVSampleFormat::AV_SAMPLE_FMT_FLT, fixed_sample_rate,
                                                     av_get_default_channel_layout(channel_count), sample_fmt, sample_rate, NULL, NULL);
        swr_init(this->audio_convert_ctx);

    }

    if(this->audio_convert_ctx != nullptr)
    {
        int size =  av_samples_get_buffer_size(NULL, 1, nb_samples, AVSampleFormat::AV_SAMPLE_FMT_FLT,0);
        unsigned char * ww = (unsigned char *)malloc(size);
        int len = swr_convert(this->audio_convert_ctx, &ww, size, (const uint8_t **)data, nb_samples);
        sherpa_putdata(this->feature_extractor,fixed_sample_rate,(const float*)ww,len);
        free(ww);
    }
}


void Sherpa_Helper::run()
{
    char token[] = "D:/k2-fsa_x86/sherpa-ncnn-2022-09-30/tokens.txt";
    char encoder_param[] = "D:/k2-fsa_x86/sherpa-ncnn-2022-09-30/encoder_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.param";
    char encoder_bin[] = "D:/k2-fsa_x86/sherpa-ncnn-2022-09-30/encoder_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.bin";
    char decoder_param[] = "D:/k2-fsa_x86/sherpa-ncnn-2022-09-30/decoder_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.param";
    char decoder_bin[] = "D:/k2-fsa_x86/sherpa-ncnn-2022-09-30/decoder_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.bin";
    char joiner_param[] = "D:/k2-fsa_x86/sherpa-ncnn-2022-09-30/joiner_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.param";
    char joiner_bin[] = "D:/k2-fsa_x86/sherpa-ncnn-2022-09-30/joiner_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.bin";

    sherpa_go(token,encoder_param,encoder_bin,decoder_param,decoder_bin,joiner_param,joiner_bin,this->feature_extractor, (void *)RecognizeCallback,this->isRunning);

}


