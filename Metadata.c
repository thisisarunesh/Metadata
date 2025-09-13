#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/log.h>

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define PINK "\033[95m"
#define RESET "\033[0m"

int metadata(const char *input)
{
    av_log_set_level(AV_LOG_QUIET);
    printf("%s[+]%s Getting metadata\n%s | %s\n", GREEN, RESET, GREEN, RESET);

    AVFormatContext *av_format_context = NULL;

    if (avformat_open_input(&av_format_context, input, NULL, NULL) < 0)
    {
        printf("%s[-]%s Failed to open input\n%s | %s\n", RED, RESET, GREEN, RESET);
        return 1;
    }

    if (avformat_find_stream_info(av_format_context, NULL) < 0)
    {
        printf("%s[-]%s Failed to find stream metadata\n%s | %s\n", RED, RESET, GREEN, RESET);
        avformat_close_input(&av_format_context);
        return 1;
    }

    int video_stream_index = -1;

    for (int i = 0; i < av_format_context->nb_streams; i++)
    {
        if (av_format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1)
    {
        printf("%s[-]%s No video stream found\n%s | %s\n", RED, RESET, GREEN, RESET);
        avformat_close_input(&av_format_context);
        return 1;
    }

    AVStream *video_stream = av_format_context->streams[video_stream_index];
    AVCodecParameters *video_codec_parameters = video_stream->codecpar;
    const AVCodec *video_codec = avcodec_find_decoder(video_codec_parameters->codec_id);

    if (!video_codec)
    {
        printf("%s[-]%s Failed to find decoder for codec ID %d\n%s | %s\n", RED, RESET, video_codec_parameters->codec_id, GREEN, RESET);
        avformat_close_input(&av_format_context);
        return 1;
    }

    double frames_per_second = av_q2d(video_stream->avg_frame_rate);
    double video_bitrate = video_codec_parameters->bit_rate ? video_codec_parameters->bit_rate / 1000.0 : 0.0;

    printf("%s[=]%s %sVideo:%s\n", GREEN, RESET, PINK, RESET);
    printf("%s |-%s Stream index: %d\n", GREEN, RESET, video_stream_index);
    printf("%s |-%s Codec: %s\n", GREEN, RESET, video_codec->long_name);
    printf("%s |-%s Resolution: %dx%d\n", GREEN, RESET, video_codec_parameters->width, video_codec_parameters->height);
    printf("%s |-%s Frames Per Second: %.2f\n", GREEN, RESET, frames_per_second);
    printf("%s |-%s Bitrate: %.2f kbps\n%s | %s\n", GREEN, RESET, video_bitrate, GREEN, RESET);

    int audio_stream_index = -1;

    for (int j = 0; j < av_format_context->nb_streams; j++)
    {
        if (av_format_context->streams[j]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audio_stream_index = j;
            break;
        }
    }

    if (audio_stream_index == -1)
    {
        printf("%s[-]%s No audio stream found\n%s | %s\n", RED, RESET, GREEN, RESET);
        avformat_close_input(&av_format_context);
        return 1;
    }

    AVStream *audio_stream = av_format_context->streams[audio_stream_index];
    AVCodecParameters *audio_codec_parameters = audio_stream->codecpar;
    const AVCodec *audio_codec = avcodec_find_decoder(audio_codec_parameters->codec_id);

    if (!audio_codec)
    {
        printf("%s[-]%s Failed to find decoder for codec ID %d\n%s | %s\n", RED, RESET, audio_codec_parameters->codec_id, GREEN, RESET);
        avformat_close_input(&av_format_context);
        return 1;
    }

    long long audio_bitrate = audio_codec_parameters->bit_rate ? audio_codec_parameters->bit_rate : 0;

    printf("%s[=]%s %sAudio:%s\n", GREEN, RESET, PINK, RESET);
    printf("%s |-%s Stream index: %d\n", GREEN, RESET, audio_stream_index);
    printf("%s |-%s Codec: %s\n", GREEN, RESET, audio_codec->long_name);
    printf("%s |-%s Sample rate: %d Hz\n", GREEN, RESET, audio_codec_parameters->sample_rate);
    printf("%s |-%s Bitrate: %lld\n%s | %s\n", GREEN, RESET, audio_bitrate, GREEN, RESET);
    avformat_close_input(&av_format_context);
    return 0;
}

int main(int argc, char *argv[])
{
    clock_t start, stop;
    double elapsed;
    int result = 0;

    if (argc != 2)
    {
        printf("Usage: %s [ input ]\n", argv[0]);
        return 1;
    }

    start = clock();
    result = metadata(argv[1]);
    stop = clock();

    elapsed = ((double)(stop - start)) / CLOCKS_PER_SEC;

    if (result == 0)
    {
        printf("%s[+]%s Metadata extraction completed in %.3f seconds\n", GREEN, RESET, elapsed);
    }
    else
    {
        printf("%s[-]%s Metadata extraction failed after %.3f seconds\n", RED, RESET, elapsed);
    }

    return result;
}