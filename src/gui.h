#ifndef GUI_H
#define GUI_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

struct X_vars {
  Display *dis;
  int screen;
  Window win;
  GC gc;
  XFontStruct *ft;
};

#ifndef DEFS
extern struct X_vars xv;
#endif

void init_x();
void close_x();

#endif
