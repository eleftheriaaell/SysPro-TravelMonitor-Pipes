#ifndef BLOOM_TRAVEL_H
#define BLOOM_TRAVEL_H

typedef struct bloomfilter bloomfilter;

struct bloomfilter{
    int monitor;
    char* virus;
    char* filter;
    bloomfilter* next;
};

void bloomfilter_push(bloomfilter* head, char* bloom_virus, char* bloomFilter, int i, int bloomSize);

int bloom_check(bloomfilter* node, char* bloomSize, unsigned char* id);

int delete_bloom_node(bloomfilter* head, char* virus, int monitor);

void free_bloom(bloomfilter* b_list);

#endif