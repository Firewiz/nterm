#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include "gui.h"
#include "term.h"

void redraw(void) {
  XClearWindow(xv.dis, xv.win);
  int x, y;
  for(y = 0; y < t_height; y++) {
    for(x = 0; x < t_width; x++) {
      if(terminal[x + (y * t_width)].c)
	XDrawString(xv.dis, xv.win, xv.gc, x * 6, y * 12 + 12, &terminal[x + (y * t_width)].c, 1);
    }
  }
  XDrawRectangle(xv.dis, xv.win, xv.gc, t_cx * 6, t_cy * 12 + 12, 6, 0);
}

void main_loop(void) {
  XEvent evt;
  KeySym key;
  char typeahead[256];
  while(1) {
    XNextEvent(xv.dis, &evt);
    if(evt.type == ClientMessage) {
      break;
    } else if(evt.type == Expose && evt.xexpose.count == 0) {
      redraw();
    }
  }
}
