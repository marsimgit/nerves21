#include "defn.h"
#include "nsdata.c"
#include "proto.h"

void drawbug(struct bug bug, int col)
{
  int i;
  double temp1, temp2, temp3;
  int bugx, bugy;

  bugx = .5 * bug.x + .5;
  bugy = .5 * bug.y * AspR + .5;

  /* calculate bug pts */
  bugp.head[0] = .5 * bug.mouthx;
  bugp.head[1] = .5 * AspR * bug.mouthy;
  temp1 = bug.ang + hdsang[0]; /* calculate head pts */
  bugp.head[2] = bugx + (int)(.5 * (hdsl * cos(temp1)));
  bugp.head[3] = bugy + (int)(.5 * AspR * hdsl * sin(temp1));
  temp3 = bug.ang + hdbang;
  bugp.head[4] = bugx + (int)(.5 * hdbl * cos(temp3));
  bugp.head[5] = bugy + (int)(.5 * AspR * hdbl * sin(temp3));
  temp1 = bug.ang + hdsang[1];
  bugp.head[6] = bugx + (int)(.5 * hdsl * cos(temp1));
  bugp.head[7] = bugy + (int)(.5 * AspR * hdsl * sin(temp1));

  temp1 = bug.ang + btang[0]; /* calculate body pts */
  temp2 = bug.ang + bmang[0];
  temp3 = bug.ang + bbang[0];
  bugp.body[0] = bugx + (int)(.5 * btl * cos(temp1));
  bugp.body[1] = bugy + (int)(.5 * AspR * btl * sin(temp1));
  bugp.body[2] = bugx + (int)(.5 * bml * cos(temp2));
  bugp.body[3] = bugy + (int)(.5 * AspR * bml * sin(temp2));
  bugp.body[4] = bugx + (int)(.5 * bbl * cos(temp3));
  bugp.body[5] = bugy + (int)(.5 * AspR * bbl * sin(temp3));
  temp1 = bug.ang + btang[1];
  temp2 = bug.ang + bmang[1];
  temp3 = bug.ang + bbang[1];
  bugp.body[10] = bugx + (int)(.5 * btl * cos(temp1));
  bugp.body[11] = bugy + (int)(.5 * AspR * btl * sin(temp1));
  bugp.body[8] = bugx + (int)(.5 * bml * cos(temp2));
  bugp.body[9] = bugy + (int)(.5 * AspR * bml * sin(temp2));
  bugp.body[6] = bugx + (int)(.5 * bbl * cos(temp3));
  bugp.body[7] = bugy + (int)(.5 * AspR * bbl * sin(temp3));

  bugp.legx[1] = bugp.body[2]; /* calculate leg attachment pts */
  bugp.legy[1] = bugp.body[3];
  bugp.legx[4] = bugp.body[8];
  bugp.legy[4] = bugp.body[9];
  for (i = 0; i < 6; i++)
  {
    if (i != 1 && i != 4)
    {
      temp1 = bug.ang + attang[i];
      bugp.legx[i] = bugx + (int)(.5 * attl[i] * cos(temp1));
      bugp.legy[i] = bugy + (int)(.5 * AspR * attl[i] * sin(temp1));
    }
    bugp.footx[i] = .5 * bug.footx[i];
    bugp.footy[i] = .5 * AspR * bug.footy[i];
  }

  for (i = 0; i < 2; i++) /* calculate antenna attachment pts */
  {
    temp2 = bug.ang + antbang[i];
    bugp.antax[i] = bugx + (int)(.5 * antbl * cos(temp2));
    bugp.antay[i] = bugy + (int)(.5 * AspR * antbl * sin(temp2));
    bugp.antx[i] = .5 * bug.antx[i];
    bugp.anty[i] = .5 * AspR * bug.anty[i];
  }

  for (i = 0; i < 2; i++) /* calculate cercus attachment pts */
  {
    temp1 = bug.ang + cerbang[i];
    bugp.cercax[i] = bugx + (int)(.5 * cerbl * cos(temp1));
    bugp.cercay[i] = bugy + (int)(.5 * AspR * cerbl * sin(temp1));
    bugp.cercx[i] = .5 * bug.cercx[i];
    bugp.cercy[i] = .5 * AspR * bug.cercy[i];
  }

  showbug(bug, col);
  if (recnum)
  { /* record bug info */
    if (bug.mouthst)
      rc[reccnt].mouthfood = rc[reccnt].mouthfood | 1;
    rc[reccnt].energy = energy / 10000L;
    for (i = 0; i < 6; i++)
      if (bug.foot[i])
        rc[reccnt].footst = rc[reccnt].footst | (1 << i);
    bugx = bugp.head[0];
    bugy = bugp.head[1];
    rc[reccnt].x = bugx;
    rc[reccnt].y = bugy;
    for (i = 0; i < 6; i += 2)
    {
      rc[reccnt].head[i] = bugp.head[i + 2] - bugx;
      rc[reccnt].head[i + 1] = bugp.head[i + 3] - bugy;
    }
    for (i = 0; i < 12; i += 2)
    {
      rc[reccnt].body[i] = bugp.body[i] - bugx;
      rc[reccnt].body[i + 1] = bugp.body[i + 1] - bugy;
    }
    for (i = 0; i < 6; i++)
    {
      rc[reccnt].footx[i] = bugp.footx[i] - bugx;
      rc[reccnt].footy[i] = bugp.footy[i] - bugy;
      rc[reccnt].legx[i] = bugp.legx[i] - bugx;
      rc[reccnt].legy[i] = bugp.legy[i] - bugy;
    }
    for (i = 0; i < 2; i++)
    {
      rc[reccnt].antx[i] = bugp.antx[i] - bugx;
      rc[reccnt].anty[i] = bugp.anty[i] - bugy;
      rc[reccnt].antax[i] = bugp.antax[i] - bugx;
      rc[reccnt].antay[i] = bugp.antay[i] - bugy;
      rc[reccnt].cercx[i] = bugp.cercx[i] - bugx;
      rc[reccnt].cercy[i] = bugp.cercy[i] - bugy;
      rc[reccnt].cercax[i] = bugp.cercax[i] - bugx;
      rc[reccnt].cercay[i] = bugp.cercay[i] - bugy;
    }
  }
}

void showbug(struct bug bug, int col)
{
  int i, poly[10];

  setcolor(col);
  /* draw head */
  for (i = 0; i < 8; i++)
    poly[i] = bugp.head[i];
  poly[8] = poly[0];
  poly[9] = poly[1];
  drawpoly(5, poly);
  if (bug.mouthst)
    line(bugp.head[0], bugp.head[1], bugp.head[4], bugp.head[5]);

  /* draw body */
  drawpoly(6, bugp.body);

  /* draw antennae */
  for (i = 0; i < 2; i++)
  {
    line(bugp.antax[i], bugp.antay[i], bugp.antx[i], bugp.anty[i]);
  }
  /* draw cerci */
  for (i = 0; i < 2; i++)
    line(bugp.cercax[i], bugp.cercay[i], bugp.cercx[i], bugp.cercy[i]);

  /* draw legs & feet */
  for (i = 0; i < 6; i++)
  {
    line(bugp.legx[i], bugp.legy[i], bugp.footx[i], bugp.footy[i]);
    if (bug.foot[i])
      rectangle(bugp.footx[i] - 1, bugp.footy[i] - 1, bugp.footx[i] + 1, bugp.footy[i] + 1);
  }
}
