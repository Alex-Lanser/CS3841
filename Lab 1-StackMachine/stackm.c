#include <stdio.h>
#include <stdlib.h>
#include "stackm.h"

void smInit(struct stackm *myStack){
	myStack->top = NULL;
}

//Print the size of the stack
int smSize(struct stackm *myStack){
	int size = 0;
	node* node_temp = myStack->top;
	while(node_temp != NULL){
		size++;
		node_temp = node_temp->next;
	}
	return size;
}

//Push to the stack. Add new node to the top
int smPush(struct stackm *myStack, int toStore){
	int success;
	node* newNode = malloc(smSize(myStack) * sizeof(int));
	if(newNode == NULL){
		success = 0;
	}
	else{
		success = 1;
	}
	newNode->value = toStore;
	newNode->next = myStack->top;
	myStack->top = newNode;
	return success;
}

//Remove top item in stack
int smPop(struct stackm *myStack){
	int success;
	node* tempNode = myStack->top;
	if(myStack->top == NULL){
		success = 0;
	}
	else{
		success = tempNode->value;
		myStack->top = tempNode->next;
	}
	free(tempNode);
	return success;
}

//Return value at top of stack
int smTop(struct stackm *myStack, int* toStore){
	int success;
	node* tempNode = myStack->top;
	if(myStack->top == NULL){
		success = 0;
	}
	else{
		*toStore = tempNode->value;
	}
	success = *toStore;
	return success;
}

//Clear all nodes and releases all dynamic memory
void smClear(struct stackm *myStack){
	while(smSize(myStack) != 0){
		smPop(myStack);
	}
}

//Prints contents of stack machine to standard output
void smPrint(struct stackm *myStack){
	node* tempNode = myStack->top;
	for(int i = 0; i < smSize(myStack); i++){
		printf("%i\n", tempNode->value);
		tempNode = tempNode->next;
	}
}

//Add top 2 elements and push to stack
int smAdd(struct stackm* myStack){
	int success;
	int add1, add2;
	if(smSize(myStack) < 2){
		success = 0;
	}
	else{
		smTop(myStack, &add1);
		smPop(myStack);
		smTop(myStack, &add2);
		smPop(myStack);
		success = add1 + add2;
		smPush(myStack, success);
	}
	return success;
}

//Subtract top 2 elements and push to stack
int smSub(struct stackm* myStack){
	int success;
	int sub1, sub2;
	if(smSize(myStack) < 2){
		success = 0;
	}
	else{
		smTop(myStack, &sub1);
		smPop(myStack);
		smTop(myStack, &sub2);
		smPop(myStack);
		success = sub1 - sub2;
		smPush(myStack, success);
	}
	return success;
}

//Multiply top 2 elements and push to stack
int smMult(struct stackm* myStack){
	int success;
	int mult1, mult2;
	if(smSize(myStack) < 2){
		success = 0;
	}
	else{
		smTop(myStack, &mult1);
		smPop(myStack);
		smTop(myStack, &mult2);
		smPop(myStack);
		success = mult1 * mult2;
		smPush(myStack, success);
	}
	return success;
}

int smRotate(struct stackm* myStack, int depth){
	int success;
	int arr[depth+2];
	if(smSize(myStack) < depth){
		success = 0;
	}
	else{
		for(int i = 0; i < depth; i++){
			smTop(myStack, &arr[i]);
			smPop(myStack);
		}
		if(depth == 1){
			smPush(myStack, arr[0]); //Push first element, which is element being rotated	
		}
		else{
			smPush(myStack, arr[0]);
			for(int i = 1; i < depth; i++){
				smPush(myStack, arr[i]);
			}
		}

	}
	return success;
}