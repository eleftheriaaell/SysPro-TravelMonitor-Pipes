#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bloom_travel.h"

void bloomfilter_push(bloomfilter* head, char* bloom_virus, char* bloomFilter, int i, int bloomSize){

    bloomfilter* node_b = (bloomfilter*)malloc(sizeof(bloomfilter));
    node_b->virus = (char*)malloc(sizeof(char) * 60);
    strcpy(node_b->virus, bloom_virus);
    node_b->filter = (char*)malloc(sizeof(char) * bloomSize + 1);
    strcpy(node_b->filter, bloomFilter);
    node_b->monitor = i;

    bloomfilter* temp_b = head;
    while(temp_b->next != NULL)
        temp_b = temp_b->next;

    temp_b->next = node_b;
    node_b->next = NULL;

} //creates new bloomfilter node in parent when sent by monitor

/* functions given by instructor*/

unsigned long djb2(unsigned char *str) {
	unsigned long hash = 5381;
	int c; 
	while (c = *str++) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}
	return hash;
}


unsigned long sdbm(unsigned char *str) {
	unsigned long hash = 0;
	int c;

	while (c = *str++) {
		hash = c + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}


unsigned long hash_i(unsigned char *str, unsigned int i) {
	return djb2(str) + i*sdbm(str) + i*i;
}

/* end of functions given by instructor */

int bloom_check(bloomfilter* node, char* bloomSize, unsigned char* id){
	
	int hash;
	
	for(int i = 0; i < 16; i++){
		hash = hash_i(id, i) % atoi(bloomSize);
		if(node->filter[hash] == '0'){								//check if the bit array has zeros in position suited for the id 
			printf("-----REQUEST REJECTED – YOU ARE NOT VACCINATED-----\n");					//if it has at least one zero then its not vaccinated
			return 1;
		}
	}
	printf("-----MAYBE!-----\n");									//else it may be vaccinated
	return 0;

} 

int delete_bloom_node(bloomfilter* head, char* virus, int monitor){

	bloomfilter* temp = head; bloomfilter* prev;
	
	if (temp != NULL && strcmp(temp->virus, virus) == 0 && temp->monitor == monitor) {
        head = temp->next; 				//changed head
        free(temp->virus);
	    free(temp->filter);
        free(temp); 					//free old head
        return 0;
    }
 
    while (temp != NULL && strcmp(temp->virus, virus) != 0) {               //search for the key to be deleted
        prev = temp;
        temp = temp->next;                                                  //fix pointers for searching
    }
 
    
    if (temp == NULL)                   //if key was not present in linked list
        return 0;
    
    if(temp->monitor == monitor){       //if the virus is in the right monitor
        prev->next = temp->next;        //unlink the node from bloom list
        
        free(temp->virus);
        free(temp->filter);
        free(temp);                     //free node

        return 0;
    }

    else
        return 1;

}

void free_bloom(bloomfilter* b_list){

    bloomfilter* temp = b_list;
    while(temp != NULL){
        b_list = temp->next;
        free(temp->virus);
        free(temp->filter);
        free(temp);
        temp = b_list;
    }
    
} //free bloom list in parent