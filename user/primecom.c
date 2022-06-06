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

	char buff[15];
	char buffc[15];
	int n;
	char comm1[10] = "prime ";
	char comm2[10] = "latest";
	char comm3[10] = "pause";
	char comm4[10] = "resume";
	char comm5[10] = "end";
	
	int flags = get_shared("brojac", &brojac);
	int flags1 = get_shared("niz", &niz);
	int flags2 = get_shared("indikator", &indikator);

	//printf("%d %d %d\n", flags, flags1, flags2);
	
	if (flags == -1 || flags1 == -1 || flags2 == -1)
	{
		printf("primestart: bad parameters\n");
		exit();
	}
	else if (flags == -2 || flags1 == -2 || flags2 == -2)
	{
		printf("primestart: no shared with provided name\n");
		exit();
	}

	for(;;)
	{
		n = read(0, buff, sizeof(buff));
		buff[n-1] = '\0';
		int i = 0;
		for (i = 0; i < 6; i++)
			buffc[i] = buff[i];
		if (n<=0)continue;
		//write(1, buff, n);
		//write(1, comm1, n);

		if (strcmp(buffc, comm1) == 0)
		{
			//prime
			char num[10];
			int j = 0;
			
			while(buff[i] != '\0')
			{
				num[j] = buff[i];
				j++; 
				i++;
			}
			num[j] = buff[i];
			int anum;
			anum = atoi(num);

			if (anum > *brojac)
			{
				printf("Prime %d. not calculated yet, latest prime index: %d.\n", anum, *brojac-1);
			}
			else
			{
				printf("Prime no %d is %d\n", anum, niz[anum]);
			}
		}
		if (strcmp(buff, comm2) == 0)
		{
			//latest
			printf("Latest prime: %d, at index %d\n", niz[*brojac-1], *brojac-1);
		}
		if (strcmp(buff, comm3) == 0)
		{
			//pause
			*indikator = 1;
		}
		if (strcmp(buff, comm4) == 0)
		{
			//resume
			*indikator = 2;
			//printf("resume: %p\n", &indikator);
			legit_wakeup(0x40000000);
		}
		if (strcmp(buff, comm5) == 0)
		{
			//end
			*indikator = 3;
			break;
		}
	}
	exit();
}
