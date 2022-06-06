#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"

#define NIZLEN 100000

int *niz;
int *brojac;
int *indikator;

int
main(int argc, char *argv[])
{
	niz = malloc(sizeof(int)*NIZLEN);

	int flags;
	int flags1;
	int flags2;

	int i;
	int num = 2;

	//printf("%d\n", *brojac);
	flags = get_shared("brojac", &brojac);
	//printf("%d\n", *brojac);
	flags1 = get_shared("niz", &niz);
	flags2 = get_shared("indikator", &indikator);

	//printf("%d %d %d\n", flags, flags1, flags2);

	//printf("%p", brojac);

	if (flags == -1 || flags1 == -1 || flags2 == -1)
	{
		printf("primecalc: bad parameters\n");
		exit();
	}
	else if (flags == -2 || flags1 == -2 || flags2 == -2)
	{
		printf("primecalc: no shared with provided name\n");
		exit();
	}

	
	while (*brojac < 100000)
	{
		if (*indikator == 1)
		{
			//printf("pause: %p\n", &indikator);
			legit_sleep(0x40000000);
		}
		else if (*indikator == 2)
		{
			//legit_wakeup(0x40000000);
		}
		else if (*indikator == 3)
		{
			break;
		}

		int prime = 1;

		for (i = 2; i <= (num / 2); i++)
			if (num % i == 0)	
				prime = 0;	
		if (prime)
		{
			niz[*brojac] = num;
			*brojac = *brojac + 1;
		}	
		num++;
	}
	exit();
}
