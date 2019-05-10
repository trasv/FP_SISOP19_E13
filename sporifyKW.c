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
#include<dirent.h>

pthread_t tid1, tid2, tid3, tid4, tid5;
int maen=0;
int putar_sekarang=0;
int paus=0;
int trig=0;
int berhenti=0;
int prin_menu=0;
int prin_menuplay=0;
int prin_list=0;
int selesaii=0;
int i, j=0;
char lagu[100];
char song[100][100];
char current[100];

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
			maen=1;
			int baris=0, kolom=0;
			while(1)
			{
				printf("\r");
				//system("clear");
				kolom=0;
				while(song[baris][kolom]!='\0')
				{
					sprintf(current, "%s%c", current, song[baris][kolom]);
					kolom++;
				}
				//printf("%s",current);
				trig=1;

				prin_menuplay=1;
				action=getch();
				if(action=='1')
				{
					if(paus==0)
		                        {
		                                paus=1;
		                                printf("pause\n");
						current[0]='\0';
		                        }
		                        else if(paus==1)
		                        {
		                                paus=0;
		                                printf("play\n");
						current[0]='\0';
		                        }

				}
				else if(action=='2')
				{
					printf("Next\n");
					berhenti=1;
					current[0]='\0';
					baris++;
				}
				else if(action=='3')
				{
					printf("Previous\n");
					berhenti=1;
					current[0]='\0';
					baris--;
				}
				else if(action=='4') //stop
				{
					printf("lagu berhenti\n");
					current[0]='\0';
					berhenti=1;
					trig=0;
					break;
				}
			}
		}
		else if(action=='2')
		{
			system("clear");
			printf("\r");
			prin_menu=0;
			prin_list=1;
			action=getch();
			if(action=='1')
			{
				prin_menu=1;
				prin_list=0;
			}
		}
		else if(action=='3')
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
	while(1)
	{
		printf("\r");
		if(trig==1)
		{
			play(current);
		}
	}
}

void* listlagu(void *arg)
{
}

void* cetak(void *arg)
{
	while(1)
	{
		printf("\r");
		if(prin_menu==1)
		{
			printf("-------- Welcome to Spotify kw --------\n");
			printf("1. Play Song\n2. List Song\n3. Exit\n");
			sleep(1);
			system("clear");
		}
		else if(prin_list==1)
		{
			printf("\r");
			printf("All Tracks :\n\n");


			DIR *d;
			struct dirent *dir;
			d = opendir(".");

			if(d)
			{
				while ((dir = readdir(d)) != NULL)
				{
					char x[100];
					strcpy(x,dir->d_name);
					if(x[strlen(x)-1] == '3' && x[strlen(x)-2] == 'p' && x[strlen(x)-3] == 'm')
					{
						for(i=0;i<strlen(x);i++)
						{
							song[j][i] = x[i];
						}
						for(i=0;i<strlen(x);i++)
						{
							printf("%c",song[j][i]);
						}
						j++;
						printf("\n");
					}
				}
				closedir(d);
			}
			printf("\nPress 1 to back\n");
			sleep(1);
			system("clear");
		}
		else if(prin_menuplay==1)
		{
			printf("Now Playing : %s\n",current);
			printf("1. Pause/Play\n2. Next\n3. Previous\n4. Stop\n");
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
	pthread_kill(tid5, SIGKILL);

}

int main(void)
{
	//strcpy(lagu,"The_Bravery.mp3");

	pthread_create(&(tid1), NULL, menu, NULL);
	pthread_create(&(tid2), NULL, trigger, NULL);
	pthread_create(&(tid3), NULL, cetak, NULL);
	pthread_create(&(tid4), NULL, selesai, NULL);
	pthread_create(&(tid5), NULL, listlagu, NULL);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	pthread_join(tid3, NULL);
	pthread_join(tid4, NULL);
	pthread_join(tid5, NULL);

    return 0;
}
