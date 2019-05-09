#define BITS 8
#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<ao/ao.h>
#include<mpg123.h>
#include<termios.h>
#include<signal.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<termios.h>

pthread_t tid1, tid2, tid3, tid4;
int maen=0;
int putar_sekarang=0;
int paus=0;
int trig=0;
int berhenti=0;
int prin_menu=0;
int prin_menuplay=0;
int selesaii=0;
char lagu[100];

int getch()
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

int play(char argv[])
{
    while(1)
    {
	//printf("coba0\n");
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

	//if(argc < 2)
	//	exit(0);

	// initializations
	ao_initialize();
	driver = ao_default_driver_id();
	mpg123_init();
	mh = mpg123_new(NULL, &err);
	buffer_size = mpg123_outblock(mh);
	buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

	// open the file and get the decoding format
	mpg123_open(mh, argv);
	mpg123_getformat(mh, &rate, &channels, &encoding);

	// set the output format and open the output device
	format.bits = mpg123_encsize(encoding) * BITS;
	format.rate = rate;
	format.channels = channels;
	format.byte_format = AO_FMT_NATIVE;
	format.matrix = 0;
	dev = ao_open_live(driver, &format, NULL);

	// decode and play
	while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
	{
                ao_play(dev, buffer, done);
		while(paus==1)
		{
			printf("\r");
			if(paus==0) break;
		}
		if(berhenti==1) break;
        }
	berhenti=0;

	// clean up
	free(buffer);
	ao_close(dev);
	mpg123_close(mh);
	mpg123_delete(mh);
	mpg123_exit();
	ao_shutdown();
	break;

    }

}


void* menu(void *arg)
{
	//printf("haai\n");
	while(1)
	{
		char action;
		prin_menu=1;
		action=getch();
		if(action=='1') //play
		{
			//printf("play\n");
			prin_menu=0;
			prin_menuplay=1;
			trig=1;
			maen=1;
			while(1)
			{
				system("clear");
				prin_menuplay=1;
				action=getch();
				if(action=='1')
				{
					if(paus==0)
		                        {
		                                paus=1;
		                                printf("pause\n");
		                        }
		                        else if(paus==1)
		                        {
		                                paus=0;
		                                printf("play\n");
		                        }

				}
				else if(action=='2') //stop
				{
					printf("lagu berhenti\n");
					berhenti=1;
					break;
				}
			}
		}
		else if(action=='2')
		{
			printf("\r");
			printf("Terimakasih\n");
			selesaii=1;
			break;
		}
	}
}

void* trigger(void *arg)
{
	//printf("tes\n");
	while(1)
	{
		printf("\r");
		if(trig==1)
		{
			//printf("ini masuk loh\n");

			//maen=0;
			//printf("putar skr %d",putar_sekarang);
			//printf("maen %d",maen);

			if(maen==1)
			{
				putar_sekarang=1;
				//printf("ple\n");
			}
			if(putar_sekarang==1)
			{
				play(lagu);
				//printf("ini dibawah play\n");
			}
			//printf("ini keluar loh\n");
			trig=0;
			maen=0;

		}
		//printf("ini dibawah\n");
	}
}

void* cetak(void *arg)
{
	while(1)
	{
		printf("\r");
		if(prin_menu==1)
		{
			printf("-------- Welcome to Spotify kw --------\n");
			printf("1. Play Song\n2. Exit\n");
			sleep(1);
			system("clear");
		}
		else if(prin_menuplay==1)
		{
			printf("Now Playing : %s\n",lagu);
			printf("1. Pause/Play\n2. Stop\n");
			sleep(1);
			system("clear");
		}
	}
}

void* selesai(void *arg)
{
	printf("\r");
	while(selesaii==0)
	{
		printf("\r");
		if(selesaii==1) break;
	}

	pthread_kill(tid1, SIGKILL);
	pthread_kill(tid2, SIGKILL);
	pthread_kill(tid3, SIGKILL);
	pthread_kill(tid4, SIGKILL);
	//pthread_kill(tid5, SIGKILL);

}

int main(void)
{
	strcpy(lagu,"The_Bravery.mp3");

	pthread_create(&(tid1), NULL, menu, NULL);
	pthread_create(&(tid2), NULL, trigger, NULL);
	pthread_create(&(tid3), NULL, cetak, NULL);
	pthread_create(&(tid4), NULL, selesai, NULL);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	pthread_join(tid3, NULL);
	pthread_join(tid4, NULL);

    return 0;
}
