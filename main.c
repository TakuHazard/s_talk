#include <stdio.h>
#include "list.h"

int data [300];

int main(){
	List* newList = List_create();

	for(int i = 0; i < 300; i++){
		data[i] = i;
	}

	for(int i = 0; i < 50; i++){
		List_append(newList, &data[i]);
	}
	printf("Hello world !\n");

	printf("Yoooo\n");

}