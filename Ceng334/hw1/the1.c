#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h> 
#include<sys/wait.h> 

#define MAX_LIMIT 1024
int main(int argc, char const *argv[]){

	int numOfMappers = atoi(argv[1]);
	pid_t pid = 1;
	int buffer[1024];
	char line[MAX_LIMIT];
	
	
	if (argc == 3){
		int pid_a = 0;
		int **pipeArray=(int**)malloc(sizeof(int*)*numOfMappers);
		for(int i=0;i<numOfMappers;i++){
			pipeArray[i]=(int*)malloc(sizeof(int)*2);
			pipe(pipeArray[i]);}
		int i = 0;
		while(1) {

			if ( i < numOfMappers ) {	
				if( pid > 0){
				//	printf("-------------- FORK ----------------\n");
					pid_a =i;
					pid = fork();
					if (pid < 0 ){
						printf("Fork Error" );
					} else if (pid >0 ){
				//		printf("-------------- CLOSE PARENT ----------------\n");
						close(pipeArray[i][0]);
					} else {
				//		printf("-------------- DUP 2 CLOSE CHILD ----------------\n");
						close(pipeArray[i][1]);
						dup2(pipeArray[i][0],0);
					}
				}		
			} else {
				break;
			}
			i++;
		}
		i=0;
		if (pid > 0){
			while(fgets(line,MAX_LIMIT,stdin)){		
				//printf("-------------- WRITE ----------------\n");
				write(pipeArray[i%numOfMappers][1],line, strlen(line));
				i++;
			}		
			
		}
		if (pid == 0){
			for (int j = 0; j < numOfMappers; j++ ){
				//printf("--------------CLOSE CHILD2   ----------------\n");
				close(pipeArray[j][0]);	
				close(pipeArray[j][1]);			
			}		
			char *id= (char*)malloc(sizeof(char)*numOfMappers);
			sprintf(id,"%d",pid_a);
			//printf("%s\n",id );
			execl(argv[2],argv[2],id,NULL);
		}


		if(pid > 0 ){
			
			for (int j = 0; j < numOfMappers; j++ ){
				//printf("--------------CLOSE PARENT'2   ----------------\n");
				close(pipeArray[j][1]);	
				close(pipeArray[j][0]);			
			}
		}
		for (i=0;i< numOfMappers;i++)
				wait(NULL);
	}
	else if (argc == 4){
		bool hasChildForkedBefore = false;
		bool isLastChild = false;
		bool isMiddleChild = false;

		pid_t pid_1 = 0;

		int **pipeArrayP_C=(int**)malloc(sizeof(int*)*numOfMappers);
		for(int i=0;i<numOfMappers;i++){
			pipeArrayP_C[i]=(int*)malloc(sizeof(int)*2);
			pipe(pipeArrayP_C[i]);}

		int **pipeArrayC_C_1=(int**)malloc(sizeof(int*)*numOfMappers);
		for(int i=0;i<numOfMappers;i++){
			pipeArrayC_C_1[i]=(int*)malloc(sizeof(int)*2);
			pipe(pipeArrayC_C_1[i]);}

		int **pipeArrayC_C_2=(int**)malloc(sizeof(int*)*(numOfMappers-1));
		for(int i=0;i<numOfMappers;i++){
			pipeArrayC_C_2[i]=(int*)malloc(sizeof(int)*2);
			pipe(pipeArrayC_C_2[i]);}

		
		pid_t pid = 1;
		int i = 0;
		while(1) {

			if ( i < numOfMappers ) {	
				if( pid > 0) {
					//printf("-------------- FORK ---------------- %d \n", i);
					pid = fork();
					
					if (pid < 0 ){
						printf("Fork Error" );
					} else if (pid >0 ){
						close(pipeArrayP_C[i][0]);
						pid_1 = i;
							//printf("--------------SECOND FORsadsadsadK ---------------- %d \n", i);
							pid = fork();

							if(pid < 0 ){
								printf("Fork Error" );
							}
							else if (pid == 0 ){
								//printf("--------------LASTC CHILD---------------- %d \n", i);
								isLastChild = true;
								close(pipeArrayC_C_1[i][1]);
								dup2(pipeArrayC_C_1[i][0],0);
								
								if(i == numOfMappers-1){
									close(pipeArrayC_C_2[i-1][1]);
									dup2(pipeArrayC_C_2[i-1][0],2);
								}
								else{
									close(pipeArrayC_C_2[i][0]);
									dup2(pipeArrayC_C_2[i][1],1);
									if( i != 0){
										close(pipeArrayC_C_2[i-1][1]);
										dup2(pipeArrayC_C_2[i-1][0],2);
									}
								}
									
									
								

							}
					} else if(pid == 0){
						pid_1 = i;
						isMiddleChild = true;
						close(pipeArrayP_C[i][1]);
						dup2(pipeArrayP_C[i][0],0); //piptakini stdine koydu
						
						//printf("--------------MIDDLE CHILD ---------------- %d %d\n", isMiddleChild,i);
						close(pipeArrayC_C_1[i][0]);	
						dup2(pipeArrayC_C_1[i][1],1);					
					}
				}		
			} else {
				break;
			}
			i++;
		}
		
		//printf("%d %d\n",isMiddleChild,isLastChild );
		i=0;
		if (pid > 0){
			while(fgets(line,MAX_LIMIT,stdin)){		
				//printf("-------------- WRITE ----------------\n");
				write(pipeArrayP_C[i%numOfMappers][1],line, strlen(line));
				i++;
			}		
			
		}
		int st;
		

		if (isMiddleChild){
			int j = 0;
			//printf("--------------CLOSE from Middle Child   ----------------%d\n",j);
			for (; j < numOfMappers-1; j++ ){
				
				close(pipeArrayP_C[j][0]);	
				close(pipeArrayP_C[j][1]);

				close(pipeArrayC_C_1[j][0]);	
				close(pipeArrayC_C_1[j][1]);

				close(pipeArrayC_C_2[j][0]);	
				close(pipeArrayC_C_2[j][1]);
					
			}	
			close(pipeArrayP_C[j][0]);	
			close(pipeArrayP_C[j][1]);

			close(pipeArrayC_C_1[j][0]);	
			close(pipeArrayC_C_1[j][1]);	
			//printf("--------------CLOSE from Middle Child   ----------------%d\n",pid_1);
			char *id= (char*)malloc(sizeof(char)*numOfMappers);
			sprintf(id,"%d",pid_1);
			//printf("%d\n",pid_1 );
			execl(argv[2],argv[2],id,NULL);	
			
		}
		if (isLastChild){
			int j= 0;
			for (; j < numOfMappers-1; j++ ){
				//printf("--------------CLOSE From last Child   ----------------\n");
				close(pipeArrayP_C[j][0]);	
				close(pipeArrayP_C[j][1]);	

				close(pipeArrayC_C_2[j][0]);	
				close(pipeArrayC_C_2[j][1]);

				close(pipeArrayC_C_1[j][0]);	
				close(pipeArrayC_C_1[j][1]);		
			}			
			close(pipeArrayP_C[j][0]);	
			close(pipeArrayP_C[j][1]);	

			close(pipeArrayC_C_1[j][0]);	
			close(pipeArrayC_C_1[j][1]);

			char *id= (char*)malloc(sizeof(char)*numOfMappers);
			sprintf(id,"%d",pid_1);
			execl(argv[3],argv[3],id,NULL);	
		}
		
		if( pid > 0){
			int j =0 ;
			for (; j < numOfMappers-1; j++ ){
				close(pipeArrayP_C[j][0]);	
				close(pipeArrayP_C[j][1]);	

				close(pipeArrayC_C_2[j][0]);	
				close(pipeArrayC_C_2[j][1]);
				
				close(pipeArrayC_C_1[j][0]);	
				close(pipeArrayC_C_1[j][1]);
			}
			close(pipeArrayP_C[j][0]);	
			close(pipeArrayP_C[j][1]);	
			
			close(pipeArrayC_C_1[j][0]);	
			close(pipeArrayC_C_1[j][1]);
			
		}
		//sleep(1);
		int j;
		for (j=0;j< numOfMappers*2;j++){
			wait(NULL);

		}

	} 
		
	return 0;
}
