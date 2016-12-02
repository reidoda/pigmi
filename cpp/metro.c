#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <sndfile.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

/* To compile: 
 * sudo apt-get install libasound2-dev
 * sudo apt-get install libsndfile-dev
 * gcc -I/usr/include -I/usr/include/alsa metro.c -lasound -lsndfile -lpthread -lm -o metro */


#define PCM_DEVICE "default"

void * playTick(void * arg) {
    struct readThreadParams {
        snd_pcm_t * pcm_handle;
        short * buf;
        int readcount;
    };
    struct readThreadParams * params = arg;
    short pcmrc;
    fprintf(stderr, "TICK\n");
    pcmrc = snd_pcm_writei(params->pcm_handle, params->buf, params->readcount);
    fprintf(stderr,"pcm_handle: %p, buf: %p, readcount: %i\n", params->pcm_handle, params->buf, params->readcount);
    if (pcmrc < 0) {
        fprintf(stderr, "pcmrc < 0\n");
    }
    pthread_exit(0);
}

double get_current_time(){
    struct timeval cur_time;
    gettimeofday(&cur_time,NULL);
    return cur_time.tv_sec + (cur_time.tv_usec / 1000000.0);
}

double sec_per_tick(double tempo) {
    return 60.0 / tempo;
}

double next_tick_time(double tempo) {
    double tick_period = sec_per_tick(tempo);
    return ceil(get_current_time() / tick_period) * tick_period;
}

int main(int argc, char **argv) {

    pthread_t pt;
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    snd_pcm_uframes_t frames;
    int dir;
    short pcmrc;

    char *infilename;
    /* infilename = "/home/roda/Dropbox/REFERENCE/sample_library/Drum Broker - Site Sampler/Ski Beatz - Karate School/SP-1200 Version/kalimbaloop2nopong(120bpm).wav"; */
    infilename = "/home/pi/global_metronome/onset_tick.wav";
    short* buf = NULL;
    int readcount;

    struct readThreadParams {
        snd_pcm_t * pcm_handle;
        short * buf;
        int readcount;
    };

    struct readThreadParams * readParams;
    readParams = malloc(sizeof(*readParams));

    SF_INFO sfinfo;
    SNDFILE *infile = NULL;

    infile = sf_open(infilename, SFM_READ, &sfinfo);
    fprintf(stderr,"Channels: %d\n", sfinfo.channels);
    fprintf(stderr,"Sample rate: %d\n", sfinfo.samplerate);
    fprintf(stderr,"Sections: %d\n", sfinfo.sections);
    fprintf(stderr,"Format: %d\n", sfinfo.format);

    /* Open the PCM device in playback mode */
    snd_pcm_open(&pcm_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0);

    /* Allocate parameters object and fill it with default values*/
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm_handle, params);

    /* Set parameters */
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle, params, sfinfo.channels);
    snd_pcm_hw_params_set_rate(pcm_handle, params, sfinfo.samplerate, 0);

    /* Write parameters */
    snd_pcm_hw_params(pcm_handle, params);

    /* Allocate buffer to hold full sample */
    snd_pcm_hw_params_get_period_size(params, &frames, &dir); // I'll use frames for the playback later


    fprintf(stderr,"Starting read/write loop\n");
    buf = malloc(sfinfo.frames * sfinfo.channels * sizeof(short));

    readcount = sf_read_short(infile, buf, sfinfo.frames);

    if (readcount < 0) {
        fprintf(stderr, "Could not read file.\n");
    }

    readParams->buf = buf;
    readParams->pcm_handle = pcm_handle;
    readParams->readcount = readcount;
    fprintf(stderr,"pcm_handle: %p, buf: %p, readcount: %i\n", readParams->pcm_handle, readParams->buf, readParams->readcount);

//    pcmrc = snd_pcm_writei(readParams->pcm_handle, readParams->buf, readParams->readcount);


    double tempo; 
    int n_ticks;
    char * ptr;
    if (argc > 1) {
        tempo = strtod(argv[1],&ptr); 
    } else {
        tempo = 140.0;
    }
    if (argc > 2) {
        n_ticks = strtod(argv[2],&ptr); 
    } else {
        n_ticks = 80;
    }
    fprintf(stderr,"tempo: %f\n",tempo);
    int i, handle_prepared;
    handle_prepared = 0;
    double lookahead = 0.0001;
    double tick_period = sec_per_tick(tempo);
    double next_tick = next_tick_time(tempo);
    double offset;
    for (i=0; i < n_ticks; ){
//        fprintf(stderr, "next_tick: %f, get_current_time(): %f\n",next_tick, get_current_time());
        offset = next_tick - get_current_time();
//        fprintf(stderr, "offset: %f, lookahead: %f\n",offset, lookahead);
        if (offset < lookahead) {
//            pthread_create(&pt, NULL, playTick, (void *) readParams);
            pcmrc = snd_pcm_writei(pcm_handle, buf, readcount);
            fprintf(stderr,"offset: %f\n",next_tick - get_current_time());
            fprintf(stderr, "pcmrc: %i\n", pcmrc);

            next_tick = next_tick + tick_period;
            i++;
//            usleep(tick_period / 2 * 1000000);
            usleep(offset*10000000);
            handle_prepared = 0;
        } else if (offset < 100 * lookahead && !handle_prepared) {
            snd_pcm_prepare(pcm_handle);
            handle_prepared = 1;
        } 
        usleep(100);
    }




    usleep(3000);
    fprintf(stderr,"End read/write loop\n");

    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);
    free(buf);

    return 0;
}
