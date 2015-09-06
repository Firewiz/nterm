#ifndef TERM_H
#define TERM_H


struct cell {
  char c;
};

#ifndef DEFS
extern int t_width, t_height;
extern int t_cx, t_cy;
extern struct cell *terminal;
extern FILE *prog;
#endif

void init_term();
void set_tchar(int x, int y, char c);
void close_term();

#endif
