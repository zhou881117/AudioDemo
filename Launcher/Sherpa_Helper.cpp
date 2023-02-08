#include "Sherpa_Helper.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <cctype>  // std::tolower

Sherpa_Helper::Sherpa_Helper(QObject *parent): QThread(parent)
{

}

Sherpa_Helper::~Sherpa_Helper()
{

}

//void Sherpa_Helper::RecognizeCallback(const char * text, int size)
//{
//    QString s = QString(text);
//    qDebug()<<s;
//}


void Sherpa_Helper::tryStart()
{
    this->isRunning = true;

    this->model_config = new SherpaNcnnModelConfig();
    model_config->tokens = "D:/k2-fsa/sherpa-ncnn-2022-09-30/tokens.txt";
    model_config->encoder_param = "D:/k2-fsa/sherpa-ncnn-2022-09-30/encoder_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.param";
    model_config->encoder_bin = "D:/k2-fsa/sherpa-ncnn-2022-09-30/encoder_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.bin";
    model_config->decoder_param = "D:/k2-fsa/sherpa-ncnn-2022-09-30/decoder_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.param";
    model_config->decoder_bin = "D:/k2-fsa/sherpa-ncnn-2022-09-30/decoder_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.bin";
    model_config->joiner_param = "D:/k2-fsa/sherpa-ncnn-2022-09-30/joiner_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.param";
    model_config->joiner_bin = "D:/k2-fsa/sherpa-ncnn-2022-09-30/joiner_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.bin";
    model_config->use_vulkan_compute = 0;
    model_config->num_threads = 4;

    this->decoder_config = new SherpaNcnnDecoderConfig();
    decoder_config->decoding_method = "greedy_search";
    decoder_config->num_active_paths = 4;
    decoder_config->enable_endpoint = 1;// 0;
    decoder_config->rule1_min_trailing_silence = 2.4;
    decoder_config->rule2_min_trailing_silence = 1.2;
    decoder_config->rule3_min_utterance_length = 300;

    this->ans = CreateRecognizer(model_config , decoder_config);

}

void Sherpa_Helper::tryStop()
{
    this->isRunning  = false;
    this->quit();
    this->wait();

    DestroyRecognizer(ans);
    delete  model_config;
    delete  decoder_config;

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

        AcceptWaveform(this->ans, fixed_sample_rate, (const float*)ww,len);

        free(ww);
    }
}


void Sherpa_Helper::run()
{
    //    char token[] = "D:/k2-fsa/sherpa-ncnn-2022-09-30/tokens.txt";
    //    char encoder_param[] = "D:/k2-fsa/sherpa-ncnn-2022-09-30/encoder_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.param";
    //    char encoder_bin[] = "D:/k2-fsa/sherpa-ncnn-2022-09-30/encoder_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.bin";
    //    char decoder_param[] = "D:/k2-fsa/sherpa-ncnn-2022-09-30/decoder_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.param";
    //    char decoder_bin[] = "D:/k2-fsa/sherpa-ncnn-2022-09-30/decoder_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.bin";
    //    char joiner_param[] = "D:/k2-fsa/sherpa-ncnn-2022-09-30/joiner_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.param";
    //    char joiner_bin[] = "D:/k2-fsa/sherpa-ncnn-2022-09-30/joiner_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.bin";


    int last_segment = 0;
    QString last_text;
    int segment_index = 0;
    while (this->isRunning) {
        Decode(this->ans);
        bool is_endpoint = IsEndpoint(this->ans);
        SherpaNcnnResult *result = GetResult(this->ans);
        QString text = QString(result->text);

        if(text.size() > 0 && text != last_text)
        {
            if(segment_index == last_segment)
            {
                QString tt = text.mid(last_text.size());
                if(tt.size()>0)
                {
                    emit this->recognized(tt,segment_index);
                    qDebug()<<"append:"<<tt<<segment_index;
                }
            }
            else
            {
                emit this->recognized(text,segment_index);
                qDebug()<<"first:"<<text<<segment_index;
            }

            last_text = text;
            last_segment = segment_index;
        }

        if (text.size() > 0 && is_endpoint) {
            segment_index++;
        }

        DestroyResult(result);
        msleep(10);
    }
}



