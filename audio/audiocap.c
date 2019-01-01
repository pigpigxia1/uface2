/******************************************************************
ALSA 简单的 playback 例子
******************************************************************/

#define ALSA_PCM_NEW_HW_PARAMS_API

//#include <alsa/asoundlib.h>
#include <tinyalsa/asoundlib.h>

int main() {
    long loops;
    int rc;
    int size;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int val;
    int dir;
    snd_pcm_uframes_t frames;
    char *buffer;

    /* Open PCM device for playback. */
    /* 打开 PCM playback 设备 */
    rc = snd_pcm_open(&handle, "default",
    SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr,
                "unable to open pcm device: %s\n",
                snd_strerror(rc));
        exit(1);
    }

    /* Allocate a hardware parameters object. */
    /* 分配一个硬件参数结构体 */
    snd_pcm_hw_params_alloca(&params);

    /* Fill it in with default values. */
    /* 使用默认参数 */
    snd_pcm_hw_params_any(handle, params);

    /* Set the desired hardware parameters. */
    /* 设置硬件参数 */

    /* Interleaved mode */
    snd_pcm_hw_params_set_access(handle, params,
                                SND_PCM_ACCESS_RW_INTERLEAVED);

    /* Signed 16-bit little-endian format */
    /* 数据格式为 16位 小端 */
    snd_pcm_hw_params_set_format(handle, params,
                                SND_PCM_FORMAT_S16_LE);

    /* Two channels (stereo) */
    /* 两个声道 */
    snd_pcm_hw_params_set_channels(handle, params, 2);

    /* 44100 bits/second sampling rate (CD quality) */
    /* 采样率为 44100 */
    val = 44100;
    snd_pcm_hw_params_set_rate_near(handle, params,
              &val, &dir);

    /* Set period size to 32 frames. */
    /* 设置一个周期为 32 帧 */
    frames = 32;
    snd_pcm_hw_params_set_period_size_near(handle,
          params, &frames, &dir);

    /* Write the parameters to the driver */
    /* 把前面设置好的参数写入到playback设备 */
    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0) {
        fprintf(stderr,
            "unable to set hw parameters: %s\n",
            snd_strerror(rc));
    exit(1);
    }

    /* Use a buffer large enough to hold one period */
    /* 得到一个周期的数据长度 */
    snd_pcm_hw_params_get_period_size(params, &frames,
                &dir);

    /* 因为我们是16位 两个通道，所以要 *2*2 也就是 *4 */
    size = frames * 4; /* 2 bytes/sample, 2 channels */
    buffer = (char *) malloc(size);

    /* We want to loop for 5 seconds */
    /* 得到一个周期的时间长度 */
    snd_pcm_hw_params_get_period_time(params,
                &val, &dir);
    /* 5 seconds in microseconds divided by
    * period time */
    loops = 5000000 / val;

    while (loops > 0) {
        loops--;
        /* 从标准输入中获取数据 */
        rc = read(0, buffer, size);
        if (rc == 0) {
            fprintf(stderr, "end of file on input\n");
            break;
        } else if (rc != size) {
            fprintf(stderr,
                "short read: read %d bytes\n", rc);
        }

        /* 播放这些数据 */
        rc = snd_pcm_writei(handle, buffer, frames);
        if (rc == -EPIPE) {
            /* EPIPE means underrun */
            fprintf(stderr, "underrun occurred\n");
            snd_pcm_prepare(handle);
        } else if (rc < 0) {
            fprintf(stderr,
                    "error from writei: %s\n",
                    snd_strerror(rc));
        }  else if (rc != (int)frames) {
            fprintf(stderr,
                    "short write, write %d frames\n", rc);
        }
    }

    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    free(buffer);

    return 0;
}