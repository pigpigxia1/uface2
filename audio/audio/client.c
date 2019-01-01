#include <asoundlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <speex/speex_preprocess.h>
#include "wav.h"
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT  8887

#define PERIOD_SIZE (64)
#define true 1
#define false 0
#define HEADLEN 44
#define SAMPLE_RATE   (44100)  
#define SAMPLES_PER_FRAME  (1024)
#define FRAME_SIZE   (SAMPLES_PER_FRAME * 1000/ SAMPLE_RATE)
#define FRAME_BYTES  (SAMPLES_PER_FRAME)


static int run_cap = 0;
static int run_play = 0;
static int noise = 0;

static int check_param(struct pcm_params *params, unsigned int param, unsigned int value,
                 char *param_name, char *param_unit)
{
    unsigned int min;
    unsigned int max;
    int is_within_bounds = 1;

    min = pcm_params_get_min(params, param);
    if (value < min) {
        fprintf(stderr, "%s is %u%s, device only supports >= %u%s\n", param_name, value,
                param_unit, min, param_unit);
        is_within_bounds = 0;
    }

    max = pcm_params_get_max(params, param);
    if (value > max) {
        fprintf(stderr, "%s is %u%s, device only supports <= %u%s\n", param_name, value,
                param_unit, max, param_unit);
        is_within_bounds = 0;
    }

    return is_within_bounds;
}


static int sample_is_playable(unsigned int card, unsigned int device, unsigned int channels,
                        unsigned int rate, unsigned int bits, unsigned int period_size,
                        unsigned int period_count)
{
    struct pcm_params *params;
    int can_play;

    params = pcm_params_get(card, device, PCM_OUT);
    if (params == NULL) {
        fprintf(stderr, "Unable to open PCM device %u.\n", device);
        return 0;
    }

    can_play = check_param(params, PCM_PARAM_RATE, rate, "Sample rate", "Hz");
    can_play &= check_param(params, PCM_PARAM_CHANNELS, channels, "Sample", " channels");
    can_play &= check_param(params, PCM_PARAM_SAMPLE_BITS, bits, "Bitrate", " bits");
    can_play &= check_param(params, PCM_PARAM_PERIOD_SIZE, period_size, "Period size", "Hz");
    can_play &= check_param(params, PCM_PARAM_PERIODS, period_count, "Period count", "Hz");

    pcm_params_free(params);

    return can_play;
}

void *Audio_Read(void *argv)
{
	int sockfd;
	struct wav_header header;
	struct pcm_config config;
    struct pcm *pcm_cap,*pcm_play;
	char *buffer;
    unsigned int size;
    unsigned int bytes_read = 0;
	SpeexPreprocessState *state;
	
	sockfd = *(int *)argv;
	
	header.riff_id = ID_RIFF;
    header.riff_sz = 0;
    header.riff_fmt = ID_WAVE;
    header.fmt_id = ID_FMT;
    header.fmt_sz = 16;
    header.audio_format = FORMAT_PCM;
    header.num_channels = 2;
    header.sample_rate = SAMPLE_RATE;
	
	header.bits_per_sample = pcm_format_to_bits(PCM_FORMAT_S16_LE);
    header.byte_rate = (header.bits_per_sample / 8) * header.num_channels * header.sample_rate;
    header.block_align = header.num_channels * (header.bits_per_sample / 8);
    header.data_id = ID_DATA;
	
	config.channels = header.num_channels;
    config.rate = header.sample_rate;
    config.period_size = PERIOD_SIZE;
    config.period_count = 4;
    config.format = PCM_FORMAT_S16_LE;
    config.start_threshold = 0;
    config.stop_threshold = 0;
    config.silence_threshold = 0;
	
	pcm_cap = pcm_open(0, 0, PCM_IN, &config);
    if (!pcm_cap || !pcm_is_ready(pcm_cap)) {
        fprintf(stderr, "Unable to open PCM device (%s)\n",
                pcm_get_error(pcm_cap));
        return (void *)0;
    }

    size = pcm_frames_to_bytes(pcm_cap, pcm_get_buffer_size(pcm_cap));
	printf("size=%d\n",size);
    buffer = malloc(size * 2);
    if (!buffer) {
        fprintf(stderr, "Unable to allocate %d bytes\n", size);
        free(buffer);
        pcm_close(pcm_cap);
        return (void *)0;
    }

    printf("Capturing sample: %u ch, %u hz, %u bit\n", config.channels, config.rate,
           pcm_format_to_bits(config.format));

	/*if (!sample_is_playable(0, 0, 2, header.sample_rate, 16, PERIOD_SIZE, 4)) {
        return (void *)0;
    }

    pcm_play = pcm_open(0, 0, PCM_OUT, &config);
    if (!pcm_play || !pcm_is_ready(pcm_play)) {
        fprintf(stderr, "Unable to open PCM device 0 (%s)\n",
                pcm_get_error(pcm_play));
        return (void *)0;
    }*/
	//if(noise)
	{
		state = speex_preprocess_state_init(size, SAMPLE_RATE);
		int denoise = 1;
		int noiseSuppress = -50;
		speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_DENOISE, &denoise);
		speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS, &noiseSuppress);
		
		int i;
		i = 1;
		speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_AGC, &i);
		i = 24000;
		speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_AGC_LEVEL, &i);
		/*i = 0;
		speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_DEREVERB, &i);
		float f = 0;
		speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_DEREVERB_DECAY, &f);
		f = 0;
		speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_DEREVERB_LEVEL, &f);
	*/
		//静音检测
		int vad = 1;
		int vadProbStart = 80;
		int vadProbContinue = 65;
		speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_VAD, &vad); 
		speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_PROB_START, &vadProbStart); 
		speex_preprocess_ctl(state, SPEEX_PREPROCESS_SET_PROB_CONTINUE, &vadProbContinue);

		//int flag = true;

	}
	
	send(sockfd, &header, sizeof(struct wav_header),0);
	printf("send head!\n");
	while(run_cap)
	{
		if(!pcm_read(pcm_cap, buffer, size))
		{
			speex_preprocess_run(state, (spx_int16_t*)(buffer));
			send(sockfd, buffer, size,0);
			/*if(noise)
				speex_preprocess_run(state, (spx_int16_t*)(buffer));
			if (pcm_write(pcm_play, buffer, size)) {
                fprintf(stderr, "Error playing sample\n");
                break;
            }*/
			
		}
	}
	run_play = 0;
	free(buffer);
    pcm_close(pcm_cap);
	//pcm_close(pcm_play);
	pthread_exit(NULL);
	//return (void *)0;
}

void *Audio_Play(void *argv)
{
	int sockfd;
	char readbuf[512];
	int read_num;
	int filelen = 0;
	struct wav_header header;
	struct pcm_config config;
    struct pcm *pcm_play;
	char *buffer;
	unsigned int size;
	
	sockfd = *(int *)argv;
	
	filelen = recv(sockfd,&header,sizeof(struct wav_header),0);
	printf("header size = %d\n",filelen);
	printf("wav: %u ch, %u hz\n", header.num_channels, header.sample_rate);
	
	
	config.channels = header.num_channels;
    config.rate = header.sample_rate;
    config.period_size = PERIOD_SIZE;
    config.period_count = 4;
    config.format = PCM_FORMAT_S16_LE;
    config.start_threshold = 0;
    config.stop_threshold = 0;
    config.silence_threshold = 0;
	
	
	if (!sample_is_playable(0, 0, 2, header.sample_rate, 16, PERIOD_SIZE, 4)) {
        return (void *)0;
    }

    pcm_play = pcm_open(0, 0, PCM_OUT, &config);
    if (!pcm_play || !pcm_is_ready(pcm_play)) {
        fprintf(stderr, "Unable to open PCM device 0 (%s)\n",
                pcm_get_error(pcm_play));
        return (void *)0;
    }
	
	size = pcm_frames_to_bytes(pcm_play, pcm_get_buffer_size(pcm_play));
	printf("size=%d\n",size);
    buffer = malloc(size * 2);
    if (!buffer) {
        fprintf(stderr, "Unable to allocate %d bytes\n", size);
        free(buffer);
        pcm_close(pcm_play);
        return (void *)0;
    }
	
	
	while(run_play)
	{
		read_num = read(sockfd,buffer,size);
		if(read_num > 0)
		{
			if (pcm_write(pcm_play, buffer, size)) {
                fprintf(stderr, "Error playing sample\n");
                break;
            }
			filelen += read_num;
			printf("read_num = %d\n",read_num);
		}
		else
		{
			//printf("wav: %u ch, %u hz\n", header.num_channels, header.sample_rate);
			//printf("filelen = %d\n",filelen);
			//sleep(1);
			//break;
		}
	
	}
	printf("filelen = %d\n",filelen);
	
	return (void *)0;
}

int main(int argc,char *argv[])
{
	pthread_t pid_cap;
	pthread_t pid_play;
	int ret;
	int cmd;
	struct sockaddr_in server;

	int sockfd;
	int port;
	char ip[20];
	char *sendbuf = "dslfkgffjgl";
	char readbuf[512];
	int read_num;
	int filelen = 0;
	int main_run = 1;
	
	if(argc < 2)
	{
		printf("enter port %s [ip]\n",argv[0]);
        exit(1);
	}

	
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))== -1){
        perror("socket error\n");
        exit(1);
    }
	
	
	bzero(&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_aton(argv[1],&server.sin_addr);
    if(connect(sockfd,(struct sockaddr *)&server,sizeof(server))== -1){
        perror("connect() error\n");
        exit(1);
    }
	
	run_cap = 1;
	ret = pthread_create(&pid_cap,NULL,Audio_Read,&sockfd);
	if(ret != 0)
	{
		printf("pthread_create error!\n");
		return -1;
		
	}
	
	run_play = 1;
	ret = pthread_create(&pid_play,NULL,Audio_Play,&sockfd);
	if(ret != 0)
	{
		printf("pthread_create error!\n");
		return -1;
		
	}
	printf("run!\n");
	while(main_run)
	{
		printf("enter cmd:1:start 2:stop 3:exit\n");
		scanf("%d",&cmd);
		switch(cmd)
		{
			case 1:
			
			break;
			case 2:
			run_cap = 0;
			pthread_join(pid_cap,NULL);
			pthread_join(pid_play,NULL);
			printf("stop audio!\n");
			break;
			case 3:
			run_cap = 0;
			pthread_join(pid_cap,NULL);
			pthread_join(pid_play,NULL);
			main_run = 0;
			break;
			
		}
	}
	
	printf("exit!\n");
	
	//pthread_join(pid_play,NULL);
	/*while(1)
	{
		read_num = read(sockfd,readbuf,sizeof(readbuf));
		if(read_num > 0)
		{
			filelen += read_num;
			printf("read_num = %d\n",read_num);
		}
		else
		{
			printf("filelen = %d\n",filelen);
			sleep(1);
			break;
		}
		sleep(1);
	}*/
	sleep(1);
	close(sockfd);
	return 0;
}