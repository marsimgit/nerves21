extern void datamod(void);
extern void ndisp(struct neuron, struct Iint, int);
extern void conmod(char *, struct con *, int *);
extern void condisp(char *, struct con *, int, int, int);

//orginale:
//extern int near update(void);
extern int update(void);

extern float sensory(enum senst, char *, float *);
extern void graph(void);
extern void Draw(void);
extern void Initialize(void);
extern void graphsetup(void);
extern void activesetup(void);
extern void nsmenu(void);
extern void erasecursor(void);
extern void clrline(int);
extern void drawbug(struct bug, int);
extern void showbug(struct bug, int);
extern int initsim(void);

//originale:
//extern int near singstep(int *, double *, FILE *, int *);
extern int singstep(int*, double*, FILE*, int*);
//originale:
//extern int near multstep(int *, double *, FILE *, int *);
extern int multstep(int*, double*, FILE*, int*);
