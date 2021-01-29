#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define B 11 //broj baketa
#define b 5 //faktor baketiranja - broj slogova u baketu

typedef struct Syllable
{
	int key; //evidential number
	char prisonersLabel[7];
	char arrivalDateAndTime[25]; // 10/11/2012 10:25  oko 16 char
	char cellLabel[5];
	unsigned  sentenceLenght;
	bool active;
}Syllable;

typedef struct Bucket
{
	Syllable syllable[b]; // 
}Bucket;

FILE *activeFile;
char activeFileName[30];
FILE *zoneFile;
char zoneFileName[35];

void createNewFile();
void chooseAnExistingFile();
void printNameOfCurrentFile();
void insertNewPrisoner();
bool findExistingPrisoner(int key);
void showAllPrisoners();
void deletePrisoner(int key);
void changeSentenceLength();
void saveFileLikeCsv();
void printAndSaveCellData();

bool insertInBucket(Syllable prisoner);
void printSyllable(Syllable prisoner);

void createNewFile(){
	printf("Input file name\n");
	scanf("%s" ,activeFileName);
	activeFile = fopen(activeFileName,"rb+");
	if(activeFile != NULL){
		printf("Error, file with name %s exist already!\n",activeFileName);
		fclose(activeFile);
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
				buckets[i].syllable[j].key = 11*j + i;
				strcpy(buckets[i].syllable[j].prisonersLabel,"RADOJKE");
				strcpy(buckets[i].syllable[j].arrivalDateAndTime,"10/11/2012 10:25"); //
				strcpy(buckets[i].syllable[j].cellLabel,"0312F");
				buckets[i].syllable[j].sentenceLenght = 480;
				buckets[i].syllable[j].active = true;
			}
		}
		fwrite(&buckets,sizeof(Bucket)*B,1, activeFile);
	}
	rewind(activeFile);
	rewind(zoneFile);
	//fclose(activeFile);
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

void insertNewPrisoner(){
	Syllable prisoner;
	printf("Creating a new prisoner in a database\n");
	printf("Input: key\n");
	scanf("%d", &prisoner.key);
	printf("Input: prisonersLabel; EXACTLY 7 CHARS\n");
	scanf("%s", &prisoner.prisonersLabel);
	char date[20];
	char time[10];
	printf("Input: arrivalDateAndTime\n");
	scanf("%s %s", &date, &time); 
	strcat(date," ");
	strcat(date,time);
	strcat(&prisoner.arrivalDateAndTime,date);
	strcat(&prisoner.arrivalDateAndTime,'\0');
	printf("Input: cellLabel; EXACTLY 5 CHARS\n");
	scanf("%s", &prisoner.cellLabel); 
	printf("Input: sentenceLenght; MAX 480 months\n");
	scanf("%u", &prisoner.sentenceLenght);

	if(prisoner.sentenceLenght > 480){
		printf("Sentence should be between 0 and 480! Input failed.\n");
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
	
	rewind(zoneFile);//set zone file to the beggining to search
	while(fread(&bucket, sizeof(Bucket), 1, zoneFile) != 0){
		//printf("searching");
		for(int i=0; i<b;i++){
			if(bucket.syllable[i].active == false){
				bucket.syllable[i] = prisoner;
				fwrite(&bucket,sizeof(Bucket),1, zoneFile);
				rewind(zoneFile);
				return true;
			}
		}
	}
	Bucket newBucket;
	for(int i=0; i<b;i++){
		newBucket.syllable[i].active = false;
	}
	newBucket.syllable[0] = prisoner;
	fwrite(&newBucket,sizeof(Bucket),1, zoneFile);
	rewind(activeFile);
	rewind(zoneFile);
	return true;
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

void printSyllable(Syllable prisoner){
	printf("key = %d\nprisonersLabel = %s\narrivalDateAndTime = %s\ncellLabel = %s\nsentenceLenght = %d\nactive = %s\n--------------------------------------------------------------\n", 
	prisoner.key, prisoner.prisonersLabel,
	prisoner.arrivalDateAndTime, prisoner.cellLabel,
	prisoner.sentenceLenght, prisoner.active ? "true" : "false"); 
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

//Find prisoner and print info
bool findExistingPrisoner(int key){
	Bucket bucket;
	rewind(activeFile);
	for(int i = 0; i<B; i++){
		fread(&bucket, sizeof(Bucket), 1, zoneFile);
		for(int j = 0; j<b; j++){
			if(bucket.syllable[i].key == key && bucket.syllable[i].active == true){
				printf("\nBlock number: %d ; file: %s\n",i,activeFileName);
				printSyllable(bucket.syllable[i]);
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
				printf("\nBlock number: %d ; file: %s\n",blockNum,zoneFileName);
				printSyllable(bucket.syllable[i]);
				return true;
			}
		}
		blockNum++;
	}
	rewind(activeFile);
	rewind(zoneFile);
	return false;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

void showAllPrisoners(){
	Bucket bucket;
	int blockNum=0;
	while(fread(&bucket, sizeof(Bucket), 1, activeFile)){
		printf("\nBlock number: %d ; file: %s\n",blockNum,activeFileName);
		for(int i=0;i<b;i++){
			if(bucket.syllable[i].active == true){
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
		fread(&bucket, sizeof(Bucket), 1, zoneFile);
		for(int j = 0; j<b; j++){
			if(bucket.syllable[i].key == key && bucket.syllable[i].active == true){
				bucket.syllable[i].active = false;
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


void changeSentenceLength(){

}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

void saveFileLikeCsv(){

}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------


void printAndSaveCellData(){

}