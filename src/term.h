#ifndef TERM_H
#define TERM_H


struct cell {
  char c;
};

#ifndef DEFS
extern int t_width, t_height;
extern int t_cx, t_cy;
extern struct cell *terminal;

#include <stdio.h>

extern FILE *prog;
#endif

void init_term();
void *term_main_loop(void *arg);
void set_tchar(int x, int y, char c);
void add_tchar(char c);
void close_term();

#define VT_STATE_READ1 0
#define VT_STATE_SELCS 1
#define VT_STATE_RDECALN 2
#define VT_STATE_DEFG0 3
#define VT_STATE_DEFG1 4
#define VT_STATE_OSC 5
#define VT_STATE_OSC_P 6
#define VT_STATE_CSI 7
#define VT_STATE_CSI_DROPCHAR 8

#endif
