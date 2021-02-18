
#include "defn.h"

#ifndef EXTERN
#define extern
#endif
enum asc_val { CTRLD = 4,
                      TAB = 9,
                      CR = 13,
                      CTRLS = 19,
                      ESC = 27,
                      SPACE = 32 };
enum ext_val { F1 = 59,
                      F2,
                      F3,
                      F4,
                      F5,
                      F6,
                      F7,
                      F8,
                      F9,
                      F10,
                      UP = 72,
                      LEFT = 75,
                      RIGHT = 77,
                      DOWN = 80 };
char neurfname[39]; /* current neuron filename */
char envfname[39];  /* current environment filename */
struct Iint
{                /* intrinsic current structure */
    int type;    /* type: 0=Vinf; 1=rand */
    float IL;    /* low intrinsic current, Ae-9 */
    float pL[3]; /* parameters for LIC function */
    float IH;    /* high intrinsic current, Ae-9 */
    float pH[2]; /* parameters for HIC function */
    int H;       /* TRUE if high intrinsic current on */
    float tint;  /* time since intrinsic current changed */
    float tmax;  /* maximum time since current changed */
};
struct Iint *Iinta; /* intrinsic current structure area pointer */
int ni;             /* number of intrinsic current blocks */
struct con
{                     /* neuron connections structure */
    char sname[7];    /* sending neuron name */
    int s;            /* sending neuron index */
    float Isr;        /* current multiplier from s */
    int ctype;        /* connection type: 0=reg; 1=gate; 2=mod */
    char cname[7];    /* compound synapse neuron name */
    int c;            /* compound synapse neuron index */
    float Icr;        /* compound current constant */
    int U;            /* ungated state, 0 or 1 */
    struct con *next; /* next connection */
};
struct con *cona; /* connections area pointer */
int nc;           /* number of connections */
enum senst { NONE = 0,
                    LAF,
                    LAB,
                    AC,
                    OS,
                    EC,
                    MC };
struct neuron
{                      /* basic neuron structure */
    char name[7];      /* name of neuron */
    float Gmem;        /* membrane conductance, Se-6=(1/ohm)e-6 */
    float Cmem;        /* membrane capacitance, pF=Fe-9 */
    float Vt;          /* threshold voltage, mV=Ve-3 */
    float Fmin;        /* minimum firing frequency, dimensionless */
    float Gain;        /* firing frequency gain, 1/mV = 1/(Ve-3) */
    struct Iint *Iint; /* pointer to intrinsic current parameters */
    enum senst Isens;  /* sensory current function name */
    float pI[2];       /* sensory current parameters */
    int mtype;         /* motor type: 0=none; 1=force; 2=state */
    int mname;         /* mtr name: 0=fwd|foot; 1=bck|mouth; 2=lat */
    float mconst;      /* motor constant */
    struct con *con;   /* neuron interconnections */
};
struct neuron *ns; /* pointer to neuron list */
int nn;            /* number of neurons */
struct neurcalc
{                 /* calculated quantities for neurons */
    double V;     /* voltage */
    double Vlast; /* last voltage */
    double F;     /* firing frequency */
    double Flast;
    double I; /* current */
};
struct neurcalc *nsc; /* pointer to neuron calculation list */
float attang[6];      /* leg attachment angle */
float attl[6];        /* leg attachment length */
float legl[6];        /* leg length when up */
float legang[6];      /* leg angles */
int lfoot[6];         /* last foot state */
int antenna[2];       /* antenna contact */
float antcang[2];     /* antenna contact angle */
float antang[2];      /* antenna angle to bug, constant */
float antl;           /* antenna length from bug center, constant */
int cercus[2];        /* cercus contact */
float cercang[2];     /* cercus angle to bug, constant */
float cercl;          /* cercus length from bug center, constant */
float mouthodor;      /* odor strength at mouth */
float odor[2];        /* odor strength at each antenna */
float energy;         /* energy capacity */
int mouth;            /* mouth contact */
float maxlegang[6];   /* maximum leg angles */
float minlegang[6];   /* minimum leg angles */
float DT;             /* time step */
float AspR;           /* aspect ratio */
float hdsang[2];
float hdsl;
float hdtang;
float hdtl;
float hdbang;
float hdbl;
float btang[2];
float btl;
float bmang[2];
float bml;
float bbang[2];
float bbl;
float cerbang[2];
float cerbl;
float antbang[2];
float antbl;
int nfood;           /* number of food patches */
int foodx[NFOOD];    /* food x coordinates */
int foody[NFOOD];    /* food y coordinates */
int foodsize[NFOOD]; /* food sizes */
float foodr[NFOOD];  /* radius of food */
float ofoodr[NFOOD]; /* old food radius */
int nblock;          /* number of blocks */
int blockx[NBLOCK];  /* block x coordinates */
int blocky[NBLOCK];  /* block y coordinates */
struct graph
{                  /* graph structure */
    int gtype;     /* graph types: 0-none, 1-var, 2-gait */
    char gneur[7]; /* graph neuron name */
    int g;         /* graph neuron index */
    int gvar;      /* graph variable: 0-V, 1-F, 2-I */
    int gscale;    /* graph scale */
} gr[5];
int Iextgr;    /* graph whose neuron has external current */
int Iextind;   /* index of neuron for external current */
float Iextint; /* external current/10E-9 */
struct bug
{                   /* bug location, etc. */
    float x;        /* bug x */
    float y;        /* bug y */
    float ang;      /* bug angle */
    int foot[6];    /* foot states */
    float footx[6]; /* foot x's */
    float footy[6]; /* foot y's */
    
    //definiti i tipi, in tc era possibile sottintenderli come ultimo definito(?)
    float antx[2];        /* antenna x's */
    float anty[2];        /* antenna y's */
    float cercx[2];       /* cercus x's */
    float cercy[2];       /* cercus y's */
    float mouthx;         /* mouth x */
    float mouthy;         /* mouth y */
    int mouthst;        /* mouth state: 0-closed, 1-open */
};
struct bug bug;    /* bug data */
struct bug obug;   /* old bug data */
int drawmult;      /* multiple of steps, before draws bug */
int edget[2];      /* time left for edge turning */
float edgecang[2]; /* angle of bug to edge while turning */
int plottype;      /* plot type: 0=graphs; 1=neuron activity */
struct bugpts
{                  /* structure of calculted bug pts */
    int head[8];   /* points of head */
    int body[12];  /* points of body */
    int footx[6];  /* foot x's */
    int footy[6];  /* foot y's */
    int legx[6];   /* leg attachment x's */
    int legy[6];   /* leg attachment y's */
    int antx[2];   /* antenna x's */
    int anty[2];   /* antenna y's */
    int antax[2];  /* antenna attachment x's */
    int antay[2];  /* antenna attachment y's */
    int cercx[2];  /* cercus x's */
    int cercy[2];  /* cercus x's */
    int cercax[2]; /* cercus attachment x's */
    int cercay[2]; /* cercus attachment y's */
};
struct bugpts bugp; /* bug pts */
char recpath[39];   /* path for recording file */
int recnum;         /* if 1-10, recording */
int reccnt;         /* record being filled */
struct recbug
{                          /* record for one bug recording */
    char Iextname[7];      /* external current name, if changed */
    int Iext;              /* external current, if changed */
    int energy;            /* energy */
    char graphx;           /* graph x pos */
    char graphy1[5];       /* 1st graph y pos */
    char graphy2[5];       /* 2nd graph y pos */
    char footst;           /* foot states */
    char mouthfood;        /* mouth & food states */
    int x;                 /* x pos of front of bug */
    int y;                 /* y pos of front of bug */
    signed char head[6];   /* points of head */
    signed char body[12];  /* points of body */
    signed char footx[6];  /* foot x's */
    signed char footy[6];  /* foot y's */
    signed char legx[6];   /* leg attachment x's */
    signed char legy[6];   /* leg attachment y's */
    signed char antx[2];   /* antenna x's */
    signed char anty[2];   /* antenna y's */
    signed char antax[2];  /* antenna attachment x's */
    signed char antay[2];  /* antenna attachment y's */
    signed char cercx[2];  /* cercus x's */
    signed char cercy[2];  /* cercus x's */
    signed char cercax[2]; /* cercus attachment x's */
    signed char cercay[2]; /* cercus attachment y's */
} rc[NREC];
struct recgraph
{                  /* graph structure for recording */
    int gtype;     /* graph types: 0-none, 1-var, 2-gait */
    char gneur[7]; /* graph neuron name */
    int gvar;      /* graph variable: 0-V, 1-F, 2-I */
    int gscale;    /* graph scale */
} rgr[5];
struct recinit
{                       /* initial data structure for recording */
    int foodr[NFOOD];   /* initial food radius */
    int foodx[NFOOD];   /* initial food x's */
    int foody[NFOOD];   /* initial food y's */
    int nblock;         /* number of blocks */
    int blockx[NBLOCK]; /* block x's */
    int blocky[NBLOCK]; /* block y's */
} rin;
