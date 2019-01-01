#include <stdio.h>  
#include <stdlib.h>  
#include <stdint.h>  
#include <assert.h>  
#include <string.h>
#include <speex/speex_preprocess.h>
//#include <speex/speex.h>

#define true 1
#define false 0
#define HEADLEN 44
#define SAMPLE_RATE   (8000)  
#define SAMPLES_PER_FRAME  (1024)
#define FRAME_SIZE   (SAMPLES_PER_FRAME * 1000/ SAMPLE_RATE)
#define FRAME_BYTES  (SAMPLES_PER_FRAME)
int main(int argc,char *argv[])
{
    size_t n = 0;
    FILE *inFile, *outFile;
	
	if(argc < 3)
	{
		printf("%s:[infile] [outfile]\n",argv[0]);
		return -1;
	}
    inFile = fopen(argv[1], "rb");
    outFile = fopen( argv[2], "wb");

    char *headBuf = (char*)malloc(HEADLEN);
    char *dataBuf = (char*)malloc(FRAME_BYTES * 2 );
    memset(headBuf, 0, HEADLEN);
    memset(dataBuf, 0, FRAME_BYTES);
    assert(headBuf != NULL);
    assert(dataBuf != NULL);

    SpeexPreprocessState *state = speex_preprocess_state_init(1024, SAMPLE_RATE);
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

    int flag = true;

    while (1)
    {
        if (flag == true)
        {
            flag = false;
            n = fread(headBuf, 1, HEADLEN, inFile);
            if (n == 0)
                break;
            fwrite(headBuf, 1, HEADLEN, outFile);
        }
        else
        {
            n = fread(dataBuf, 1, SAMPLES_PER_FRAME, inFile);
            if (n == 0)
                break;
            speex_preprocess_run(state, (spx_int16_t*)(dataBuf));
            fwrite(dataBuf, 1, SAMPLES_PER_FRAME, outFile);
        }
    }

    free(headBuf);
    free(dataBuf);
    fclose(inFile);
    fclose(outFile);
    speex_preprocess_state_destroy(state);
    return 0;
}