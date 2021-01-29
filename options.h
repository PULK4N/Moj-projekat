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
void deletePrisoner();
void changeSentenceLength();
void saveFileLikeCsv();
void printAndSaveCellData();

bool insertInBucket(Syllable prisoner);

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
		// for(int i = 0; i<B; i++){
		// 	for(int j = 0; j<b; j++){
		// 		buckets[i].syllable[j].key = 1000000 + i + j;
		// 		strcpy(buckets[i].syllable[j].prisonersLabel,"RADOJKE");
		// 		strcpy(buckets[i].syllable[j].arrivalDateAndTime,"10/11/2012 10:25"); //
		// 		strcpy(buckets[i].syllable[j].cellLabel,"0312F");
		// 		buckets[i].syllable[j].sentenceLenght = 480;
		// 		buckets[i].syllable[j].active = true;
		// 	}
		// }
		fwrite(&buckets,sizeof(Bucket)*B,1, activeFile);
	}
	rewind(activeFile);
	rewind(zoneFile);
	//fclose(activeFile);
}

void chooseAnExistingFile(){
	printf("Input file name\n");
	scanf("%s" ,activeFileName);
	activeFile = fopen(activeFileName,"rb+");
	if(activeFile == NULL){
		printf("File with name %s doesn't exist!\n",activeFileName);
		return;
	}
	rewind(activeFile);
	rewind(zoneFile);
}
void printNameOfCurrentFile(){//TODO:
	Bucket bucket;
	//activeFile = fopen(activeFileName,"rb+");
	int i=0;
	while(fread(&bucket, sizeof(Bucket), 1, activeFile)){
		printf("\nBlock number: %d ; file: %s\n",i,activeFileName);
		for(int i=0;i<b;i++){
			if(bucket.syllable[i].active == true){
				printf("key = %d\nprisonersLabel = %s\narrivalDateAndTime = %s\ncellLabel = %s\nsentenceLenght = %d\nactive = %s\n", 
				bucket.syllable[i].key, bucket.syllable[i].prisonersLabel,
				bucket.syllable[i].arrivalDateAndTime, bucket.syllable[i].cellLabel,
				bucket.syllable[i].sentenceLenght, bucket.syllable[i].active ? "taken" : "free"); 
			}
		}
	}

	while(fread(&bucket, sizeof(Bucket), 1, zoneFile)){
		printf("\nBlock number: %d ; file: %s\n",i,zoneFileName);
		for(int i=0;i<b;i++){
			if(bucket.syllable[i].active == true){
				printf("key = %d\nprisonersLabel = %s\narrivalDateAndTime = %s\ncellLabel = %s\nsentenceLenght = %d\nactive = %s\n", 
				bucket.syllable[i].key, bucket.syllable[i].prisonersLabel,
				bucket.syllable[i].arrivalDateAndTime, bucket.syllable[i].cellLabel,
				bucket.syllable[i].sentenceLenght, bucket.syllable[i].active ? "taken" : "free"); 
			}
		}
	}
}
void insertNewPrisoner(){
	Syllable prisoner;
	printf("Creating a new prisoner in a database\n");
	printf("Input: key");
	scanf("%d", &prisoner.key);
	printf("Input: prisonersLabel; EXACTLY 7 CHARS\n");
	scanf("%s", &prisoner.prisonersLabel);
	printf("Input: arrivalDateAndTime\n");
	scanf("%s", &prisoner.arrivalDateAndTime); 
	printf("Input: cellLabel; EXACTLY 5 CHARS\n");
	scanf("%s", &prisoner.cellLabel); 
	printf("Input: sentenceLenght; MAX 480 months");
	scanf("%u", &prisoner.sentenceLenght);

	if(prisoner.sentenceLenght > 480){
		printf("Sentence should be between 0 and 480! Input failed.\n");
		return;
	}else if(strlen(prisoner.prisonersLabel) != 7){
		printf("Prisoner label should be exactly 7 chars! Input failed.\n");
		return;
	}else if(strlen(prisoner.cellLabel) != 5){
		printf("Cell label should be exactly 5 chars! Input failed.\n");
		return;
	}

	printf("\n\nChecking to see if prisoner with key value %d already exists\n", prisoner.key);
	if(findExistingPrisoner(prisoner.key)){
		printf("\nError, can't input 2 prisoners with the same key value\n");
	}else{
		insertInBucket(prisoner);
		printf("Prisoner input succesful\n");
	}
}

bool insertInBucket(Syllable prisoner){
	// rewind(activeFile);//Vracamo na pocetak fajla
	// rewind(zoneFile);
	int BucketNum = prisoner.key % B;
	Bucket bucket;
	fseek(activeFile,sizeof(Bucket)* BucketNum, SEEK_SET);
	fread(&bucket, sizeof(Bucket), 1, activeFile);//TODO: mozda treba samo slog ubaciti, a ne ceo bucket
	fseek(activeFile,sizeof(Bucket)* BucketNum, SEEK_SET);
	for(int i=0; i<b;i++){
		if(bucket.syllable[i].active == false){
			bucket.syllable[i] = prisoner;
			fwrite(&bucket,sizeof(Bucket),1, activeFile);
			fseek(activeFile,sizeof(Bucket)* BucketNum, SEEK_SET);
			return true;
		}
	}
	
	fseek(zoneFile,sizeof(Bucket)* BucketNum, SEEK_SET);
	while(fread(&bucket, sizeof(Bucket), 1, zoneFile) != EOF){
		for(int i=0; i<b;i++){
			if(bucket.syllable[i].active == false){
				bucket.syllable[i] = prisoner;
				fwrite(&bucket,sizeof(Bucket),1, zoneFile);
				fseek(zoneFile,sizeof(Bucket)* BucketNum, SEEK_SET);
				return true;
			}
		}
	}
	Bucket newBucket;
	newBucket.syllable[0] = prisoner;
	fwrite(&newBucket,sizeof(Bucket),1, zoneFile);
	fseek(zoneFile,sizeof(Bucket)* BucketNum, SEEK_SET);
	return true;
}

bool findExistingPrisoner(int key){

}
void showAllPrisoners(){

}
void deletePrisoner(){

}
void changeSentenceLength(){

}
void saveFileLikeCsv(){

}
void printAndSaveCellData(){

}