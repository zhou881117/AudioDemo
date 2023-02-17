# AudioDemo

#### 介绍
使用【k2-fsa、portaudio、ffmpeg、qt】进行实时语音识别 

#### 软件架构
1、使用k2-fsa的c-api实时识别
2、使用portaudio时，使用无锁队列
3、使用ffmpeg，注意文件采样率与播放采样率一致，注意帧长的获取


#### 安装教程

1.  修改sherpa-ncnn/c-api/c-api.h

    `
  #if defined(__clang__)
  #define DLL_EXPORT extern "C"
  #elif defined(__GNUC__) || defined(__GNUG__)
  #define DLL_EXPORT extern "C"
  #elif defined(_MSC_VER)
  #define DLL_EXPORT extern "C" _declspec(dllexport)

  ...
  DLL_EXPORT typedef struct SherpaNcnnModelConfig {

 ...

DLL_EXPORT SherpaNcnnRecognizer *CreateRecognizer(
    const SherpaNcnnModelConfig *model_config,
    const SherpaNcnnDecoderConfig *decoder_config);

 ...

DLL_EXPORT void Decode(SherpaNcnnRecognizer *p);

  #endif

`

    参考本仓库的3rdparty/sherpa-ncnn-c-api/include/c-api.h

    方便struct与method的导出
     
2.   qt的输出目录是DESTDIR = $$PWD/../bin

     所以把 3rdparty 目录下bin的文件copy到bin目录下

     3rdparty\sherpa_models目录下的 encoder_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.bin_1     encoder_jit_trace-v2-epoch-11-avg-2-pnnx.ncnn.bin_2 使用run.bat  
 合并后再copy

3.   也可以使用audio_demo_2023_2_10 这个release版本试用

#### 使用说明

![输入图片说明](%E7%A4%BA%E4%BE%8B.png)

#### 参与贡献


#### 特技



