#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h> 
#include <signal.h>
int stcount=0, pharcount=0, vaccount=0, endsim=0;
float x=0.3;
pthread_mutex_t stmutex, pharmutex, vacmutex, endmutex;

int min(int a, int b){
	if (a>b){
		return b;
	}
	else{
		return a;
	}
}
struct Student{
	pthread_mutex_t mutex;
	int name;
	int numVac;
	int done;	
}students[200];


struct Vacc{
	pthread_mutex_t mutex;
	int name;
	int store;
	int slots;
	float x;
}vaccs[100];


struct Comp{
	pthread_mutex_t mutex;
	int name;
	float x;
}comps[100];

void stvaccinate(int stu){ //pass current count
	if (students[stu].numVac>=3){
		pthread_mutex_lock(&stmutex);
		students[stu].done=1;
		pthread_mutex_unlock(&stmutex);

		printf("Student %d went home\n", stu);
		int leftSt=0;
		pthread_mutex_lock(&stmutex);
		//printf("I am St %d and names of sts left are ", stu);
		for (int k=0; k<stcount;k++){
			if(!students[k].done){
				//printf("%d ", k);
				leftSt++;
			}											
		}
		pthread_mutex_unlock(&stmutex);
		//printf("\n" );

		pthread_mutex_lock(&endmutex);
		if (!leftSt){
			endsim=1;								
		}
		pthread_mutex_unlock(&endmutex);
		//printf("endsim %d\n", endsim );
		pthread_exit(NULL);
	}
	else{
 		printf("Student %d has arrived for his %dth round of Vaccination \n", stu, students[stu].numVac+1 );
 		printf("Student %d is waiting to be allocated a slot on a Vaccination Zone\n", stu );
 		while(students[stu].done==0 && students[stu].numVac<3 && endsim==0){
			for (int i=0; i<vaccount && students[stu].done==0; i++){
				
				pthread_mutex_lock(&vaccs[i].mutex);
					if (vaccs[i].slots>0){
						printf("Student %d assigned a slot on the Vaccination Zone %d and waiting to be vaccinated\n", stu, i );
						printf("Vaccination Zone %d entering Vaccination Phase\n", i );
						students[stu].numVac++;
						vaccs[i].slots--;
						printf("Student %d on Vaccination Zone %d has been vaccinated which has success probability %f\n", stu, i, vaccs[i].x);
						printf("Vaccination Zone %d exiting Vaccination Phase\n", i );

						pthread_mutex_unlock(&vaccs[i].mutex);
						printf("Vaccination Zone %d has %d slots left\n", i, vaccs[i].slots);
						
						float perc=100.00;
						float prob=(rand()%100)/perc;
						//printf("%f\n", prob);
						if (prob>vaccs[i].x){
							pthread_mutex_lock(&stmutex);
							students[stu].done=0;
							pthread_mutex_unlock(&stmutex);
							
							printf("Student %d tested negative for antibodies\n",stu );
							if (students[stu].numVac>=3){
								printf("Student %d went home\n", stu);
								pthread_mutex_lock(&stmutex);
								students[stu].done=1;
								pthread_mutex_unlock(&stmutex);
								int leftSt=0;
								pthread_mutex_lock(&stmutex);
								//printf("I am St %d and names of sts left are ", stu);
								for (int k=0; k<stcount;k++){
									if(!students[k].done){
										//printf("%d ", k);
										leftSt++;
									}								
								}
								pthread_mutex_unlock(&stmutex);
								//printf("\n" );
								pthread_mutex_lock(&endmutex);
								if (!leftSt){
									endsim=1;
									//printf("Ending\n");	
								}
								
								pthread_mutex_unlock(&endmutex);
								//printf("endsim %d\n", endsim );
								//printf("Killing student %d\n",students[stu].name );
								pthread_exit(NULL);

							}
							else{
								printf("Student %d has arrived for his %dth round of Vaccination \n", stu, students[stu].numVac+1 );
								printf("Student %d is waiting to be allocated a slot on a Vaccination Zone\n", stu );

							}
						}
						else{
							pthread_mutex_lock(&stmutex);
							students[stu].done=1;
							pthread_mutex_unlock(&stmutex);
							printf("Student %d tested positive for antibodies\n",stu );

							int leftSt=0;
							pthread_mutex_lock(&stmutex);
							//printf("I am St %d and names of sts left are ", stu);
							for (int k=0; k<stcount;k++){
								if(!students[k].done){
									//printf("%d ", k);
									leftSt++;
								}								
							}
							pthread_mutex_unlock(&stmutex);
							//printf("\n" );
							pthread_mutex_lock(&endmutex);
							if (!leftSt){
								endsim=1;
								//printf("Ending\n");	
							}
							
							pthread_mutex_unlock(&endmutex);
							//printf("endsim %d\n", endsim );
							//printf("Killing student %d\n",students[stu].name );
							pthread_exit(NULL);

						}
					}
					else{
						pthread_mutex_unlock(&vaccs[i].mutex);
					}
			
			}

		}
		//if (endsim){
		//	printf("Ending sim\n" );
		//}
	}
}

void *stFunc(){
	//sleep(2);
	//printf("%d\n",stcount );
	pthread_mutex_lock(&stmutex);
	int i = stcount;
	stcount++;
	students[i].name=i;
	students[i].numVac=0;
	students[i].done=0;
	pthread_mutex_unlock(&stmutex);
	int delay = rand()%4;
	sleep(delay);
	//printf("delay %d\n", delay);
	printf("Student %d has arrived at college\n", students[i].name);
	while(students[i].done==0 && endsim==0){
		stvaccinate(i);
	}
	pthread_exit(NULL);
	//int delay = 100;
	//sleep(delay);
	//printf("Student %d slept through delay %d\n",stu.name, delay );
}

void vacProd(int pharm){

	int prepTime=rand()%4 + 2;
	
	int batchNo=rand()%5 +1;
	int batchSize = rand()%10 +10;
	printf("Pharmaceutical Company %d is preparing %d batches of vaccines which have success probability %f\n",  comps[pharm].name, batchNo, comps[pharm].x);
	sleep(prepTime);
	if(endsim==1){
		pthread_exit(NULL);
	}
	printf("Pharmaceutical Company %d has prepared %d batches of vaccines which have success probability %f. Waiting for all the vaccines to be used to resume production\n",  comps[pharm].name, batchNo, comps[pharm].x);
	sleep(prepTime);
	while(batchNo>0 && endsim==0){
		for (int i=0; i<vaccount && batchNo>0 && endsim==0; i++){
			if (vaccs[i].store<=0){
				if(pthread_mutex_trylock(&vaccs[i].mutex)==0){// && vaccs[i].store<=0){
					vaccs[i].store=batchSize;
					vaccs[i].x=comps[pharm].x;
					batchNo--;
					pthread_mutex_unlock(&vaccs[i].mutex);
					printf("Pharmaceutical Company %d is delivering a vaccine batch to Vaccination Zone %d which has success probability %f\n", pharm, vaccs[i].name,  comps[pharm].x);
					printf("Pharmaceutical Company %d has delivered vaccines to Vaccination Zone %d, resuming vaccinations now\n", pharm, vaccs[i].name);
				}	
			}
			//else{
				//printf("Vacc %d  has %d vaccines\n", i, vaccs[i].store);
			//}
			if(endsim==1){
				//printf("Killing Pharm %d\n", pharm);
				pthread_exit(NULL);
			}
		}
	}
}

void *compFunc(){
	pthread_mutex_lock(&pharmutex);
	int i=pharcount;
	pharcount++;
	comps[i].name=i;
	pthread_mutex_unlock(&pharmutex);

	/*int prepTime=rand()%4 + 2;
	sleep(prepTime);
	int batchNo=rand()%5 +1;
	int batchSize = rand()%10 +10;
	printf("Pharm %d has prepped %d batches with %d samples each\n", comps[i].name, batchNo, batchSize);
	*///replace above part with func
	while(!endsim){
		if(endsim==1){
				//printf("Killing Pharm %d\n", pharm);
				pthread_exit(NULL);
		}
		//printf("I am Pharm %d and endsim is %d\n",i, endsim );
		vacProd(i);
		if(endsim==1){
				//printf("Killing Pharm %d\n", i);
				pthread_exit(NULL);
		}
		printf("All the vaccines prepared by Pharmaceutical Company %d are emptied. Resuming production now.\n",i);
		
		
	}
	pthread_exit(NULL);
}

void prepVac(int vacc){
	while(endsim==0){
		if(vaccs[vacc].store>0 && vaccs[vacc].slots==0){
			pthread_mutex_lock(&vaccs[vacc].mutex);
			vaccs[vacc].slots=(rand()%(min(8, vaccs[vacc].store)))+1;
			vaccs[vacc].store-=vaccs[vacc].slots;
			pthread_mutex_unlock(&vaccs[vacc].mutex);
			printf("Vaccination Zone %d is ready to vaccinate with %d slots\n", vacc,vaccs[vacc].slots );
		}
		else if(vaccs[vacc].store==0){
			printf("Vaccination Zone %d has run out of vaccines\n", vacc);
			while(vaccs[vacc].store==0 && endsim==0){}
		}
	}	
}

void *vacFunc(){
	pthread_mutex_lock(&vacmutex);
	int i = vaccount;
	vaccount++;
	pthread_mutex_init(&stmutex, NULL);
	pthread_mutex_lock(&vaccs[i].mutex);
	vaccs[i].name=i;
	vaccs[i].slots=0;
	vaccs[i].store=0;
	pthread_mutex_unlock(&vaccs[i].mutex);

	pthread_mutex_unlock(&vacmutex);

	printf("Vaccination Zone %d ready\n", vaccs[i].name );
	
	prepVac(i);
	
	pthread_exit(NULL);
}



int main(){
	pthread_t stid[200];
	pthread_t vacid[100];
	pthread_t compid[100];
	//pthread_t checkExit;
	srand(time(0));
	pthread_mutex_init(&stmutex, NULL);
	pthread_mutex_init(&pharmutex, NULL);
	pthread_mutex_init(&vacmutex, NULL);
	pthread_mutex_unlock(&stmutex);
	pthread_mutex_unlock(&pharmutex);
	pthread_mutex_unlock(&vacmutex); 
	int vaclen, stlen, comlen;
	scanf("%d %d %d", &comlen, &vaclen, &stlen);
	if(comlen<=0){
		printf("Error: No Pharmaceutical Companies\n");
		return 0;
	}
	if(vaclen<=0){
		printf("Error: No Vaccination Zones\n");
		return 0;
	}
	if(stlen<=0){
		printf("No Students\n");
		printf("Simulation Over\n");
		return 0;
	}

	for (int m=0;m<comlen;m++){
		scanf("%f", &comps[m].x);
	}

	for (int j=0; j<comlen; j++){
		pthread_create(&compid[j], NULL, compFunc, NULL);	
	}
	for (int i=0; i<stlen; i++){		
		pthread_create(&stid[i], NULL, stFunc, NULL);
	}
	for (int k=0; k<vaclen;k++){
		pthread_create(&vacid[k], NULL, vacFunc, NULL);
	}

	for (int i=0; i<stlen; i++){
		pthread_join(stid[i], NULL); 
	}
	/*if (endsim==1){
		for (int i=0; i<5; i++){
			pthread_kill(compid[i], 9);
		}		
	}*/
	for (int i=0; i<vaclen; i++){
		pthread_join(vacid[i], NULL); 
	}
	for (int i=0; i<comlen; i++){
		pthread_join(compid[i], NULL); 
	}
	printf("Simulation Over\n");
	return 0;
}