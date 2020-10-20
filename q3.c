#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h> 
#include<signal.h>
#include<string.h>
#include<semaphore.h>
#include <errno.h>
#include<time.h>
int muscount=0;
int acctr=5, elctr=5, coordctr=5, musctr=10;
int elearr[100][2], acarr[100][2];
char info[100][2][20];
pthread_mutex_t musmutex, acmutex, elmutex;
sem_t acoustic, electric, coord;
struct Musician{
	char name[20];
	char instr[1];
	int delay;
	int stage;
	//int stageno;
	pthread_mutex_t mutex;
}musics[100];

void waitAcous(int i, int sing){
	struct timespec ts;
	
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1){
        printf("Clock error\n");
        return;
    }
   	ts.tv_sec += 5;
   	
	int s = sem_timedwait(&acoustic, &ts);

	if(musics[i].stage==1){
		sem_post(&acoustic);
		return;
	}
	else if (s==-1 && errno == ETIMEDOUT){
		printf("%s got tired of waiting\n", musics[i].name);
		return;
	}
	else{
		pthread_mutex_lock(&musics[i].mutex);
		musics[i].stage=1;
		pthread_mutex_unlock(&musics[i].mutex);
		pthread_mutex_lock(&acmutex);
		int j=0;
		for (j=0; j<acctr; j++){
			if(acarr[j][0]==-1){
				acarr[j][0]=i;
				//musics[i].stageno=j;
				break;	
			}
		}
		//printf("Acoustic ");
		//for (int j=0; j<acctr; j++){
		//	printf("%d ",acarr[j] );
		//}
		
		pthread_mutex_unlock(&acmutex);
		//printf("\n");
		printf("%s joins acoustic stage %d\n",musics[i].name,j );
		
		//printf("%s entered acoustic stage\n", musics[i].name);
		int stdelay = rand()%6;
		sleep(stdelay);
		if(acarr[j][1]==1){
			printf("%s performance extended by singer\n",musics[i].name );
			sleep(2);
		}
		printf("%s %s played for %dsecs on acoustic, leaving\n", musics[i].name, musics[i].instr, stdelay);
		acarr[j][0]=-1;
		sem_post(&acoustic);
		
	}
	return;
}

void waitElec(int i, int sing){
	struct timespec ts;
	
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1){
        printf("Clock error\n");
        return;
	}
   	ts.tv_sec += 5;
	int s = sem_timedwait(&electric, &ts);

	if(musics[i].stage==1){
		sem_post(&electric);
		return;
	}
	else if (s==-1 && errno == ETIMEDOUT){
		printf("%s got tired of waiting\n", musics[i].name);
		return;
	}
	else{
		pthread_mutex_lock(&musics[i].mutex);
		musics[i].stage=1;
		pthread_mutex_unlock(&musics[i].mutex);
		pthread_mutex_lock(&elmutex);
		int j=0;
		for (j=0; j<elctr; j++){
			if(elearr[j][0]==-1){
				elearr[j][0]=i;
				//musics[i].stageno=j;
				break;	
			}
		}
		//printf("Electric ");
		
		//for (int j=0; j<elctr; j++){
		//	printf("%d ",elearr[j] );
		//}
		pthread_mutex_unlock(&elmutex);
		//printf("\n");
		printf("%s joins electric stage %d\n",musics[i].name,j );
		//printf("%s entered electric stage\n", musics[i].name);
		int stdelay = rand()%6;
		sleep(stdelay);
		if(elearr[j][1]==1){
			printf("%s performance extended by singer\n",musics[i].name );
			sleep(2);
		}
		printf("%s %s played for %dsecs on electric, leaving\n",  musics[i].name, musics[i].instr, stdelay);
		elearr[j][0]=-1;
		sem_post(&electric);
		
	}
	return;
}

void waitShirt(int i){
	printf("%s is waiting for shirt\n", musics[i].name);
	sem_wait(&coord);
	printf("%s is collecting shirt\n", musics[i].name);
	sleep(2);
	printf("%s has collected shirt\n", musics[i].name);
	sem_post(&coord);
}

void * waitAcousFn(void * i){
	waitAcous(*((int *) i), 1);
	pthread_exit(NULL);

}

void * waitElecFn(void * i){
	waitElec(*((int *) i), 1);
	pthread_exit(NULL);
}

//waitSinger(int i)
void * waitAcousSingerFn(void * i){
	waitAcous(*((int *) i), 0);
	pthread_exit(NULL);

}

void * waitElecSingerFn(void * i){
	waitElec(*((int *) i), 0);
	pthread_exit(NULL);
}

void * musFunc(void * input){
pthread_mutex_lock(&musmutex);
int i=muscount;
muscount++;
pthread_mutex_unlock(&musmutex);
pthread_mutex_init(&musics[i].mutex, NULL);

pthread_mutex_lock(&musics[i].mutex);
int delay = rand()%6;
musics[i].delay=delay;
musics[i].stage=0;
//musics[i].stageno=-1;
strcpy(musics[i].name,info[i][0]);
strcpy(musics[i].instr, info[i][1]);
pthread_mutex_unlock(&musics[i].mutex);
sleep(delay);
printf("%s %s has arrived\n",musics[i].name, musics[i].instr);
if (strcmp(musics[i].instr,"v")==0){
	//printf("%s has a violin\n",musics[i].name );
	printf("%s is waiting on acoustic\n",musics[i].name );
	waitAcous(i, 0);
}
else if(strcmp(musics[i].instr,"b")==0){
	printf("%s is waiting on electric\n",musics[i].name );
	waitElec(i, 0);
}
else if(strcmp(musics[i].instr,"s")==0){
	pthread_t id1, id2;
	printf("%s is waiting on acoustic or electric\n",musics[i].name );
	int *mem= (int *)malloc(sizeof(int));
	*mem = i;
	pthread_create(&id1, NULL, waitElecSingerFn, (void *)mem);
	pthread_create(&id2, NULL, waitAcousSingerFn,  (void *)mem);
	pthread_join(id1, NULL);
	pthread_join(id2, NULL);
	free(mem);
}
else{
	pthread_t id1, id2;
	printf("%s is waiting on acoustic or electric\n",musics[i].name );
	int *mem= (int *)malloc(sizeof(int));
	*mem = i;
	pthread_create(&id1, NULL, waitElecFn, (void *)mem);
	pthread_create(&id2, NULL, waitAcousFn,  (void *)mem);
	pthread_join(id1, NULL);
	pthread_join(id2, NULL);
	free(mem);
}
if (musics[i].stage==1){
	waitShirt(i);
}
printf("%s is done\n", musics[i].name);
pthread_exit(NULL);
}


int main(){
srand(time(0));
pthread_t musid[100];
sem_init(&acoustic, 0, acctr);
sem_init(&electric, 0, elctr);
sem_init(&coord, 0, coordctr);
pthread_mutex_init(&musmutex, NULL);
pthread_mutex_init(&acmutex, NULL);
pthread_mutex_init(&elmutex, NULL);

strcpy(info[0][0],"Tanvi");
strcpy(info[0][1],"v");
strcpy(info[1][0],"Tanvi2");
strcpy(info[1][1],"b");
strcpy(info[2][0],"Tanvi3");
strcpy(info[2][1],"p");
strcpy(info[3][0],"Tanvi4");
strcpy(info[3][1],"g");
strcpy(info[4][0],"Tanvi5");
strcpy(info[4][1],"v");
strcpy(info[5][0],"Tanvi6");
strcpy(info[5][1],"v");
strcpy(info[6][0],"Tanvi7");
strcpy(info[6][1],"b");
strcpy(info[7][0],"Tanvi8");
strcpy(info[7][1],"p");
strcpy(info[8][0],"Tanvi9");
strcpy(info[8][1],"g");
strcpy(info[9][0],"Tanvi10");
strcpy(info[9][1],"g");
//for (int i=0; i<5; i++){

	//scanf("%s", info[i][0]);
	//scanf("%s", info[i][1]);

//}
for (int i=0; i<100; i++){
	elearr[i][0]=-1;
	elearr[i][1]=0;
	acarr[i][0]=-1;
	acarr[i][1]=0;
}
//for(int i=0; i<elctr; i++){
//	printf("%d ",elearr[i] );
//}
//for(int i=0; i<acctr; i++){
//	printf("%d ",acarr[i] );
//}
for (int i=0; i<musctr; i++){
	pthread_create(&musid[i], NULL, musFunc, NULL);
}
for (int i=0; i<musctr; i++){
	pthread_join(musid[i], NULL); 
}
printf("Finished\n");
	return 0;
}