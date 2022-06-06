#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"

#define NIZLEN 100000

int *niz;
int brojac;
int indikator;

int
main(int argc, char *argv[])
{
	niz = malloc(sizeof(int)*NIZLEN);
	brojac = 0;
	
	//printf("%p in primestart\n", &niz);
	
	share_mem("niz", niz, NIZLEN * sizeof(int));
	share_mem("brojac", &brojac, sizeof(brojac));
	share_mem("indikator", &indikator, sizeof(indikator));
	

	int pid;
	int pid1;
	char *argv2[] = {"/bin/primecom",0};
	char *argv3[] = {"/bin/primecalc",0};
	
	pid = fork();
	if (pid == 0)
	{
		exec("/bin/primecom",argv2);
	}
	else
	{
		//printf("pid %d\n", pid);
		pid1 = fork();
		if (pid1 == 0)
		{
			exec("/bin/primecalc",argv3);
		}
		else
		{
			wait();
			//printf("pid1 %d\n", pid1);
			
		}
		//printf("ovde\n");
		wait();
	}
	printf("primestart");
	exit();
}
