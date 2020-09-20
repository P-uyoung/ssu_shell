#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line);

int main(int argc, char* argv[]) 
{
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i;

	FILE* fp;
	if(argc == 2) 
	{
		fp = fopen(argv[1],"r");
		if(fp == NULL) 
		{
			printf("File doesn't exists.");
			
			return -1;
		}
	}

	while(1) 
	{			
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		if(argc == 2)	// batch mode
		{ 
			if(fgets(line, sizeof(line), fp) == NULL) { // file reading finished
				break;	
			}
			line[strlen(line) - 1] = '\0';
		} 
		else	// interactive mode
		{ 
			printf("$ ");
			scanf("%[^\n]", line);
			getchar();
		}
//		printf("Command entered: %s (remove this debug output later)\n", line);
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);
   
       //do whatever you want with the commands, here we just print them

/*		for(i=0;tokens[i]!=NULL;i++)
		{
			printf("found token %s (remove this debug output later)\n", tokens[i]);
		}
*/
		// fork() > exec() > wait()
		pid_t pid;
		int status;
		char path[MAX_TOKEN_SIZE] = {0};	// /bin 내장명령어 경로
		char usr_path[MAX_TOKEN_SIZE] = {0};	// /usr/bin 내장명령어 경로

		if ((pid = fork()) < 0) {
			fprintf(stderr, "fork error\n"); 
			exit(1);
		}

		else if (pid == 0)
		{	
			sprintf(path,"/bin/%s", tokens[0]);
//			printf("실행파일 경로명 %s\n", path);
			
			if (execv(path, tokens) < 0) 
			{
				if(tokens[0][0] == '\n')
					exit(1);

				else 
				{
					sprintf(usr_path, "/usr%s", path);

					if (execv(usr_path, tokens) < 0)
					{
						printf("SSUShell : Incorrect command\n");
						exit(1);
					}
				}
			}
		}

		else
		{
			wait(&status);	// 자식프로세스 회수
//			printf("자식 프로세스 회수 완료\n");
			// ...
		}

		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

	}
	return 0;
}

char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++)
  {
    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t')
	{
      token[tokenIndex] = '\0';
      if (tokenIndex != 0)
	  {
		tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
		strcpy(tokens[tokenNo++], token);
		tokenIndex = 0; 
      }
    } 
	else 
	{
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  
  return tokens;
}

