while(fread(&bucket, sizeof(Bucket), 1, activeFile)){
		for(int i=0;i<b;i++){
			if(bucket.syllable[i].active == true){
				if(cell != NULL){
					bool different = true;
					for(int j=0; j < sizeof(*cell)/sizeof(Cell);j++){
						if(strcmp(cell[j].cellLabel,bucket.syllable[i].cellLabel) == 0){//if they are dif returns 1
							if(cell[j].minSentenceTime > bucket.syllable[i].SentenceTime){
								cell[j].minSentenceTime = bucket.syllable[i].SentenceTime;
							}
							cell[j].sentenceTime += bucket.syllable[i].SentenceTime;
							cell[j].prisonersNum += 1;
							different = false;
						}
					}
					if(different){//if a new cell realoc and put values in that cell
						cell = realloc(cell,sizeof(*cell)+sizeof(Cell));
						int endIndex = sizeof(*cell)/sizeof(Cell) - 1;
						strcpy(cell[endIndex].cellLabel,bucket.syllable[i].cellLabel);
						cell[endIndex].minSentenceTime = bucket.syllable[i].SentenceTime;
						cell[endIndex].sentenceTime = cell->minSentenceTime;
						cell[endIndex].prisonersNum = 1;
					}
				}else{
					cell = (Cell*)malloc(sizeof(Cell));
					strcpy(cell->cellLabel,bucket.syllable[i].cellLabel);
					cell->minSentenceTime = bucket.syllable[i].SentenceTime;
					cell->sentenceTime = cell->minSentenceTime;
					cell->prisonersNum = 1;
				}

			}
		}
	}