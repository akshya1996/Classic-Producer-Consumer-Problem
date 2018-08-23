#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h> //for including sleep
#define QUEUE_SIZE 5 //queue size as mentioned in the requirement
int size; //size of the queue
char circular_queue[QUEUE_SIZE];//i am using circular queue as an array
int pindex=0;//index of the circular queue at producer end
int cindex=0;//index of the circular queue at the consumer end
int elements=0;//at any point of time , it tells us the number of elements in the queue
int file_eof=1;//1 if the file is read completely
pthread_mutex_t mutex;
pthread_cond_t itemavail;
pthread_cond_t spaceavail;

void *producer()
{
FILE *fp=fopen("message.txt","r");
char ch=getc(fp);
while(ch!=EOF)
{
	sleep(1);
	pthread_mutex_lock(&mutex);
	//if the number of elements in the queue is greater than or equal to queue sixe i.e, queue is full
	while(elements>=size)
	{
		//then wait until the sapace is available in the quqeue
		pthread_cond_wait(&spaceavail,&mutex);
	

	}
	//inserting the element into the queue
	circular_queue[pindex]=ch;
	
	elements++;
	pindex++;

	//if pindex reaches the end of the queue
	if(pindex==size)
	{
		pindex=0;
	}
	printf("Character inserted into the circular queue by producer:\t%c\n",ch);
	//after inserting sending signal to the consumer that the item is now available
	pthread_cond_signal(&itemavail);
	pthread_mutex_unlock(&mutex);
	sleep(1);
	ch=getc(fp);

}
pthread_mutex_lock(&mutex);
file_eof=0;//the file has been read completely
pthread_cond_signal(&itemavail);
pthread_mutex_unlock(&mutex);
fclose(fp);
pthread_exit(0);
}

void * consumer()
{
	//printf("Printing from consumer end\n");
	//writing the outpyut into result.txt file
FILE *fp1=fopen("result.txt","w");
while(file_eof!=0)
{
	pthread_mutex_lock(&mutex);
	//if the file has not been completely read and if there are no elemets in the queue then wait
	while(file_eof!=0 && elements<=0)
	{
		pthread_cond_wait(&itemavail,&mutex);
	}
	char ch;
	//after reading insert the element into the queue
	ch=circular_queue[cindex];
	cindex++;
	if(cindex==size)
	{
		cindex=0;
	}
	elements--;
	if(elements==0)
	{
	printf("Characater removed from the circular queue by consumer:\t%c\n",ch);
		fprintf(fp1,"%c",ch);
        }
	pthread_cond_signal(&spaceavail);
	pthread_mutex_unlock(&mutex);
}

fclose(fp1);
//reading the message read from the consumer end from result.txt
	printf("Final Message read from Consumer end :\n");
FILE *fp=fopen("result.txt","r");
char ch;
ch=getc(fp);
while(ch!=EOF)
{
	printf("%c",ch);
	ch=getc(fp);
}
fclose(fp);
printf("\n\n\n\n To view the received message in a file.Enter the following command \n");
printf("vi result.txt\n");


pthread_exit(0);
}



int main(int argc, char *argv[])
{

	//reading the message that is going to be sent by the producer
	printf("Message to be sent into the circular queue\n");
	FILE *fp3=fopen("message.txt","r");
	char e;
	e=getc(fp3);
	while(e!=EOF)
	{
		printf("%c",e);
		e=getc(fp3);
	}
	printf("\n\n\n\n");
	size=QUEUE_SIZE;
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&spaceavail,NULL);
	pthread_cond_init(&itemavail,NULL);
	pthread_t p;
	pthread_create(&p,NULL,producer,NULL);
	pthread_t c;
	pthread_create(&c,NULL,consumer,NULL);
	pthread_join(p,NULL);
	pthread_join(c,NULL);



	return 0;

}


