#ifndef EXTRA_LISTS_H
#define EXTRA_LISTS_H

#include "skiplist.h"

typedef struct record record;
typedef struct virus virus;
typedef struct country country;
typedef struct countryTo countryTo;
typedef struct txt txt;
typedef struct stats stats;

struct record{
    char* id;
    char* firstName;
    char* lastName;
    char* country;
    char* age;
    char* virus;
    char* yesNo; 
    char* date;
    int check;                      
    record* next;
};                                  //struct for keeping the record parts

struct virus{
    char* virus_name;
    skiplist* vaccinated_persons;
    skiplist* not_vaccinated_persons;
    virus* next;
};                                  //struct for saving skiplists for each virus

struct country{
    char* country;
    int monitor;              
    country* next;              //struct for saving the population of vaccinated and non vaccinated people in each country
};

struct txt{
    char* filename;
    txt* next;
};

struct stats{
    char* countryTo;
    char* date;
    char* virus;
    int accept;
    int reject; 
    stats* next;
};

void virus_push(virus* head, record* node);

void country_push(country* head, char* name);

void txt_push(txt* head, char* filename);

void stats_push(stats* head, char* countryTo, char* date, char* virus, int check);

void free_txt(txt* t_list);

void free_stats(stats* s_list);

void free_virus(virus* v_list);

void free_country(country* c_list);

void free_record(record* r_list);

#endif