#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "get_string.h"

void create_head(record* head){
    
    head->id = (char*)malloc(sizeof(char) * 60);
    head->firstName = (char*)malloc(sizeof(char) * 60);
    head->lastName = (char*)malloc(sizeof(char) * 60);
    head->country = (char*)malloc(sizeof(char) * 60);
    head->age = (char*)malloc(sizeof(char) * 60);
    head->virus = (char*)malloc(sizeof(char) * 60);
    head->yesNo = (char*)malloc(sizeof(char) * 60);
    head->date = (char*)malloc(sizeof(char) * 60);

    strcpy(head->id, "-1");
    strcpy(head->firstName, "-1");
    strcpy(head->lastName, "-1");
    strcpy(head->country, "-1");
    strcpy(head->age, "-1");
    strcpy(head->virus, "-1");
    strcpy(head->yesNo, "-1");
    strcpy(head->date, "-1");
    head->check = 0;

    head->next = NULL;                                                   //create head of record list that its gonna be empty
}


record* get_record(record* head, char* fileName){
    
    FILE *fp;

    if((fp = fopen(fileName, "r")) == NULL){                                     //fp has the address of input file 
        printf("Error opening file %s\n", fileName);
        return NULL;
    }

    char* buffer = NULL;
    size_t length = 0;
    int flag;
    
    record* node = head; 
    record* temp;

    while((getline(&buffer, &length, fp)) != -1){                               //while to get all lines of file                       

        flag = 0;

        char* id = strtok(buffer, " ");
        char* name = strtok(NULL, " ");
        char* surname = strtok(NULL, " ");
        char* country = strtok(NULL, " ");
        char* age = strtok(NULL, " ");
        char* virus = strtok(NULL, " ");
        char* yesno = strtok(NULL, " ");
        char* date = " ";
        if(strcmp(yesno, "YES") == 0)
            date = strtok(NULL, " ");

        temp = head;
        while(temp != NULL){
            if(strcmp(temp->id, id) == 0 && strcmp(temp->virus, virus) == 0){
                printf("ERROR IN RECORD %s %s %s %s %s %s %s %s\n", id, name, surname, country, age, virus, yesno, date);
                flag = 1;
                break;
            }
            temp = temp->next;
        }

        if(flag == 1)
            continue;

        char* date1 = strtok(NULL, "\n");
        if(strcmp(yesno, "NO") == 0){          
            printf("ERROR IN RECORD %s %s %s %s %s %s %s %s\n", id, name, surname, country, age, virus, yesno, date1);
            continue;
        }

        /* end of error checking */
        
        record* new = (record*)malloc(sizeof(record));                          //create new node   
    
        new->id = (char*)malloc(sizeof(char) * 60);
        new->firstName = (char*)malloc(sizeof(char) * 60);
        new->lastName = (char*)malloc(sizeof(char) * 60);
        new->country = (char*)malloc(sizeof(char) * 60);
        new->age = (char*)malloc(sizeof(char) * 60);
        new->virus = (char*)malloc(sizeof(char) * 60);
        new->yesNo = (char*)malloc(sizeof(char) * 60);
        new->date = (char*)malloc(sizeof(char) * 60);
        new->check = 0;
    
        strcpy(new->id, id);
        strcpy(new->firstName, name);
        strcpy(new->lastName, surname);
        strcpy(new->country, country);
        strcpy(new->age, age);
        strcpy(new->virus, virus);
        strcpy(new->yesNo, yesno);
        strcpy(new->date, date);

        temp = head;
        while(temp != NULL){
            if(strcmp(temp->id, new->id) == 0){
                new->check = 1;
                break;
            }
            temp = temp->next;
        }                                          //find yes or no and put it in the new node         
        
        node->next = new;
        new->next = NULL;
        node = new;                                     //put node in the correct position in list
        
    }   
    
    free(buffer);                                                                   
    fclose(fp); 

    return node;
}

char** get_command(char* command, int num){     

    char** array = (char**)malloc(sizeof(char*) * (num + 1) * 60);

    for(int i = 0; i < num; i++)
        array[i] = (char*)malloc(sizeof(char) * 60);

    char* word;

    for(int i = 0; i < num - 1; i++){
        word = (char*)malloc(sizeof(command) * 60);
        strcpy(word, "");

        while(*command != ' '){
            strncat(word, command, 1);
            *command++;
        }
        *command++;
        strcpy(array[i], word);                         //saves all the parts of the command in the array
        
        free(word);
    }                                                               

    word = (char*)malloc(sizeof(command) * 60);
    strcpy(word, "");

    while(*command != '\n'){
        strncat(word, command, 1);
        *command++;
    }
    strcpy(array[num - 1], word);                   //saves the last part of the command in the last position of the array                                

    free(word);
    
    return array;

}


char** get_command_monitor(char* command, int num){     

    char** array = (char**)malloc(sizeof(char*) * (num + 1) * 60);

    for(int i = 0; i < num; i++)
        array[i] = (char*)malloc(sizeof(char) * 60);

    char* word;

    for(int i = 0; i < num - 1; i++){
        word = (char*)malloc(sizeof(command) * 60);
        strcpy(word, "");

        while(*command != ' '){
            strncat(word, command, 1);
            *command++;
        }
        *command++;
        strcpy(array[i], word);                         //saves all the parts of the command in the array
        
        free(word);
    }                                                               

    word = (char*)malloc(sizeof(command) * 60);
    strcpy(word, "");

    while(*command != '\0'){
        strncat(word, command, 1);
        *command++;
    }
    strcpy(array[num - 1], word);                   //saves the last part of the command in the last position of the array                                

    free(word);
    
    return array;

}


char** get_date(char* date){

    char** array = (char**)malloc(sizeof(char*) * 60);

    for(int i = 0; i < 3; i++)
        array[i] = (char*)malloc(sizeof(char) * 60);

    char* word;

    for(int i = 0; i < 2; i++){
        word = (char*)malloc(sizeof(date) * 60);
        strcpy(word, "");

        while(*date != '-'){
            strncat(word, date, 1);
            *date++;
        }
        *date++;
        strcpy(array[i], word);                                 //saves day and month in array
        
        free(word);
    }

    strcpy(array[2], "");
    strncat(array[2], date, 4);                                 //then saves the year
        
    return array;

}