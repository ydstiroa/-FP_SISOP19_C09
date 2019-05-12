#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include <termios.h>
#include<sys/ipc.h>
#include <sys/shm.h>
#include <ao/ao.h>
#include <mpg123.h>

#define BITS 8

char choose;
int ch;
pthread_t tid1;


int mygetch(void)
{
    struct termios oldt,
    newt;
    int ch;
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    return ch;
}
char pilih;

void* play(void *arg){
    ch=1;
}

int main(int argc,char *argv[])
{
    ch=0;
   
    while(1){
        printf("Menu \n");
        printf("1. Play \n");
        printf("2. Stop \n");

        choose= mygetch();

        if(choose=='1'){
            pthread_create(&(tid1), NULL, play, NULL);          
        }

        if(ch==1){
            //printf("%d\n",flag);
            mpg123_handle *mh;
            unsigned char *buffer;
            size_t buffer_size;
            size_t done;
            int err;

            int driver;
            ao_device *dev;

            ao_sample_format format;
            int channels, encoding;
            long rate;

            if(argc < 2)
                exit(0);

            /* initializations */
            ao_initialize();
            driver = ao_default_driver_id();
            mpg123_init();
            mh = mpg123_new(NULL, &err);
            buffer_size = mpg123_outblock(mh);
            buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

            /* open the file and get the decoding format */
            mpg123_open(mh, argv[1]);
            mpg123_getformat(mh, &rate, &channels, &encoding);

            /* set the output format and open the output device */
            format.bits = mpg123_encsize(encoding) * BITS;
            format.rate = rate;
            format.channels = channels;
            format.byte_format = AO_FMT_NATIVE;
            format.matrix = 0;
            dev = ao_open_live(driver, &format, NULL);

            /* decode and play */
            while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
                ao_play(dev, buffer, done);

            /* clean up */
            free(buffer);
            ao_close(dev);
            mpg123_close(mh);
            mpg123_delete(mh);
            mpg123_exit();
            ao_shutdown();
        }
        if(choose==2){
            ch=0;
            break;
        }
    }
}