
#include "defn.h"

enum asc_val {CTRLD=4,TAB=9,CR=13,CTRLS=19,ESC=27,SPACE=32};
enum ext_val {F1=59,F2,F3,F4,F5,F6,F7,F8,F9,F10,
		     UP=72,LEFT=75,RIGHT=77,DOWN=80};
char neurfname[39];      /* current neuron filename */
char envfname[39];       /* current environment filename */
struct Iint {		/* intrinsic current structure */
       int type;		/* type: 0=Vinf; 1=rand */
       float IL;		/* low intrinsic current, Ae-9 */
       float pL[3];		/* parameters for LIC function */
       float IH;		/* high intrinsic current, Ae-9 */
       float pH[2];		/* parameters for HIC function */
       int H;                   /* TRUE if high intrinsic current on */
       int iIL;			/* low intrinsic current */
       int ipL[3];		/* parameters for LIC function */
       int iIH;			/* high intrinsic current, Ae-9 */
       int ipH[2];		/* parameters for HIC function */
       int tint;		/* time since intrinsic current changed, msec */
       int tmax;		/* maximum time since current changed, msec */
       };
struct Iint *Iinta;	/* intrinsic current structure area pointer */
int ni;			/* number of intrinsic current blocks */
struct con {		/* neuron connections structure */
       char sname[7];           /* sending neuron name */
       int s;			/* sending neuron index */
       float Isr;		/* current multiplier from s */
       int ctype;		/* connection type: 0=reg; 1=gate; 2=mod */
       char cname[7];           /* compound synapse neuron name */
       int c;			/* compound synapse neuron index */
       float Icr;		/* compound current constant */
       int U;			/* ungated state, 0 or 1 */
       struct con *next;	/* next connection */
       int iIsr;		/* current integer multiplier from s */
       int iIcr;		/* integer compound current constant */
       };
struct con *cona;	/* connections area pointer */
int nc;			/* number of connections */
enum senst {NONE=0,LAF,LAB,AC,OS,EC,MC};
struct neuron {		/* basic neuron structure */
       char name[7];		/* name of neuron */
       float Gmem;		/* membrane conductance, Se-6=(1/ohm)e-6 */
       float Cmem;		/* membrane capacitance, pF=Fe-9 */
       float Vt;		/* threshold voltage, mV=Ve-3 */
       float Fmin;		/* minimum firing frequency, dimensionless */
       float Gain;		/* firing frequency gain, 1/mV = 1/(Ve-3) */
       struct Iint *Iint;	/* pointer to intrinsic current parameters */
       enum senst Isens;	/* sensory current function name */
       float pI[2];             /* sensory current parameters */
       int mtype;		/* motor type: 0=none; 1=force; 2=state */
       int mname;		/* mtr name: 0=fwd|foot; 1=bck|mouth; 2=lat */
       float mconst;		/* motor constant */
       struct con *con;		/* neuron interconnections */
       int iGmem;		/* membrane conductance */
       int iCmem;		/* membrane capacitance */
       long iVt;		/* threshold voltage */
       int iFmin;		/* minimum firing frequency; 0-100 */
       int iGain;		/* gain */
       long ipI[2];		/* sensory current parameters */
       int imconst;		/* motor constant */
       };
struct neuron *ns;	/* pointer to neuron list */
int nn;			/* number of neurons */
struct neurcalc {	/* calculated quantities for neurons */
       long V;			/* voltage; millivolts */
       long Vlast;		/* last voltage */
       int F;			/* firing frequency; 0-100 */
       int Flast;		/* last firing frequency */
       long I;			/* current */
       };
struct neurcalc *nsc;	/* pointer to neuron calculation list */
int attang[6];	        /* leg attachment angle (100xradians)*/
int attl[6];             /* leg attachment length */
int legl[6];             /* leg length when up */
int legang[6];           /* leg angles */
int lfoot[6];            /* last foot state */
int antenna[2];          /* antenna contact */
int antcang[2];          /* antenna contact angle */
int antang[2];           /* antenna angle to bug, constant */
int antl;                /* antenna length from bug center, constant */
int cercus[2];           /* cercus contact */
int cercang[2];          /* cercus angle to bug, constant */
int cercl;               /* cercus length from bug center, constant */
long mouthodor;           /* odor strength at mouth */
long odor[2];             /* odor strength at each antenna */
long energy;             /* energy capacity */
int mouth;               /* mouth contact */
int maxlegang[6];        /* maximum leg angles */
int minlegang[6];        /* minimum leg angles */
int DT;			/* time step */
int AspR;                /* aspect ratio */
int hdsang[2];
int hdsl;
int hdtang;
int hdtl;
int hdbang;
int hdbl;
int btang[2];
int btl;
int bmang[2];
int bml;
int bbang[2];
int bbl;
int cerbang[2];
int cerbl;
int antbang[2];
int antbl;
int nfood;               /* number of food patches */
int foodx[NFOOD];        /* food x coordinates */
int foody[NFOOD];        /* food y coordinates */
long foodsize[NFOOD];    /* food sizes */
int foodr[NFOOD];        /* radius of food */
int ofoodr[NFOOD];       /* old food radius */
int nblock;              /* number of blocks */
int blockx[NBLOCK];      /* block x coordinates */
int blocky[NBLOCK];      /* block y coordinates */
struct graph {           /* graph structure */
       int gtype;               /* graph types: 0-none, 1-var, 2-gait */
       char gneur[7];           /* graph neuron name */
       int g;                   /* graph neuron index */
       int gvar;                /* graph variable: 0-V, 1-F, 2-I */
       int gscale;              /* graph scale */
       } gr[5];
int Iextgr;              /* graph whose neuron has al current */
int Iextind;             /* index of neuron for al current */
long Iextint;            /* al current */
struct bug {             /* bug location, etc. */
       long x;                  /* bug x (1000xactual) */
       long y;                  /* bug y (1000xactual) */
       long ang;                /* bug angle (100000xactual) */
       int foot[6];             /* foot states */
       int footx[6];            /* foot x's */
       int footy[6];            /* foot y's */
       int antx[2];		/* antenna x's */
       int anty[2];		/* antenna y's */
       int cercx[2];		/* cercus x's */
       int cercy[2];		/* cercus y's */
       int mouthx;		/* mouth x */
       int mouthy;		/* mouth y */
       int mouthst;		/* mouth state: 0-closed, 1-open */
       };
struct bug bug;          /* bug data */
struct bug obug;         /* last bug data */
struct bugpts {		/* structure of calculted bug pts */
       int head[8];		/* points of head */
       int body[12];		/* points of body */
       int footx[6];		/* foot x's */
       int footy[6];		/* foot y's */
       int legx[6];		/* leg attachment x's */
       int legy[6];		/* leg attachment y's */
       int antx[2];		/* antenna x's */
       int anty[2];		/* antenna y's */
       int antax[2];		/* antenna attachment x's */
       int antay[2];		/* antenna attachment y's */
       int cercx[2];		/* cercus x's */
       int cercy[2];		/* cercus x's */
       int cercax[2];		/* cercus attachment x's */
       int cercay[2];		/* cercus attachment y's */
       };
struct bugpts bugp;	/* bug pts */
int drawmult;            /* multiple of steps, before draws bug */
int edget[2];            /* time left for edge turning */
int edgecang[2];         /* angle of bug to edge while turning */
int plottype;            /* plot type: 0=graphs; 1=neuron activity */
int isin[TWOPI/2+1];     /* int sin table */
int icos[TWOPI/2+1];	/* int cos table */
char recpath[39];	/* path for recording file */
int recnum;		/* if 1-10, recording */
int reccnt;		/* record being filled */
struct recbug {		/* record for one bug recording */
       char Iextname[7];	/* al current name, if changed */
       int Iext;		/* al current, if changed */
       int energy;		/* energy */
       char graphx;		/* graph x pos */
       char graphy1[5];		/* 1st graph y pos */
       char graphy2[5];		/* 2nd graph y pos */
       char footst;		/* foot states */
       char mouthfood;		/* mouth & food states */
       int x;			/* x pos of front of bug */
       int y;			/* y pos of front of bug */
       signed char head[6];	/* points of head */
       signed char body[12];	/* points of body */
       signed char footx[6];	/* foot x's */
       signed char footy[6];	/* foot y's */
       signed char legx[6];	/* leg attachment x's */
       signed char legy[6];	/* leg attachment y's */
       signed char antx[2];	/* antenna x's */
       signed char anty[2];	/* antenna y's */
       signed char antax[2];	/* antenna attachment x's */
       signed char antay[2];	/* antenna attachment y's */
       signed char cercx[2];	/* cercus x's */
       signed char cercy[2];	/* cercus x's */
       signed char cercax[2];	/* cercus attachment x's */
       signed char cercay[2];	/* cercus attachment y's */
       } rc[NREC];
struct recgraph {	/* graph structure for recording */
       int gtype;               /* graph types: 0-none, 1-var, 2-gait */
       char gneur[7];           /* graph neuron name */
       int gvar;                /* graph variable: 0-V, 1-F, 2-I */
       int gscale;              /* graph scale */
       } rgr[5];
struct recinit {		/* initial data structure for recording */
       int foodr[NFOOD];	/* initial food radius */
       int foodx[NFOOD];	/* initial food x's */
       int foody[NFOOD];	/* initial food y's */
       int nblock;		/* number of blocks */
       int blockx[NBLOCK];	/* block x's */
       int blocky[NBLOCK];	/* block y's */
       } rin;
