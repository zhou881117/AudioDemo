#pragma once
//#define _CRT_SECURE_NO_WARNINGS

//#ifdef DLLEXPORT_API
//#define DLLEXPORT_API extern "c" _declspec(dllimport)
//#else
//#define DLLEXPORT_API
//    extern "C" _declspec(dllexport)
//#endif

#if defined(__clang__)
#define DLL_EXPORT extern "C"
#elif defined(__GNUC__) || defined(__GNUG__)
#define DLL_EXPORT extern "C"
#elif defined(_MSC_VER)
#define DLL_EXPORT extern "C" _declspec(dllexport)
#endif
 

typedef void (*RecognizeCallback)(const char *, int);

DLL_EXPORT int sherpa_add(int a, int b);

DLL_EXPORT char *sherpa_init_feature();

DLL_EXPORT void sherpa_release_feature(char *_feature_extractor);

DLL_EXPORT void sherpa_go(char *_tokens, char *_encoder_param,
                         char *_encoder_bin,
               char *_decoder_param, char *_decoder_bin, char *_joiner_param,
               char *_joiner_bin, char *_feature_extractor, void *_callback);

DLL_EXPORT void sherpa_putdata(char *_feature_extractor, float sampling_rate,
                    const float *waveform, int frames_per_buffer);

