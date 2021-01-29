#include <stdio.h>
#include <stdlib.h>
#include "options.h"


void optionChoosing(){
	int key;
	int value;
	while(1){
		printf("------------------------------------------\n\nInput a value of an option of your choice\n");
		printf("1. Create a new file\n");
		printf("2. Choose an existing file\n");
		printf("3. Show the name of an active file\n");
		printf("4. Insert a new prisoner\n");
		printf("5. Find an existing prisoner\n");
		printf("6. Show all prisoners from an active file\n");
		printf("7. Delete a prisoner from a file\n");
		printf("8. Change sentence length for an existing prisoner\n");
		printf("9. Save the content of a file to a .csv file\n");
		printf("10. Show an average and minimal sentence length by cells and save to a new file\n");
		printf("0. Quit\n\n");
		scanf("%d",&value);

		switch (value)
		{
		case 0:
			if(activeFile)
				fclose(activeFile);
			if(zoneFile)
				fclose(zoneFile);
			return;
		case 1:
			createNewFile();
			break;
		case 2:
			chooseAnExistingFile();
			break;
		case 3:
			if(activeFile && zoneFile){
				printNameOfCurrentFile();
			}else{
				printf("Error, file must be selected\n");
			}
			break;
		case 4:
			if(activeFile && zoneFile){
				insertNewPrisoner();
			}else{
				printf("Error, file must be selected\n");
			}
			break;
		case 5:
			if(activeFile && zoneFile){
				printf("Input key value of a selected prisoner\n");
				scanf("%d", &key);
				findExistingPrisoner(key);
			}else{
				printf("Error, file must be selected\n");
			}
			break;
		case 6:
			if(activeFile && zoneFile){
				showAllPrisoners();
			}else{
				printf("Error, file must be selected\n");
			}
			break;
		case 7:
			if(activeFile && zoneFile){
				printf("Input key value of a selected prisoner\n");
				scanf("%d", &key);
				deletePrisoner(key);
			}else{
				printf("Error, file must be selected\n");
			}
			break;
		case 8:
			if(activeFile && zoneFile){
				printf("Input key value of a selected prisoner\n");
				scanf("%d", &key);
				changeSentenceTime(key);
			}else{
				printf("Error, file must be selected\n");
			}
			break;
		case 9:
			if(activeFile && zoneFile){
				saveFileLikeCsv();
			}else{
				printf("Error, file must be selected\n");
			}
			break;
		case 10:
			if(activeFile && zoneFile){
				printAndSaveCellData();
			}else{
				printf("Error, file must be selected\n");
			}
			break;
		default:
			printf("\nNon-existing option\n\n\n");
			break;
		}
	}
}


int main(){
	optionChoosing();
}