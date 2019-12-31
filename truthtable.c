#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct Node{
	char* name;
	struct Node *next;
	int index;
}node;	

typedef struct graphNode{
	char *name;
	node *neighbors;
	int value;
	int decoderIndex;
	char directive;
}gNode;

typedef struct dirNode{
	char dir;
	char **mOutputs;
	node *inputs;
	node *outputs;
	struct dirNode *next;
}dNode;

/* ==============DIRECTIVES TO SYMBOLS
 * NOT:		!
 * AND:		*
 * OR:		+
 * NAND:	/
 * NOR:		-
 * XOR:		X
 * DECODER:	D
 * MULTIPLEXER:	M
 */

char dirToSymbol(char *dir){
	if(strcmp(dir, "NOT") == 0){
		return '!';
	}
	else if(strcmp(dir, "AND") == 0){
		return '*';
	} 
	else if(strcmp(dir, "OR") == 0){
                return '+';
        }
	else if(strcmp(dir, "NAND") == 0){
                return '/';
        }
	else if(strcmp(dir, "NOR") == 0){
                return '-';
        }
	else if(strcmp(dir, "XOR") == 0){
                return 'X';
        }
	else if(strcmp(dir, "DECODER") == 0){
                return 'D';
        }
	else if(strcmp(dir, "MULTIPLEXER") == 0){
		return 'M';
	}
	return 'P';
}

int not(int in){
	if(in == 0){
		return 1;
	}
	return 0;
}

int and(int in1,int in2){
	if(in1 == 1 && in2 ==1){
		return 1;
	}
	return 0;
}

int or(int in1, int in2){
	if(in1 == 1 || in2 == 1){
		return 1;
	}
	return 0;
}

int nand(int in1, int in2){
	if(in1 == 1 && in2 == 1){
		return 0;
	}
	return 1;
}

int nor(int in1, int in2){
	if(in1 == 0 && in2 == 0){
		return 1;
	}
	return 0;
}

int xor(int in1, int in2){
	if((in1 == 1 || in2 == 1) && !(in1 == 1 && in2 == 1)){
		return 1;
	}
	return 0;
}

int pass(int in1){
	return in1;
}




int main(int argc, char**argv){
	
	if(argc != 2){
		return EXIT_FAILURE;
	}
	
	FILE* fp = fopen(argv[1], "r");
	
	//First Check the INPUT directive=================================================
	
	char dir[20];
	int numInputs;
	
	fscanf(fp,"%16s %d", dir, &numInputs);
		
//	printf("%s %d\n", dir, numInputs);	
	char* inputVar[numInputs];

	int i;			//array indexes
	int j;
	
	for(i = 0; i < numInputs; i++){
		inputVar[i] = malloc(sizeof(char*)*5);
		fscanf(fp, "%16s", inputVar[i]);
		if(strcmp(inputVar[i], ":") == 0){
			fscanf(fp, "%16s", inputVar[i]);
		}
	} 	
	
	
	//Second Check the OUTPUT directive========================================================
	
	int numOutputs;
	fscanf(fp,"%16s %d", dir, &numOutputs);
	
	char* outputVar[numOutputs];
	
	for(i = 0; i < numOutputs; i++){
		outputVar[i] = malloc(sizeof(char*)*5);
		fscanf(fp, "%16s", outputVar[i]);
		
			
	}
	
	//build the truthtable=============================================================================
	float fRows = pow(2,numInputs);
	int rows = (int)fRows;
	int cols = numInputs + numOutputs;
	
	int **tTable = malloc(sizeof(int*)*rows);
//	printf("here\n");
	for(i = 0; i < rows; i++){			//assigning all the values in the table to 0
		tTable[i] = malloc(sizeof(int)*cols);
		for(j = 0; j < cols; j++){
			tTable[i][j] = 0;
			
		}
	}
	
	int on = 0;
	int count = 0;
	int sectionSize = rows;
	for(j = 0; j < numInputs; j++){			//assigning the correct 1's and 0's to the input values
		sectionSize /= 2;
		for(i = 0; i < rows; i++){
			if(count < sectionSize){
				tTable[i][j] = on;
				count++;
			}
			else{
				if(on == 0){
					on = 1;
				}
				else{
					on = 0;
				}
				tTable[i][j] = on;
				count = 1;
			}
		}
		on = 0;
		count = 0;
		
	}
/*	for(i = 0; i < rows; i++){ 						//PRINTING TABLE (TEST)                     
                for(j = 0; j < cols; j++){
                        printf("%d ", tTable[i][j]);
                }
		printf("\n");
        }
*/
	//assign index values to inputs and outputs in a linked list==(NOT SURE IF I NEED THIS, BUT EH)========================
	
//	node *varIndex = NULL;
	node *ptr = NULL;
	node *temp = NULL;
	int inputVarLength = sizeof(inputVar)/sizeof(inputVar[0]);
/*	for(i = 0; i < inputVarLength; i++){
		temp = malloc(sizeof(node));
		temp->name = inputVar[i];
		temp->index = i;
		temp->next = NULL;
		
		if(ptr == NULL){
			varIndex = temp;
			ptr = varIndex;
		}
		
		else{
			ptr->next = temp;
			ptr = temp;
		}
	}
*/	
	int outputVarLength = sizeof(outputVar)/sizeof(outputVar[0]);
/*	for(i = 0; i < outputVarLength; i++){
		temp = malloc(sizeof(node));
		temp->name = outputVar[i];
		temp->index = i+inputVarLength;
		temp->next = NULL;

		ptr->next = temp;
		ptr = temp;

	}
*/	
	//store the remaining directives========================================================================
	dNode *directives = NULL;
	dNode *dTemp = NULL;
	dNode *dPtr = directives;
	char *dName = malloc(sizeof(char*)*4); 
	char *tempStr = malloc(sizeof(char*)*5);

	int isTempVar = 1;
	int tempVarCount = 0;
	node *tempVar = NULL;


	


	while(fscanf(fp, "%16s", dName)!=EOF){
		
		if(strcmp(dName, ":") == 0){
			fscanf(fp, "%16s", dName);
		}	
		isTempVar = 1;			//For Searching For Temporary Variables
		
		dTemp = malloc(sizeof(dNode));
		dTemp->dir = dirToSymbol(dName);
		dTemp->next = NULL;
		dTemp->outputs = NULL;
		dTemp->inputs = NULL;

		if(dTemp->dir == '!' || dTemp->dir == 'P'){					//If the directive is NOT
			temp = malloc(sizeof(node));				//creating temp node for dNode->inputs
			fscanf(fp, "%16s", tempStr);
			
			if(strcmp(tempStr,":")==0){
				fscanf(fp, "%16s", tempStr);
			}
			temp->name = malloc(sizeof(char*)*5);
			strcpy(temp->name, tempStr);
			temp->next = NULL;
			temp->index = -1;

			dTemp->inputs = temp;

			temp = malloc(sizeof(node));				//creating temp node for dNode->outputs
			fscanf(fp, "%16s", tempStr);
			
			if(strcmp(tempStr,":")==0){
                                fscanf(fp, "%16s", tempStr);
                        }
			temp->name = malloc(sizeof(char*)*5);
			strcpy(temp->name, tempStr);
			temp->next = NULL;
			temp->index = -1;
			
			for(i = 0; i < outputVarLength; i++){			//checking for temporary variables
				if(strcmp(tempStr, outputVar[i]) == 0){		//If temporary variables exist, I will save them into a linked list
					isTempVar = 0;					//and then add them into my graph later.
				}
			}	
			if(isTempVar == 1){
				tempVarCount++;
				temp = malloc(sizeof(node));
				temp->name = malloc(sizeof(char*)*5);
				strcpy(temp->name, tempStr);
				temp->index = -1;
				temp->next = NULL;
					
				if(tempVar == NULL){
					tempVar = temp;
				}
				else{
					temp->next= tempVar;
					tempVar = temp;
				}
				
			}
			
			
			dTemp->outputs = temp;
		}

		else if(dTemp->dir == 'M'){				//If the Directive is MULTIPLEXER
			
			double num;
			fscanf(fp, "%lf", &num);
			int intNum = (int) num;
			double numOutputs = pow(2,num);
			int intNumOutputs = (int) numOutputs;
			dTemp->mOutputs = malloc(sizeof(char*)*intNumOutputs);		//store all the multiplexer inputs first
			
			
			for(i = 0; i < intNumOutputs; i++){
				dTemp->mOutputs[i] = malloc(sizeof(char*)*5);
				fscanf(fp, "%16s", tempStr);
				if(strcmp(tempStr,":")==0){
					fscanf(fp, "%16s", tempStr);
				}
				strcpy(dTemp->mOutputs[i], tempStr);
			}
			
			
			for(i = 0; i < intNum; i++){					//store the input variables
				temp = malloc(sizeof(node));
				fscanf(fp, "%16s", tempStr);
				if(strcmp(tempStr,":")==0){
                                        fscanf(fp, "%16s", tempStr);
                                }
				temp->name = malloc(sizeof(char*)*5);
				strcpy(temp->name, tempStr);
				temp->index = -1;
				temp->next = NULL;
				if(dTemp->inputs == NULL){
					dTemp->inputs = temp;
				}
				else{
					temp->next = dTemp->inputs;
					dTemp->inputs = temp;
				}
			}

			temp = malloc(sizeof(node));					//store the output variable
			fscanf(fp, "%16s", tempStr);
			if(strcmp(tempStr,":")==0){
				fscanf(fp, "%16s", tempStr);
			}
			temp->name = malloc(sizeof(char*)*5);
			strcpy(temp->name, tempStr);
			temp->index = -1;
			temp->next = NULL;
			dTemp->outputs = temp;
				
			for(i = 0; i < outputVarLength; i++){
				if(strcmp(tempStr, outputVar[i]) == 0){
					isTempVar = 0;
				}
			}
			

			if(isTempVar == 1){
				
				tempVarCount++;
				temp = malloc(sizeof(node));
				temp->name = malloc(sizeof(char*)*5);
				strcpy(temp->name, tempStr);
				temp->index = -1;
				temp->next = NULL;

				if(tempVar == NULL){
					tempVar = temp;
				}
				else{
					temp->next = tempVar;
					tempVar = temp;
				}
			}	
			
/*			printf("Multiplexer print: \n");
			for(i = 0; i < intNumOutputs; i++){
				printf("%s --> ", dTemp->mOutputs[i]);
			}
			printf("\n");
			ptr = dTemp->inputs;
			while(ptr!=NULL){
				printf("%s --> ", ptr->name);
				ptr = ptr->next;
			}
			printf("\n");

			printf("%s\n", dTemp->outputs->name); 
*/		}

		else if(dTemp->dir == 'D'){			//If the directive is DECODER
			int num;
			fscanf(fp, "%d", &num);
			
			for(i = 0; i < num; i++){					//inputs
				temp = malloc(sizeof(node));
				fscanf(fp, "%16s", tempStr);
				if(strcmp(tempStr,":")==0){
	                                fscanf(fp, "%16s", tempStr);
				}
				temp->name = malloc(sizeof(char*)*5);
				strcpy(temp->name, tempStr);
				temp->index = -1;
				temp->next = NULL;

				if(dTemp->inputs == NULL){
					dTemp->inputs = temp;
				}
				else{
					temp->next = dTemp->inputs;
					dTemp->inputs = temp;
				}
			}
/*			printf("TESTING\n");
			ptr = dTemp->inputs; 								//TEST PRINT STATEMENT
			while(ptr!=NULL){
				printf("%s ", ptr->name);
				ptr = ptr->next;
			}
			printf("\n");
*/			
			double num2 = pow(2,num);
			num = (int) num2;						//outputs
//			printf("num: %d\n", num);
			for(i = 0; i < num; i++){
				temp = malloc(sizeof(node));
				fscanf(fp, "%16s", tempStr);
                                if(strcmp(tempStr,":")==0){
                                        fscanf(fp, "%16s", tempStr);
				}
				temp->name = malloc(sizeof(char*)*5);
				strcpy(temp->name, tempStr);
				temp->index = -1;
				temp->next = NULL;
				
//				printf("temp->name: %s\n", temp->name);
				
				if(dTemp->outputs == NULL){
					dTemp->outputs = temp;
				}
				else{
					temp->next = dTemp->outputs;
					dTemp->outputs = temp;
				}

								

				isTempVar = 1;
				for(j = 0; j < outputVarLength; j++){			//check temp variables
					if(strcmp(tempStr, outputVar[j]) == 0){
						isTempVar = 0;
					}
				}
				
				if(isTempVar == 1){
					tempVarCount++;
					temp = malloc(sizeof(node));
					temp->name = malloc(sizeof(char*)*5);
					strcpy(temp->name, tempStr);
					temp->index = -1;
					temp->next = NULL;
					
					if(tempVar == NULL){
						tempVar = temp;
					}
					else{
						temp->next = tempVar;
						tempVar = temp;
					}
				}
			}
/*			printf("TESTING\n");
			
			ptr = dTemp->outputs;
			while(ptr!=NULL){
				printf("%s ", ptr->name);
				ptr = ptr->next;
			}
			printf("\n");
*/			
		}
		
		

		else{							//If the directive is AND, OR, NAND, NOR, XOR
			
			ptr = malloc(sizeof(node));				//inputs 		
			fscanf(fp, "%16s", tempStr);
			if(strcmp(tempStr,":")==0){
				fscanf(fp, "%16s", tempStr);
			}
			ptr->name = malloc(sizeof(char*)*5);
			strcpy(ptr->name, tempStr);
			ptr->index = -1;
			ptr->next = NULL;
						
			temp = malloc(sizeof(node));
			fscanf(fp, "%16s", tempStr);
			if(strcmp(tempStr,":")==0){
                                fscanf(fp, "%16s", tempStr);
                        }
			temp->name = malloc(sizeof(char*)*5);;
			strcpy(temp->name, tempStr);
			temp->index = -1;
			temp->next = NULL;
			
			ptr->next = temp;
	
			dTemp->inputs = ptr;
			
			temp = malloc(sizeof(node));				//output
			fscanf(fp, "%16s", tempStr);
			if(strcmp(tempStr,":")==0){
                                fscanf(fp, "%16s", tempStr);
                        }
			temp->name = malloc(sizeof(char*)*5);
			strcpy(temp->name, tempStr);
			temp->index = -1;
			temp->next = NULL;
			
			
			for(i = 0; i < outputVarLength; i++){
				if(strcmp(tempStr, outputVar[i]) == 0){
					isTempVar = 0;
				}
			}

			if(isTempVar == 1){
				tempVarCount++;
				temp = malloc(sizeof(node));
				temp->name = malloc(sizeof(char*)*5);
				strcpy(temp->name, tempStr);
				temp->index = -1;
				temp->next = NULL;

				if(tempVar == NULL){
					tempVar = temp;
				}
				else{
					temp->next = tempVar;
					tempVar = temp;
				}
			}

			dTemp->outputs = temp;	
		
		}

		if(dPtr == NULL){
			directives = dTemp;
			dPtr = directives;
		}
		
		else{
			dPtr->next = dTemp;
			dPtr = dPtr->next;
		}

		

	}								//END OF SAVING DIRECTIVES
/*	printf("MORE TESTING\n");
	dPtr = directives;
	while(dPtr!=NULL){
		printf("%c ", dPtr->dir);
		dPtr = dPtr->next;
	}
	printf("\n");
*/	


//SORTING THE DIRECTIVES (EXTRA CREDIT)

/*	dPtr = directives;						//dTemp will act hold the node that needs to be removed and reinserted
	node *tempVarPtr = NULL;
	node *varList = NULL;							//find the temp vars in the inputs and order them so that they are put
									//into the graph in the correct order
	while(dPtr!=NULL){
		ptr = dPtr->inputs;
		while(ptr!=NULL){
			
			tempVarPtr = tempVar;
			while(tempVarPtr!=NULL){
				if(strcmp(ptr->name, tempVarPtr->name)){
					temp = malloc(sizeof(node));
					temp->name = malloc(sizeof(char*));
					strcpy(temp->name, tempVarPtr->name);
					temp->next = NULL;
					temp->index = -1;
					
					temp->next = varList;
					varList = temp;
					break;
				}
				tempVarPtr = tempVarPtr->next;
			}


			
			ptr = ptr->next;
		}
		dPtr = dPtr->next;
	}
	

	ptr = tempVar;
	while(ptr!=NULL){
		printf("%s -> ", ptr->name);
		ptr = ptr->next;
	}
	printf("\n");

	ptr = varList;
	while(ptr!=NULL){
                printf("%s -> ", ptr->name);
                ptr = ptr->next;
        }
	printf("\n");
*/
//BUILDING THE GRAPH===============================================================================================
	
//	printf("BUILDING GRAPH NOW\n");			//TEST PRINT STATEMENT
		
	int graphSize = inputVarLength + outputVarLength + tempVarCount;

	gNode *graph[graphSize + 2];
	gNode *gTemp = NULL;

	for(i = 0; i < graphSize + 2; i++){			//malloc space for each node in graph
		graph[i] = malloc(sizeof(gNode));
	}
	
	gTemp = malloc(sizeof(gNode));				//creating node for value 1 (Used for multiplexer)
	gTemp->name = "1";
	gTemp->neighbors = NULL;
	gTemp->value = 1;
	graph[graphSize] = gTemp;

	gTemp = malloc(sizeof(gNode));                          //creating node for value 0 (Used for multiplexer)
        gTemp->name = "0";
        gTemp->neighbors = NULL;
        gTemp->value = 0;
        graph[graphSize+1] = gTemp;
	
	for(i = 0; i < inputVarLength; i++){						//copying names of input vars into graph
		gTemp = malloc(sizeof(gNode));
		gTemp->name = malloc(sizeof(char*)*5);
		strcpy(gTemp->name, inputVar[i]);
		gTemp->neighbors = NULL;
		gTemp->value = -1;
		gTemp->directive = '0';
		graph[i] = gTemp;
	}
	
	
	for(i = 0; i < outputVarLength; i++){		//copying names of output vars into graph
		gTemp = malloc(sizeof(gNode));
		gTemp->name = malloc(sizeof(char*)*5);
		strcpy(gTemp->name, outputVar[i]);
		gTemp->neighbors = NULL;
		gTemp->value = -1;
		gTemp->directive = '0';
		graph[inputVarLength+i] = gTemp;
	}
	
	ptr = tempVar;
	for(i = 0; i < tempVarCount; i++){
		gTemp = malloc(sizeof(gNode));
		gTemp->name = malloc(sizeof(char*)*5);
		strcpy(gTemp->name, ptr->name);
		gTemp->neighbors = NULL;
		gTemp->value = -1;
		gTemp->directive = '0';
		int gIndex = graphSize - 1 - i;
		graph[gIndex] = gTemp;
		ptr = ptr->next;
	}									//END OF INITIALIZING GRAPH
	
/*	for(i = 0; i < graphSize; i++){
		printf("%s -> ", graph[i]->name);
	}
*/	
	//BUILDING POINTER ARRAYS FOR THE MULTIPLEXERS
	gNode ***selectors = malloc(sizeof(gNode**)*100);
	dPtr = directives;
	int selectorIndex = 0;
	while(dPtr!=NULL){
		if(dPtr->dir == 'M'){
			int counter = 0;
			ptr = dPtr->inputs;
			while(ptr!=NULL){			//count how many inputs
				counter++;
				ptr = ptr->next;
			}
			double tempNum = pow(2,counter);
			int tempNum2 = (int) tempNum;
			selectors[selectorIndex] = malloc(sizeof(gNode*)*tempNum2*20);		//malloc size of array
/*			printf("%d\n", tempNum2);
*/			for(i = 0; i < tempNum2; i++){						//assigning the graph nodes to the selector inputs
				selectors[selectorIndex][i] = malloc(sizeof(gNode*)*tempNum2);
				if(strcmp(dPtr->mOutputs[i], "1")==0){
					selectors[selectorIndex][i] = graph[graphSize];		//assign the 1 node
				}
				else if(strcmp(dPtr->mOutputs[i], "0")==0){
					selectors[selectorIndex][i] = graph[graphSize+1];		//assign the 0 node
				}
				else{
					for(j = 0; j < graphSize; j++){
						if(strcmp(dPtr->mOutputs[i],graph[j]->name) == 0){	//assign the correct graph node
							selectors[selectorIndex][i] = graph[j];
						}
					}
				}
			}
			selectorIndex++;
		}


		dPtr = dPtr->next;
	}
	
	
	
	//FILLING OUT decoderIndex for all the decoder outputs============================
	
//	printf("FILLING OUT DECODER INDEXES\n");		//TEST PRINT
//	printf("%s %s\n", dPtr->outputs->name, dPtr->outputs->next->name);
	dPtr = directives;						//assign the decoder index to the output variables of the decoder
	int decoderCounter = 0;
	while(dPtr!=NULL){
		if(dPtr->dir == 'D'){					//find the decoder directive nodes
			decoderCounter = 0;
/*			ptr = dPtr->outputs;
			while(ptr!=NULL){
				for(i = 0; i < graphSize; i++){				//go through each output node of the dNode
					if(strcmp(ptr->name, graph[i]->name)==0){			//and match it up with the corresponding graph node
						printf("ptr->name: %s\n", ptr->name);
						graph[j]->decoderIndex = decoderCounter;
						printf("decoder counter: %d\n", decoderCounter);
						decoderCounter++;
						
					}
				}
				ptr = ptr->next;
			}
*/						//^is similar to below, but wrong
			for(i = 0; i < graphSize; i++){
				ptr = dPtr->outputs;
				while(ptr!=NULL){
					if(strcmp(ptr->name, graph[i]->name) == 0){
//						printf("ptr->name: %s\n", ptr->name);
                                                graph[i]->decoderIndex = decoderCounter;
//                                              printf("decoder counter: %d\n", decoderCounter);
                                                decoderCounter++;
					}
					ptr = ptr->next;
				}
			}
			
		}

		dPtr = dPtr->next;
	}
	
	

	//FILL IN THE DIRECTIVES INTO GRAPH NODES================================================
	
//	printf("FILLING DIRECTIVES INTO GRAPH NODES\n"); 	//TEST PRINT
	
	dPtr = directives;
	
	while(dPtr!=NULL){
//		printf("%c\n", dPtr->dir);
//		printf("%d\n", inputVarLength);
//		printf("%d\n", graphSize);
/*		if(dPtr->dir == 'D'){

				

		}


		else{
*/
			ptr = dPtr->outputs;
			while(ptr!=NULL){
	
				for(i = inputVarLength; i < graphSize; i++){
					
					if(strcmp(ptr->name, graph[i]->name) == 0){
						if(graph[i]->directive ==  '0'){
//							printf("%s: %c\n", graph[i]->name, dPtr->dir);
							graph[i]->directive = dPtr->dir;
							node *ptr2 = dPtr->inputs;
							
							while(ptr2!=NULL){
	
								for(j = 0; j < graphSize + 2; j++){
	
									if(strcmp(ptr2->name, graph[j]->name) == 0){
										temp = malloc(sizeof(node));
										temp->index = j;
										temp->next = graph[i]->neighbors;
										graph[i]->neighbors = temp;
//										printf("%s -> %s\n", graph[i]->name, graph[j]->name);
										break;
									}
	
								}
								ptr2 = ptr2->next;
							}
						}
						break;
					}
					
				}
				ptr = ptr->next;

			}




//		}
		dPtr = dPtr->next;			
	
	}					
	
	//SOLVING FOR THE OUTPUTS===========================================================================
	
//	printf("SOLVING FOR OUTPUTS\n");		//TEST PRINT
	
	int rowIndex;
	int in1 = -1;
	int in2 = -1;
	int selectorIndexCounter = 0;
	for(rowIndex = 0; rowIndex < rows; rowIndex++){				
		selectorIndexCounter = 0;
		for(i = 0; i < inputVarLength; i++){						//assign values to the input variables
			graph[i]->value = tTable[rowIndex][i];
		}
//		printf("inputs: %d %d\n", graph[0]->value, graph[1]->value);
		
		
		for(i = graphSize - tempVarCount; i < graphSize; i++){ 				//solve for the temporary variables first
			
//			printf("(%d,%d)\n", rowIndex, i);
			
			if(graph[i]->directive == '!'){					//Not
				in1 = graph[graph[i]->neighbors->index]->value;
				graph[i]->value = not(in1);
			}

                        if(graph[i]->directive == '*'){					//And
				in1 = graph[graph[i]->neighbors->index]->value;
				in2 = graph[graph[i]->neighbors->next->index]->value;
				graph[i]->value = and(in1,in2);
                        }

                        if(graph[i]->directive == '+'){					//Or
				in1 = graph[graph[i]->neighbors->index]->value;
                                in2 = graph[graph[i]->neighbors->next->index]->value;
				graph[i]->value = or(in1,in2);
                        }

			if(graph[i]->directive == '/'){					//Nand
				in1 = graph[graph[i]->neighbors->index]->value;
                                in2 = graph[graph[i]->neighbors->next->index]->value;
                                graph[i]->value = nand(in1,in2);
                        }

			if(graph[i]->directive == '-'){					//Nor
				in1 = graph[graph[i]->neighbors->index]->value;
                                in2 = graph[graph[i]->neighbors->next->index]->value;
                                graph[i]->value = nor(in1,in2);
                        }

                        if(graph[i]->directive == 'X'){					//Xor
				in1 = graph[graph[i]->neighbors->index]->value;
                                in2 = graph[graph[i]->neighbors->next->index]->value;
                                graph[i]->value = xor(in1,in2);
                        }

                        if(graph[i]->directive == 'D'){					//Decoder=========================(GOTTA DO THESE)
				
				double sum = 0;
				in1 = 0;
				ptr = graph[i]->neighbors;
				while(ptr!=NULL){
					in1++;
					ptr = ptr->next;
				}
				ptr = graph[i]->neighbors;
				in1--;
				while(ptr!=NULL){
					sum += graph[ptr->index]->value * pow(2,in1);
					in1--;
					ptr = ptr->next;
				}
				in2 = (int) sum;
//				printf("sum: %d\n", in2);
				if(sum == graph[i]->decoderIndex){
					graph[i]->value = 1;
				}
				else{
					graph[i]->value = 0;
				}
				
				
                        }

                        if(graph[i]->directive == 'M'){					//Multiplexer=====================(GOTTA DO THESE)
				double sum = 0;
				in1 = 0;				//in1 is used as counter
				ptr = graph[i]->neighbors;
//				printf("1\n");
				while(ptr!=NULL){
					in1++;
					ptr = ptr->next;
				}
				ptr = graph[i]->neighbors;
//				printf("in1: %d\n", in1);
				in1--;
//				printf("2\n");
				while(ptr!=NULL){
//					printf("%lf + ",graph[ptr->index]->value * pow(2,in1));
					sum+= graph[ptr->index]->value * pow(2,in1);
					in1--;
					ptr = ptr->next;
				}
//				printf("\n");
				int mSum = (int) sum;
//				printf("autism\n");
//				printf("%lf\n", sum);
//				printf("%d\n",mSum);
				graph[i]->value = selectors[selectorIndexCounter][mSum]->value;
//				printf("3\n");
//				printf("temp: %d \n", graph[i]->value);
				selectorIndexCounter++;
                        }

			if(graph[i]->directive == 'P'){					//Pass
				in1 = graph[graph[i]->neighbors->index]->value;
				graph[i]->value = pass(in1);
			}
			
			
			

		}
		
//		printf("row index: %d\n", rowIndex);
		for(i = inputVarLength; i < inputVarLength+outputVarLength; i++){		//solve for the output variables last
//			printf("there\n");
//			printf("(%d,%d)\n", rowIndex, i);
			if(graph[i]->directive == '!'){					//Not
				in1 = graph[graph[i]->neighbors->index]->value;
                                graph[i]->value = not(in1);
			}

			if(graph[i]->directive == '*'){					//And
				in1 = graph[graph[i]->neighbors->index]->value;
                                in2 = graph[graph[i]->neighbors->next->index]->value;
                                graph[i]->value = and(in1,in2);
			}

			if(graph[i]->directive == '+'){					//Or
				in1 = graph[graph[i]->neighbors->index]->value;
                                in2 = graph[graph[i]->neighbors->next->index]->value;
                                graph[i]->value = or(in1,in2);
			}

			if(graph[i]->directive == '/'){                                 //Nand
                                in1 = graph[graph[i]->neighbors->index]->value;
                                in2 = graph[graph[i]->neighbors->next->index]->value;
                                graph[i]->value = nand(in1,in2);
                        }

                        if(graph[i]->directive == '-'){                                 //Nor
                                in1 = graph[graph[i]->neighbors->index]->value;
                                in2 = graph[graph[i]->neighbors->next->index]->value;
                                graph[i]->value = nor(in1,in2);
                        }

                        if(graph[i]->directive == 'X'){                                 //Xor
                                in1 = graph[graph[i]->neighbors->index]->value;
                                in2 = graph[graph[i]->neighbors->next->index]->value;
                                graph[i]->value = xor(in1,in2);
                        }
			
			if(graph[i]->directive == 'D'){
				
				double sum = 0;
				in1 = 0;
				ptr = graph[i]->neighbors;
				
				while(ptr!=NULL){
					in1++;
					ptr = ptr->next;
				}
				
				ptr = graph[i]->neighbors;
				in1--;
				while(ptr!=NULL){
					
					sum += graph[ptr->index]->value * pow(2,in1);
					in1--;
					ptr = ptr->next;
				}
				
				in2 = (int)sum;
//				printf("%s\n", graph[i]->name);
//				printf("sum: %d\n", in2);
//				printf("decoder index: %d\n", graph[i]->decoderIndex);
				if(sum == graph[i]->decoderIndex){
					graph[i]->value = 1;
				}
				else{
					graph[i]->value = 0;
				}
			}
		
			if(graph[i]->directive == 'M'){
				double sum = 0;
				in1 = 0;
				ptr = graph[i]->neighbors;
//				printf("1\n");
				while(ptr!=NULL){
					in1++;
					ptr = ptr->next;
				}
//				printf("2\n");
				ptr = graph[i]->neighbors;
				in1--;
				while(ptr!=NULL){
//					printf("%lf + ", graph[ptr->index]->value * pow(2,in1));
					sum += graph[ptr->index]->value * pow(2,in1);
					in1--;
					ptr = ptr->next;
				}
//				printf("\n");
//				printf("%lf\n", sum);
				int mSum = (int) sum;
				graph[i]->value = selectors[selectorIndexCounter][mSum]->value;
//				printf("output: %d \n", graph[i]->value);
				selectorIndexCounter++;
			}
			
			if(graph[i]->directive == 'P'){
				in1 = graph[graph[i]->neighbors->index]->value;
				graph[i]->value = pass(in1);
			}
			tTable[rowIndex][i] = graph[i]->value;

		}
		
		
		
	}
	

//	printf("===================\n");
	//print statements for testing=====================================================================
	

/*	dPtr = directives;						
	while(dPtr != NULL){
		printf("%c\n", dPtr->dir);
		printf("Inputs: ");
		ptr = dPtr->inputs;
		while(ptr != NULL){
			printf("%s ", ptr->name);
			ptr = ptr->next;
		}
		printf("\n");
		printf("Outputs: %s\n", dPtr->outputs->name);
		dPtr = dPtr->next;
	}
	
	printf("Temporary Variables:");
	ptr = tempVar;
	for(i = 0; i < tempVarCount; i++){
		printf(" %s", ptr->name);
		ptr = ptr->next;
	}
	printf("\n");
	
	printf("GRAPH:\n");
	for(i = 0; i < graphSize +2; i++){
		printf("%s: %c\n",graph[i]->name, graph[i]->directive);
		ptr = graph[i]->neighbors;
		while(ptr!=NULL){
			printf("| %s |", graph[ptr->index]->name);
			ptr = ptr->next;
		}
		printf("\n");
	}
	printf("\n");

	printf("COMPLETED GRAPH\n");
*/	for(i = 0; i < rows; i++){
		printf("%d", tTable[i][0]);
		for(j = 1; j < inputVarLength; j++){
			printf(" %d", tTable[i][j]);
		}
		printf(" |");
		for(j = inputVarLength; j < inputVarLength + outputVarLength; j++){
			printf(" %d", tTable[i][j]);
		}
		printf("\n");
	}	
	
	return 0;

}













