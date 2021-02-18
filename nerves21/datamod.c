#include "defn.h"
#include "nsdata.c"
#include "proto.h"

void datamod(void) /* allows user to modify/add/delete neuron data */
{
	enum asc_val asc;
	enum ext_val ext;
	char str[41];
	char *p;
	int newneur;
	FILE *file;
	int i, j, done, line, cc, newcon;
	struct neuron *np, tn;
	struct con *cp, *tc, *cq, *cs, *cl;
	struct Iint *ip, ti;
	double d;

	/* modify data */
start:
	clrscr();
	//textattr(BLUE + (LIGHTGRAY << 4));
	gotoxy(1, 24);
	cputs(" ^S-Save file   Esc-Main menu                                                   ");
	//textattr(LIGHTGRAY);
getname:
	clrline(1);
	gotoxy(1, 1);
	cputs("Neuron name: ");
	i = 0;
	while (!i)
		i = bioskey(1);
	asc = i & 0xff;
	if (asc == ESC)
	{ /* return to main menu without saving */
		bioskey(0);
		return; /* go to main menu */
	}
	else if (asc == CTRLS)
	{ /* save file */
		bioskey(0);
		/* count neurons */
		for (i = 0, j = 0, np = ns; i < nn; i++, np++)
			if (np->name[0])
				j++;
		clrscr();
		if (neurfname[0])
		{
			cprintf("Save to %s? ", neurfname);
			i = toupper(bioskey(0) & 0xff);
		}
		if (!neurfname[0] || i != 'Y')
		{
			gotoxy(1, 1);
			cputs("Enter file spec:                                      ");
			gotoxy(18, 1);
			str[0] = 38;
			p = cgets(str);
			strcpy(neurfname, p);
		}
		file = fopen(neurfname, "wb");
		fwrite(&j, 2, 1, file);
		for (i = 0, np = ns; i < nn; i++, np++)
			if (np->name[0])
			{
				fwrite(np, 27, 1, file);
				if (np->Iint)
					j = (int)np->Iint - (int)Iinta + 1;
				else
					j = 0;
				fwrite(&j, 2, 1, file);
				fwrite(&(np->Isens), 18, 1, file);
				if (np->con)
					j = (int)np->con - (int)cona + 1;
				else
					j = 0;
				fwrite(&j, 2, 1, file);
			}
		fwrite(&ni, 2, 1, file);
		fwrite(Iinta, sizeof(struct Iint), ni, file);
		fwrite(&nc, 2, 1, file);
		for (i = 0, cp = cona; i < nc; i++, cp++)
		{
			fwrite(cp, sizeof(struct con) - 2, 1, file);
			if (cp->next)
				j = (int)cp->next - (int)cona + 1;
			else
				j = 0;
			fwrite(&j, 2, 1, file);
		}
		fclose(file);
		return; /* go to main menu */
	}
	else
	{ /* neuron name input */
		str[0] = 7;
		p = cgets(str);
		if ((*p == '?' || *p == '/') && *(p + 1) == 0)
		{ /* list neuron names */
			for (np = ns, i = 0, j = 0; i < nn & j < 10; j++)
				for (line = 3; line < 24 && i < nn; line++, i++, np++)
				{
					gotoxy(1 + j * 8, line);
					while (!np->name[0] && i < nn)
					{
						np++;
						i++;
					}
					if (i < nn)
						cprintf("%s", np->name);
				}
			goto getname;
		}
		else if (!*p)
			goto getname;
		else
		{
			for (np = ns, i = 0; i < nn; i++, np++)
				if (!strncmp(p, np->name, 6))
					break;
			if (i == nn)
			{
				gotoxy(1, 2);
				cputs("Neuron not found; create new neuron? ");
				if (toupper(bioskey(0) & 0xff) != 'Y')
					goto getname;
				newneur = TRUE;
				nn++;
			}
			else
				newneur = FALSE;
		}
		/* save neuron's data in temporary storage */
		tn = *np;
		if (newneur)
			strcpy(tn.name, p);
		if (np->Iint)
			ti = *(np->Iint);
		else
			ti = *(Iinta + ni);
		for (cp = np->con, tc = cona + nc, cc = 0; cp != NULL; cp = cp->next, cc++, tc++)
			*tc = *cp;
		tc = cona + nc;

		ndisp(tn, ti, 3); /* display neuron's parameters */

		/* modify parameters */
		line = 3;
		done = FALSE;
		while (!done)
		{
			i = 0;
			while (!i)
				i = bioskey(1);
			asc = i & 0xff;
			if (asc)
			{
				if (asc < 32)
				{
					bioskey(0);
					switch (asc)
					{
					case CTRLD:							/* delete neuron */
						if (!strcmp(tn.name, np->name)) /* if existing neuron, delete */
							np->name[0] = 0;
						if (newneur)
							nn--;
						goto start;
					case CTRLS: /* save neuron */
						if (!newneur && strcmp(np->name, tn.name))
						{ /* copied neuron */
							if (tn.Iint)
								tn.Iint = Iinta + ni;
							np = ns + nn;
							np->Iint = 0;
							np->con = 0;
							nn++;
						}
						if (!np->con)
							newcon = TRUE;
						else
							newcon = FALSE;
						if (!np->Iint && tn.Iint)
							ni++;
						*(np) = tn;
						if (tn.Iint)
							*tn.Iint = ti;
						j = 0;
						cq = tc;
						if (!newcon)
							for (cp = np->con; cp != NULL && j < cc; cp = cs, cq++, j++)
							{
								cs = cp->next;
								*cp = *cq;
								if (j == cc - 1)
									cp->next = NULL;
								else if (cs == NULL && j < cc - 1)
									cp->next = cona + nc;
								else
									cp->next = cs;
							}
						if (cp == NULL)
						{
							if (j != cc)
							{
								for (cp = cona + nc, i = 0; j < cc; cp++, j++, cq++, i++)
								{
									if (!newcon)
										*cp = *cq;
									cp->next = cp + 1;
								}
								(cp - 1)->next = NULL;
								if (newcon)
									np->con = cona + nc;
								nc += i;
							}
						}
						goto start;
					case CR:
						switch (line)
						{
						case 8: /* Intrinsic current type */
							if (!tn.Iint)
							{
								tn.Iint = Iinta + ni;
								ti.type = 0;
							}
							else if (ti.type == 0)
								ti.type = 1;
							else
								tn.Iint = NULL;
							ndisp(tn, ti, line);
							break;
						case 16: /* Sensory current function */
							tn.Isens++;
							if (tn.Isens > 6)
								tn.Isens = 0;
							ndisp(tn, ti, line);
							break;
						case 19: /* motor output type */
							tn.mtype++;
							if (tn.mtype > 2)
								tn.mtype = 0;
							ndisp(tn, ti, line);
							break;
						case 20: /* motor output sub type */
							if (tn.mtype == 1)
							{
								tn.mname++;
								if (tn.mname > 2)
									tn.mname = 0;
							}
							else if (tn.mtype == 2)
								tn.mname = !tn.mname;
							else
								break;
							ndisp(tn, ti, line);
							break;
						case 22: /* Switch to connections page */
							conmod(tn.name, tc, &cc);
							ndisp(tn, ti, 3);
							line = 3;
							break;
						default:
							if (line < 22)
								line++;
							else
								line = 1;
							gotoxy(40, line);
							break;
						}
						break;
					case ESC:
						if (newneur)
							nn--;
						goto start;
					default:
						break;
					}
				}
				else
				{ /* process input */
					if (line <= 7 ||
						(tn.Iint && line >= 9 && line <= 15 &&
						 !(line == 12 && ti.type == 1)) ||
						(tn.Isens && (line == 17) || (tn.Isens == OS && line == 18)) ||
						(tn.mtype && (line == 21)))
					{ /* get numerical input */
						gotoxy(40, line);
						cputs("                   ");
						gotoxy(40, line);
						if (line == 1)
							str[0] = 7;
						else
							str[0] = 15;
						p = cgets(str);
						if (line == 1)
						{
							strcpy(tn.name, p);
							line = 3;
							gotoxy(40, 3);
						}
						else
						{
							d = atof(p);
							gotoxy(40, line);
							if (line == 18 || line == 17)
								cprintf("%.4f", d);
							else
								cprintf("%.2f", d);
							switch (line)
							{
							case 3: /* Gmem */
								tn.Gmem = d * 1e-6;
								break;
							case 4: /* Cmem */
								tn.Cmem = d * 1e-9;
								break;
							case 5: /* Vt */
								tn.Vt = d * 1e-3;
								break;
							case 6: /* Fmin */
								tn.Fmin = d;
								break;
							case 7: /* Gain */
								tn.Gain = d * 1e3;
								break;
							case 9: /* IL */
								ti.IL = d * 1e-9;
								break;
							case 10:
								ti.pL[0] = d * 1e-3;
								break;
							case 11:
								ti.pL[1] = d * 1e-3;
								break;
							case 12:
								ti.pL[2] = d;
								break;
							case 13:
								ti.IH = d * 1e-9;
								break;
							case 14:
								ti.pH[0] = d * 1e-3;
								break;
							case 15:
								ti.pH[1] = d * 1e-3;
								break;
							case 17:
								tn.pI[0] = d * 1e-9;
								break;
							case 18:
								tn.pI[1] = d * 1e-9;
								break;
							case 21:
								tn.mconst = d;
								break;
							default:
								break;
							}
							if (line < 22)
								line++;
							else
								line = 3;
							gotoxy(40, line);
						}
					}
					else
						bioskey(0);
				}
			}
			else
			{
				ext = i >> 8;
				bioskey(0);
				switch (ext)
				{
				case UP:
					if (line > 1)
						line--;
					else
						line = 22;
					gotoxy(40, line);
					break;
				case DOWN:
					if (line < 22)
						line++;
					else
						line = 1;
					gotoxy(40, line);
					break;
				default:
					break;
				}
			}
		}
	}
}

void ndisp(struct neuron tn, struct Iint ti, int line)
{ /* displays neuron parameters */

	clrscr();
	cputs("Neuron name:");
	gotoxy(40, 1);
	cprintf("%s", tn.name);
	gotoxy(1, 2);
	cputs("Parameters:");
	gotoxy(3, 3);
	cputs("Membrane conductance, Gmem [microS]:");
	gotoxy(40, 3);
	cprintf("%.2f", tn.Gmem * 1e6);
	gotoxy(3, 4);
	cputs("Membrane capacitance, Cmem [nanoF]:");
	gotoxy(40, 4);
	cprintf("%.2f", tn.Cmem * 1e9);
	gotoxy(3, 5);
	cputs("Threshold voltage, Vt [milliV]:");
	gotoxy(40, 5);
	cprintf("%.2f", tn.Vt * 1e3);
	gotoxy(3, 6);
	cputs("Minimum firing frequency, Fmin:");
	gotoxy(40, 6);
	cprintf("%.2f", tn.Fmin);
	gotoxy(3, 7);
	cputs("Gain [1/milliV]:");
	gotoxy(40, 7);
	cprintf("%.2f", tn.Gain * 1e-3);
	gotoxy(1, 8);
	cputs("Intrinsic current:");
	gotoxy(40, 8);
	if (tn.Iint == NULL)
		cputs("OFF");
	else
	{
		if (ti.type == 0)
			cputs("NON-RANDOM");
		else
			cputs("RANDOM");
		gotoxy(3, 9);
		cputs("Low intrinsic current, LIC [nanoA]:");
		gotoxy(40, 9);
		cprintf("%.2f", ti.IL * 1e9);
		gotoxy(5, 10);
		if (ti.type == 0)
		{
			cputs("Threshold voltage [milliV]:");
			gotoxy(40, 10);
			cprintf("%.2f", ti.pL[0] * 1e3);
			gotoxy(5, 11);
			cputs("Base duration [millisec]:");
			gotoxy(40, 11);
			cprintf("%.2f", ti.pL[1] * 1e3);
			gotoxy(5, 12);
			cputs("Multiplier:");
			gotoxy(40, 12);
			cprintf("%.2f", ti.pL[2]);
		}
		else
		{
			cputs("Minimum duration [millisec]:");
			gotoxy(40, 10);
			cprintf("%.2f", ti.pL[0] * 1e3);
			gotoxy(5, 11);
			cputs("Maximum duration [millisec]:");
			gotoxy(40, 11);
			cprintf("%.2f", ti.pL[1] * 1e3);
		}
		gotoxy(3, 13);
		cputs("High intrinsic current, HIC [nanoA]:");
		gotoxy(40, 13);
		cprintf("%.2f", ti.IH * 1e9);
		gotoxy(5, 14);
		if (ti.type == 0)
		{
			cputs("Duration [millisec]:");
			gotoxy(40, 14);
			cprintf("%.2f", ti.pH[0] * 1e3);
		}
		else
		{
			cputs("Minimum duration [millisec]:");
			gotoxy(40, 14);
			cprintf("%.2f", ti.pH[0] * 1e3);
			gotoxy(5, 15);
			cputs("Maximum duration [millisec]:");
			gotoxy(40, 15);
			cprintf("%.2f", ti.pH[1] * 1e3);
		}
	}
	gotoxy(1, 16);
	cputs("Sensory current function:");
	gotoxy(40, 16);
	switch (tn.Isens)
	{
	case NONE:
		cputs("NONE");
		break;
	case LAF:
		cputs("LEG ANGLE FORWARD");
		break;
	case LAB:
		cputs("LEG ANGLE BACKWARD");
		break;
	case AC:
		cputs("ANTENNA CONTACT");
		break;
	case OS:
		cputs("ODOR STRENGTH");
		break;
	case EC:
		cputs("ENERGY CAPICITY");
		break;
	case MC:
		cputs("MOUTH CONTACT");
		break;
	}
	if (tn.Isens)
	{
		gotoxy(3, 17);
		cputs("Sensory current parameter 1 [nanoA]:");
		gotoxy(40, 17);
		cprintf("%.4f", tn.pI[0] * 1e9);
		if (tn.Isens == OS)
		{
			gotoxy(3, 18);
			cputs("Sensory current parameter 2 [nanoA]:");
			gotoxy(40, 18);
			cprintf("%.4f", tn.pI[1] * 1e9);
		}
	}
	gotoxy(1, 19);
	cputs("Motor output type:");
	gotoxy(40, 19);
	if (tn.mtype == 0)
		cputs("NONE");
	else
	{
		if (tn.mtype == 1)
		{
			cputs("FORCE");
			gotoxy(5, 20);
			cputs("Force type:");
			gotoxy(40, 20);
			if (tn.mname == 0)
				cputs("Forward");
			else if (tn.mname == 1)
				cputs("Backward");
			else
				cputs("Lateral");
			gotoxy(5, 21);
			cputs("Firing frequency multiplier:");
			gotoxy(40, 21);
			cprintf("%.2f", tn.mconst);
		}
		else
		{
			cputs("STATE");
			gotoxy(5, 20);
			cputs("State type:");
			gotoxy(40, 20);
			if (tn.mname == 0)
				cputs("Foot");
			else
				cputs("Mouth");
			gotoxy(5, 21);
			cputs("Firing frequency threshold");
			gotoxy(40, 21);
			cprintf("%.2f", tn.mconst);
		}
	}
	gotoxy(1, 22);
	cputs("Connections:");
	gotoxy(40, 22);
	cputs("MODIFY");
	textattr(BLUE + (LIGHTGRAY << 4));
	gotoxy(1, 24);
	cputs(" ^D-Delete neuron   ^S-Save neuron   Esc-New neuron                             ");
	gotoxy(40, line);
	textattr(LIGHTGRAY);
}

void conmod(char *nname, struct con *tc, int *cc)
{ /* allows modification of neurons connections */
	int i, j, done, line, field;
	struct con *cp;
	enum asc_val asc;
	enum ext_val ext;
	char str[10], *p;
	double d;

	line = 3;
	field = 0;
	condisp(nname, tc, *cc, line, field);
	done = FALSE;
	while (!done)
	{
		cp = tc + line - 3;
		i = 0;
		while (!i)
			i = bioskey(1);
		asc = i & 0xff;
		if (asc)
		{
			if (asc < 32)
			{
				bioskey(0);
				switch (asc)
				{
				case CTRLD: /* delete connection */
					for (j = line, cp = tc + line - 2; j <= *cc + 2; j++, cp++)
						*(cp - 1) = *cp;
					(*cc)--;
					condisp(nname, tc, *cc, line, field);
					break;
				case CR: /* change type */
					if (field == 2)
					{
						cp->ctype++;
						if (cp->ctype > 2)
							cp->ctype = 0;
						condisp(nname, tc, *cc, line, field);
					}
					else if (cp->ctype == 1 && field == 5)
					{
						cp->U = !cp->U;
						gotoxy(3 + field * 10, line);
						cprintf("%d", cp->U);
						gotoxy(3 + field * 10, line);
					}
					break;
				case ESC: /* back to parameters */
					done = TRUE;
					break;
				default:
					break;
				}
			}
			else
			{ /* enter connection data */
				if (field == 2 || (field > 2 && !cp->ctype) || field == 5)
					bioskey(0);
				else
				{
					gotoxy(3 + field * 10, line);
					cputs("          ");
					gotoxy(3 + field * 10, line);
					str[0] = 7;
					p = cgets(str);
					if (field == 1 || field == 4)
					{
						d = atof(p);
						gotoxy(3 + field * 10, line);
						cprintf("%.2f", d);
					}
					switch (field)
					{
					case 0: /* sending neuron */
						strcpy(cp->sname, p);
						if (line == *cc + 3)
						{
							(*cc)++; /* add a new connection */
							cp->Isr = 0.;
							cp->ctype = 0;
							cp->cname[0] = 0;
							cp->Icr = 0;
							cp->U = 0;
							condisp(nname, tc, *cc, line, field);
						}
						break;
					case 1: /* sending current */
						cp->Isr = d * 1e-9;
						break;
					case 3: /* compound neuron */
						strcpy(cp->cname, p);
						break;
					case 4: /* compound current */
						cp->Icr = d * 1e-9;
						break;
					}
					field++;
					if (field > 5)
					{
						field = 0;
						line++;
					}
					gotoxy(3 + field * 10, line);
				}
			}
		}
		else
		{
			ext = i >> 8;
			bioskey(0);
			switch (ext)
			{
			case UP:
				if (line > 3)
					line--;
				else
				{
					line = *cc + 3;
					if (line > 23)
						;
					line = 23;
				}
				break;
			case DOWN:
				if (line < *cc + 3 && line < 23)
					line++;
				else
					line = 3;
				break;
			case RIGHT:
				if (field < 5)
					field++;
				else
					field = 0;
				break;
			case LEFT:
				if (field > 0)
					field--;
				else
					field = 5;
				break;
			default:
				break;
			}
			gotoxy(3 + field * 10, line);
		}
	}
}

void condisp(char *nname, struct con *tc, int cc, int line, int field)
{ /* displays current neuron's connections */
	int i;
	struct con *cp;

	clrscr();
	cprintf("Neuron name: %s", nname);
	gotoxy(3, 2);
	cputs("Sending   Current   Type      Compound  Current   State");
	for (i = 0, cp = tc; i < cc; i++, cp++)
	{
		gotoxy(3, i + 3);
		cprintf("%-6s    %.2f", cp->sname, cp->Isr * 1e9);
		gotoxy(23, i + 3);
		if (cp->ctype == 0)
			cputs("DIRECT");
		else if (cp->ctype == 1)
			cputs("GATED");
		else
			cputs("MODULATED");
		if (cp->ctype)
		{
			gotoxy(33, i + 3);
			cprintf("%-6s    %.2f", cp->cname, cp->Icr * 1e9);
			if (cp->ctype == 1)
			{
				gotoxy(53, i + 3);
				cprintf("%d", cp->U);
			}
		}
	}
	textattr(BLUE + (LIGHTGRAY << 4));
	gotoxy(1, 24);
	cputs(" ^D-Delete connection   Esc-Return to parameters                                ");
	textattr(LIGHTGRAY);
	gotoxy(3 + field * 10, line);
}
