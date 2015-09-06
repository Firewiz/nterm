#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#define DEFS
#include "gui.h"
#undef DEFS

struct X_vars xv;

void init_x() {
  unsigned long black, white;
  xv.dis = XOpenDisplay((char *) 0);
  xv.screen = DefaultScreen(xv.dis);
  black = BlackPixel(xv.dis, xv.screen);
  white = WhitePixel(xv.dis, xv.screen);
  xv.win = XCreateSimpleWindow(xv.dis, DefaultRootWindow(xv.dis), 0, 0, 480, 288, 5, white, black);
  XSetStandardProperties(xv.dis, xv.win, "NTerm", "NT", None, NULL, 0, NULL);
  XSelectInput(xv.dis, xv.win, ExposureMask | ButtonPressMask | KeyPressMask);
  xv.ft = XLoadQueryFont(xv.dis, "6x12");
  xv.gc = XCreateGC(xv.dis, xv.win, 0, 0);
  XSetBackground(xv.dis, xv.gc, black);
  XSetForeground(xv.dis, xv.gc, white);
  XSetFont(xv.dis, xv.gc, xv.ft->fid);
  
  XClearWindow(xv.dis, xv.win);
  XMapRaised(xv.dis, xv.win);

  Atom WM_DELETE_WINDOW = XInternAtom(xv.dis, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(xv.dis, xv.win, &WM_DELETE_WINDOW, 1);
}

void close_x() {
  XFreeFont(xv.dis, xv.ft);
  XFreeGC(xv.dis, xv.gc);
  XDestroyWindow(xv.dis, xv.win);
  XCloseDisplay(xv.dis);
}
