#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//scheduling algorithms types
#define S_FCFS		0 
#define S_SJF_P		1 //preemptive
#define S_SJF_NP	2 //not-preemptive
#define S_RR		3
#define S_PR_P		4 //preemptive
#define S_PR_NP		5 //not-preemptive

#define S_PREEMPTIVE		1
#define S_NON_PREEMPTIVE	0

#define S_PRIORITY		1
#define S_NON_PRIORITY	0

#define QT_ACTIVE_MULTIPLE	2
#define QT_ACTIVE_SIMPLE	1
#define QT_DEACTIVE			0

//sorting rules
#define ARRIVAL_TIME	0
#define BURST_TIME		1
#define WAITING_TIME	2
#define PROCESSED_TIME	3
#define STARTING_TIME	4
#define END_TIME		5
#define REMAINING_TIME  6
#define PRIORITY		7

//statistic rules
#define GANTT	0
#define REPORT	1

//others
#define YES 1
#define NO 0

#define TRUE 1
#define FALSE 0

#define FREE 0
#define BLANK 0
#define LOADED 1

#define SET 1
#define NOT_SET -1


typedef struct {
	int id;
	int priority;
	int arrivalTime;
	int burstTime;
	int waitingTime;
	int processedTime;
	int startingTime;
	int endTime;
	int isProcessing;
	int isLastProcessed;
} Process;

typedef struct {
	Process * runningProcess;
	int isBusy;
	int quantumeTime;
} CPU;



int clock = 0; //her bir döngü de 1 artar. Bu değerin her bir artışı bir birimlik zamanın geçtiğini gösterir. 
//int quantumTime = 0;
int finishedProcess = 0;


void processSorter(Process ** processArray, int length, int sortingRule, int isReverse){
	Process ** tmp;
	int i,j,min,oldmin; 
	
	tmp = (Process **) malloc (sizeof(Process*) * length);	
	for(i = 0; i < length; i++) tmp[i] = processArray[i];
	
	j = 0;
	oldmin = NOT_SET;

	switch(sortingRule){

	case ARRIVAL_TIME : 
		while(j < length){
			min = NOT_SET;
			for(i = 0; i < length; i++){	//sıradaki min değeri bulur		
				if(oldmin != NOT_SET)
					if(tmp[i]->arrivalTime <= tmp[oldmin]->arrivalTime)
						continue;				
				if(min == NOT_SET) min = i;	//ilk uygun olan sıradaki indis 					
				if(tmp[i]->arrivalTime < tmp[min]->arrivalTime) //min bulma algoritması
					min = i;
			}
			for(i = 0; i < length; i++){
				if(tmp[i]->arrivalTime == tmp[min]->arrivalTime){
					processArray[j] = tmp[i];
					j++;
					if(j >= length) break;
				}
			}
			oldmin = min;
		}
		break;

	case PRIORITY : 
		while(j < length){
			min = NOT_SET;
			for(i = 0; i < length; i++){			
				if(tmp[i]->priority == NOT_SET) continue;//-1 ler atlanmıştır. 
				if(oldmin != NOT_SET)
					if(tmp[i]->priority <= tmp[oldmin]->priority)
						continue;				
				if(min == NOT_SET) min = i;						
				if(tmp[i]->priority < tmp[min]->priority) 
					min = i;
			}
			if(min == NOT_SET)//eğer min set edilememişse geriye priority'si olmayanlar kalmıştır. -1 ler atlandığı için processArray dizisinde boş kalan bölmeler vardır. burada onlara -1 değerli processleri atama yapılmaktadır.
			{
				for(i = 0; i < length; i++){          //dizi içindeki
					if(tmp[i]->priority == NOT_SET){  //priority'si olmayanlar
						processArray[j] = tmp[i];	  //sıranın sonuna yerleştirilir.
						j++;
						if(j >= length) break;
					}
				}
				break;
			}
			for(i = 0; i < length; i++){
				if(tmp[i]->priority == tmp[min]->priority){
					processArray[j] = tmp[i];
					j++;
					if(j >= length) break;
				}
			}
			oldmin = min;
		}
		break;

	case BURST_TIME : 
		while(j < length){
			min = NOT_SET;
			for(i = 0; i < length; i++){			
				if(oldmin != NOT_SET)
					if(tmp[i]->burstTime <= tmp[oldmin]->burstTime)
						continue;				
				if(min == NOT_SET) min = i;						
				if(tmp[i]->burstTime < tmp[min]->burstTime) 
					min = i;
			}
			for(i = 0; i < length; i++){
				if(tmp[i]->burstTime == tmp[min]->burstTime){
					processArray[j] = tmp[i];
					j++;
					if(j >= length) break;
				}
			}
			oldmin = min;
		}
		break;

	case REMAINING_TIME : 
		while(j < length){
			min = NOT_SET;
			for(i = 0; i < length; i++){			
				if(oldmin != NOT_SET)
					if((tmp[i]->burstTime - tmp[i]->processedTime) <= (tmp[oldmin]->burstTime - tmp[oldmin]->processedTime))
						continue;				
				if(min == NOT_SET) min = i;						
				if((tmp[i]->burstTime - tmp[i]->processedTime) < (tmp[min]->burstTime - tmp[min]->processedTime)) 
					min = i;
			}
			for(i = 0; i < length; i++){
				if((tmp[i]->burstTime - tmp[i]->processedTime) == (tmp[min]->burstTime - tmp[min]->processedTime)){
					processArray[j] = tmp[i];
					j++;
					if(j >= length) break;
				}
			}
			oldmin = min;
		}
		break;

	case PROCESSED_TIME : 
		while(j < length){
			min = NOT_SET;
			for(i = 0; i < length; i++){			
				if(oldmin != NOT_SET)
					if(tmp[i]->processedTime <= tmp[oldmin]->processedTime)
						continue;				
				if(min == NOT_SET) min = i;						
				if(tmp[i]->processedTime < tmp[min]->processedTime) 
					min = i;
			}
			for(i = 0; i < length; i++){
				if(tmp[i]->processedTime == tmp[min]->processedTime){
					processArray[j] = tmp[i];
					j++;
					if(j >= length) break;
				}
			}
			oldmin = min;
		}
		break;

	case WAITING_TIME : 
		while(j < length){
			min = NOT_SET;
			for(i = 0; i < length; i++){			
				if(oldmin != NOT_SET)
					if(tmp[i]->waitingTime <= tmp[oldmin]->waitingTime)
						continue;				
				if(min == NOT_SET) min = i;						
				if(tmp[i]->waitingTime < tmp[min]->waitingTime) 
					min = i;
			}
			for(i = 0; i < length; i++){
				if(tmp[i]->waitingTime == tmp[min]->waitingTime){
					processArray[j] = tmp[i];
					j++;
					if(j >= length) break;
				}
			}
			oldmin = min;
		}
		break;
	}

	if(isReverse == YES){
		for(i = 0; i < length; i++) tmp[i] = processArray[i];
		for(i = 0; i < length; i++) processArray[i] = tmp[length - i - 1];
	}

	free(tmp);
}


/**
	allProcesses dizisinden zamanı gelen processleri çekerek activeProcesses dizisini oluşturan fonksiyon.
	Yeniden oluşturulan activeProcesses dizisinin yeni buyutunu return eder.
*/
int schedular(Process ** sourceProcessArray, int sourceLength, Process ** destinationProcessArray, int destinationLenght){
	int i,j;
	//filter(destinationArray) activeProcesses dizisini boşaltmak için çok da önemli değil
	for(i = 0; i < destinationLenght-1; i++)
		destinationProcessArray[i] = 0;

	//getActiveProcesses() sırası gelen processleri almak için
	j = 0;
	for(i = 0; i < sourceLength; i++)
		if((sourceProcessArray[i]->arrivalTime <= clock) && (sourceProcessArray[i]->burstTime > sourceProcessArray[i]->processedTime)){
			destinationProcessArray[j] = sourceProcessArray[i];
			j = j + 1;
		}

	return j;
}

int processManager(Process ** processesInQueue, int queueLength, CPU ** cpuArray, int cpuArrayLength, int isQuantumeTimeActive, int isLastProcessSelectable){
	int ptr;
	int i;
	int isLastOk;

	ptr = 0;
	isLastOk = NO;
	//Ayırma işlemleri

	//Biten process'i ayırma
	for(i = 0; i < cpuArrayLength; i++){
		if(cpuArray[i]->isBusy == YES){
			if(cpuArray[i]->runningProcess->burstTime == cpuArray[i]->runningProcess->processedTime){
				finishedProcess++;
			//process'in ilişkisini CPU ile kesme işlemleri
				cpuArray[i]->runningProcess->endTime = clock;
				cpuArray[i]->runningProcess->isProcessing = NO;
			//	cpuArray[i]->runningProcess->isLastProcessed = YES;
			//CPU'nun ilişkisini process ile kesme işlemleri
				cpuArray[i]->isBusy = NO;
				cpuArray[i]->runningProcess = FREE;
			}
		}
	}
	
	//QuantumTime süresi dolunca ayırma
	for(i = 0; i < cpuArrayLength; i++){
		if(cpuArray[i]->isBusy == YES){
			if(isQuantumeTimeActive){//todo dışa al
				if(clock % cpuArray[i]->quantumeTime == 0){ //mod işlemi sonucu 0 dönüyorsa kesme zamanı gelmiştir
				//process'in ilişkisini CPU ile kesme işlemleri
					cpuArray[i]->runningProcess->isProcessing = NO;
				//	cpuArray[i]->runningProcess->isLastProcessed = YES;
				//CPU'nun ilişkisini process ile kesme işlemleri
					cpuArray[i]->isBusy = NO;
					cpuArray[i]->runningProcess = FREE;
				}
			}
		}
	}

	//Yerleştirme işlemleri
	for(i = 0; i < cpuArrayLength; i++){
		if(cpuArray[i]->isBusy == NO){
			while(ptr < queueLength){
				if(processesInQueue[ptr]->isProcessing == YES){ptr++; continue;} //eğer bakılan process zaten işleniyorsa bir sonrakine geç
				if(isLastProcessSelectable == NO && processesInQueue[ptr]->isLastProcessed == YES){ptr++; continue;} //eğer en son çalışan process ise sonrakine geç
			
				//buraya gelinebildiyse uygun process bulunmuştur
				cpuArray[i]->isBusy = YES;
				cpuArray[i]->runningProcess = processesInQueue[ptr]; //uygun olan process işlemciye atanır
				processesInQueue[ptr]->isProcessing = YES;
				if(processesInQueue[ptr]->startingTime = NOT_SET)
					processesInQueue[ptr]->startingTime = clock;
				processesInQueue[ptr]->isLastProcessed = YES;
				ptr++;
				break;
			}
		}
		if(ptr == queueLength){
			isLastOk = YES;
			break;
		}
	}

	//Boşta işlemci kaldıysa ve işleme girmemiş last process varsa boş işlemcilere atanır 
	if(isLastProcessSelectable == NO){
		ptr = 0;
		for(i = 0; i < cpuArrayLength; i++){
			if(cpuArray[i]->isBusy == NO){
				while(ptr < queueLength){
					if(processesInQueue[ptr]->isProcessing == YES){ptr++; continue;} //eğer bakılan process zaten işleniyorsa bir sonrakine geç
					//buraya gelinebildiyse uygun process bulunmuştur
					cpuArray[i]->isBusy = YES;
					cpuArray[i]->runningProcess = processesInQueue[ptr]; //uygun olan process işlemciye atanır
					processesInQueue[ptr]->isProcessing = YES;
					if(processesInQueue[ptr]->startingTime = NOT_SET)
						processesInQueue[ptr]->startingTime = clock;
					processesInQueue[ptr]->isLastProcessed = NO;
					ptr++;
					break;
				}
			}
			if(ptr == queueLength){
				isLastOk = YES;
				break;
			}
		}
	}

	//Sayaçları arttırma işlemleri
	for(i = 0; i < queueLength; i++){
		if(processesInQueue[i]->isProcessing == YES) processesInQueue[i]->processedTime++;
		if(processesInQueue[i]->isProcessing == NO)  processesInQueue[i]->waitingTime++;
	}

	return isLastOk;
}

void report(Process ** processes, int length, FILE * fp){
	int i;
	int total_burstTime, total_waitingTime;
	float avg_burstTime, avg_waitingTime;

	total_burstTime   = avg_burstTime   = 0;
	total_waitingTime = avg_waitingTime = 0;


	for(i = 0; i < length; i++){
		total_burstTime += processes[i]->burstTime;
		total_waitingTime += processes[i]->waitingTime;
	}

	avg_burstTime = total_burstTime*1.0 / length;
	avg_waitingTime = total_waitingTime*1.0  / length;

	fprintf(fp,"%d,%f,%f\n",length,avg_burstTime,avg_waitingTime);
}

void ganttChart(CPU ** cpus, int length, FILE * fp){
	int i;

	fprintf(fp,"%6d    ", clock);
	for(i = 0; i < length; i++){
		if(cpus[i]->isBusy == YES) fprintf(fp,"P%d\t",cpus[i]->runningProcess->id);
		else fprintf(fp," - \t"); //P-1 hiçbir processin çalışmadığını temsil ediyor.
	}
	fprintf(fp,"\n");
}

int SJF(Process ** allProcesses, int length, CPU ** cpus, int cpuNumber, int isQuantumeTimeActive,  FILE * fp){
	int aLen;
	int i;

	Process ** activeProcesses;

	activeProcesses = (Process **) malloc(sizeof(Process *) * length);
	
	while(finishedProcess < length){
	aLen = schedular(allProcesses, length, activeProcesses, length);
		   processSorter(activeProcesses, aLen, PRIORITY, NO);
		   processSorter(activeProcesses, aLen, BURST_TIME, NO);
		   processManager(activeProcesses, aLen, cpus, cpuNumber, isQuantumeTimeActive, YES);
		   ganttChart(cpus, cpuNumber, fp);
		   clock++;
	}

	free(activeProcesses);

	return clock-1;
}

int PR(Process ** allProcesses, int length, CPU ** cpus, int cpuNumber, int isQuantumeTimeActive,  FILE * fp){
	int aLen;
	int i;

	Process ** activeProcesses;

	activeProcesses = (Process **) malloc(sizeof(Process *) * length);
	
	while(finishedProcess < length){
	aLen = schedular(allProcesses, length, activeProcesses, length);
		   processSorter(activeProcesses, aLen, PRIORITY, NO);
		   processManager(activeProcesses, aLen, cpus, cpuNumber, isQuantumeTimeActive, YES);
		   ganttChart(cpus, cpuNumber, fp);
		   clock++;
	}

	free(activeProcesses);

	return clock-1;
}

int RR(Process ** allProcesses, int length, CPU ** cpus, int cpuNumber, int isPriorityActive,  FILE * fp){
	int aLen;
	int i;
	int ptr;
	int isLast;

	Process ** activeProcesses;

	activeProcesses = (Process **) malloc(sizeof(Process *) * length);
	
	while(finishedProcess < length){
	aLen   = schedular(allProcesses, length, activeProcesses, length);
			 if(isPriorityActive == YES)
				 processSorter(activeProcesses, aLen, PRIORITY, NO);
		     processSorter(activeProcesses, aLen, ARRIVAL_TIME, NO);
	isLast = processManager(activeProcesses, aLen, cpus, cpuNumber, YES, NO);
	         if(isLast == YES) //sona gelinmiş mi diye bakılır
				 for(i = 0; i < aLen; i++)
					 activeProcesses[i]->isLastProcessed = NO;
		     
		     ganttChart(cpus, cpuNumber, fp);
		     clock++;
	}

	free(activeProcesses);

	return clock-1;
}

int FCFS(Process ** allProcesses, int length, CPU ** cpus, int cpuNumber, FILE * fp){
	int aLen;
	int i;

	Process ** activeProcesses;

	activeProcesses = (Process **) malloc(sizeof(Process *) * length);
	
	while(finishedProcess < length){
	aLen = schedular(allProcesses, length, activeProcesses, length);
		   processSorter(activeProcesses, aLen, ARRIVAL_TIME, NO);
		   processManager(activeProcesses, aLen, cpus, cpuNumber, NO, NO);
		   ganttChart(cpus, cpuNumber, fp);
		   clock++;
	}

	free(activeProcesses);

	return clock-1;
}

Process ** parser(char * file, int * length){
	FILE * fp;
	Process ** destProcessArray;
	int arrivalTime;
	int burstTime;
	int priority;
	int cnt = 0;
	int i,j;

	fp = fopen(file,"r");
	while(!feof(fp)){//dosyanın sonuna gelene kadar tüm processleri sayıyor
		if(fscanf(fp,"%d,%d,%d\n",&arrivalTime,&burstTime,&priority)!=-1)
			cnt++;
	}

	destProcessArray = (Process ** )malloc(sizeof(Process *) * cnt);
	for(i = 0; i < cnt	; i++) destProcessArray[i] = (Process *) malloc(sizeof(Process));
	
	j = 0;
	fseek(fp,0,SEEK_SET);//dosyanın başına dönülüyor
	while(!feof(fp)){//dosyanın sonuna gelene kadar tüm processleri yüklüyor
		priority = NOT_SET; //eğer priority yoksa bu değer atanacak.
		if(fscanf(fp,"%d,%d,%d\n*",&arrivalTime,&burstTime,&priority)!=-1){
			destProcessArray[j]->arrivalTime = arrivalTime;
			destProcessArray[j]->burstTime = burstTime;
			destProcessArray[j]->endTime = NOT_SET;
			destProcessArray[j]->id = j;
			destProcessArray[j]->isLastProcessed = NO;
			destProcessArray[j]->isProcessing = NO;
			destProcessArray[j]->priority = priority;
			destProcessArray[j]->processedTime = 0;
			destProcessArray[j]->startingTime = NOT_SET;
			destProcessArray[j]->waitingTime = 0;
		}
		j++;
	}
	fclose(fp);
	
	*length = cnt;
	return destProcessArray;
}

int random(int min, int max){
	return (rand()%(max-min+1)) + min;
}

FILE * randomFile(char * file, int processNumber, int min_arrival, int max_arrival,	int min_burst, int max_burst, int isPriorityActive, int min_priority, int max_priority){
	FILE * fp;
	int i;

	fp = fopen(file,"w+");
	for(i = 0; i < processNumber; i++){
		if(isPriorityActive == YES) fprintf(fp,"%d,%d,%d\n",random(min_arrival, max_arrival), random(min_burst,max_burst), random(min_priority,max_priority));
		if(isPriorityActive == NO) fprintf(fp,"%d,%d\n",random(min_arrival, max_arrival), random(min_burst,max_burst));
	}
	fclose(fp);
	fp = fopen(file,"r");
	return fp;
}

CPU ** createCpus(int cpuNumber, int qtType){
	CPU ** cpus;
	int i;
	int global_qt;

	if(qtType == QT_ACTIVE_SIMPLE){
		printf("\n");
		printf("Genel CPU Quantum Time: ");
		scanf("%d",&global_qt);
	}

	printf("\n");	
	cpus = (CPU **) malloc(sizeof(CPU *) * cpuNumber);
	for(i = 0; i < cpuNumber; i++){
		cpus[i] = (CPU *) malloc(sizeof(CPU));
		cpus[i]->isBusy = NO;
		cpus[i]->runningProcess = (Process *)NOT_SET;

		switch(qtType){
			case QT_DEACTIVE : 
				cpus[i]->quantumeTime = NOT_SET; 
				break;
			case QT_ACTIVE_SIMPLE : 
				cpus[i]->quantumeTime = global_qt; 
				break;
			case QT_ACTIVE_MULTIPLE : 
				printf("CPU%3d Quantum Time: ", i); 
				scanf("%d", &(cpus[i]->quantumeTime)); 
				break;	
			default : 
				cpus[i]->quantumeTime = NOT_SET; 
				break;
		}
	}
	return cpus;
}

void fmenu(int * s, int * q, int * p){
	char input[100];
	int i;

	printf("\n");
    printf("1 - (FCFS) First-Come First-Served\n");
    printf("2 - (SJF) Shortest Job First - Preemptive\n");
    printf("3 - (SJF) Shortest Job First - Not-Preemptive\n");
    printf("4 - (RR) Round-Robin\n");
    printf("5 - (PR) Priority - Preemptive\n");
    printf("6 - (PR) Priority - Not-Preemptive\n");
//	printf("q - Preemptive(QuantumeTime)\n");
//	printf("p - Priority\n");
    printf("\n");
	giristekrari:
	printf("Uygulanacak alagoritmanin numarasini giriniz.\n");
//	printf("Orn : 2pq (SJF with priority with preemptive)\n");
    printf(" >> ");
	scanf("%s",input);

	if(strlen(input) > 1){
		printf("!!Hatali giris!! Max 1 karakter\n"); goto giristekrari;
	}

	i = 0;

	//*p = S_NON_PRIORITY;
	//*q = S_NON_PREEMPTIVE;
	while((i < 1) && (input[i] != 0)){
		switch(input[i]){
			case '1' : *s = S_FCFS; break;
			case '2' : *s = S_SJF_P; break;
			case '3' : *s = S_SJF_NP; break;
			case '4' : *s = S_RR; break;
			case '5' : *s = S_PR_P; break;
			case '6' : *s = S_PR_NP; break;
			//case 'p' : *p = S_PRIORITY; break;
			//case 'q' : *q = S_PREEMPTIVE; break;
			default  : printf("!!Hatali giris!!\n"); goto giristekrari;
		}
		i++;
	}
}

Process ** pmenu(int * length){
	char input[100];
	char inputFile[100];
	int i,j;
	FILE * fp;
	int arrival, burst, priority;
	int min_a,max_a,min_b,max_b,min_p,max_p;

	printf("\n");
    printf("f - Input dosyasi gir\n");
    printf("r - Input dosyasini random olustur\n");
    printf("c - Input degerlerini konsol'dan gir\n");
	giristekrari:
	printf("\n");
	printf("Input tipini giriniz.\n");
	printf(" >> ");
	scanf("%s",input);

	if(strlen(input) != 1){
		printf("!!Hatali giris!! 1 karakter giriniz\n"); goto giristekrari;
	}

	i = 0;
	//*length = 50;
	while((i < 1) && (input[i] != 0)){
		switch(input[i]){
		case 'r' : 
			giristekrari2 :
			min_a = max_a = min_b = max_b = min_p = max_p = NOT_SET;
			printf("\n");
			printf("Random araliklarini giriniz!\n");
			printf("[min_arrival],[max_arrival],\n[min_burst],[max_burst],\n[min_priority]*,[max_priority]* (* istege bagli girdi)\nOrn: 0,12,1,20 veya 0,12,1,20,1,8\n");
			printf(" >> "); scanf("%d,%d,%d,%d,%d,%d", &min_a,&max_a,&min_b,&max_b,&min_p,&max_p); 
			printf("\n");
			if(min_a < 0){ printf("!!Hatali giris!! min_arrival > -1 olmali\n"); goto giristekrari2;}
			if(max_a < 0){ printf("!!Hatali giris!! max_arrival > -1 olmali\n"); goto giristekrari2;}
			if(min_b < 1){ printf("!!Hatali giris!! min_burst > 0 olmali\n"); goto giristekrari2;}
			if(max_b < 1){ printf("!!Hatali giris!! max_burst > 0 olmali\n"); goto giristekrari2;}
			if(min_a > max_a){ printf("!!Hatali giris!! min_arrival > max_arrival olamaz\n"); goto giristekrari2;}
			if(min_b > max_b){ printf("!!Hatali giris!! min_burst > max_burst olamaz\n"); goto giristekrari2;}
			if(min_p > max_p){ printf("!!Hatali giris!! min_priority > max_priority olamaz\n"); goto giristekrari2;}
			printf("\n");
			printf("Random Process sayisini giriniz!\n");
			printf(" >> "); scanf("%d", length); 
			printf("\n");
			randomFile("randominput.txt", *length, min_a,max_a,min_b,max_b, YES, min_p, max_p);
			return parser("randominput.txt", length); 
			break;		
		case 'f' : 
			printf("\n");
			printf("Process dosyasini giriniz. \n");
			printf("not: dosya yapisi [arrival],[burst] veya [arrival],[burst],[priority] seklinde girilmis satirlardan olusmus olmalidir.(1,15 veya 1,15,3 gibi)\n");
			printf("(Orn: processes.txt)");
			printf("\n");
			printf(" >> "); scanf("%s", inputFile); printf("\n");
			return parser(inputFile, length);
			break;
		case 'c' : 
			fp = fopen("consoleOut.txt","w+");
			printf("\n");
			printf("Process sayisini giriniz.\n");
			printf(" >> "); scanf("%d", length); printf("\n");
			printf("Process bilgilerini giriniz. (arrival,burst veya arrival,burst,priority)\nOrn: 1,12 veya 1,12,3\n");
			printf("\n");
			for(j = 0; j < *length; j++){
				printf(" >> "); 
				priority = NOT_SET;
				scanf("%d,%d,%d",&arrival,&burst,&priority);
				fprintf(fp,"%d,%d,%d\n",arrival,burst,priority);
			}
			fclose(fp);
			return parser("consoleOut.txt", length);
			break;
		default  : 
			printf("!!Hatali giris!!\n"); goto giristekrari;
		}
		i++;
	}
}


CPU ** cmenu(int * cpuNumber){
	char input[100];
	int i;
	printf("\n");
	printf("CPU sayisini giriniz : \n");
    printf(" >> ");
	scanf("%d", cpuNumber);
	

	printf("\n");
    printf("a - Quantumetime active(her islemci ayni)\n");
    printf("m - Quantumetime active(her islemci farkli)\n");
	//printf("d - Quantumetime de-active\n");
	giristekrari:
	printf("\n");
	printf("Quantumetime tipini giriniz.\n");
	printf(" >> ");
	scanf("%s",input);

	if(strlen(input) != 1){
		printf("!!Hatali giris!! 1 karakter giriniz\n"); goto giristekrari;
	}

	i = 0;

	while((i < 1) && (input[i] != 0)){
		switch(input[i]){
			case 'a' : return createCpus(*cpuNumber, QT_ACTIVE_SIMPLE); break;
			case 'm' : return createCpus(*cpuNumber, QT_ACTIVE_MULTIPLE); break;
			//case 'd' : return createCpus(*cpuNumber, QT_DEACTIVE); break;
			default  : printf("!!Hatali giris!!\n"); goto giristekrari;
		}
		i++;
	}
}

void resetProcesses(Process ** processes, int length){
	int i;

	for(i = 0; i < length; i++){
		processes[i]->isLastProcessed = NO;
		processes[i]->isProcessing = NO;
		processes[i]->processedTime = 0;
		processes[i]->startingTime = NOT_SET;
		processes[i]->waitingTime = 0;
	}
}


void main(){
	Process ** allProcesses;
	CPU ** cpus;
	int length,i,j,k;
	FILE * out_file;
	FILE * report_file;
	int s,p,q,cn,plen;

	allProcesses = pmenu(&plen);

//test için
//	processSorter(allProcesses,plen,PRIORITY,NO);
//	for(i = 0; i < plen; i++) printf("%d\t%d\t%d\n",allProcesses[i]->arrivalTime,allProcesses[i]->burstTime,allProcesses[i]->priority);

	cpus = cmenu(&cn);
	fmenu(&s, &q, &p);//p ve q artık kullanılmıyor
	
	report_file = fopen("report.csv","w+");
	for(i = 1; i < plen + 1; i++){
		out_file = fopen("statistics.txt","w+");
		switch(s){
		case S_FCFS :
			FCFS(allProcesses, i, cpus, cn, out_file);
			break;
		case S_SJF_P : 
			SJF(allProcesses,i,cpus,cn,YES,out_file);
			break;
		case S_SJF_NP :
			SJF(allProcesses,i,cpus,cn,NO,out_file);
			break;
		case S_RR :
			RR(allProcesses,i,cpus,cn,NO,out_file);
			break;
		case S_PR_P :
			PR(allProcesses,i,cpus,cn,YES,out_file);
			break;
		case S_PR_NP :
			PR(allProcesses,i,cpus,cn,NO,out_file);
			break;
		}
				
		report(allProcesses, i, report_file);
		fclose(out_file);
		
		//reset
		resetProcesses(allProcesses, i);
		clock = 0; 
		finishedProcess = 0;
	}
	fclose(report_file);
	system("pause");
}
