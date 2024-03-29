#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define B 11 //broj baketa
#define b 5 //faktor baketiranja - broj slogova u baketu

typedef struct Syllable
{
	int key; //evidential number
	char prisonersLabel[8];// 8 chars, for \0 at the end
	char arrivalDate[15]; 
	char arrivalTime[10];
	char cellLabel[6];//\0 at the end
	unsigned  SentenceTime;
	bool active;
}Syllable;

typedef struct Bucket
{
	Syllable syllable[b]; // 
}Bucket;

typedef struct Cell
{
	char cellLabel[6];
	unsigned prisonersNum;
	unsigned sentenceTime;
	unsigned minSentenceTime;
}Cell;

FILE *activeFile;
char activeFileName[30];
FILE *zoneFile;
char zoneFileName[35];
FILE *csvFile;
char csvFileName[35];
FILE *cellFile;
char cellFileName[35];

void createNewFile();
void chooseAnExistingFile();
void printNameOfCurrentFile();
void insertNewPrisoner();
bool findExistingPrisoner(int key);
void showAllPrisoners();
void deletePrisoner(int key);
void changeSentenceTime();
void saveFileLikeCsv();
void printAndSaveCellData();


bool insertInBucket(Syllable prisoner);
void printSyllable(Syllable prisoner);
void readCell(Cell **cell, Bucket bucket, int* cellCounter);
void writeCells(int cellCounter,Cell** cells);
void printCells();
bool checkConstraints(Syllable prisoner);

void createNewFile(){
	printf("Input file name\n");
	scanf("%s" ,activeFileName);
	activeFile = fopen(activeFileName,"rb+");
	if(activeFile != NULL){
		printf("Error, file with name %s exist already!\n",activeFileName);
//		fclose(activeFile);
		return;
	}else{
		activeFile = fopen(activeFileName,"wb+");
		strcpy(zoneFileName,activeFileName);
		strcat(zoneFileName,"-zone");
		zoneFile = fopen(zoneFileName,"wb+");
		Bucket buckets[B];
		//TODO: remove this later
		for(int i = 0; i<B; i++){
			for(int j = 0; j<b; j++){
				// buckets[i].syllable[j].key = 11*j + i;
				// strcpy(buckets[i].syllable[j].prisonersLabel,"UNKNOWN");
				// strcpy(buckets[i].syllable[j].arrivalDate,"10/11/2012"); //
				// strcpy(buckets[i].syllable[j].arrivalTime,"10:25");
				// sprintf(buckets[i].syllable[j].cellLabel,"%d",10000+i);
				// buckets[i].syllable[j].SentenceTime = 480;
				buckets[i].syllable[j].active = false;
			}
		}
		fwrite(&buckets,sizeof(Bucket)*B,1, activeFile);
	}
	rewind(activeFile);
	rewind(zoneFile);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

void chooseAnExistingFile(){
	printf("Input file name\n");
	scanf("%s" ,activeFileName);
	printf("\n");
	activeFile = fopen(activeFileName,"rb+");
	strcpy(zoneFileName,activeFileName);
	strcat(zoneFileName,"-zone");
	zoneFile = fopen(zoneFileName,"rb+");
	if(activeFile == NULL || zoneFile == NULL){
		printf("File with name %s doesn't exist!\n",activeFileName);
		return;
	}
	rewind(activeFile);
	rewind(zoneFile);
	return;
}
void printNameOfCurrentFile(){//TODO:
	printf("Name of a current file is '%s'\n\n", activeFileName);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool checkConstraints(Syllable prisoner){//return false if ok
	if(prisoner.SentenceTime > 480){
		printf("Sentence should be between 0 and 480! Input failed.\n");
		return true;
	}

	if(prisoner.key > 99999999 || prisoner.key < 0){
		return true;
	}

	for(int i=0;i<7;i++)
		if(prisoner.prisonersLabel[i] == '\0')
			return true;
	for(int i=0;i<5;i++)
		if(prisoner.cellLabel[i] == '\0')
			return true;
	return false;
}


void insertNewPrisoner(){
	Syllable prisoner;
	printf("Creating a new prisoner in a database\n");
	printf("Input: key\n");
	scanf("%d", &prisoner.key);
	printf("Input: prisonersLabel; EXACTLY 7 CHARS\n");
	scanf("%s", &prisoner.prisonersLabel);
	prisoner.prisonersLabel[8] = '\0';
	printf("Input: Arrival Date\n");
	scanf("%s", &prisoner.arrivalDate); 
	printf("Input: Arrival Time\n");
	scanf("%s", &prisoner.arrivalTime); 

	printf("Input: cellLabel; EXACTLY 5 CHARS\n");
	scanf("%s", &prisoner.cellLabel); 
	prisoner.cellLabel[6] = '\0';
	printf("Input: SentenceTime; MAX 480 months\n");
	scanf("%u", &prisoner.SentenceTime);

	if(checkConstraints(prisoner)){
		printf("\n Input failed\n");
		return;
	}


	printf("\n\nChecking to see if prisoner with key value %d already exists\n", prisoner.key);
	if(findExistingPrisoner(prisoner.key)){
		printf("\nError, can't input 2 prisoners with the same key value\n");
	}else{
		prisoner.active = true;
		insertInBucket(prisoner);
		printf("Prisoner input succesful\n");
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

bool insertInBucket(Syllable prisoner){
	rewind(activeFile);//Vracamo na pocetak fajla
	rewind(zoneFile);
	int BucketNum = prisoner.key % B;
	Bucket bucket;
	fseek(activeFile,sizeof(Bucket)* BucketNum, SEEK_SET);//set to the current block
	fread(&bucket, sizeof(Bucket), 1, activeFile);//read the current block, move to the next one
	fseek(activeFile,sizeof(Bucket)* BucketNum, SEEK_SET);//set back one block
	for(int i=0; i<b;i++){
		if(bucket.syllable[i].active == false){
			bucket.syllable[i] = prisoner;
			fwrite(&bucket,sizeof(Bucket),1, activeFile);
			rewind(activeFile);
			return true;
		}
	}
	BucketNum = 0;
	rewind(zoneFile);//set zone file to the beggining to search
	while(fread(&bucket, sizeof(Bucket), 1, zoneFile) != 0){

		for(int i=0; i<b;i++){
			if(bucket.syllable[i].active == false){
				fseek(zoneFile,sizeof(Bucket)* BucketNum, SEEK_SET);
				bucket.syllable[i] = prisoner;
				fwrite(&bucket,sizeof(Bucket),1, zoneFile);
				rewind(zoneFile);
				return true;
			}
		}
		BucketNum++;
	}
	Bucket newBucket;
	for(int i=0; i<b;i++){
		newBucket.syllable[i].active = false;
	}
	newBucket.syllable[0] = prisoner;

	showAllPrisoners();

	printf("\n\n");
	
	fwrite(&newBucket,sizeof(Bucket),1, zoneFile);

	showAllPrisoners();
	rewind(activeFile);
	rewind(zoneFile);
	return true;
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

void printSyllable(Syllable prisoner){
	printf("key = %d\nprisonersLabel = %s\narrivalDate = %s\narrivalTime = %s\ncellLabel = %s\nSentenceTime = %d\nactive = %s\n--------------------------------------------------------------\n", 
	prisoner.key, prisoner.prisonersLabel,
	prisoner.arrivalDate, prisoner.arrivalTime, prisoner.cellLabel,
	prisoner.SentenceTime, prisoner.active ? "true" : "false"); 
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

//Find prisoner and print info
bool findExistingPrisoner(int key){
	Bucket bucket;
	rewind(activeFile);
	for(int i = 0; i<B; i++){
		fread(&bucket, sizeof(Bucket), 1, activeFile);
		for(int j = 0; j<b; j++){
			//printSyllable(bucket.syllable[j]);
			if(bucket.syllable[j].key == key && bucket.syllable[j].active == true){
				printf("\nBlock number: %d; Num in bucket: %d ; file: %s\n",i,j,activeFileName);
				printSyllable(bucket.syllable[j]);
				return true;
			}
		}
	}
	rewind(zoneFile);
	int blockNum = 0;
	while(fread(&bucket, sizeof(Bucket), 1, zoneFile) != 0){
		printf("searching");
		for(int i=0; i<b;i++){
			if(bucket.syllable[i].key == key && bucket.syllable[i].active == true){
				printf("\nBlock number: %d ; Num in bucket %d ; file: %s\n",blockNum,i,zoneFileName);
				printSyllable(bucket.syllable[i]);
				return true;
			}
		}
		blockNum++;
	}
	rewind(activeFile);
	rewind(zoneFile);
	printf("\nPrisoner with a key %d not found\n",key);
	return false;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

void showAllPrisoners(){
	rewind(activeFile);
	rewind(zoneFile);
	Bucket bucket;
	int blockNum=0;
	while(fread(&bucket, sizeof(Bucket), 1, activeFile)){
		printf("\n-----Block number: %d ; file: %s\n",blockNum,activeFileName);
		for(int i=0;i<b;i++){
			if(bucket.syllable[i].active == true){
				printf("Num in bucket %d\n",i);
				printSyllable(bucket.syllable[i]);
			}
		}
		blockNum++;
	}
	blockNum = 0;
	while(fread(&bucket, sizeof(Bucket), 1, zoneFile)){
		printf("\nBlock number: %d ; file: %s\n",blockNum,zoneFileName);
		for(int i=0;i<b;i++){
			if(bucket.syllable[i].active == true){
				printf("Num in bucket %d\n",i);
				printSyllable(bucket.syllable[i]);
			}
		}
		blockNum++;
	}
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

void deletePrisoner(int key){
	if(findExistingPrisoner(key) == false){
		printf("\n------------Prisoner not found, deletion unsuccesful\n");
		return;
	}
	printf("\n------------Prisoner found,\n\n");
	Bucket bucket;
	rewind(activeFile);
	for(int i = 0; i<B; i++){
		fread(&bucket, sizeof(Bucket), 1, activeFile);
		for(int j = 0; j<b; j++){
			if(bucket.syllable[j].key == key && bucket.syllable[j].active == true){
				printSyllable(bucket.syllable[j]);
				bucket.syllable[j].active = false;
				fseek(activeFile,sizeof(Bucket)* i, SEEK_SET);
				fwrite(&bucket, sizeof(Bucket), 1, activeFile);
				printf("\n------------Deletion succesful\n");
				return;
			}
		}
	}
	rewind(zoneFile);
	int blockNum = 0;
	while(fread(&bucket, sizeof(Bucket), 1, zoneFile) != 0){
		printf("searching");
		printf("\nBlock number: %d ; file: %s\n",blockNum,zoneFileName);
		for(int i=0; i<b;i++){
			if(bucket.syllable[i].key == key && bucket.syllable[i].active == true){
				bucket.syllable[i].active = false;
				printSyllable(bucket.syllable[i]);
				rewind(zoneFile);
				fseek(zoneFile,sizeof(Bucket) * blockNum, SEEK_SET);
				fwrite(&bucket, sizeof(Bucket), 1, zoneFile);
				printf("\n Deletion succesful\n");
				return;
			}
		}
		blockNum++;
	}
	rewind(activeFile);
	rewind(zoneFile);
	printf("\n Deletion unsuccesful\n");
	return;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------


void changeSentenceTime(int key){
	if(findExistingPrisoner(key) == false){
		printf("\n------------Prisoner not found, sentence time change unsuccesful %d\n", key);
		return;
	}
	unsigned newSentenceTime = 481;
	while(newSentenceTime > 480 || newSentenceTime < 0){
		printf("\nEnter a new sentence time, value between 0 and 480\n");
		scanf("%u", &newSentenceTime);
	}

	Bucket bucket;
	rewind(activeFile);
	for(int i = 0; i<B; i++){
		fread(&bucket, sizeof(Bucket), 1, activeFile);
		for(int j = 0; j<b; j++){
			if(bucket.syllable[j].key == key && bucket.syllable[j].active == true){
				bucket.syllable[j].SentenceTime = newSentenceTime;
				fseek(activeFile,sizeof(Bucket)* i, SEEK_SET);
				fwrite(&bucket, sizeof(Bucket), 1, activeFile);
				printf("\n------------Change in sentence time succesful\n");
				rewind(activeFile);
				rewind(zoneFile);
				return;
			}
		}
	}
	rewind(zoneFile);
	int bucketNum = 0;
	while(fread(&bucket, sizeof(Bucket), 1, zoneFile) != 0){
		printf("searching");
		for(int i=0; i<b;i++){
			if(bucket.syllable[i].key == key && bucket.syllable[i].active == true){
				bucket.syllable[i].SentenceTime = newSentenceTime;
				fseek(zoneFile,sizeof(Bucket)* bucketNum, SEEK_SET);
				fwrite(&bucket, sizeof(Bucket), 1, zoneFile);
				printf("\n------------Change in sentence time succesful\n");
				rewind(activeFile);
				rewind(zoneFile);
				return;
			}
		}
		bucketNum++;
	}
	rewind(activeFile);
	rewind(zoneFile);
	return;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

void saveFileLikeCsv(){
	rewind(activeFile);
	rewind(zoneFile);
	Bucket bucket;
	strcpy(csvFileName,activeFileName);
	strcat(csvFileName,".csv");
	csvFile = fopen(csvFileName,"w+");
	if(csvFile == NULL){
		printf("error while oppening the file");
		return;
	}

	while(fread(&bucket, sizeof(Bucket), 1, activeFile) != 0){
		for(int i=0; i<b;i++){
			if(bucket.syllable[i].active == true){
				fprintf(csvFile,"%d,%s,%s,%s,%s,%d\n", 
				bucket.syllable[i].key, bucket.syllable[i].prisonersLabel,
				bucket.syllable[i].arrivalDate, bucket.syllable[i].arrivalTime, bucket.syllable[i].cellLabel,
				bucket.syllable[i].SentenceTime); 
			}
		}
	}

	while(fread(&bucket, sizeof(Bucket), 1, zoneFile) != 0){
		for(int i=0; i<b;i++){
			if(bucket.syllable[i].active == true){
				fprintf(csvFile,"%d,%s,%s,%s,%s,%d\n", 
				bucket.syllable[i].key, bucket.syllable[i].prisonersLabel,
				bucket.syllable[i].arrivalDate, bucket.syllable[i].arrivalTime, 
				bucket.syllable[i].cellLabel,
				bucket.syllable[i].SentenceTime); 
			}
		}
	}
//	fclose(csvFile);
	rewind(activeFile);
	rewind(zoneFile);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------


void printAndSaveCellData(){//TODO: finish this
	rewind(activeFile);
	rewind(zoneFile);
	Bucket bucket;
	int cellCounter = 0;
	Cell *cells = NULL;
	
	while(fread(&bucket, sizeof(Bucket), 1, activeFile)){
		readCell(&cells,bucket,&cellCounter);
	}
	while(fread(&bucket, sizeof(Bucket), 1, zoneFile)){
		readCell(&cells,bucket,&cellCounter);
	}
	printf("NUM OF CELLS IS: %d",cellCounter);

	writeCells(cellCounter, cells);
	printCells();

	free(cells);
	cells = NULL;
}

void readCell(Cell **cell, Bucket bucket, int* cellCounter){
	for(int i=0;i<b;i++){
		if(bucket.syllable[i].active == true){
			if((*cell) != NULL){
				bool different = true;
				for(int j=0; j < (*cellCounter);j++){
					if(strcmp((*cell)[j].cellLabel,bucket.syllable[i].cellLabel) == 0){//if they are dif returns 1
						if((*cell)[j].minSentenceTime > bucket.syllable[i].SentenceTime){
							(*cell)[j].minSentenceTime = bucket.syllable[i].SentenceTime;
						}
						(*cell)[j].sentenceTime += bucket.syllable[i].SentenceTime;
						(*cell)[j].prisonersNum += 1;
						different = false;
					}
				}
				if(different){//if a new cell realoc and put values in that cell
					(*cell) = realloc((*cell),(1+(*cellCounter))*sizeof(Cell));
					int endIndex = (*cellCounter) - 1;
					strcpy((*cell)[endIndex].cellLabel,bucket.syllable[i].cellLabel);
					(*cell)[endIndex].minSentenceTime = bucket.syllable[i].SentenceTime;
					(*cell)[endIndex].sentenceTime = (*cell)[endIndex].minSentenceTime;
					(*cell)[endIndex].prisonersNum = 1;
					(*cellCounter)++;
				}
			}else{
				(*cell) = (Cell*)calloc(1,sizeof(Cell));
				strcpy((*cell)->cellLabel,bucket.syllable[i].cellLabel);
				(*cell)->minSentenceTime = bucket.syllable[i].SentenceTime;
				(*cell)->sentenceTime = (*cell)->minSentenceTime;
				(*cell)->prisonersNum = 1;
				(*cellCounter)++;
			}
		}
	}
}

void writeCells(int cellCounter,Cell** cells){
	strcpy(cellFileName,activeFileName);
	strcat(cellFileName,"-cell");
	cellFile = fopen(cellFileName,"wb+");
	rewind(cellFile);
	fwrite(cells,sizeof(Cell)*(cellCounter-1),1,cellFile);
}

void printCells(){
	Cell cell;
	rewind(cellFile);
	while(fread(&cell, sizeof(Cell), 1, cellFile)){
		printf("\n---------------------------------------\nCell label: %s\nMinimal sentence time %d\nAverage sentence time: %d", cell.cellLabel,cell.minSentenceTime,cell.sentenceTime/cell.prisonersNum);
	}
}