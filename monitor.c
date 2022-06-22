#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#include "get_string.h"
#include "bloom.h"

int sigflag = 0;

void sigusr1_handler(int sig){
    sigflag = 1;
}

void sig_handler(int sig){
    sigflag = 2;
}

int main(int argc, char *argv[]){

    struct sigaction act = {0};

    act.sa_handler = &sigusr1_handler;

    if (sigaction(SIGUSR1, &act, NULL) == -1) {
        perror ("sigaction"); exit(1);
    }

    struct sigaction act1 = {0};

    act1.sa_handler = &sig_handler;

    if (sigaction(SIGINT, &act1, NULL) == -1) {
        perror ("sigaction"); exit(1);
    }

    if (sigaction(SIGQUIT, &act1, NULL) == -1) {
        perror ("sigaction"); exit(1);
    }                                                                //declaration of signals
        
    char *r_fifo = (char*)malloc(sizeof(char) * 60);
    char *w_fifo = (char*)malloc(sizeof(char) * 60);
    strcpy(r_fifo, argv[1]);
    strcpy(w_fifo, argv[2]);
    
    int readfd, writefd;
    
    if((writefd = open(r_fifo, 1)) < 0) {                           //opens pipe for writing
        perror("Monitor: Can't open write fifo!"); exit(1);
    }
    
    if((readfd = open(w_fifo, 0)) < 0) {                            //opens pipe for reading
        perror("Monitor: Can't open read fifo!"); exit(1);
    }

    char buff[10];

    if(read(readfd, buff, 10) < 0){                       //read bufferSize sent from travelMonitor
        perror("Error in reading!"); exit(1);
    }
    
    int bufferSize = atoi(buff);
    char msg[bufferSize];

    int readmsg;

    char* message = (char*)malloc(sizeof(char) * bufferSize * 60);
    strcpy(message, ""); strcpy(msg, "");

    while(1){
        if((readmsg = read(readfd, msg, bufferSize)) < 0){              //reads content
            perror("Monitor: Error in reading!"); exit(1);
        }
        
        msg[readmsg] = '\0';

        if(msg[0] != '!')                                               //until the ! is sent
            strcat(message, msg);
        else
            break;

    }

    //////////////////////////////////////////  creates paths for regular files and creates record list  ///////////////////////////////////

    char** paths = (char**)malloc(sizeof(char*) * 1000);                //creates array with paths
    char* msssg = message;

    int count = 0;
    
    while(paths[count] = strtok_r(msssg, " ", &msssg)){                 //cuts content in pieces to take every path on its own
        count++;
    }

    record* head = (record*)malloc(sizeof(record));
    create_head(head);                                                 //creates head of record list
    
    txt* t_head = (txt*)malloc(sizeof(txt));
    t_head->filename = (char*)malloc(sizeof(char) * 60);
    strcpy(t_head->filename, "");
    t_head->next = NULL;

    char* filename = (char*)malloc(sizeof(char) * 60);
    
    record* node = head;

    for(int j = 0; j < count; j++){
        struct dirent *de1;                     //pointer for subdirectory entry
        DIR *dr1 = opendir(paths[j]);            //opens subdirectory

        if(dr1 == NULL){                                       //returns NULL if couldn't open subdirectory
            printf("Couldn't open current subdirectory!");
            return 0;
        }
        
        while((de1 = readdir(dr1)) != NULL){
            
            if(de1->d_type == DT_REG){               //if it's a regular file
                strcpy(filename, "");
                strcpy(filename, paths[j]);
                strcat(filename, "/");
                strcat(filename, de1->d_name);      //create filename
                txt_push(t_head, filename);
                node = get_record(node, filename);  //built list with records
            }

        }
        closedir(dr1);                         //close subdirectory
    }   
    free(filename);

    country* c_head = (country*)malloc(sizeof(country));                  //creates country head
    c_head->country = (char*)malloc(sizeof(char) * 60);
    strcpy(c_head->country, "-1");
    c_head->next = NULL;

    record* temp_r = head->next;
    country* temp_c;
    
    while(temp_r != NULL){
        int flag = 0;
        temp_c = c_head;    

        while(temp_c != NULL){                                                  
            if(strcmp(temp_c->country, temp_r->country) == 0){                    //if country exists in country list
                flag = 1;
                break;
            }
            else
                temp_c = temp_c->next;
        }

        if(flag == 0)                                                        //creates new country node      
            country_push(c_head, temp_r->country);                                  
    
        temp_r = temp_r->next;
    } 

    ///////////////////////////////////// creates bloom filter ////////////////////////////////////////////////////////////////////

    char bloomS[10];

    if(read(readfd, bloomS, 10) < 0){                       //read bufferSize sent from travelMonitor
        perror("Error in reading!"); exit(1);
    }

    int bloomSize = atoi(bloomS);

    bloom* bloom_head = (bloom*)malloc(sizeof(bloom));
    
    int flg = 0;

    record* temp = head->next;                                  //initializes with head->next because the head is empty
    while(temp != NULL){
        if(strcmp(temp->yesNo, "YES") == 0){
            bloom_push(bloom_head, temp, bloomSize);
            bloom_head->next = NULL;
            flg = 1;
            break;
        }
        temp = temp->next;
    }                                                           //creates the head of bloom filter

    if(flg == 0){
        bloom_head->virus = (char*)malloc(sizeof(char) * 60);
	    strcpy(bloom_head->virus, "-1");							
	
	    bloom_head->bit_array = (char*)malloc(sizeof(char) * bloomSize);
	    strcpy(bloom_head->bit_array, "-1");

        bloom_head->next = NULL;

        //in this case no people in this monitor are vaccinated
    }

    else{
        temp = head->next;
        bloom* temp_b;

        int flag;

        while(temp != NULL){
            if(strcmp(temp->yesNo, "YES") == 0){
                
                flag = 0;
                temp_b = bloom_head;    

                while(temp_b != NULL){
                    if(strcmp(temp_b->virus, temp->virus) == 0){        //if the virus exists in bloom filter
                        bloom_insert(temp_b, temp->id, bloomSize);      //just insert the id in the correct bloom filter
                        flag = 1;
                        break; 
                    }
                    else
                        temp_b = temp_b->next;
                    
                }                                               

                if(flag == 0){                                          //if virus not existant in bloom filter yet
                    bloom* node_b = (bloom*)malloc(sizeof(bloom));
                    bloom_push(node_b, temp, bloomSize);                //create new bloom node with the virus
                    temp_b = bloom_head;
                    while(temp_b->next != NULL)
                        temp_b = temp_b->next;
                    
                    temp_b->next = node_b;
                    node_b->next = NULL;                                //place new bloom node in the correct position
                    
                }
            }
            temp = temp->next;
        }
        
    }

    ///////////////////////////// passing bloomfilters to travelmonitor ////////////////////////////////
    
    bloom* tempb = bloom_head;
    char k[bufferSize]; 

    while(tempb != NULL){
        strcpy(k, "");
        char* word = (char*)malloc(strlen(tempb->virus) + strlen(tempb->bit_array) + 6);
        strcpy(word, "");
        strcpy(word, tempb->virus);
        strcat(word, "/");
        strcat(word, tempb->bit_array);
        strcat(word, "/");
        
        int s = 0; 
                
        while(strlen(word) > s){
            strncpy(k, word+s, bufferSize);
            
            if(write(writefd, k, bufferSize) < 0){                     //passes path to monitors piece by piece according to its size
                perror("Monitor: Error in writing!"); exit(1);
            }     
            
            s+=bufferSize;           
        }
        
        if(write(writefd, "@", bufferSize) < 0){                     //sends ! to travelmonitor, to know that monitor's ready for quests
            perror("Monitor: Error in writing!"); exit(1);
        }

        free(word);

        tempb = tempb->next;
    }

    if(write(writefd, "!", bufferSize) < 0){                     //sends ! to travelmonitor, to know that monitor's ready for quests
        perror("Monitor: Error in writing!"); exit(1);
    }

    //////////////////////////////////////////////// creates viruses list  //////////////////////////////////////////
    
    virus* virus_head = (virus*)malloc(sizeof(virus));
    
    temp = head->next;

    virus_push(virus_head, temp);                                  //creates the head of the virus list that includes the skiplists for each virus
    virus_head->next = NULL;
    
    virus* temp_v;
    
    while(temp != NULL){
            
        int flag = 0;
        temp_v = virus_head;    

        while(temp_v != NULL){
            if(strcmp(temp_v->virus_name, temp->virus) == 0){                   //if virus exists in virus list 
                if(strcmp(temp->yesNo, "YES") == 0)                             //check if the id is vaccinated
                    skiplist_insert(temp_v->vaccinated_persons, temp->id);      //if yes insert it in the vaccinated skiplist of the virus
                else
                    skiplist_insert(temp_v->not_vaccinated_persons, temp->id);  //if not insert it in the not vaccinated skiplist of the virus
                
                flag = 1;
                break;
            }
            else
                temp_v = temp_v->next;
        }

        if(flag == 0){                                                          //if virus non existant in virus list yet       
            virus* node_v = (virus*)malloc(sizeof(virus));
            virus_push(node_v, temp);                                           //create new virus node for the virus
            temp_v = virus_head;
            while(temp_v->next != NULL)
                temp_v = temp_v->next;
            
            temp_v->next = node_v;
            node_v->next = NULL;                                                //place the virus node on the correct position

            if(strcmp(temp->yesNo, "YES") == 0)                                 //check if the id is vaccinated
                skiplist_insert(node_v->vaccinated_persons, temp->id);          //if yes insert it in the vaccinated skiplist of the virus
            else
                skiplist_insert(node_v->not_vaccinated_persons, temp->id);      //if not insert it in the not vaccinated skiplist of the virus
        }
    
        temp = temp->next;
    }

    ////////////////////////////////////////////////// accepting quests from monitor //////////////////////////////////////////////
    
    int r;
    char q[50];
    char* quest = (char*)malloc(sizeof(char) * bufferSize * 50);
    strcpy(quest, ""); strcpy(q, "");

    int total_requests = 0;
    int total_accepted = 0;
    int total_rejected = 0;

    while(1){

        /* addVaccinationRecords */
        if(sigflag == 1){
            
            sigflag = 0;

            char* filename = (char*)malloc(sizeof(char) * 60);

            for(int j = 0; j < count; j++){
                
                struct dirent *de1;                     //pointer for subdirectory entry
                DIR *dr1 = opendir(paths[j]);            //opens subdirectory

                if(dr1 == NULL){                                       //returns NULL if couldn't open subdirectory
                    printf("Couldn't open current subdirectory!");
                    return 0;
                }
                
                while((de1 = readdir(dr1)) != NULL){

                    int flag = 0;
                    if(de1->d_type == DT_REG){               //if it's a regular file
                        strcpy(filename, "");
                        strcpy(filename, paths[j]);
                        strcat(filename, "/");
                        strcat(filename, de1->d_name);      //create filename
                                    
                        txt* t = t_head->next;
                        while(t != NULL){
                            if(strcmp(t->filename, filename) == 0){
                                flag = 1;
                                break;
                            }
                            t = t->next;
                        }
                        
                        if(flag == 0){
                            printf("New records were added!\n");                                    
                            txt_push(t_head, filename);
                            node = get_record(node, filename);
                            
                            if(strcmp(node->yesNo, "YES") == 0){
                                int flg = 0;
                                bloom* temp_b = bloom_head;    

                                while(temp_b != NULL){
                                    if(strcmp(temp_b->virus, node->virus) == 0){        //if the virus exists in bloom filter
                                        bloom_insert(temp_b, node->id, bloomSize);      //just insert the id in the correct bloom filter
                                        flg = 1;
                                        break; 
                                    }
                                    else
                                        temp_b = temp_b->next;                                                        
                                }                                               

                                if(flg == 0){                                          //if virus not existant in bloom filter yet
                                    bloom* node_b = (bloom*)malloc(sizeof(bloom));
                                    bloom_push(node_b, node, bloomSize);                //create new bloom node with the virus
                                    temp_b = bloom_head;
                                    while(temp_b->next != NULL)
                                        temp_b = temp_b->next;
                                    
                                    temp_b->next = node_b;
                                    node_b->next = NULL;                                //place new bloom node in the correct position
                                }
                            }

                            int flag1 = 0;
                            temp_v = virus_head;    

                            while(temp_v != NULL){
                                if(strcmp(temp_v->virus_name, node->virus) == 0){                   //if virus exists in virus list 
                                    if(strcmp(node->yesNo, "YES") == 0)                             //check if the id is vaccinated
                                        skiplist_insert(temp_v->vaccinated_persons, node->id);      //if yes insert it in the vaccinated skiplist of the virus
                                    else
                                        skiplist_insert(temp_v->not_vaccinated_persons, node->id);  //if not insert it in the not vaccinated skiplist of the virus
                                    
                                    flag1 = 1;
                                    break;
                                }
                                else
                                    temp_v = temp_v->next;
                            }

                            if(flag1 == 0){                                                          //if virus non existant in virus list yet       
                                virus* node_v = (virus*)malloc(sizeof(virus));
                                virus_push(node_v, node);                                           //create new virus node for the virus
                                temp_v = virus_head;
                                while(temp_v->next != NULL)
                                    temp_v = temp_v->next;
                                
                                temp_v->next = node_v;
                                node_v->next = NULL;                                                //place the virus node on the correct position

                                if(strcmp(node->yesNo, "YES") == 0)                                 //check if the id is vaccinated
                                    skiplist_insert(node_v->vaccinated_persons, node->id);          //if yes insert it in the vaccinated skiplist of the virus
                                else
                                    skiplist_insert(node_v->not_vaccinated_persons, node->id);      //if not insert it in the not vaccinated skiplist of the virus
                            }
                        }   
                    }

                }
                closedir(dr1);                         //close subdirectory
            }   
            free(filename);

            tempb = bloom_head;
            
            while(tempb != NULL){
                strcpy(k, "");
                char* word = (char*)malloc(strlen(tempb->virus) + strlen(tempb->bit_array) + 6);
                strcpy(word, "");
                strcpy(word, tempb->virus);
                strcat(word, "/");
                strcat(word, tempb->bit_array);
                strcat(word, "/");
                
                int s = 0; 
                        
                while(strlen(word) > s){
                    strncpy(k, word+s, bufferSize);
                    
                    if(write(writefd, k, bufferSize) < 0){                     //passes path to monitors piece by piece according to its size
                        perror("Monitor: Error in writing!"); exit(1);
                    }     
                    
                    s+=bufferSize;           
                }
                
                if(write(writefd, "@", bufferSize) < 0){                     //sends ! to travelmonitor, to know that monitor's ready for quests
                    perror("Monitor: Error in writing!"); exit(1);
                }

                free(word);

                tempb = tempb->next;
            }

            if(write(writefd, "!", bufferSize) < 0){                     //sends ! to travelmonitor, to know that monitor's ready for quests
                perror("Monitor: Error in writing!"); exit(1);
            }

        }
       
        if(sigflag == 2){
            
            sigflag = 0;

            FILE *fp1;
            char filename[20];
            sprintf(filename, "./log_file.%ld", (long)getpid());
            fp1 = fopen(filename, "w");                             //creates logfile

            if(fp1 == NULL){
                printf("Unable to create file.\n");
                exit(EXIT_FAILURE);
            }

            country* temp = c_head->next;
            while(temp != NULL){
                fputs(temp->country, fp1);
                fputs("\n", fp1);
                temp = temp->next;
            }                                                   //puts monitor's countries in logfile

            fputs("TOTAL TRAVEL REQUESTS:", fp1);
            fprintf(fp1,"%d",total_requests);
            fputs("\n", fp1);

            fputs("ACCEPTED:", fp1);
            fprintf(fp1,"%d", total_accepted);
            fputs("\n", fp1);

            fputs("REJECTED:", fp1);
            fprintf(fp1,"%d", total_rejected);              //puts statistics in logfile

            fclose(fp1);

        }

        if((r = read(readfd, q, bufferSize)) < 0){              //reads content
            if(errno == EINTR)
                continue;
            perror("Monitor: Error in reading!"); exit(1);
        }      
    
        q[r] = '\0';

        if(q[0] != '!')                                            //until the ! is sent
            strcat(quest, q);
        
        else{
            
            char** Command;

            int num, counter = 0, i = 0;

            /* travelRequest */
            if(strstr(quest, "travelRequest") != 0){
                
                while(quest[i] != '\0'){
                    if(quest[i] == ' ')
                        counter++;
                    i++;
                }

                if(counter == 5)
                    num = 6;   
                else
                    num = 0;                                       //used to check many cases of command
                
                if(num != 0){
                    Command = get_command_monitor(quest, num);
                    strcpy(quest, "");

                    total_requests++;
                
                    virus* temp = virus_head;
                    while(temp != NULL){
                        if(strcmp(temp->virus_name, Command[5]) == 0){
                            if(skiplist_search(temp->vaccinated_persons, Command[1]) == 1){
                                record* t = head;
                                while(t != NULL){
                                    if(strcmp(t->id, Command[1]) == 0 && strcmp(t->virus, Command[5]) == 0){
                                        char** date1 = get_date(t->date);
                                        char** date2 = get_date(Command[2]);
                                        char** date3 = (char**)malloc(sizeof(char*) * 60);          //accepted limit date for traveling

                                        for(int i = 0; i < 3; i++)
                                            date3[i] = (char*)malloc(sizeof(char) * 60);

                                        strcpy(date3[0], date1[0]);
                                        if(strcmp(date1[1], "06") > 0){
                                            strcpy(date3[1], "");
                                            sprintf(date3[1], "0%d", atoi(date1[1]) - 6);

                                            strcpy(date3[2], "");
                                            sprintf(date3[2], "%d", atoi(date1[2]) + 1);
                                        }
                                        else{
                                            strcpy(date3[1], "");
                                            sprintf(date3[1], "%d", atoi(date1[1]) + 6);

                                            strcpy(date3[2], "");
                                            sprintf(date3[2], "%d", atoi(date1[2]));
                                        }                                                       //creates date3 that is used for 6 months period checking
                                        
                                        //checks if date is suitable for traveling or not and sends the correct case in travel monitor
                                        if(atoi(date2[2]) > atoi(date3[2]) || atoi(date2[2]) < atoi(date1[2])){
                                            total_rejected++;  
                                                                   
                                            strcpy(msg, "");
                                            char* word = (char*)malloc(strlen("NO") + strlen("case1") + 6);
                                            strcpy(word, "");
                                            strcpy(word, "NO");
                                            strcat(word, "/");
                                            strcat(word, "case1");
                                            strcat(word, "/");
                                            
                                            int s = 0; 
                                                    
                                            while(strlen(word) > s){
                                                strncpy(msg, word+s, bufferSize);
                                                
                                                if(write(writefd, msg, bufferSize) < 0){                     //passes case to monitors piece by piece according to its size
                                                    perror("Monitor: Error in writing!"); exit(1);
                                                }     
                                                
                                                s+=bufferSize;           
                                            }                                                                                 
                                                                                   
                      
                                            if(write(writefd, "!", bufferSize) < 0){                            
                                                perror("Monitor: Error in writing!"); exit(1);
                                            }

                                            free(word);
                                        }

                                        else{
                                            if(atoi(date2[1]) == atoi(date3[1])){
                                                if(atoi(date2[0]) <= atoi(date3[0])){
                                                    total_accepted++;  
                                                    
                                                    strcpy(msg, "");
                                                    char* word = (char*)malloc(strlen("YES") + strlen(t->date) + 6);
                                                    strcpy(word, "");
                                                    strcpy(word, "YES");
                                                    strcat(word, "/");
                                                    strcat(word, t->date);
                                                    strcat(word, "/");
                                                    printf("%s\n", word);
                                                    
                                                    int s = 0; 
                                                            
                                                    while(strlen(word) > s){
                                                        strncpy(msg, word+s, bufferSize);
                                                        
                                                        if(write(writefd, msg, bufferSize) < 0){                     //passes case to monitors piece by piece according to its size
                                                            perror("Monitor: Error in writing!"); exit(1);
                                                        }     
                                                        
                                                        s+=bufferSize;           
                                                    }    
                                                    
                                                    if(write(writefd, "!", bufferSize) < 0){                            
                                                        perror("Monitor: Error in writing!"); exit(1);
                                                    }

                                                    free(word);                                                                             
                                                                                        
                                                }
                                                else{
                                                    total_rejected++;                                                    
                                                    
                                                    strcpy(msg, "");
                                                    char* word = (char*)malloc(strlen("NO") + strlen("case1") + 6);
                                                    strcpy(word, "");
                                                    strcpy(word, "NO");
                                                    strcat(word, "/");
                                                    strcat(word, "case1");
                                                    strcat(word, "/");
                                                    
                                                    int s = 0; 
                                                            
                                                    while(strlen(word) > s){
                                                        strncpy(msg, word+s, bufferSize);
                                                        
                                                        if(write(writefd, msg, bufferSize) < 0){                     //passes case to monitors piece by piece according to its size
                                                            perror("Monitor: Error in writing!"); exit(1);
                                                        }     
                                                        
                                                        s+=bufferSize;           
                                                    }                                                                                 
                                                                                        
                            
                                                    if(write(writefd, "!", bufferSize) < 0){                            
                                                        perror("Monitor: Error in writing!"); exit(1);
                                                    }

                                                    free(word);
                                                }
                                            }
                                            else if(atoi(date2[1]) < atoi(date3[1])){
                                                total_accepted++;                                                
                                                
                                                strcpy(msg, "");
                                                char* word = (char*)malloc(strlen("YES") + strlen(t->date) + 6);
                                                strcpy(word, "");
                                                strcpy(word, "YES");
                                                strcat(word, "/");
                                                strcat(word, t->date);
                                                strcat(word, "/");
                                                
                                                int s = 0; 
                                                        
                                                while(strlen(word) > s){
                                                    strncpy(msg, word+s, bufferSize);
                                                    
                                                    if(write(writefd, msg, bufferSize) < 0){                     //passes case to monitors piece by piece according to its size
                                                        perror("Monitor: Error in writing!"); exit(1);
                                                    }     
                                                    
                                                    s+=bufferSize;           
                                                }    
                                                
                                                if(write(writefd, "!", bufferSize) < 0){                            
                                                    perror("Monitor: Error in writing!"); exit(1);
                                                }

                                                free(word);  
                                            }
                                            else{
                                                total_rejected++;                                            
                                                
                                                strcpy(msg, "");
                                                char* word = (char*)malloc(strlen("NO") + strlen("case1") + 6);
                                                strcpy(word, "");
                                                strcpy(word, "NO");
                                                strcat(word, "/");
                                                strcat(word, "case1");
                                                strcat(word, "/");
                                                
                                                int s = 0; 
                                                        
                                                while(strlen(word) > s){
                                                    strncpy(msg, word+s, bufferSize);
                                                    
                                                    if(write(writefd, msg, bufferSize) < 0){                     //passes case to monitors piece by piece according to its size
                                                        perror("Monitor: Error in writing!"); exit(1);
                                                    }     
                                                    
                                                    s+=bufferSize;           
                                                }                                                                                 
                                                                                    
                        
                                                if(write(writefd, "!", bufferSize) < 0){                            
                                                    perror("Monitor: Error in writing!"); exit(1);
                                                }

                                                free(word);
                                            }
                                        }
                                        for(int i = 0; i < 3; i++){
                                            free(date1[i]); free(date2[i]); free(date3[i]); 
                                        }                                       
                                        free(date1); free(date2); free(date3);
                                    }
                                    t = t->next;  
                                }
                            }
                            else{
                                total_rejected++;
                                strcpy(msg, "");
                                char* word = (char*)malloc(strlen("NO") + strlen("case2") + 6);
                                strcpy(word, "");
                                strcpy(word, "NO");
                                strcat(word, "/");
                                strcat(word, "case2");
                                strcat(word, "/");
                                
                                int s = 0; 
                                        
                                while(strlen(word) > s){
                                    strncpy(msg, word+s, bufferSize);
                                    
                                    if(write(writefd, msg, bufferSize) < 0){                     //passes case to monitors piece by piece according to its size
                                        perror("Monitor: Error in writing!"); exit(1);
                                    }     
                                    
                                    s+=bufferSize;           
                                }                                                                                 
                                                                        
            
                                if(write(writefd, "!", bufferSize) < 0){                            
                                    perror("Monitor: Error in writing!"); exit(1);
                                }

                                free(word);
                            } 
                        }
                        temp = temp->next;
                    }
                    for(int i = 0; i < num; i++)
                        free(Command[i]);
                    free(Command);
                }

            }

            /* searchVaccinationStatus */
            if(strstr(quest, "searchVaccinationStatus") != 0){
            
                while(quest[i] != '\0'){
                    if(quest[i] == ' ')
                        counter++;
                    i++;
                }

                if(counter == 1)
                    num = 2;   
                else
                    num = 0;                                       //used to check many cases of command
                
                if(num != 0){
                    Command = get_command_monitor(quest, num);
                    strcpy(quest, "");

                    int flag = 0;
                    record* temp = head->next;
                    while(temp != NULL){
                        if(strcmp(temp->id, Command[1]) == 0){
                            //sends info of citizenID in travelmonitor
                            flag = 1;
                            strcpy(k, "");
                            char* word = (char*)malloc(strlen(temp->id) + strlen(temp->firstName) + strlen(temp->lastName) +
                            strlen(temp->country) + strlen(temp->age) + 20);
                            strcpy(word, "");
                            strcpy(word, temp->id); strcat(word, " "); strcat(word, temp->firstName); strcat(word, " ");
                            strcat(word, temp->lastName); strcat(word, " "); strcat(word, temp->country); strcat(word, "\n");
                            strcat(word, "AGE ");strcat(word, temp->age);
                                                                                   
                            int s = 0; 
                                    
                            while(strlen(word) > s){
                                strncpy(k, word+s, bufferSize);
                                
                                if(write(writefd, k, bufferSize) < 0){                     //passes info to monitors piece by piece according to its size
                                    perror("Monitor: Error in writing!"); exit(1);
                                }     
                                
                                s+=bufferSize;           
                            }
                            
                            free(word);

                            if(write(writefd, "\n", bufferSize) < 0){                     //sends ! to travelmonitor, to know that monitor's ready for quests
                                perror("Monitor: Error in writing!"); exit(1);
                            }                            

                            record* t = head->next;
                            while(t != NULL){
                                if(strcmp(temp->id, t->id) == 0){
                                    //sends info for every virus a citizenID was vaccinated for or not in travelmonitor
                                    if(strcmp(t->yesNo, "YES") == 0){
                                        strcpy(k, "");
                                        char* word = (char*)malloc(strlen(t->virus) + strlen(t->date) + 25);
                                        strcpy(word, "");
                                        strcpy(word, t->virus); strcat(word, " VACCINATED ON "); strcat(word, t->date);
                                                                                  
                                        int s = 0; 
                                                
                                        while(strlen(word) > s){
                                            strncpy(k, word+s, bufferSize);
                                            
                                            if(write(writefd, k, bufferSize) < 0){                     //passes info to monitors piece by piece according to its size
                                                perror("Monitor: Error in writing!"); exit(1);
                                            }     
                                            
                                            s+=bufferSize;           
                                        }
                                        
                                        free(word);

                                        if(write(writefd, "\n", bufferSize) < 0){                     //sends ! to travelmonitor, to know that monitor's ready for quests
                                            perror("Monitor: Error in writing!"); exit(1);
                                        }
                                        
                                    }
                                    else{    
                                        strcpy(k, "");
                                        char* word = (char*)malloc(strlen(t->virus) + 25);
                                        strcpy(word, "");
                                        strcpy(word, t->virus); strcat(word, " NOT YET VACCINATED");
                                                                                                                           
                                        int s = 0; 
                                                
                                        while(strlen(word) > s){
                                            strncpy(k, word+s, bufferSize);
                                            
                                            if(write(writefd, k, bufferSize) < 0){                     //passes info to monitors piece by piece according to its size
                                                perror("Monitor: Error in writing!"); exit(1);
                                            }     
                                            
                                            s+=bufferSize;           
                                        }
                                        
                                        free(word);

                                        if(write(writefd, "\n", bufferSize) < 0){                     //sends ! to travelmonitor, to know that monitor's ready for quests
                                            perror("Monitor: Error in writing!"); exit(1);
                                        }
                                        
                                    }
                                }
                                t = t->next;
                            }

                            if(write(writefd, "!", bufferSize) < 0){                     //sends ! to travelmonitor, to know that monitor's ready for quests
                                perror("Monitor: Error in writing!"); exit(1);
                            }

                            break;
                        }
                        temp = temp->next;
                    }

                    if(flag == 0){
                        if(write(writefd, "!", bufferSize) < 0){                     //sends ! to travelmonitor, to know that monitor's ready for quests
                            perror("Monitor: Error in writing!"); exit(1);
                        }
                    }

                    for(int i = 0; i < num; i++)
                        free(Command[i]);
                    free(Command);
                }
            }
        }

    }

    /////////////////////////////////////// free memory and unlink pipes ////////////////////////////////////////////////

    if (unlink(r_fifo) < 0) {
        perror("Monitor: Can't unlink!"); exit(1);
    }

    if (unlink(w_fifo) < 0) {
        perror("Monitor: Can't unlink!"); exit(1);
    }

    free_record(head);
    free_virus(virus_head);
    free_txt(t_head);
    free_country(c_head);
    bloom_free(bloom_head);
    
    free(paths); free(message); free(quest);
    free(r_fifo); free(w_fifo);

    close(readfd);
    close(writefd);    

}