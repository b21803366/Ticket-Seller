#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum seatClasses{business, economy, standard};
enum priorities{veteran, diplomat,none};

typedef struct Passengers
{
	char* name;
	int wantedClass;
	int soldClass;
	int priority;
	int sold;
}Passenger;

typedef struct Flights
{
	char* name;
	int seatQuota[3];
	Passenger* queues[4];
	int passengerCount[4];
	int closed;
	
}Flight;

int seatNameToIndex(char* seatName);  //seat adýný enum karsiliðina cevirir
int priorityToIndex(char* priority);  //priority adýný enum karsiliðina cevirir
char* enumToSeatNames(enum seatClasses seat);  //seat enumunu string haline getirir
char* enumToPriority(enum priorities priority);  //priority enumunu string haline getirir
void addseat(Flight** flightsP,int seatCount,enum seatClasses seatClass,char* flightName,int* flightCountP,FILE* output);  //ucus ya da koltuk ekler
void enqueue(Flight** flightsP,char* priority,char* passengerName,enum seatClasses seatClass,char* flightName,int flightCount,FILE* output);  //istenilen kuyruga yolcu ekler
void sell(Flight** flightsP,int flightCount,char* flightNameA,FILE* output);  //kuyrukta bekleyen yolculara uygun bilet satýlýr
void close(Flight** flightsP,int flightCount,char* flightName,FILE* output);  //ucus bilet satimina kapanýr
void report(Flight* flights,int flightCount,char* flightName,FILE* output);  //istenilen ucusu raporlar
void info(Flight* flights,int flightCount,char* passengerName,FILE* output);  //istenilen yolcunun bilgilerini verir

int main(int argc,char *argv[])
{
	int i,j,k;
	if(argc!=3)
	{
		printf("Not enough arguments!\n");
		return;
	}
	
	printf("argc:%d\n",argc);
	
	int lineLen=0,maxLineLen=0;
	char ch;
	char* line;
	char* word;
	line = (char*)malloc((lineLen+20)*sizeof(char));
	maxLineLen=20;
	
	FILE* input;
	input = fopen(argv[1],"r");
	
	FILE* output;
	output = fopen(argv[2],"w");
	
	Flight* flights;
	flights = (Flight*)malloc(1*sizeof(Flight));
	int flightCount=0;
	
	while(1)
	{
		ch = fgetc(input);
		if(ch==EOF || ch=='\n')
		{
			word = strtok(line," ");
			while(word != NULL)
			{
				if(strcmp(word,"addseat")==0)
				{
					addseat(&flights,atoi(strtok(NULL, " ")),seatNameToIndex(strtok(NULL, " ")),strtok(NULL, " "),&flightCount,output);
				}
				else if(strcmp(word,"enqueue")==0)
				{
					enqueue(&flights,strtok(NULL, " "),strtok(NULL, " "),seatNameToIndex(strtok(NULL, " ")),strtok(NULL, " "),flightCount,output);
				}
				else if(strcmp(word,"sell")==0)
				{
					sell(&flights,flightCount,strtok(NULL, " "),output);
				}
				else if(strcmp(word,"close")==0)
				{
					close(&flights,flightCount,strtok(NULL, " "),output);
				}
				else if(strcmp(word,"report")==0)
				{
					report(flights,flightCount,strtok(NULL, " "),output);
				}
				else if(strcmp(word,"info")==0)
				{
					info(flights,flightCount,strtok(NULL, " "),output);
				}
				word = strtok(NULL, " ");
			}
			line[0]='\0';
			if(ch==EOF)
				break;	
			lineLen=0;
		}
		else
		{
			lineLen++;
			if(lineLen>=maxLineLen)
			{
				line = (char*)realloc(line,(lineLen+20)*sizeof(*line));
				maxLineLen=lineLen+20;
			}
			
			line[lineLen-1]=ch;
			line[lineLen]='\0';
		}
	}
	free(line);
	fclose(input);
	fclose(output);
	
	for(i=0;i<flightCount;i++)
	{
		for(j=0;j<4;j++)
		{
			for(k=0;k<flights[i].passengerCount[j];k++)
			{
				free(flights[i].queues[j][k].name);
			}
			free(flights[i].queues[j]);
		}
		free(flights[i].name);
	}
	free(flights);
	
	printf("Returned 0\n");
	return 0;
}

int seatNameToIndex(char* seatName)
{
	if(strcmp(seatName,"business")==0)
		return 0;
	else if(strcmp(seatName,"economy")==0)
		return 1;
	else if(strcmp(seatName,"standard")==0)
		return 2;
	else
		return -1;
}

int priorityToIndex(char* priority)
{
	if(priority==NULL)
		return 2;
	if(priority[0] == 'v')
		return 0;
	else if(priority[0] == 'd')
		return 1;
	else if(strlen(priority)>1)
		return -1;
	else
		return 2;
}

char* enumToSeatNames(enum seatClasses seat)
{
	int i;
	char *names[]={"business", "economy", "standard"};
	for(i=0;i<3;i++)
	{
		if((int)seat==i)
			return names[i];
	}
}

char* enumToPriority(enum priorities priority)
{
	int i;
	char *names[]={"veteran","diplomat","none"};
	for(i=0;i<3;i++)
	{
		if((int)priority==i)
			return names[i];
	}
}

void addseat(Flight** flightsP,int seatCount,enum seatClasses seatClass,char* flightName,int* flightCountP,FILE* output)
{
	int i,index;
	if((*flightCountP)==0)
	{
		index = 0;
		(*flightCountP) = 1;
		
		(*flightsP)[0].name = (char*)malloc((strlen(flightName)+1)*sizeof(char)); 
		strcpy((*flightsP)[0].name,flightName);
		(*flightsP)[0].name[strlen(flightName)] = '\0';
		
		for(i=0;i<3;i++)
		{
			if(i==(int)seatClass)
				(*flightsP)[0].seatQuota[i] = seatCount;
				
			else
				(*flightsP)[0].seatQuota[i] = 0;
		}
		
		(*flightsP)[0].queues[(int)seatClass] = (Passenger*)malloc((seatCount)*sizeof(Passenger));
		(*flightsP)[0].queues[3] = (Passenger*)malloc((seatCount)*10*sizeof(Passenger));
		(*flightsP)[0].closed = 0;
		
		for(i=0;i<4;i++)
			(*flightsP)[0].passengerCount[i] = 0;
	}
	else
	{
		index = -1;
		for(i=0;i<(*flightCountP);i++)
		{
			if(strcmp((*flightsP)[i].name,flightName)==0)
			{
				index = i;
				break;
			}
		}
		if(index==-1)
		{
			(*flightCountP) = (*flightCountP) + 1;
			(*flightsP) = (Flight*)realloc((*flightsP),((*flightCountP)*sizeof(Flight)));
			
			(*flightsP)[(*flightCountP) - 1].name = (char*)malloc((strlen(flightName)+1)*sizeof(char));
			strcpy((*flightsP)[(*flightCountP) - 1].name,flightName);
			(*flightsP)[(*flightCountP) - 1].name[strlen(flightName)] = '\0';
			
			for(i=0;i<3;i++)
			{
				if(i==(int)seatClass)
					(*flightsP)[(*flightCountP) - 1].seatQuota[i] = seatCount;
					
				else
					(*flightsP)[(*flightCountP) - 1].seatQuota[i] = 0;
			}
			
			index = (*flightCountP) - 1;
			
			(*flightsP)[(*flightCountP) - 1].queues[(int)seatClass] = (Passenger*)malloc((seatCount)*sizeof(Passenger));
			(*flightsP)[(*flightCountP) - 1].queues[3] = (Passenger*)malloc((seatCount)*10*sizeof(Passenger));
			(*flightsP)[(*flightCountP) - 1].closed = 0;
			
			for(i=0;i<4;i++)
				(*flightsP)[(*flightCountP) - 1].passengerCount[i] = 0;
		}
		else
		{
			if((*flightsP)[index].seatQuota[(int)seatClass] == 0)
			{
				(*flightsP)[index].seatQuota[(int)seatClass] = seatCount;
				(*flightsP)[index].queues[(int)seatClass] = (Passenger*)malloc((seatCount)*sizeof(Passenger));
			}
			else
			{
				(*flightsP)[index].seatQuota[(int)seatClass] = (*flightsP)[index].seatQuota[(int)seatClass] + seatCount;
				(*flightsP)[index].queues[(int)seatClass] = (Passenger*)realloc((*flightsP)[index].queues[(int)seatClass],((*flightsP)[index].seatQuota[(int)seatClass])*sizeof(Passenger));
			}
		}	
	}
	
	fprintf(output,"addseats ");
	fprintf(output,"%s",flightName);
	fprintf(output," ");
	fprintf(output,"%d",(*flightsP)[index].seatQuota[0]);
	fprintf(output," ");
	fprintf(output,"%d",(*flightsP)[index].seatQuota[1]);
	fprintf(output," ");
	fprintf(output,"%d",(*flightsP)[index].seatQuota[2]);
	fprintf(output,"\n");
}

void enqueue(Flight** flightsP,char* priorityS,char* passengerNameA,enum seatClasses seatClass,char* flightName,int flightCount,FILE* output)
{
	enum priorities priority = priorityToIndex(priorityS);
	int i,index=-1,index2=-1,counter;
	
	for(i=0;i<strlen(passengerNameA);i++)
	{
		if(((int)passengerNameA[i] >= 65 && (int)passengerNameA[i] <= 122) || ((int)passengerNameA[i] >= 48 && (int)passengerNameA[i] <= 57))
		{
			counter++;
		}
	}
	char* passengerName = (char*)malloc(counter*sizeof(char));
	counter=0;
	for(i=0;i<strlen(passengerNameA);i++)
	{
		if((int)passengerNameA[i] >= 65 && (int)passengerNameA[i] <= 122 || ((int)passengerNameA[i] >= 48 && (int)passengerNameA[i] <= 57))
		{
			passengerName[counter]=passengerNameA[i];
			counter++;
		}
	}
	
	if((int)priority == -1)
	{
		fprintf(output,"error\n");
		return;
	}
	for(i=0;i<flightCount;i++)
	{
		if(strcmp((*flightsP)[i].name,flightName)==0)
		{
			index=i;
			break;
		}
	}
	if(((int)seatClass == 0) && ((int)priority == 0))
	{
		fprintf(output,"error\n");
		return;
	}
	if(((int)seatClass == 1) && ((int)priority == 1))
	{
		fprintf(output,"error\n");
		return;
	}
	if((((int)seatClass == 2) && ((int)priority == 0)) || (((int)seatClass == 2) && ((int)priority == 1)))
	{
		fprintf(output,"error\n");
		return;
	}
	if(index==-1)
	{
		fprintf(output,"error\n");
		return;
	}
	if((*flightsP)[index].closed == 1)
	{
		fprintf(output,"error\n");
		return;
	}
		
	if((*flightsP)[index].seatQuota[(int)seatClass]>(*flightsP)[index].passengerCount[(int)seatClass])
	{
		(*flightsP)[index].queues[(int)seatClass][(*flightsP)[index].passengerCount[(int)seatClass]].name = (char*)malloc((strlen(passengerName)+1)*sizeof(char));
		strcpy((*flightsP)[index].queues[(int)seatClass][(*flightsP)[index].passengerCount[(int)seatClass]].name,passengerName);
		(*flightsP)[index].queues[(int)seatClass][(*flightsP)[index].passengerCount[(int)seatClass]].name[strlen(passengerName)] = '\0';
		
		(*flightsP)[index].queues[(int)seatClass][(*flightsP)[index].passengerCount[(int)seatClass]].wantedClass = (int)seatClass;
		(*flightsP)[index].queues[(int)seatClass][(*flightsP)[index].passengerCount[(int)seatClass]].soldClass = (int)seatClass;
		(*flightsP)[index].queues[(int)seatClass][(*flightsP)[index].passengerCount[(int)seatClass]].priority = (int)priority;
		(*flightsP)[index].queues[(int)seatClass][(*flightsP)[index].passengerCount[(int)seatClass]].sold = 0;
		(*flightsP)[index].passengerCount[(int)seatClass] = (*flightsP)[index].passengerCount[(int)seatClass] + 1;
	}
	else
	{
			if((int)seatClass == 0 && (int)priority == 1)
			{
				index2 = -1;
				for(i=((*flightsP)[index].seatQuota[0]-1);i>=0;i--)
				{
					if(((*flightsP)[index].queues[0][i].priority == 2 || (*flightsP)[index].queues[0][i].priority == 0) && ((*flightsP)[index].queues[0][i].sold == 0))
					{
						index2 = i;
						break;
					}
				}
				if(index2 == -1)
				{
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].name = (char*)malloc((strlen(passengerName)+1)*sizeof(char));
					strcpy((*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].name,passengerName);
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].name[strlen(passengerName)] = '\0';
					
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].wantedClass = (int)seatClass;
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].soldClass = 3;
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].priority = (int)priority;
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].sold = 0;
					
					(*flightsP)[index].passengerCount[3] = (*flightsP)[index].passengerCount[3] + 1;
				}
				else
				{
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].name = (char*)malloc((strlen((*flightsP)[index].queues[0][index2].name)+1)*sizeof(char));
					strcpy((*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].name,(*flightsP)[index].queues[0][index2].name);
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].name[strlen((*flightsP)[index].queues[0][index2].name)] = '\0';
					
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].wantedClass = (*flightsP)[index].queues[0][index2].wantedClass;
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].soldClass = 3;
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].priority = (*flightsP)[index].queues[0][index2].priority;
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].sold = 0;
					
					(*flightsP)[index].passengerCount[3] = (*flightsP)[index].passengerCount[3] + 1;
					
					if(strlen((*flightsP)[index].queues[0][index2].name) < strlen(passengerName))
					{
						(*flightsP)[index].queues[0][index2].name = (char*)realloc((*flightsP)[index].queues[0][index2].name,(strlen(passengerName)+1)*sizeof(char));
					}
					strcpy((*flightsP)[index].queues[0][index2].name,passengerName);
					(*flightsP)[index].queues[0][index2].name[strlen(passengerName)] = '\0';
					
					(*flightsP)[index].queues[0][index2].wantedClass = 0;
					(*flightsP)[index].queues[0][index2].soldClass = 0;
					(*flightsP)[index].queues[0][index2].priority = 1;
				}	
			}
			else if((int)seatClass == 1 && (int)priority == 0)
			{
				index2 = -1;
				for(i=((*flightsP)[index].seatQuota[1]-1);i>=0;i--)
				{
					if(((*flightsP)[index].queues[1][i].priority == 1 || (*flightsP)[index].queues[1][i].priority == 2) && ((*flightsP)[index].queues[1][i].sold == 0))
					{
						index2 = i;
						break;
					}
				}
				if(index2 == -1)
				{
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].name = (char*)malloc((strlen(passengerName)+1)*sizeof(char));
					strcpy((*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].name,passengerName);
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].name[strlen(passengerName)] = '\0';
					
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].wantedClass = (int)seatClass;
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].soldClass = 3;
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].priority = (int)priority;
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].sold = 0;
					
					(*flightsP)[index].passengerCount[3] = (*flightsP)[index].passengerCount[3] + 1;
				}
				else
				{
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].name = (char*)malloc((strlen((*flightsP)[index].queues[1][index2].name)+1)*sizeof(char));
					strcpy((*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].name,(*flightsP)[index].queues[1][index2].name);
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].name[strlen((*flightsP)[index].queues[1][index2].name)] = '\0';
					
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].wantedClass = (*flightsP)[index].queues[1][index2].wantedClass;
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].soldClass = 3;
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].priority = (*flightsP)[index].queues[1][index2].priority;
					(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].sold = 0;
					
					(*flightsP)[index].passengerCount[3] = (*flightsP)[index].passengerCount[3] + 1;
					
					if(strlen((*flightsP)[index].queues[1][index2].name) < strlen(passengerName))
					{
						(*flightsP)[index].queues[1][index2].name = (char*)realloc((*flightsP)[index].queues[1][index2].name,(strlen(passengerName)+1)*sizeof(char));
					}
					strcpy((*flightsP)[index].queues[1][index2].name,passengerName);
					(*flightsP)[index].queues[1][index2].name[strlen(passengerName)] = '\0';
					
					(*flightsP)[index].queues[1][index2].wantedClass = 1;
					(*flightsP)[index].queues[1][index2].soldClass = 1;
					(*flightsP)[index].queues[1][index2].priority = 0;
				}
				
			}
			else
			{
				(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].name = (char*)malloc((strlen(passengerName)+1)*sizeof(char));
				strcpy((*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].name,passengerName);
				(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].name[strlen(passengerName)] = '\0';
				
				(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].wantedClass = (int)seatClass;
				(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].soldClass = 3;
				(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].priority = (int)priority;
				(*flightsP)[index].queues[3][(*flightsP)[index].passengerCount[3]].sold = 0;
				
				(*flightsP)[index].passengerCount[3] = (*flightsP)[index].passengerCount[3] + 1;
			}
	}	
	
	fprintf(output,"queue ");
	fprintf(output,"%s",flightName);
	fprintf(output," ");
	fprintf(output,"%s",passengerName);
	fprintf(output," ");
	fprintf(output,"%s",enumToSeatNames(seatClass));
	fprintf(output," ");
	
	counter = (*flightsP)[index].passengerCount[(int)seatClass];
	for(i=0;i<(*flightsP)[index].passengerCount[3];i++)
	{
		if((*flightsP)[index].queues[3][i].wantedClass == seatClass)
			counter++;
	}
	fprintf(output,"%d",counter);
	fprintf(output,"\n");
}

void sell(Flight** flightsP,int flightCount,char* flightNameA,FILE* output)
{
	int i,j,k,index=-1,index2,counter=0;
	
	for(i=0;i<strlen(flightNameA);i++)
	{
		if(((int)flightNameA[i] >= 65 && (int)flightNameA[i] <= 122) || ((int)flightNameA[i] >= 48 && (int)flightNameA[i] <= 57))
		{
			counter++;
		}
	}
	char* flightName = (char*)malloc(counter*sizeof(char));
	counter=0;
	for(i=0;i<strlen(flightNameA);i++)
	{
		if((int)flightNameA[i] >= 65 && (int)flightNameA[i] <= 122 || ((int)flightNameA[i] >= 48 && (int)flightNameA[i] <= 57))
		{
			flightName[counter]=flightNameA[i];
			counter++;
		}
	}
	
	for(i=0;i<flightCount;i++)
	{
		if(strcmp((*flightsP)[i].name,flightName)==0)
		{
			index = i;
			break;
		}
	}
	
	if(index==-1)
	{
		fprintf(output,"error\n");
		return;
	}
	if((*flightsP)[index].closed == 1)
	{
		fprintf(output,"error\n");
		return;
	}
	
	if((*flightsP)[index].seatQuota[2]>(*flightsP)[index].passengerCount[2])
	{
		for(i=(*flightsP)[index].passengerCount[2];i<(*flightsP)[index].seatQuota[2];i++)
		{
			for(j=0;j<((*flightsP)[index].passengerCount[3]);j++)
			{
				if(((*flightsP)[index].queues[3][j].wantedClass) == 2)
				{
					(*flightsP)[index].queues[2][i].name = (char*)malloc((strlen((*flightsP)[index].queues[3][j].name) + 1) * sizeof(char));
					strcpy((*flightsP)[index].queues[2][i].name,(*flightsP)[index].queues[3][j].name);
					(*flightsP)[index].queues[2][i].name[strlen((*flightsP)[index].queues[3][j].name)] = '\0';
					
					(*flightsP)[index].queues[2][i].wantedClass = 2;
					(*flightsP)[index].queues[2][i].soldClass = 2;
					(*flightsP)[index].queues[2][i].priority = (*flightsP)[index].queues[3][j].priority;
					
					(*flightsP)[index].passengerCount[2] = (*flightsP)[index].passengerCount[2] + 1;
					
					for(k=j;k<((*flightsP)[index].passengerCount[3] - 1);k++)
					{
						if(strlen((*flightsP)[index].queues[3][k].name) < strlen((*flightsP)[index].queues[3][k+1].name))
							(*flightsP)[index].queues[3][k].name = (char*)realloc((*flightsP)[index].queues[3][k].name,((strlen((*flightsP)[index].queues[3][k+1].name)) + 1)*sizeof(char));
						strcpy((*flightsP)[index].queues[3][k].name,(*flightsP)[index].queues[3][k+1].name);
						(*flightsP)[index].queues[3][k].name[strlen((*flightsP)[index].queues[3][k+1].name)] = '\0';
						
						(*flightsP)[index].queues[3][k].wantedClass = (*flightsP)[index].queues[3][k+1].wantedClass;
						(*flightsP)[index].queues[3][k].priority = (*flightsP)[index].queues[3][k+1].priority;
					}
					
					(*flightsP)[index].passengerCount[3] = (*flightsP)[index].passengerCount[3] - 1;
					break;
				}
			}
		}
	}
	if((*flightsP)[index].seatQuota[2]>(*flightsP)[index].passengerCount[2])
	{
		for(i=(*flightsP)[index].passengerCount[2];i<(*flightsP)[index].seatQuota[2];i++)
		{
			for(j=0;j<((*flightsP)[index].passengerCount[3]);j++)
			{
				if(((*flightsP)[index].queues[3][j].wantedClass) == 0)
				{
					(*flightsP)[index].queues[2][i].name = (char*)malloc((strlen((*flightsP)[index].queues[3][j].name) + 1) * sizeof(char));
					strcpy((*flightsP)[index].queues[2][i].name,(*flightsP)[index].queues[3][j].name);
					(*flightsP)[index].queues[2][i].name[strlen((*flightsP)[index].queues[3][j].name)] = '\0';
					
					(*flightsP)[index].queues[2][i].wantedClass = 0;
					(*flightsP)[index].queues[2][i].soldClass = 2;
					(*flightsP)[index].queues[2][i].priority = (*flightsP)[index].queues[3][j].priority;
					
					(*flightsP)[index].passengerCount[2] = (*flightsP)[index].passengerCount[2] + 1;
					
					for(k=j;k<((*flightsP)[index].passengerCount[3] - 1);k++)
					{
						if(strlen((*flightsP)[index].queues[3][k].name) < strlen((*flightsP)[index].queues[3][k+1].name))
							(*flightsP)[index].queues[3][k].name = (char*)realloc((*flightsP)[index].queues[3][k].name,((strlen((*flightsP)[index].queues[3][k+1].name)) + 1)*sizeof(char));
						strcpy((*flightsP)[index].queues[3][k].name,(*flightsP)[index].queues[3][k+1].name);
						(*flightsP)[index].queues[3][k].name[strlen((*flightsP)[index].queues[3][k+1].name)] = '\0';
						
						(*flightsP)[index].queues[3][k].wantedClass = (*flightsP)[index].queues[3][k+1].wantedClass;
						(*flightsP)[index].queues[3][k].priority = (*flightsP)[index].queues[3][k+1].priority;
					}
					
					(*flightsP)[index].passengerCount[3] = (*flightsP)[index].passengerCount[3] - 1;
					break;
				}
			}
		}
	}
	if((*flightsP)[index].seatQuota[2]>(*flightsP)[index].passengerCount[2])
	{
		for(i=(*flightsP)[index].passengerCount[2];i<(*flightsP)[index].seatQuota[2];i++)
		{
			for(j=0;j<((*flightsP)[index].passengerCount[3]);j++)
			{
				if(((*flightsP)[index].queues[3][j].wantedClass) == 1)
				{
					(*flightsP)[index].queues[2][i].name = (char*)malloc((strlen((*flightsP)[index].queues[3][j].name) + 1) * sizeof(char));
					strcpy((*flightsP)[index].queues[2][i].name,(*flightsP)[index].queues[3][j].name);
					(*flightsP)[index].queues[2][i].name[strlen((*flightsP)[index].queues[3][j].name)] = '\0';
					
					(*flightsP)[index].queues[2][i].wantedClass = 1;
					(*flightsP)[index].queues[2][i].soldClass = 2;
					
					(*flightsP)[index].passengerCount[2] = (*flightsP)[index].passengerCount[2] + 1;
					
					for(k=j;k<((*flightsP)[index].passengerCount[3] - 1);k++)
					{
						if(strlen((*flightsP)[index].queues[3][k].name) < strlen((*flightsP)[index].queues[3][k+1].name))
							(*flightsP)[index].queues[3][k].name = (char*)realloc((*flightsP)[index].queues[3][k].name,((strlen((*flightsP)[index].queues[3][k+1].name)) + 1)*sizeof(char));
						strcpy((*flightsP)[index].queues[3][k].name,(*flightsP)[index].queues[3][k+1].name);
						(*flightsP)[index].queues[3][k].name[strlen((*flightsP)[index].queues[3][k+1].name)] = '\0';
						
						(*flightsP)[index].queues[3][k].wantedClass = (*flightsP)[index].queues[3][k+1].wantedClass;
						(*flightsP)[index].queues[3][k].priority = (*flightsP)[index].queues[3][k+1].priority;
					}
					
					(*flightsP)[index].passengerCount[3] = (*flightsP)[index].passengerCount[3] - 1;
					break;
				}
			}
		}
	}
	for(i=0;i<3;i++)
	{
		for(j=0;j<(*flightsP)[index].passengerCount[i];j++)
		{
			(*flightsP)[index].queues[i][j].sold = 1;
		}
	}
	
	fprintf(output,"sold ");
	fprintf(output,"%s",flightName);
	fprintf(output," ");
	fprintf(output,"%d",(*flightsP)[index].passengerCount[0]);
	fprintf(output," ");
	fprintf(output,"%d",(*flightsP)[index].passengerCount[1]);
	fprintf(output," ");
	fprintf(output,"%d",(*flightsP)[index].passengerCount[2]);
	fprintf(output,"\n");
}

void close(Flight** flightsP,int flightCount,char* flightNameA,FILE* output)
{
	int i,j,counterSold,counterWaits,index=-1,counter;
	
	for(i=0;i<strlen(flightNameA);i++)
	{
		if(((int)flightNameA[i] >= 65 && (int)flightNameA[i] <= 122) || ((int)flightNameA[i] >= 48 && (int)flightNameA[i] <= 57))
		{
			counter++;
		}
	}
	char* flightName = (char*)malloc(counter*sizeof(char));
	counter=0;
	for(i=0;i<strlen(flightNameA);i++)
	{
		if((int)flightNameA[i] >= 65 && (int)flightNameA[i] <= 122 || ((int)flightNameA[i] >= 48 && (int)flightNameA[i] <= 57))
		{
			flightName[counter]=flightNameA[i];
			counter++;
		}
	}
	
	for(i=0;i<flightCount;i++)
	{
		if(strcmp((*flightsP)[i].name,flightName) == 0)
		{
			index = i;
			break;
		}
	}
	
	(*flightsP)[i].closed = 1;
	
	fprintf(output,"closed ");
	fprintf(output,"%s",flightName);
	counterSold = 0,counterWaits = 0;
	for(i=0;i<3;i++)
	{
		for(j=0;j<(*flightsP)[index].passengerCount[i];j++)
		{
			if((*flightsP)[index].queues[i][j].sold == 1)
				counterSold++;
			else
				counterWaits++;
		}
	}
	counterWaits += (*flightsP)[index].passengerCount[3];
	fprintf(output," ");
	fprintf(output,"%d",counterSold);
	fprintf(output," ");
	fprintf(output,"%d",counterWaits);
	fprintf(output,"\n");
	
	for(j=0;j<(*flightsP)[index].passengerCount[0];j++)
	{
		if(((*flightsP)[index].queues[0][j].sold == 0) && ((*flightsP)[index].queues[0][j].priority == 1) && ((*flightsP)[index].queues[0][j].wantedClass == 0))
		{
			fprintf(output,"waiting ");
			fprintf(output,"%s",(*flightsP)[index].queues[0][j].name);
			fprintf(output,"\n");
		}
	}
	for(j=0;j<(*flightsP)[index].passengerCount[0];j++)
	{
		if(((*flightsP)[index].queues[0][j].sold == 0) && ((*flightsP)[index].queues[0][j].priority == 2) && ((*flightsP)[index].queues[0][j].wantedClass == 0))
		{
			fprintf(output,"waiting ");
			fprintf(output,"%s",(*flightsP)[index].queues[0][j].name);
			fprintf(output,"\n");
		}
	}
	for(j=0;j<(*flightsP)[index].passengerCount[3];j++)
	{
		if(((*flightsP)[index].queues[3][j].sold == 0) && ((*flightsP)[index].queues[3][j].priority == 2) && ((*flightsP)[index].queues[3][j].wantedClass == 0))
		{
			fprintf(output,"waiting ");
			fprintf(output,"%s",(*flightsP)[index].queues[3][j].name);
			fprintf(output,"\n");
		}
	}
	for(j=0;j<(*flightsP)[index].passengerCount[1];j++)
	{
		if(((*flightsP)[index].queues[1][j].sold == 0) && ((*flightsP)[index].queues[1][j].priority == 0) && ((*flightsP)[index].queues[1][j].wantedClass == 1))
		{
			fprintf(output,"waiting ");
			fprintf(output,"%s",(*flightsP)[index].queues[1][j].name);
			fprintf(output,"\n");
		}
	}
	for(j=0;j<(*flightsP)[index].passengerCount[1];j++)
	{
		if(((*flightsP)[index].queues[1][j].sold == 0) && ((*flightsP)[index].queues[1][j].priority == 2) && ((*flightsP)[index].queues[1][j].wantedClass == 1))
		{
			fprintf(output,"waiting ");
			fprintf(output,"%s",(*flightsP)[index].queues[1][j].name);
			fprintf(output,"\n");
		}
	}
	for(j=0;j<(*flightsP)[index].passengerCount[3];j++)
	{
		if(((*flightsP)[index].queues[3][j].sold == 0) && ((*flightsP)[index].queues[3][j].priority == 2) && ((*flightsP)[index].queues[3][j].wantedClass == 1))
		{
			fprintf(output,"waiting ");
			fprintf(output,"%s",(*flightsP)[index].queues[3][j].name);
			fprintf(output,"\n");
		}
	}
	for(j=0;j<(*flightsP)[index].passengerCount[2];j++)
	{
		if(((*flightsP)[index].queues[2][j].sold == 0) && ((*flightsP)[index].queues[2][j].priority == 2) && ((*flightsP)[index].queues[2][j].wantedClass == 2))
		{
			fprintf(output,"waiting ");
			fprintf(output,"%s",(*flightsP)[index].queues[2][j].name);
			fprintf(output,"\n");
		}
	}
	for(j=0;j<(*flightsP)[index].passengerCount[3];j++)
	{
		if(((*flightsP)[index].queues[3][j].sold == 0) && ((*flightsP)[index].queues[3][j].priority == 2) && ((*flightsP)[index].queues[3][j].wantedClass == 2))
		{
			fprintf(output,"waiting ");
			fprintf(output,"%s",(*flightsP)[index].queues[3][j].name);
			fprintf(output,"\n");
		}
	}
}

void report(Flight* flights,int flightCount,char* flightNameA,FILE* output)
{
	int i,j,solded,index=-1,counter;
	
	for(i=0;i<strlen(flightNameA);i++)
	{
		if(((int)flightNameA[i] >= 65 && (int)flightNameA[i] <= 122) || ((int)flightNameA[i] >= 48 && (int)flightNameA[i] <= 57))
		{
			counter++;
		}
	}
	char* flightName = (char*)malloc(counter*sizeof(char));
	counter=0;
	for(i=0;i<strlen(flightNameA);i++)
	{
		if((int)flightNameA[i] >= 65 && (int)flightNameA[i] <= 122 || ((int)flightNameA[i] >= 48 && (int)flightNameA[i] <= 57))
		{
			flightName[counter]=flightNameA[i];
			counter++;
		}
	}
	
	for(i=0;i<flightCount;i++)
	{
		if(strcmp(flights[i].name,flightName) == 0)
		{
			index = i;
			break;
		}
	}
	fprintf(output,"report ");
	fprintf(output,"%s",flightName);
	fprintf(output,"\n");
	for(i=0;i<3;i++)
	{
		solded = 0;
		for(j=0;j<(flights[index].passengerCount[i]);j++)
		{
			if(flights[index].queues[i][j].sold == 1)
				solded++;
		}
		fprintf(output,"%s",enumToSeatNames(i));
		fprintf(output," ");
		fprintf(output,"%d",solded);
		fprintf(output,"\n");
		for(j=0;j<(flights[index].passengerCount[i]);j++)
		{
			if(flights[index].queues[i][j].sold == 1 && flights[index].queues[i][j].priority == (-(i-1)))
			{
				fprintf(output,"%s",flights[index].queues[i][j].name);
				fprintf(output,"\n");
			}		
		}
		for(j=0;j<(flights[index].passengerCount[i]);j++)
		{
			if(flights[index].queues[i][j].sold == 1 && flights[index].queues[i][j].priority != (-(i-1)))
			{
				fprintf(output,"%s",flights[index].queues[i][j].name);
				fprintf(output,"\n");
			}		
		}		
	}
	fprintf(output,"end of report ");
	fprintf(output,"%s",flightName);
	fprintf(output,"\n");
}

void info(Flight* flights,int flightCount,char* passengerNameA,FILE* output)
{
	int i,j,k,counter;
	
	for(i=0;i<strlen(passengerNameA);i++)
	{
		if(((int)passengerNameA[i] >= 65 && (int)passengerNameA[i] <= 122) || ((int)passengerNameA[i] >= 48 && (int)passengerNameA[i] <= 57))
		{
			counter++;
		}
	}
	char* passengerName = (char*)malloc(counter*sizeof(char));
	counter=0;
	for(i=0;i<strlen(passengerNameA);i++)
	{
		if((int)passengerNameA[i] >= 65 && (int)passengerNameA[i] <= 122 || ((int)passengerNameA[i] >= 48 && (int)passengerNameA[i] <= 57))
		{
			passengerName[counter]=passengerNameA[i];
			counter++;
		}
	}
	
	for(i=0;i<flightCount;i++)
	{
		for(j=0;j<4;j++)
		{
			for(k=0;k<flights[i].passengerCount[j];k++)
			{
				if(strcmp(flights[i].queues[j][k].name,passengerName) == 0)
				{
					fprintf(output,"info ");
					fprintf(output,"%s",passengerName);
					fprintf(output," ");
					fprintf(output,"%s",flights[i].name);
					fprintf(output," ");
					fprintf(output,"%s",enumToSeatNames(flights[i].queues[j][k].wantedClass));
					fprintf(output," ");
					if(flights[i].queues[j][k].soldClass == 3)
					{
						fprintf(output,"none");
					}
					else
					{
						fprintf(output,"%s",enumToSeatNames(flights[i].queues[j][k].soldClass));
					}
					fprintf(output,"\n");
					return;
				}
			}
		}
	}
	fprintf(output,"error\n");
}
