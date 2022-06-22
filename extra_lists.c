#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "extra_lists.h"

void virus_push(virus* head, record* node){

    head->virus_name = (char*)malloc(sizeof(char) * 60);
    strcpy(head->virus_name, node->virus);                                          //sets name for the virus of the node
    
    head->vaccinated_persons = (skiplist*)malloc(sizeof(skiplist));
    head->vaccinated_persons = skiplist_init(head->vaccinated_persons);             //initializes the vaccinated skiplist for the virus of the node

    head->not_vaccinated_persons = (skiplist*)malloc(sizeof(skiplist));
    head->not_vaccinated_persons = skiplist_init(head->not_vaccinated_persons);     //initializes the non vaccinated skiplist for the virus of the node
    
}

void country_push(country* head, char* name){

    country* node_c = (country*)malloc(sizeof(country));   
    node_c->country = (char*)malloc(sizeof(char) * 60);

    strcpy(node_c->country, name);                            //sets name of country in new node
    
    country* temp_c = head;
    while(temp_c->next != NULL)
        temp_c = temp_c->next;

    temp_c->next = node_c;
    node_c->next = NULL;           

} //creates list with countries in travelmonitor

void txt_push(txt* head, char* filename){

    txt* node = (txt*)malloc(sizeof(txt));
    node->filename = (char*)malloc(sizeof(char) * 60);
    strcpy(node->filename, filename);

    txt* temp = head;
    while(temp->next != NULL)
        temp = temp->next;

    temp->next = node;
    node->next = NULL;

}  //creates list with filenames used in monitor for adding vaccinations

void stats_push(stats* head, char* countryTo, char* date, char* virus, int check){
    
    stats* node_s = (stats*)malloc(sizeof(stats));
    node_s->countryTo = (char*)malloc(sizeof(char) * 60);
    strcpy(node_s->countryTo, countryTo);   
    node_s->date = (char*)malloc(sizeof(char) * 60);
    strcpy(node_s->date, date);
    node_s->virus = (char*)malloc(sizeof(char) * 60);
    strcpy(node_s->virus, virus);

    if(check == 1){
        node_s->accept = 1;
        node_s->reject = 0;
    }
    else{
        node_s->reject = 1;
        node_s->accept = 0;
    }

    stats* temp_s = head;
    while(temp_s->next != NULL)
        temp_s = temp_s->next;

    temp_s->next = node_s;
    node_s->next = NULL;

} //creates list with statistics in travelmonitor used for travelStats

void free_txt(txt* t_list){

    txt* temp = t_list;
    while(temp != NULL){
        t_list = temp->next;
        free(temp->filename);
        free(temp);
        temp = t_list;
    }

} //free txt list

void free_stats(stats* s_list){

    stats* temp = s_list;
    while(temp != NULL){
        s_list = temp->next;
        free(temp->countryTo);
        free(temp->date);
        free(temp->virus);
        free(temp);
        temp = s_list;
    }
    
} //free stats list

void free_virus(virus* v_list){

    virus* temp = v_list;
    while(temp != NULL){
        v_list = temp->next;
        free(temp->virus_name);
        skiplist_free(temp->vaccinated_persons);
        skiplist_free(temp->not_vaccinated_persons);
        free(temp);
        temp = v_list;
    }

} //free viruses list

void free_country(country* c_list){

    country* temp = c_list;
    while(temp != NULL){
        c_list = temp->next;
        free(temp->country);
        free(temp);
        temp = c_list;
    }

} //free countries list

void free_record(record* r_list){

    record* temp = r_list;
    while(temp != NULL){
        r_list = temp->next;
        free(temp->id);
        free(temp->firstName);
        free(temp->lastName);
        free(temp->country);
        free(temp->age);
        free(temp->virus);
        free(temp->yesNo);
        free(temp->date);
        free(temp);
        temp = r_list;
    }

}  //free records list