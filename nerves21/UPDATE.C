int near update(void)
{
  int i, j, k, cf;
  struct neuron *np;
  struct neurcalc *ncp;
  struct Iint *ip;
  double d, Gi, I, f, xa, ya, xf, yf, mo;
  struct con *cp;
  struct leg
  {
    double bF; /* backward force */
    double fF; /* forward force */
    double lF; /* lateral force */
  } leg[6];
  double bugx, bugy;
  int xinc, yinc;
  double mouthd[NFOOD];

  for (j = 0; j < 6; j++)
  {
    lfoot[j] = bug.foot[j]; /* save foot states */
    leg[j].bF = leg[j].fF = leg[j].lF = 0;
  }
  for (i = 0, ncp = nsc; i < nn; i++, ncp++)
    ncp->Flast = ncp->F; /* save last F's for synaptic connections */
  for (i = 0, np = ns, ncp = nsc; i < nn; i++, np++, ncp++)
  {
    if (np->name[0] != 0) /* undefined? */
    {
      Gi = 0;
      for (cp = np->con; cp != NULL; cp = cp->next)
      { /* get currents from connected neurons */
        d = (nsc + cp->s)->Flast * cp->Isr;
        if (cp->ctype > 0)
        {
          I = (nsc + cp->c)->Flast * cp->Icr;
          if (cp->ctype == 1)
            d = d * (cp->U + I * 1e9);
          else if (I >= 0)
            d = d * (1 + I * 1e9);
          else
            d = d / (1 - I * 1e9);
        }
        Gi += d;
      }

      /* add intrinsic current */
      if (np->Iint != NULL)
      {
        ip = np->Iint;
        j = 1;
        if (!ip->H && ip->type == 0)
        {                        /* calculate tmax for V-infinity type */
          d = ncp->I / np->Gmem; /* calculate Vinfinity */
          if (d > ip->pL[0])
            ip->tmax = ip->pL[1] - ip->pL[2] * d; /* using 31.25, change from original
	      */
          else
            ip->tmax = MAXDOUBLE;
        }
        if ((ncp->Vlast < np->Vt && ncp->V >= np->Vt) ||
            (!ip->H && ip->tint >= ip->tmax))
        { /* turn on high current */
          ip->H = TRUE;
          j = 0; /* restart time count */
          if (ip->type == 0)
            ip->tmax = ip->pH[0]; /* time is constant */
          else
          {
            ip->tmax = (random((int)(1000. * ip->pH[1]) - (int)(1000 * ip->pH[0]) + 1) + (int)(1000. * ip->pH[0])) / 1000.;
          }
        }
        else if (ip->H && ip->tint >= ip->tmax)
        { /* turn off high current */
          ip->H = FALSE;
          j = 0; /* restart time count */
          if (ip->type == 1)
            ip->tmax = (random((int)(1000. * ip->pL[1]) - (int)(1000 * ip->pL[0]) + 1) + (int)(1000. * ip->pL[0])) / 1000.;
        }
        ip->tint = (ip->tint + DT) * j; /* increment time count */

        Gi += ip->IL + ip->H * (ip->IH - ip->IL);
      }

      if (i == Iextind)
        Gi += Iextint * 1e-9; /* add external current */

      /* add sensory current */
      Gi += sensory(np->Isens, np->name, np->pI);

      ncp->I = Gi; /* save current */

      /* calculate new V */
      Gi = (Gi - ncp->V * np->Gmem) / np->Cmem;
      ncp->Vlast = ncp->V;
      ncp->V = ncp->V + Gi * DT;
      if (fabs(ncp->V) < 1e-30) /* prevent underflow on our XT clone! */
        ncp->V = 0.0;

      /* calculate F */
      if (ncp->V < np->Vt)
        ncp->F = 0;
      else
      {
        d = np->Fmin - np->Gain * np->Vt;
        if (ncp->V < (1 - d) / np->Gain)
          ncp->F = np->Gain * ncp->V + d;
        else
          ncp->F = 1;
      }

      /* calculate any motor output */
      if (np->mtype)
      {
        if (np->mtype == 1 || np->mname == 0)
        {
          k = strlen(np->name);
          if (np->name[k - 2] == 'R')
            j = 2;
          else
            j = -1;
          j += np->name[k - 1] - '0';
        }
        if (np->mtype == 1)
        { /* force */
          if (np->mname == 2)
            leg[j].lF = np->mconst * ncp->F;
          else if (np->mname == 0)
            leg[j].fF = np->mconst * ncp->F;
          else
            leg[j].bF = np->mconst * ncp->F;
        }
        else
        { /* state */
          if (np->mname == 0)
          { /* foot */
            if (ncp->F > np->mconst)
              bug.foot[j] = 1;
            else
              bug.foot[j] = 0;
          }
          else
          { /* mouth */
            if (ncp->F > np->mconst)
              bug.mouthst = 1;
            else
              bug.mouthst = 0;
          }
        }
      }
    }
  }

  /* calculate bug position etc., info */
  d = 0.;
  for (j = 0; j < 6; j++) /* sum forces of legs that are down */
    if (bug.foot[j])
      d += leg[j].bF - leg[j].fF;
  d = d / 2.;                                            /* calculate bug's translational velocity (adjustable!) */
  f = bug.foot[0] * leg[0].lF - bug.foot[3] * leg[3].lF; /* calc turning force */
  f = f / 6;                                             /* calculate bug's angular velocity (adjustable!) (5 to 6) */
  bugx = bug.x - sin(bug.ang) * d * DT;
  bugy = bug.y + cos(bug.ang) * d * DT;
  d = bug.foot[1] * leg[1].lF - bug.foot[4] * leg[4].lF; /* calc sideways force*/
  d = 2. * d;                                            /* (adjustable!) (20 to 2) */
  bugx -= cos(bug.ang) * d * DT;
  bugy -= sin(bug.ang) * d * DT;
  bug.ang += f * DT;
  if (bug.ang < 0) /* trig!!! */
    bug.ang = TWOPI + bug.ang;
  if (bug.ang >= TWOPI)
    bug.ang = bug.ang - TWOPI;
  for (j = 0; j < 2; j++) /* calculate antenna & cercus tip positions */
  {
    bug.antx[j] = bugx + antl * cos(bug.ang + antang[j]);
    bug.anty[j] = bugy + antl * sin(bug.ang + antang[j]);
    bug.cercx[j] = bugx + cercl * cos(bug.ang + cercang[j]);
    bug.cercy[j] = bugy + cercl * sin(bug.ang + cercang[j]);
  }

  /* determine antenna contact, antenna contact angle, & cercus contact */
  xinc = 0; /* "touching" variables */
  yinc = 0;
  antenna[0] = antenna[1] = 0;
  for (j = 0; j < 2; j++)
  {
    /* check world edge contact */
    if (bug.antx[j] <= MINX)
    {
      xinc += 1;
      if (!edget[j])
      {
        antenna[j] = TRUE;
        if (bug.ang > HALFPI && bug.ang <= PIANDAHALF) /* more trig!!! */
          antcang[j] = PI - bug.ang;
        else if (bug.ang < PIANDAHALF)
          antcang[j] = bug.ang;
        else
          antcang[j] = TWOPI - bug.ang;
      }
    }
    else if (bug.antx[j] >= MAXX)
    {
      xinc -= 1;
      if (!edget[j])
      {
        antenna[j] = TRUE;
        if (bug.ang <= PIANDAHALF && bug.ang >= HALFPI)
          antcang[j] = bug.ang - PI;
        else if (bug.ang > PIANDAHALF)
          antcang[j] = TWOPI - bug.ang;
        else
          antcang[j] = -bug.ang;
      }
    }
    if (bug.anty[j] <= MINY)
    {
      yinc += 1;
      if (!edget[j])
      {
        antenna[j] = TRUE;
        if (bug.ang <= PI)
          antcang[j] = bug.ang - HALFPI;
        else
          antcang[j] = PIANDAHALF - bug.ang;
      }
    }
    else if (bug.anty[j] >= MAXY)
    {
      yinc -= 1;
      if (!edget[j])
      {
        antenna[j] = TRUE;
        if (bug.ang <= PI)
          antcang[j] = HALFPI - bug.ang;
        else
          antcang[j] = bug.ang - PIANDAHALF;
      }
    }
    if (bug.cercx[j] <= MINX)
      xinc += 1;
    else if (bug.cercx[j] >= MAXX)
      xinc -= 1;
    if (bug.cercy[j] <= MINY)
      yinc += 1;
    else if (bug.cercy[j] >= MAXY)
      yinc -= 1;

    /* check box edge contact */
    for (i = 0; i < nblock; i++)
    { /* first check antenna */
      if (bugx < blockx[i] && bug.antx[j] >= blockx[i] - 1 && bug.anty[j] >= blocky[i] && bug.anty[j] <= blocky[i] + BLOCKHEIGHT)
      {
        xinc -= 1;
        if (!edget[j])
        {
          antenna[j] = TRUE;
          if (bug.ang <= PIANDAHALF && bug.ang >= HALFPI)
            antcang[j] = bug.ang - PI;
          else if (bug.ang > PIANDAHALF)
            antcang[j] = TWOPI - bug.ang;
          else
            antcang[j] = -bug.ang;
        }
      }
      else if (bugx > blockx[i] + BLOCKWIDTH && bug.antx[j] <= blockx[i] + BLOCKWIDTH + 1 && bug.anty[j] >= blocky[i] && bug.anty[j] <= blocky[i] + BLOCKHEIGHT)
      {
        xinc += 1;
        if (!edget[j])
        {
          antenna[j] = TRUE;
          if (bug.ang > HALFPI && bug.ang <= PIANDAHALF)
            antcang[j] = PI - bug.ang;
          else if (bug.ang < PIANDAHALF)
            antcang[j] = bug.ang;
          else
            antcang[j] = TWOPI - bug.ang;
        }
      }
      if (bugy < blocky[i] && bug.anty[j] >= blocky[i] - 2 && bug.antx[j] >= blockx[i] && bug.antx[j] <= blockx[i] + BLOCKWIDTH)
      {
        yinc -= 1;
        if (!edget[j])
        {
          antenna[j] = TRUE;
          if (bug.ang <= PI)
            antcang[j] = HALFPI - bug.ang;
          else
            antcang[j] = bug.ang - PIANDAHALF;
        }
      }
      else if (bugy > blocky[i] + BLOCKHEIGHT && bug.anty[j] <= blocky[i] + BLOCKHEIGHT + 2 && bug.antx[j] >= blockx[i] && bug.antx[j] <= blockx[i] + BLOCKWIDTH)
      {
        yinc += 1;
        if (!edget[j])
        {
          antenna[j] = TRUE;
          if (bug.ang <= PI)
            antcang[j] = bug.ang - HALFPI;
          else
            antcang[j] = PIANDAHALF - bug.ang;
        }
      }
      /* now check cerci */
      if (bugx < blockx[i] && bug.cercx[j] >= blockx[i] - 1 && bug.cercy[j] >= blocky[i] && bug.cercy[j] <= blocky[i] + BLOCKHEIGHT)
        xinc -= 1;
      else if (bugx > blockx[i] + BLOCKWIDTH && bug.cercx[j] <= blockx[i] + BLOCKWIDTH + 2 && bug.cercy[j] >= blocky[i] && bug.cercy[j] <= blocky[i] + BLOCKHEIGHT)
        xinc += 1;
      if (bugy < blocky[i] && bug.cercy[j] >= blocky[i] - 2 && bug.cercx[j] >= blockx[i] && bug.cercx[j] <= blockx[i] + BLOCKWIDTH)
        yinc -= 1;
      else if (bugy > blocky[i] + BLOCKHEIGHT && bug.cercy[j] <= blocky[i] + BLOCKHEIGHT + 2 && bug.cercx[j] >= blockx[i] && bug.cercx[j] <= blockx[i] + BLOCKWIDTH)
        yinc += 1;
    }
  }

  if (xinc || yinc)           /* if it bounced (it didn't if touch was on both sides) */
  {                           /* calculate bug position from old position + bounce */
    bug.x += 2.5 * sgn(xinc); /* (adjustable!) bounce */
    bug.y += 2.5 * sgn(yinc);
    for (j = 0; j < 2; j++) /* recalculate antenna & cercus tip positions */
    {
      bug.antx[j] = bug.x + antl * cos(bug.ang + antang[j]);
      bug.anty[j] = bug.y + antl * sin(bug.ang + antang[j]);
      bug.cercx[j] = bug.x + cercl * cos(bug.ang + cercang[j]);
      bug.cercy[j] = bug.y + cercl * sin(bug.ang + cercang[j]);
    }
  }
  else
  {
    bug.x = bugx;
    bug.y = bugy;
  }

  for (j = 0; j < 6; j++) /* calculate leg angles & foot positions */
  {
    if (lfoot[j] && bug.foot[j])
    { /* foot stays down, stretch leg */
      xa = bug.x + attl[j] * cos(bug.ang + attang[j]);
      ya = bug.y + attl[j] * sin(bug.ang + attang[j]);
      d = atan2(bug.footy[j] - ya, bug.footx[j] - xa);
      if (d < 0.)
        d = TWOPI + d;
      legang[j] = d - bug.ang;
      if (j < 3)
      {
        while (legang[j] > PI)
          legang[j] = legang[j] - TWOPI;
        while (legang[j] <= -PI)
          legang[j] = TWOPI + legang[j];
      }
      else
      {
        while (legang[j] < 0.)
          legang[j] = TWOPI + legang[j];
        while (legang[j] >= TWOPI)
          legang[j] = legang[j] - TWOPI;
      }
    }
    else
    { /* move foot */
      if (lfoot[j] && ((j >= 3 && legang[j] < maxlegang[j]) ||
                       (j < 3 && legang[j] > maxlegang[j])))
        legang[j] = maxlegang[j];
      if (j < 3)
        legang[j] += DT * (leg[j].fF + leg[j].bF) * PI / 15;
      else
        legang[j] -= DT * (leg[j].fF + leg[j].bF) * PI / 15;
      bug.footx[j] = bug.x + attl[j] * cos(bug.ang + attang[j]) + legl[j] * cos(bug.ang + legang[j]);
      bug.footy[j] = bug.y + attl[j] * sin(bug.ang + attang[j]) + legl[j] * sin(bug.ang + legang[j]);
    }
  }
  energy -= .25 * ENERGYPERT; /* decrement bug's energy */
  if (energy <= 0.)
  {
    bar(430, 336, 454, 348);
    outtextxy(430, 336, "RIP");
    return (1); /* back to initsim */
  }
  odor[0] = odor[1] = mouthodor = 0.;
  bug.mouthx = bug.x + hdtl * cos(bug.ang + hdtang);
  bug.mouthy = bug.y + hdtl * sin(bug.ang + hdtang);
  cf = 0;
  mo = 0.;
  for (j = 0; j < nfood; j++)
  { /* calculate odors for each antenna & mouth */
    for (k = 0; k < 2; k++)
    {
      d = bug.antx[k] - foodx[j];
      f = bug.anty[k] - foody[j];
      odor[k] += .75 * foodsize[j] / (d * d + f * f); /* (adjustable!) */
    }
    d = bug.mouthx - foodx[j];
    f = bug.mouthy - foody[j];
    d = d * d + f * f;
    mouthodor += foodsize[j] / d;
    mouthd[j] = sqrt(d);
    if (mouthodor > mo)
    { /* save largest mouthodor */
      mo = mouthodor;
      cf = j;
    }
  }

  if (obug.mouthst && !bug.mouthst) /* mouth was open, now closed */
  {
    energy += BITEENERGY / (2 * drawmult);
    foodsize[cf] -= BITEENERGY / (2 * drawmult);
    foodr[cf] = sqrt(foodsize[cf] / PI);
  }
  mouth = 0;
  for (j = 0; j < nfood; j++)
  {                                /* check for mouth contact */
    if (mouthd[j] <= 2 * foodr[j]) /* change from original */
    {
      mouth = TRUE;
      break;
    }
  }
  return (0);
}

float sensory(enum senst type, char *name, float *pI)
{ /* returns a sensory current */
  int i, j;
  float I;

  switch (type)
  {
  case LAF: /* leg angle forward */
    i = strlen(name);
    if (name[i - 2] == 'R')
      j = 2;
    else
      j = -1;
    j += name[i - 1] - '0';
    if (j < 3)
    {
      if (legang[j] >= maxlegang[j])
        I = *pI;
      else
        I = 0;
    }
    else
    {
      if (legang[j] <= maxlegang[j])
        I = *pI;
      else
        I = 0;
    }
    break;

  case LAB: /* leg angle backward */
    i = strlen(name);
    if (name[i - 2] == 'R')
      j = 2;
    else
      j = -1;
    j += name[i - 1] - '0';
    if (j < 3)
    {
      if (legang[j] <= minlegang[j])
        I = *pI;
      else
        I = 0;
    }
    else
    {
      if (legang[j] >= minlegang[j])
        I = *pI;
      else
        I = 0;
    }
    break;

  case AC: /* antenna contact */
    i = strlen(name);
    if (name[i - 1] == 'L')
      j = 0;
    else
      j = 1;
    if (!edget[j])
    {
      if (antenna[j])
      {
        edget[j] = 100;
        edgecang[j] = antcang[j];
        I = *pI * edgecang[j];
      }
      else
        I = 0;
    }
    else
    {
      edget[j]--;
      I = *pI * edgecang[j];
    }
    break;

  case OS: /* odor strength */
    i = strlen(name);
    if (name[i - 1] == 'L')
      j = 0;
    else if (name[i - 1] == 'R')
      j = 1;
    else
      j = 2;
    if (j < 2)
      I = *pI * odor[j] - *(pI + 1);
    else
      I = *pI * mouthodor - *(pI + 1);
    break;

  case EC: /* energy capacity */
    I = *pI * energy;
    break;

  case MC: /* mouth contact */
    if (mouth)
      I = *pI;
    else
      I = 0;
    break;

  default:
    I = 0;
    break;
  }

  return (I);
}
