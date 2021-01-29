#include <stdio.h>
#include <stdlib.h>
#include "options.h"


void optionChoosing(){
	int key;
	int value;
	while(1){
		printf("Input a value of an option of your choice\n");
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
			printNameOfCurrentFile();
			break;
		case 4:
			insertNewPrisoner();
			break;
		case 5:
			printf("Input key value of a selected prisoner\n");
			scanf("%d",key);
			findExistingPrisoner(key);
			break;
		case 6:
			showAllPrisoners();
			break;
		case 7:
			deletePrisoner();
			break;
		case 8:
			changeSentenceLength();
			break;
		case 9:
			saveFileLikeCsv();
			break;
		case 10:
			printAndSaveCellData();
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