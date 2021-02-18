#include "defn.h"
#include "nsdata.c"
#include "proto.h"

int GraphDriver, GraphMode;
int ErrorCode = 0;
int MaxColors;
double AspectRatio;
int xasp, yasp;
int MaxX, MaxY;
struct palettetype palette;

void graph(void)
{
       Initialize();
       Draw();
}

void Initialize(void)
{
       GraphDriver = EGA;
       GraphMode = EGAHI;
       initgraph(&GraphDriver, &GraphMode, "");
       ErrorCode = graphresult();
       if (ErrorCode != grOk)
       {
              closegraph();
              printf("Graphics Error:%s\n", grapherrormsg(ErrorCode));
              exit(1);
       }
       getpalette(&palette);
       MaxColors = getmaxcolor() + 1;
       MaxX = 640;
       MaxY = 350;
       getaspectratio(&xasp, &yasp);
}

void Draw(void)
{
       int i, j;
       char str[26];

       setcolor(15);
       line(0, 0, 639, 0); /* screen border lines */
       line(0, 0, 0, 349);
       line(639, 0, 639, 349);
       line(0, 349, 639, 349);

       line(0, 330, 639, 330); /* world border lines */
       line(479, 0, 479, 329);

       /* plot either graphs or neuron activity */
       if (plottype == 0)
              graphsetup();
       else
              activesetup();

       setcolor(3);

       outtextxy(10, 336, "Enter:Run  Space:Step  Esc:Menu  Tab:ChgPlots");
       sprintf(str, "D:%d", drawmult);
       outtextxy(392, 336, str);
       sprintf(str, "%d", (int)energy);
       outtextxy(430, 336, str);
       if (Iextgr >= 0 && gr[Iextgr].gtype == 1)
       {
              settextjustify(2, 2);
              sprintf(str, "Iext(%s)[nA]:%-.1f", gr[Iextgr].gneur, Iextint);
              outtextxy(629, 336, str);
       }
       setviewport(1, 1, 478, 329, 1);
       setfillstyle(1, 7);
       for (i = 0; i < nblock; i++)
       {
              bar(blockx[i] / 2, AspR * blocky[i] / 2, (blockx[i] + BLOCKWIDTH) / 2,
                  AspR * (blocky[i] + BLOCKHEIGHT) / 2);
       }
       settextjustify(0, 2);
       setfillstyle(0, 0);
}

void graphsetup(void)
{
       int j;
       char str[26];

       setcolor(15);
       line(479, 66, 639, 66); /* plot border horizontal lines */
       line(479, 132, 639, 132);
       line(479, 198, 639, 198);
       line(479, 264, 639, 264);

       setcolor(3);
       for (j = 0; j < 5; j++)
       {
              if (gr[j].gtype == 1)
              {
                     line(537, 53 + j * 66, 637, 53 + j * 66); /* graph lines */
                     line(537, 3 + j * 66, 537, 56 + j * 66);

                     line(533, 3 + j * 66, 537, 3 + j * 66); /* graph y-axis ticks */
                     line(535, 8 + j * 66, 537, 8 + j * 66);
                     line(535, 13 + j * 66, 537, 13 + j * 66);
                     line(535, 18 + j * 66, 537, 18 + j * 66);
                     line(535, 23 + j * 66, 537, 23 + j * 66);
                     line(533, 28 + j * 66, 537, 28 + j * 66);
                     line(535, 33 + j * 66, 537, 33 + j * 66);
                     line(535, 38 + j * 66, 537, 38 + j * 66);
                     line(535, 43 + j * 66, 537, 43 + j * 66);
                     line(535, 48 + j * 66, 537, 48 + j * 66);
                     line(533, 53 + j * 66, 537, 53 + j * 66);

                     line(547, 3 + j * 66, 547, 55 + j * 66); /*graph x-axis ticks */
                     line(557, 3 + j * 66, 557, 55 + j * 66);
                     line(567, 3 + j * 66, 567, 55 + j * 66);
                     line(577, 3 + j * 66, 577, 55 + j * 66);
                     line(587, 3 + j * 66, 587, 56 + j * 66);
                     line(597, 3 + j * 66, 597, 55 + j * 66);
                     line(607, 3 + j * 66, 607, 55 + j * 66);
                     line(617, 3 + j * 66, 617, 55 + j * 66);
                     line(627, 3 + j * 66, 627, 55 + j * 66);
                     line(637, 3 + j * 66, 637, 56 + j * 66);

                     outtextxy(538, 58 + j * 66, "0"); /* graph labels (always same) */
                     outtextxy(576, 58 + j * 66, "sec");
                     outtextxy(630, 58 + j * 66, "1");

                     settextjustify(2, 2); /* graph labels (user-set) */
                     if (gr[j].gvar == 1)
                     {
                            outtextxy(532, 4 + j * 66, "1");
                            outtextxy(532, 46 + j * 66, "0");
                     }
                     else
                     {
                            outtextxy(532, 4 + j * 66, itoa(gr[j].gscale, str, 10));
                            outtextxy(532, 46 + j * 66, itoa(-gr[j].gscale, str, 10));
                            outtextxy(532, 25 + j * 66, "0");
                     }

                     settextjustify(0, 2); /* graph labels (also user-set) */
                     outtextxy(483, 15 + j * 66, gr[j].gneur);

                     if (gr[j].gvar == 0)
                     {
                            outtextxy(483, 35 + j * 66, "V [mV]");
                     }
                     else if (gr[j].gvar == 2)
                     {
                            outtextxy(483, 35 + j * 66, "I [pA]");
                     }
                     else
                            outtextxy(483, 35 + j * 66, "Freq");
              }
              else if (gr[j].gtype == 2)
              {
                     outtextxy(483, 266 - (4 - j) * 66, "R3"); /* gait labels (always same) */
                     outtextxy(483, 275 - (4 - j) * 66, "R2");
                     outtextxy(483, 284 - (4 - j) * 66, "R1");
                     outtextxy(483, 293 - (4 - j) * 66, "L3");
                     outtextxy(483, 302 - (4 - j) * 66, "L2");
                     outtextxy(483, 312 - (4 - j) * 66, "L1");
                     outtextxy(538, 322 - (4 - j) * 66, "0");
                     outtextxy(576, 322 - (4 - j) * 66, "sec");
                     outtextxy(630, 322 - (4 - j) * 66, "1");

                     line(537, 317 - (4 - j) * 66, 637, 317 - (4 - j) * 66); /* gait x-axis */

                     line(537, 267 - (4 - j) * 66, 537, 320 - (4 - j) * 66); /* gait x-axis ticks */
                     line(547, 267 - (4 - j) * 66, 547, 319 - (4 - j) * 66);
                     line(557, 267 - (4 - j) * 66, 557, 319 - (4 - j) * 66);
                     line(567, 267 - (4 - j) * 66, 567, 319 - (4 - j) * 66);
                     line(577, 267 - (4 - j) * 66, 577, 319 - (4 - j) * 66);
                     line(587, 267 - (4 - j) * 66, 587, 320 - (4 - j) * 66);
                     line(597, 267 - (4 - j) * 66, 597, 319 - (4 - j) * 66);
                     line(607, 267 - (4 - j) * 66, 607, 319 - (4 - j) * 66);
                     line(617, 267 - (4 - j) * 66, 617, 319 - (4 - j) * 66);
                     line(627, 267 - (4 - j) * 66, 627, 319 - (4 - j) * 66);
                     line(637, 267 - (4 - j) * 66, 637, 320 - (4 - j) * 66);
              }
       }
}

void activesetup(void)
{
       char block[4];

       block[0] = block[1] = block[2] = 219;
       block[3] = 0;
       settextjustify(0, 2);
       setcolor(4);
       moveto(483, 322);
       outtext("F:");
       setcolor(1);
       outtext("0");
       setcolor(3);
       outtext(block);
       setcolor(4);
       outtext("1/3");
       setcolor(5);
       outtext(block);
       setcolor(4);
       outtext("2/3");
       setcolor(7);
       outtext(block);
       setcolor(4);
       outtext("1");
}