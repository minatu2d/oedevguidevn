/*
 * p_hello.c -- a hello program (in pthread)
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>

#define MAX_THREAD 1000
#define CURRENT_TERMINAL_CMD "tty"

typedef struct {
	int id;
	char *terminal;
} parm;

void *hello(void *arg)
{
	parm *p=(parm *)arg;
	FILE *fo = fopen(p->terminal, "wt");
	if (fo == NULL)
	{
		printf("Using stdout as output terminal\n");
		printf("[Node %d] : Hello các anh\n", p->id);
		sleep (p->id * 2);
		printf("[Node %d] : Em kết thúc đây...\n",p->id);
		return ;
	}
	
	fprintf(fo,"Using %s as output terminal\n",p->terminal);
	fprintf(fo,"[Node %d] : Hello các anh\n", p->id);
	sleep (p->id * 2);
	fprintf(fo,"[Node %d] : Em kết thúc đây...\n",p->id);
	
	fclose(fo);
	
	return (NULL);
}

int getCurrentTerminal(char *terminal)
{
	int retVal = 0;
	
	FILE *ttyCmdFo = popen(CURRENT_TERMINAL_CMD,"r");
	if (ttyCmdFo == NULL)
	{
			retVal = -1;
			return retVal;
	}
	
	if (fscanf(ttyCmdFo,"%s",terminal) != 0)
	{
			retVal = -1;
			return retVal;
	}
	
	printf("Terminal :%s",terminal);
	
	return retVal;
}

typedef char T_TERM_NAME[100];
void main(int argc, char* argv[]) {
	int n,i;
	pthread_t *threads;
	parm *p;
	char currentTerminal[100];
	T_TERM_NAME terminals[10];
	char tmpTerm[100];

	if (argc != 2)
	{
		printf ("Usage: %s n\n  N là số threads được tạo, xin nhập lại đi.\n",argv[0]);
		exit(1);
	}

	n=atoi(argv[1]);

	if ((n < 1) || (n > MAX_THREAD))
	{
		printf ("Số lượng thread phải >=1 và < %d.\n", MAX_THREAD);
		exit(1);
	}

	threads=(pthread_t *)malloc(n*sizeof(*threads));

	p=(parm *)malloc(sizeof(parm)*n);

	/* Tạo và bắt đầu các threads */
	getCurrentTerminal(&currentTerminal[0]);
	
	for (i=0; i<n; i++)
	{
		printf("Đầu ra của thread %d (default =%s) : ",i, currentTerminal);
		gets(tmpTerm);
		if (strlen(tmpTerm) == 0)
		{
			strcpy(terminals[i], currentTerminal);
		}
		else
		{
			strcpy(terminals[i], tmpTerm);
		}
	}
	
#if 0
	for (i=0; i<n; i++)
	{
		printf("Thread : %s\n",terminals[i]);
	}
#endif

	for (i=0; i<n; i++)
	{
		p[i].id=i;
		p[i].terminal = terminals[i];
		pthread_create(&threads[i], NULL, hello, (void *)(p+i));
	}

	/* Đợt để các thread được tạo cùng kết thúc. */

	for (i=0; i<n; i++)
	{
		printf("[Main] : Đang đợi [Node %d] kết thúc \n",i);
		pthread_join(threads[i],NULL);
		printf("[Main] : [Node %d] đã kết thúc\n",i);
	}

	free(p);
}
