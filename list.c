#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "list.h"

 Node arrayOfNodes[LIST_MAX_NUM_NODES];
 List arrayOfLists[LIST_MAX_NUM_HEADS];
 int sizeOfListHeads = -1;
 int sizeOfNodes = -1;

// General Error Handling:
// Client code is assumed never to call these functions with a NULL List pointer, or 
// bad List pointer. If it does, any behaviour is permitted (such as crashing).
// HINT: Use assert(pList != NULL); just to add a nice check, but not required.

// Makes a new, empty list, and returns its reference on success. 
// Returns a NULL pointer on failure.

void List_print(List* pList){
	assert(pList != NULL);
	int size = 0;
	Node* headTrav = pList -> head;
	while(headTrav != NULL){
		printf("%s ", headTrav->data);
		headTrav = headTrav -> next;
		size++;
	}
	
	printf("\n");
}

static Node* createNode(void* pItem){
	if(sizeOfNodes == 0){
		return NULL;
	}
	else if(sizeOfNodes > 100){
		printf("BAD STUFF HERE HOW HOW ????????? \n");
		return NULL;
	}
	Node* node = arrayOfNodes[sizeOfNodes - 1].stackNodePointer;
	sizeOfNodes--;
	node->prev = NULL;
	node->next = NULL;
	node->data = pItem;
	// printf("Current size of list heads is %d\n", sizeOfListHeads);
	return node;
}


List* List_create(){
	if(sizeOfListHeads == -1){
		sizeOfListHeads = LIST_MAX_NUM_HEADS;
		sizeOfNodes = LIST_MAX_NUM_NODES;

		for(int i = 0; i < LIST_MAX_NUM_HEADS; i++){
			List* pList = &arrayOfLists[i];
			pList -> stateOfCurrentNode = LIST_OOB_VALID;
			pList -> currentNode = NULL;
			pList -> head = NULL;
			pList -> tail = NULL;
			pList -> stackListPointer = pList;
			pList->countOfNodes = 0;
		}
		for(int i = 0; i < LIST_MAX_NUM_NODES; i++){
			Node* pNode = &arrayOfNodes[i];
			pNode -> next = NULL;
			pNode -> prev = NULL;
			pNode -> data = NULL;
			pNode -> used = false;
			pNode -> stackNodePointer = pNode;
		}
	}
	
	if(sizeOfListHeads == 0){
		return NULL;
	}
	else{
		List* pNewList = arrayOfLists[sizeOfListHeads - 1].stackListPointer;
		sizeOfListHeads--;
		pNewList->stateOfCurrentNode = LIST_OOB_VALID;
		pNewList->head = NULL;
		pNewList->tail = NULL;
		pNewList->currentNode = NULL;
		return pNewList;
	}
	
}

// Returns the number of items in pList.
int List_count(List* pList){
	assert(pList != NULL);
	return pList->countOfNodes;
}

// Returns a pointer to the first item in pList and makes the first item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_first(List* pList){
	assert(pList != NULL);
	if(pList-> head == NULL && pList -> tail == NULL){
		return NULL;
	} else {
		pList->currentNode = pList->head;
		return pList->head->data;
	}
}

// Returns a pointer to the last item in pList and makes the last item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_last(List* pList){
	assert(pList != NULL);
	if(pList -> head == NULL && pList -> tail == NULL){
		return NULL;
	} else {
		pList->currentNode = pList-> tail;
		return pList->tail->data;
	}

} 

// Advances pList's current item by one, and returns a pointer to the new current item.
// If this operation advances the current item beyond the end of the pList, a NULL pointer 
// is returned and the current item is set to be beyond end of pList.
void* List_next(List* pList){
	assert(pList != NULL);
	if(pList->head == NULL && pList->tail == NULL){
		pList->stateOfCurrentNode = LIST_OOB_END;
		return NULL;
	}
	if(pList->stateOfCurrentNode == LIST_OOB_START){
		pList->stateOfCurrentNode = LIST_OOB_VALID;
		pList->currentNode = pList->head;
		return pList->currentNode->data;
	}
	if(pList->stateOfCurrentNode == LIST_OOB_END){
		pList->currentNode = NULL;
		pList->stateOfCurrentNode = LIST_OOB_END;
		return NULL;

	} else if(pList->currentNode == pList->tail){
		pList->currentNode = NULL;
		pList->stateOfCurrentNode = LIST_OOB_END;
		return NULL;
	} else{
		pList->currentNode = pList->currentNode->next;
		return pList->currentNode->data;
	}
}

// Backs up pList's current item by one, and returns a pointer to the new current item. 
// If this operation backs up the current item beyond the start of the pList, a NULL pointer 
// is returned and the current item is set to be before the start of pList.
void* List_prev(List* pList){
	assert(pList != NULL);
	if(pList->head == NULL && pList->tail == NULL){
		pList->stateOfCurrentNode = LIST_OOB_START;
		return NULL;
	}
	if(pList->stateOfCurrentNode == LIST_OOB_END){
		pList->stateOfCurrentNode = LIST_OOB_VALID;
		pList->currentNode = pList->tail;
		return pList->currentNode->data;
	}
	if(pList->stateOfCurrentNode == LIST_OOB_START){
		pList->currentNode = NULL;
		pList->stateOfCurrentNode = LIST_OOB_START;
		return NULL;
	} else if (pList->currentNode == pList->head){
		pList->currentNode = NULL;
		pList->stateOfCurrentNode = LIST_OOB_START;
		return NULL;
	} else {
		pList->currentNode = pList->currentNode->prev;
		return pList->currentNode->data;
	}
}

// Returns a pointer to the current item in pList.
void* List_curr(List* pList){
	assert(pList != NULL);
	if(pList->stateOfCurrentNode == LIST_OOB_START || pList->stateOfCurrentNode == LIST_OOB_END){
		return NULL;
	}
	if(pList->currentNode == NULL){
		return NULL;
	}

	return pList->currentNode->data;
}

// Adds the new item to pList directly after the current item, and makes item the current item. 
// If the current pointer is before the start of the pList, the item is added at the start. If 
// the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.


int List_add(List* pList, void* pItem){
	assert(pList != NULL);
	if(sizeOfNodes == 0){
		return -1;
	} else{
		if(pList->head == NULL && pList->tail == NULL){
			Node* newNode = createNode(pItem);
			pList->countOfNodes = pList->countOfNodes + 1;
			pList->head = newNode;
			pList->tail = newNode;
			pList->currentNode = newNode;
			return 0;
		}
		// if currentNode is beyond start
		if(pList->stateOfCurrentNode == LIST_OOB_START){
			Node* newNode = createNode(pItem);
			pList->countOfNodes = pList->countOfNodes + 1;
			pList->currentNode = newNode;
			newNode->next = pList->head;
			pList->head->prev = newNode;
			pList->head = newNode;
			pList->stateOfCurrentNode = LIST_OOB_VALID;
			return 0;
		}

		// if currentNode is head
		if(pList->currentNode == pList->tail){
			Node* newNode = createNode(pItem);
			pList->countOfNodes = pList->countOfNodes + 1;
			pList->tail->next = newNode;
			newNode->prev = pList->tail;
			pList->tail = newNode;
			pList->currentNode = newNode;
			return 0;
		}

		// if currentNode is beyond end
		if(pList->stateOfCurrentNode == LIST_OOB_END){
			Node* newNode = createNode(pItem);
			pList->countOfNodes = pList->countOfNodes + 1;
			pList -> currentNode = newNode;
			newNode->prev = pList->tail;
			pList->tail->next = newNode;
			pList->tail = newNode;
			pList->stateOfCurrentNode = LIST_OOB_VALID;
			return 0;

		} 

		Node* newNode = createNode(pItem);
		pList->countOfNodes = pList->countOfNodes + 1;
		pList->currentNode->next->prev = newNode;
		newNode->next = pList->currentNode->next;

		pList->currentNode->next = newNode;
		newNode->prev = pList->currentNode;

		pList->currentNode = newNode;
		return 0;

	}
}

// Adds item to pList directly before the current item, and makes the new item the current one. 
// If the current pointer is before the start of the pList, the item is added at the start. 
// If the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.
int List_insert(List* pList, void* pItem){
	assert(pList != NULL);
	if(sizeOfNodes == 0){
		return -1;
	} else {
		// If list is currently empty
		if(pList -> head == NULL && pList ->tail == NULL){
			Node* newNode = createNode(pItem);
			pList->countOfNodes = pList->countOfNodes + 1;
			pList->head = newNode;
			pList->tail = newNode;
			pList->currentNode = newNode;
			return 0;
		}
		// if currentNode is beyond start
		if(pList->stateOfCurrentNode == LIST_OOB_START){
			Node* newNode = createNode(pItem);
			pList->countOfNodes = pList->countOfNodes + 1;
			pList->currentNode = newNode;
			newNode->next = pList->head;
			pList->head->prev = newNode;
			pList->head = newNode;
			pList->stateOfCurrentNode = LIST_OOB_VALID;
			return 0;
		}

		// if currentNode is head
		if(pList->currentNode == pList->head){
			Node* newNode = createNode(pItem);
			pList->countOfNodes = pList->countOfNodes + 1;
			pList->head->prev = newNode;
			newNode->next = pList->head;
			pList->head = newNode;
			pList->currentNode = newNode;
			return 0;
		}

		// if currentNode is beyond end
		if(pList->stateOfCurrentNode == LIST_OOB_END){
			Node* newNode = createNode(pItem);
			pList->countOfNodes = pList->countOfNodes + 1;
			pList -> currentNode = newNode;
			newNode->prev = pList->tail;
			pList->tail->next = newNode;
			pList->tail = newNode;
			pList->stateOfCurrentNode = LIST_OOB_VALID;
			return 0;

		} 
		
		// normal case

		Node* newNode = createNode(pItem);
		pList->countOfNodes = pList->countOfNodes + 1;
		pList->currentNode->prev->next = newNode;
		newNode->prev = pList->currentNode->prev;

		pList->currentNode->prev = newNode;
		newNode->next = pList->currentNode;

		pList->currentNode = newNode;
		return 0;
	}
}

// Adds item to the end of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.
int List_append(List* pList, void* pItem){
	assert(pList != NULL);
	if(sizeOfNodes == 0){
		return -1;
	} else {
		if(pList -> head == NULL && pList ->tail == NULL){
			Node* newNode = createNode(pItem);
			pList->countOfNodes = pList->countOfNodes + 1;
			pList->stateOfCurrentNode = LIST_OOB_VALID;
			pList->head = newNode;
			pList->tail = newNode;
			pList->currentNode = newNode;
		} 
		else{
			Node* newNode = createNode(pItem);
			if(pList->stateOfCurrentNode == LIST_OOB_START){
				pList->currentNode = pList->head;
			}
			pList->countOfNodes = pList->countOfNodes + 1;
			pList -> currentNode = newNode;
			pList->stateOfCurrentNode = LIST_OOB_VALID;
			newNode -> prev = pList->tail;
			pList->tail->next = newNode;
			pList->tail = newNode;
		}

		return 0;
	}

}

// Adds item to the front of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.
int List_prepend(List* pList, void* pItem){
	if(sizeOfNodes == 0){
		return -1;
	} else {
		if(pList -> head == NULL && pList -> tail == NULL){
			Node* newNode = createNode(pItem);
			pList->countOfNodes = pList->countOfNodes + 1;
			pList->stateOfCurrentNode = LIST_OOB_VALID;
			pList->head = newNode;
			pList->currentNode = newNode;
			pList->tail = newNode;
		} else{
			Node* newNode = createNode(pItem);
			pList->countOfNodes = pList->countOfNodes + 1;
			pList->currentNode = newNode;
			pList->stateOfCurrentNode = LIST_OOB_VALID;
			newNode->next = pList->head;
			pList->head->prev = newNode;
			pList->head = newNode;
		}
	}
	return 0;
}

// Return current item and take it out of pList. Make the next item the current one.
// If the current pointer is before the start of the pList, or beyond the end of the pList,
// then do not change the pList and return NULL.
void* List_remove(List* pList){
	assert(pList != NULL);

	if(pList->head == NULL && pList->tail == NULL){
		pList->countOfNodes = 0;
		return NULL;
	}
	if(pList->stateOfCurrentNode == LIST_OOB_END || pList->stateOfCurrentNode == LIST_OOB_START){
		pList->countOfNodes = 0;
		return NULL;
	} else {

		void* dataCurrentItem = pList->currentNode->data;
		arrayOfNodes[sizeOfNodes].stackNodePointer = pList->currentNode;
		sizeOfNodes++;
		pList->countOfNodes = pList->countOfNodes - 1;
		if(pList->head == pList->tail){
			pList->head = NULL;
			pList->tail = NULL;
			pList->currentNode = NULL;
			pList->stateOfCurrentNode = LIST_OOB_VALID;
			return dataCurrentItem;
		}
		if(pList->currentNode == pList->tail){
			 pList->tail = pList->currentNode->prev;
			 pList->tail->next = NULL;
			 pList->currentNode = NULL;
			 pList->stateOfCurrentNode = LIST_OOB_END;
			 return dataCurrentItem;
		} if(pList->currentNode == pList->head){
			pList->head = pList->head->next;
			pList->head->prev = NULL;
			pList->currentNode = pList->head;
			return dataCurrentItem;
		}

		pList->currentNode->prev->next = pList->currentNode->next;
		pList->currentNode->next->prev = pList->currentNode->prev;
		pList->currentNode = pList->currentNode->next;
		return dataCurrentItem;
	}
}

// Adds pList2 to the end of pList1. The current pointer is set to the current pointer of pList1. 
// pList2 no longer exists after the operation; its head is available
// for future operations.
void List_concat(List* pList1, List* pList2){
	assert(pList1 != NULL);
	assert(pList2 != NULL);

	if(pList1-> head == NULL && pList1->tail == NULL && pList2->head != NULL && pList2->tail != NULL){
		pList1->head = pList2->head;
		pList1->tail = pList2->tail;
		pList1->currentNode = NULL;
		pList1->countOfNodes = pList2->countOfNodes;
		if(pList1->stateOfCurrentNode == LIST_OOB_VALID){
			pList1->stateOfCurrentNode = LIST_OOB_START;
		}
		arrayOfLists[sizeOfListHeads].stackListPointer = pList2;
		sizeOfListHeads++;
		return;
	}

	if(pList2->head == NULL && pList2 ->tail == NULL){
		arrayOfLists[sizeOfListHeads].stackListPointer = pList2;
		sizeOfListHeads++;
		return;
	}
	pList1->tail->next = pList2->head;
	pList2->head->prev = pList1->tail;
	pList1->countOfNodes = pList1->countOfNodes + pList2->countOfNodes;

	arrayOfLists[sizeOfListHeads].stackListPointer = pList2;
	sizeOfListHeads++;
	pList2 = NULL;
	return;
}

// Delete pList. pItemFreeFn is a pointer to a routine that frees an item. 
// It should be invoked (within List_free) as: (*pItemFreeFn)(itemToBeFreedFromNode);
// pList and all its nodes no longer exists after the operation; its head and nodes are 
// available for future operations.
typedef void (*FREE_FN)(void* pItem);
void List_free(List* pList, FREE_FN pItemFreeFn){
	assert(pList != NULL);
	Node* headTrav = pList->head;
	while(headTrav != NULL){
		(*pItemFreeFn)(headTrav);
		pList->countOfNodes = pList->countOfNodes - 1;
		arrayOfNodes[sizeOfNodes].stackNodePointer = headTrav;
		sizeOfNodes++;
		headTrav = headTrav->next;
	}

	arrayOfLists[sizeOfListHeads].stackListPointer = pList;
	sizeOfListHeads++;
	return;
}

void printState(){
	printf("Size of nodes is %d\n", sizeOfNodes);
	printf("Size of listheads is %d\n", sizeOfListHeads);
}

// Return last item and take it out of pList. Make the new last item the current one.
// Return NULL if pList is initially empty.
void* List_trim(List* pList){
	assert(pList != NULL);

	// Case where plist is empty
	if(pList->head == NULL && pList->tail == NULL){
		pList->countOfNodes = 0;

		return NULL;
	}

	void* dataTail = pList->tail->data;


	// Case where pList has only 1 item;
	if(pList->head == pList->tail){
		arrayOfNodes[sizeOfNodes].stackNodePointer = pList->tail;
		sizeOfNodes++;
		pList->countOfNodes = pList->countOfNodes - 1;
		pList->head = NULL;
		pList->tail = NULL;
		pList->stateOfCurrentNode = LIST_OOB_VALID;
		return dataTail;
	}

	// Rest of cases

	arrayOfNodes[sizeOfNodes].stackNodePointer = pList->tail;
	sizeOfNodes++;
	pList->countOfNodes = pList->countOfNodes - 1;
	pList->tail->prev->next = NULL;
	pList->tail = pList->tail->prev;
	pList->currentNode = pList->tail;
	pList->stateOfCurrentNode = LIST_OOB_VALID;


	return dataTail;
}

// Search pList, starting at the current item, until the end is reached or a match is found. 
// In this context, a match is determined by the comparator parameter. This parameter is a
// pointer to a routine that takes as its first argument an item pointer, and as its second 
// argument pComparisonArg. Comparator returns 0 if the item and comparisonArg don't match, 
// or 1 if they do. Exactly what constitutes a match is up to the implementor of comparator. 
// 
// If a match is found, the current pointer is left at the matched item and the pointer to 
// that item is returned. If no match is found, the current pointer is left beyond the end of 
// the list and a NULL pointer is returned.
// 
// If the current pointer is before the start of the pList, then start searching from
// the first node in the list (if any).
typedef bool (*COMPARATOR_FN)(void* pItem, void* pComparisonArg);
void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg){
	assert(pList != NULL);
	if(pList->head == NULL && pList->tail == NULL){
		pList->currentNode = NULL;
		pList->stateOfCurrentNode = LIST_OOB_END;
		return NULL;
	}
	Node* listHead  = pList->head;

	void* solution = NULL;
	while(listHead != NULL){
		void* currItem = listHead->data;
		if((*pComparator)(currItem,pComparisonArg) == true){
			pList->currentNode = listHead;
			solution = currItem;
			break;
		} 
		listHead = listHead->next;
	}

	if(solution == NULL){
		pList->currentNode = NULL;
		pList->stateOfCurrentNode = LIST_OOB_END;
	}
	return solution;
}
