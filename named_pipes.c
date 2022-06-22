#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>  
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "named_pipes.h"

void create_pipes(int numMonitors){
    
    char* r_fifo = (char*)malloc(sizeof(char) * 60);
    char* w_fifo = (char*)malloc(sizeof(char) * 60);
    
    for(int i = 0; i < numMonitors; i++){
        
        strcpy(r_fifo, "");
        strcpy(w_fifo, "");

        sprintf(r_fifo, "/tmp/fifo.%d", (2*i) );
	    sprintf(w_fifo, "/tmp/fifo.%d", (2 * i) + 1);                            //creates names for pipes
        
        if((mkfifo(r_fifo, 0666)) == -1 && (errno != EEXIST)){      //checks for error in creating pipe and for existant pipe
            perror("Error creating fifo!");
            exit(1);
        }

        if((mkfifo(w_fifo, 0666)) == -1 && (errno != EEXIST)){      //checks for error in creating pipe and for existant pipe
            unlink(r_fifo);
            perror("Error creating fifo!");
            exit(1);
        }

    }

    free(r_fifo); free(w_fifo);

}

void delete_pipes(int numMonitors){
    
    char* r_fifo = (char*)malloc(sizeof(char) * 60);
    char* w_fifo = (char*)malloc(sizeof(char) * 60);

    for(int i = 0; i < numMonitors; i++){
        
        strcpy(r_fifo, "");
        strcpy(w_fifo, "");
        
        sprintf(r_fifo, "/tmp/fifo.%d", (2*i) );
	    sprintf(w_fifo, "/tmp/fifo.%d", (2 * i) + 1);                            //creates names for pipes

        if(remove(r_fifo) != 0){ 
            perror("Error deleting fifo!"); 
            exit(1);
        };                 
        
        if(remove(w_fifo) != 0){ 
            perror("Error deleting fifo!"); 
            exit(1);
        };      
    }

    free(r_fifo); free(w_fifo);
}