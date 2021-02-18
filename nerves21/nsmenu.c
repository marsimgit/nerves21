#pragma warning(disable : 4996)

#include "defn.h"
#include "nsdata.c"
#include "proto.h"
#include "nsmenu.h"

char* p;
FILE *file;
struct neuron* np;
struct con* cp;
struct Iint* ip;
int msg;
int i, j, k, done;
enum asc_val asc;
char str[41];

void nsmenu(void)
{ /* main menu */


	msg = FALSE;
  
	/* display menu */
	
	if (msg)
	{
		printf("Recording path not found, Press P to change\n");
		msg = FALSE;
	}
	
	printbanner();
	printf("[R]ead files\n");
	printf("[W]rite files\n");
	printf("[N]euron data modification\n");
	printf("[E]nvironment specification\n");
	printf("[I]nitialize conditions\n");
	printf("[G]raph specification\n");
	printf("[P]ath specification for recording\n");
	printf("[S]imulate\n");
	printf("[Q]uit\n");


	//if (neurfname[0])
	//	printf("Neuron file: %s", neurfname);

	//if (envfname[0])
	//	printf("Environment file: %s", envfname);

	//if (recpath[0])
	//	printf("Recording path: %s", recpath);
	//erasecursor();

	///* get choice */
	done = FALSE;
	while (!done)
	{
		i = toupper(getchar());

		switch (i)
		{
			case 'R': /* read files */
				readfiles();
				break;
			case 'W': /* write files */
				writefiles();
				break;
			case 'N': /* neuron data modification */
				//datamod();
				break;
			case 'E': /* environment set up */
				envset();
				break;
			case 'I': /* initial conditions */
				initcond();
				break;
			case 'G': /* graph specification */
				grspec();
				break;
			case 'P': /* path for recording */
				pathrec();
				break;
			case 'S': /* simulate */
				msg = initsim();				
				break;
			case 'Q': /* quit */
				done = TRUE;
				break;
			default:
				break;
		}
	}
}

void erasecursor(void)
{
	/*
	union REGS regs;

	regs.x.bx = 0;
	regs.x.dx = 0x1900;
	regs.x.ax = 0x200;
	int86(0x10, &regs, &regs);
	*/
}

void clrline(int n)
{

}

void readfiles() {
	//	start:	/* get the neuron filespec & read in */

	printf("Enter neuron file spec:\n");

	p = "app3.neu";
	printf("%s\n", p);

	errno_t err;
	err = fopen_s(&file, p, "rb");
	if (err == 0)
	{
		printf("The file '' was opened\n");
		fread(&nn, 2, 1, file);
 		fread(ns, sizeof(struct neuron), nn, file);

		fread(&ni, 2, 1, file);
		fread(Iinta, sizeof(struct Iint), ni, file);

		fread(&nc, 2, 1, file);
		fread(cona, sizeof(struct con), nc, file);
		fclose(file);

		printf("------------------------------\n");
		printf("NEURONS INFO\n");
		printf("Number of neurons                  [nn]: \t %d \n", nn);
		printf("Number of intrinsic current blocks [ni]: \t %d \n", ni);
		printf("Number of connections              [nc]: \t %d \n", nc);
		printf("------------------------------\n");

		/* change pointers */
		for (j = 0, np = ns; j < nn; j++, np++)
		{
			if (np->Iint != NULL)
				(int)np->Iint = (int)Iinta + (int)np->Iint - 1;
			if (np->con != NULL)
				(int)np->con = (int)cona + (int)np->con - 1;
		}

		for (j = 0, cp = cona; j < nc; j++, cp++)
		{
			if (cp->next != NULL)
				(int)cp->next = (int)cona + (int)cp->next - 1;
		}
		
		strcpy(neurfname, p);

		printf("Neuron file: %s ", neurfname);

	}
	else
	{
		printf("The file '' was not opened\n");
	}



  printf("Enter environment file spec:\n");

	p = "app3.env";
	printf("%s\n", p);

	err = fopen_s(&file, p, "rb");
	if (err == 0)
	{
		printf("The file '' was opened\n");
		fread(&bug.x, sizeof(float), 1, file);
		fread(&bug.y, sizeof(float), 1, file);
		fread(&bug.ang, sizeof(float), 1, file);
		fread(&nfood, 2, 1, file);
		fread(foodx, 2, NFOOD, file);
		fread(foody, 2, NFOOD, file);
		fread(foodsize, 2, NFOOD, file);
		fread(&nblock, 2, 1, file);
		fread(blockx, 2, NBLOCK, file);
		fread(blocky, 2, NBLOCK, file);
		fread(gr, sizeof(struct graph), 5, file);
		fclose(file);
		strcpy(envfname, p);
		printf("Environment file: %s ", envfname);
	}
	else
	{
		printf("The file '' was not opened\n");
	}
}

void writefiles()
{
	//	case 'W': /* write files */
//		gotoxy(1, 1);
//		cputs("Write neuron file? ");
//		if (toupper(bioskey(0) & 0xff) != 'Y')
//			goto wenvfile;
//		/* write neuron file */
//		/* count neurons */
//		for (i = 0, j = 0, np = ns; i < nn; i++, np++)
//			if (np->name[0])
//				j++;
//		gotoxy(1, 1);
//		if (neurfname[0])
//		{
//			cprintf("Save neurons to %s? ", neurfname);
//			i = toupper(bioskey(0) & 0xff);
//		}
//		if (!neurfname[0] || i != 'Y')
//		{
//			gotoxy(1, 1);
//			cputs("Enter file spec:                                      ");
//			gotoxy(18, 1);
//			str[0] = 38;
//			p = cgets(str);
//			strcpy(neurfname, p);
//		}
//		file = fopen(neurfname, "wb");
//		fwrite(&j, 2, 1, file);
//		for (i = 0, np = ns; i < nn; i++, np++)
//			if (np->name[0])
//			{
//				fwrite(np, 27, 1, file);
//				if (np->Iint)
//					j = (int)np->Iint - (int)Iinta + 1;
//				else
//					j = 0;
//				fwrite(&j, 2, 1, file);
//				fwrite(&(np->Isens), 18, 1, file);
//				if (np->con)
//					j = (int)np->con - (int)cona + 1;
//				else
//					j = 0;
//				fwrite(&j, 2, 1, file);
//			}
//		fwrite(&ni, 2, 1, file);
//		fwrite(Iinta, sizeof(struct Iint), ni, file);
//		fwrite(&nc, 2, 1, file);
//		for (i = 0, cp = cona; i < nc; i++, cp++)
//		{
//			fwrite(cp, sizeof(struct con) - 2, 1, file);
//			if (cp->next)
//				j = (int)cp->next - (int)cona + 1;
//			else
//				j = 0;
//			fwrite(&j, 2, 1, file);
//		}
//		fclose(file);
//	wenvfile: /* write environment file */
//		gotoxy(1, 1);
//		cputs("Write environment file?                                    ");
//		gotoxy(1, 25);
//		if (toupper(bioskey(0) & 0xff) != 'Y')
//			goto display;
//		gotoxy(1, 1);
//		if (envfname[0])
//		{
//			cprintf("Save environment to %s? ", envfname);
//			j = toupper(bioskey(0) & 0xff);
//		}
//		if (!envfname[0] || j != 'Y')
//		{
//			gotoxy(1, 1);
//			cputs("Enter file spec:                                      ");
//			gotoxy(18, 1);
//			str[0] = 38;
//			p = cgets(str);
//			strcpy(envfname, p);
//		}
//		file = fopen(envfname, "wb");
//		fwrite(&bug.x, sizeof(float), 1, file);
//		fwrite(&bug.y, sizeof(float), 1, file);
//		fwrite(&bug.ang, sizeof(float), 1, file);
//		fwrite(&nfood, 2, 1, file);
//		fwrite(foodx, 2, NFOOD, file);
//		fwrite(foody, 2, NFOOD, file);
//		fwrite(foodsize, 2, NFOOD, file);
//		fwrite(&nblock, 2, 1, file);
//		fwrite(blockx, 2, NBLOCK, file);
//		fwrite(blocky, 2, NBLOCK, file);
//		fwrite(gr, sizeof(struct graph), 5, file);
//		fclose(file);
//		goto display;
}

void envset()
{
	printf("Enter number of food patches (0 - %d): ", NFOOD);
	scanf_s("%d", &nfood);
	printf("%d\n", nfood);
		
	for (int k = 0; k < nfood; k++)
	{
		printf("Enter patch's x coordinate (0 - 956): ");
		scanf_s("%d", &foodx[k]);
		printf("Enter patch's y coordinate (0 - 902): ");
		scanf_s("%d", &foody[k]);
		printf("Enter size of patch (0 - 5000): ");
		scanf_s("%d", &foodsize[k]);
	}
		
	printf("Enter number of blocks (0 - %d): ", NBLOCK);
	scanf_s("%d", &nblock);
	
	for (int k = 0; k < nblock; k++)
	{
		printf("Block #%d", k + 1);
		printf("Enter block's x coordinate (0 - 760): ");
		scanf_s("%d",&blockx[k]);
		
		printf("Enter block's y coordinate (0 - 805): ");
		scanf_s("%d", &blocky[k]);
	}

}

void initcond()
{
	printf("Enter x coordinate of bug (80 - 880): ");
	scanf_s("%f", &bug.x);
	printf("%f\n", bug.x);

	printf("Enter y coordinate of bug (80 - 820): ");
	scanf_s("%f", &bug.y);
	printf("%f\n", bug.y);

	printf("Enter bug angle (0 - 359): ");
	scanf_s("%f", &bug.ang);
	printf("%d\n",(int)(180. * bug.ang / PI));
	//		bug.ang = PI * j / 180;

	printf("Enter energy (100 - 999): ");
	scanf_s("%f", &energy);
	printf("%f\n", energy);
}


void grspec()
{
	//		for (k = 0; k < 5; k++)
	//		{
	//			gotoxy(1, 1);
	//			cprintf("Graph #%d", k + 1);
	//		gtent:
	//			gotoxy(1, 2);
	//			cprintf("Enter graph type (0=None, 1=Variable, 2=Gait): %d   ", gr[k].gtype);
	//			gotoxy(48, 2);
	//			j = 0;
	//			while (!j)
	//				j = bioskey(1);
	//			asc = j & 0xff;
	//			if (asc == CR)
	//			{
	//				bioskey(0);
	//				if (gr[k].gtype != 1)
	//					goto grend;
	//				goto gneur;
	//			}
	//			else if (asc == ESC)
	//			{
	//				bioskey(0);
	//				goto display;
	//			}
	//			gotoxy(48, 2);
	//			cputs("      ");
	//			gotoxy(48, 2);
	//			str[0] = 2;
	//			p = cgets(str);
	//			j = atoi(p);
	//			if (j < 0 || j > 2)
	//				goto gtent;
	//			gr[k].gtype = j;
	//			gotoxy(48, 2);
	//			cprintf("%d", gr[k].gtype);
	//			if (gr[k].gtype != 1)
	//				goto grend;
	//		gneur:
	//			gotoxy(1, 3);
	//			cprintf("Enter neuron name: %s       ", gr[k].gneur);
	//			gotoxy(20, 3);
	//			j = 0;
	//			while (!j)
	//				j = bioskey(1);
	//			asc = j & 0xff;
	//			if (asc == CR)
	//			{
	//				bioskey(0);
	//				goto gvar;
	//			}
	//			else if (asc == ESC)
	//			{
	//				bioskey(0);
	//				goto display;
	//			}
	//			gotoxy(20, 3);
	//			cputs("      ");
	//			gotoxy(20, 3);
	//			str[0] = 7;
	//			p = cgets(str);
	//			strcpy(gr[k].gneur, p);
	//		gvar:
	//			gotoxy(1, 4);
	//			cprintf("Enter variable (0=V [milliV], 1=F, 2=I [pA]): %d", gr[k].gvar);
	//			gotoxy(47, 4);
	//			j = 0;
	//			while (!j)
	//				j = bioskey(1);
	//			asc = j & 0xff;
	//			if (asc == CR)
	//			{
	//				bioskey(0);
	//				if (gr[k].gvar == 1)
	//					goto grend;
	//				goto gscale;
	//			}
	//			else if (asc == ESC)
	//			{
	//				bioskey(0);
	//				goto display;
	//			}
	//			gotoxy(47, 4);
	//			cputs("      ");
	//			gotoxy(47, 4);
	//			str[0] = 2;
	//			p = cgets(str);
	//			j = atoi(p);
	//			if (j < 0 || j > 2)
	//				goto gvar;
	//			gr[k].gvar = j;
	//			gotoxy(47, 4);
	//			cprintf("%d", gr[k].gvar);
	//			if (gr[k].gvar == 1)
	//				goto grend;
	//		gscale:
	//			gotoxy(1, 5);
	//			gr[k].gscale = 50;
	//			cprintf("Enter scale (1 - 300): %d", gr[k].gscale);
	//			gotoxy(24, 5);
	//			j = 0;
	//			while (!j)
	//				j = bioskey(1);
	//			asc = j & 0xff;
	//			if (asc == CR)
	//			{
	//				bioskey(0);
	//				goto grend;
	//			}
	//			else if (asc == ESC)
	//			{
	//				bioskey(0);
	//				goto display;
	//			}
	//			gotoxy(24, 5);
	//			cputs("      ");
	//			gotoxy(24, 5);
	//			str[0] = 10;
	//			p = cgets(str);
	//			j = atoi(p);
	//			if (j < 1 || j > 300)
	//				goto gscale;
	//			gr[k].gscale = j;
	//			gotoxy(24, 5);
	//			cprintf("%d", gr[k].gscale);
	//		grend:
	//			clrline(1);
	//			clrline(2);
	//			clrline(3);
	//			clrline(4);
	//			clrline(5);
	//		}
	//		goto display;
}

void pathrec()
{
	//		gotoxy(1, 1);
	//		cputs("Enter recording path:                                       ");
	//		gotoxy(23, 1);
	//		j = 0;
	//		while (!j)
	//			j = bioskey(1);
	//		asc = j & 0xff;
	//		if (asc == ESC || asc == CR)
	//		{
	//			bioskey(0);
	//			goto display;
	//		}
	//		str[0] = 25;
	//		p = cgets(str);
	//		strcpy(recpath, p);
	//		i = strlen(recpath) - 1;
	//		if (recpath[i] == '\\')
	//			recpath[i] = 0; /* get rid of final \ */
	//		goto display;
}

void printbanner()
{
	printf("                                   ____   ___ ____  _ \n");
	printf(" _ __   ___ _ ____   _____   ___  |___ \\ / _ \\___ \\/ |\n");
	printf("| '_ \\ / _ \\ '__\\ \\ / / _ \\ / __|   __) | | | |__) | |\n");
    printf("| | | |  __/ |   \\ V / __ / \\__ \\  / __/| |_| / __/| |\n");
	printf("|_| |_|\\___|_|    \\_/\\___|  |___/ |_____|\\___/_____|_|\n");
	printf("\n");
}