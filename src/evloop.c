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

void *x_main_loop(void *arg) {
  XEvent evt;
  KeySym key;
  char typeahead[256];
  int len;
  while(1) {
    XNextEvent(xv.dis, &evt);
    if(evt.type == ClientMessage) {
      break;
    } else if(evt.type == Expose && evt.xexpose.count == 0) {
      redraw();
    } else if(evt.type == KeyPress) {
      len = XLookupString(&evt.xkey, typeahead, 256, &key, 0);
      int i;
      for(i = 0; i < len; i++) {
	if(typeahead[i] == '\r') {
	  typeahead[i] = '\n';
	}
	add_tchar(typeahead[i]);
      }
      fwrite(typeahead, sizeof(char), len, prog);
      redraw();
    }
  }
}
