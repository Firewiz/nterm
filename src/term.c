#define DEFS
#include "term.h"
#undef DEFS

#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/select.h>
#include <strings.h>
#include <string.h>
#define __USE_BSD
#include <termios.h>
#include <X11/Xlib.h>

#include "gui.h"

int t_width, t_height, t_cx, t_cy;
struct cell *terminal;
FILE *prog;

int fdm, fds, rc;

void init_term() {
  t_width = 80;
  t_height = 24;
  terminal = calloc(t_width * t_height, sizeof(struct cell));

  fdm = posix_openpt(O_RDWR);
  if(fdm < 0) {
    perror("openpt");
    exit(1);
  }
  if(grantpt(fdm) < 0) {
    perror("grantpt");
    exit(1);
  }
  if(unlockpt(fdm) < 0) {
    perror("unlockpt");
    exit(1);
  }
  fds = open(ptsname(fdm), O_RDWR);
  if(fork()) {
    close(fds);
    prog = fdopen(fdm, "r+");
    // read/write happens in the event loop
  } else {
    struct termios orig_term_settings;
    struct termios new_term_settings;
    close(fdm);
    tcgetattr(fds, &orig_term_settings);
    new_term_settings = orig_term_settings;
    cfmakeraw(&new_term_settings);
    tcsetattr(fds, TCSANOW, &new_term_settings);
    close(0);
    close(1);
    close(2);
    dup2(fds, 0);
    dup2(fds, 1);
    dup2(fds, 2);
    execl("/bin/bash", "bash", NULL);
  }
}

void *term_main_loop(void *arg) {
  fd_set rfds;
  char rbuf[64];
  XEvent expose;
  while(1) {
    FD_ZERO(&rfds);
    FD_SET(fdm, &rfds);
    select(fdm + 1, &rfds, NULL, NULL, NULL);
    int nchars = read(fdm, rbuf, 64);
    int i;
    for(i = 0; i < nchars; i++) {
      add_tchar(rbuf[i]);
    }
    expose.type = Expose;
    expose.xexpose.window = xv.win;
    XSendEvent(xv.dis, xv.win, False, ExposureMask, &expose);
    XFlush(xv.dis);
  }
}

void close_term() {
  free(terminal);
}

void set_tchar(int x, int y, char c) {
  terminal[x + (y * t_width)].c = c;
}

void scroll_term() {
  int y;
  for(y = 0; y < t_height-1; y++) {
    memcpy(&terminal[y * t_width], &terminal[(y + 1) * t_width], sizeof(struct cell) * t_width);
  }
  bzero(&terminal[y * t_width], sizeof(struct cell) * t_width);
}

int vt100_escape = 0;
int vt100_argp, vt100_args[16];
int vt100_state;

void vtchar(char c) {
  switch(vt100_state) {
  case VT_STATE_READ1:
    switch(c) {
    case 'c': // RIS
    case 'D': // IND
    case 'E': // HTS
    case 'M': // RI
    case 'Z': // DECID
    case '7': // DECSC
    case '8': // DECRC
    case '>': // DECPNM
    case '=': // DECPAM
      vt100_state = -1;
      break;
    case '%':
      vt100_state = VT_STATE_SELCS;
      break;
    case '#':
      vt100_state = VT_STATE_RDECALN;
      break;
    case '(':
      vt100_state = VT_STATE_DEFG0;
      break;
    case ')':
      vt100_state = VT_STATE_DEFG1;
      break;
    case ']':
      vt100_state = VT_STATE_OSC;
      break;
    case '[':
      vt100_state = VT_STATE_CSI;
    }
    break;
  case VT_STATE_SELCS:
    switch(c) {
    case '@': // Select default character set
    case 'G': // Select UTF-8
    case '8': // Select UTF-8
      vt100_state = -1;
      break;
    }
    break;
  case VT_STATE_RDECALN:
    switch(c) {
    case '8': // DECALN
      vt100_state = -1;
      break;
    }
    break;
  case VT_STATE_DEFG0:
    switch(c) {
    case 'B': // select default graphics mapping
    case '0': // select VT100 graphics mapping
    case 'U': // select null graphics mapping
    case 'K': // select user graphics mapping
      vt100_state = -1;
      break;
    }
    break;
  case VT_STATE_DEFG1:
    switch(c) {
    case 'B': // select default graphics mapping
    case '0': // select VT100 graphics mapping
    case 'U': // select null graphics mapping
    case 'K': // select user graphics mapping
      vt100_state = -1;
      break;
    }
    break;
  case VT_STATE_OSC:
    if(c == 'P') {
      vt100_argp = 0;
      vt100_state = VT_STATE_OSC_P;
    } else if(c == 'R') {
      vt100_state = -1;
    } else if(c == '\a') {
      vt100_state = -1;
    }
    break;
  case VT_STATE_OSC_P:
    if(vt100_argp < 6) vt100_argp++;
    else vt100_state = -1;
    break;
  case VT_STATE_CSI:
    vt100_argp = 0;
    switch(c) {
    case '[':
      vt100_state = VT_STATE_CSI_DROPCHAR;
      break;
    case ';':
      vt100_argp++;
      break;
    case '@': // ICH
    case 'A': // CUU
    case 'B': // CUD
    case 'C': // CUF
    case 'D': // CUB
    case 'E': // CNL
    case 'F': // CPL
    case 'G': // CHA
    case 'H': // CUP
    case 'J': // ED
    case 'K': // EL
    case 'L': // IL
    case 'M': // DL
    case 'P': // DCH
    case 'X': // ECH
    case 'a': // HPR
    case 'c': // DA
    case 'd': // VPA
    case 'e': // VPR
    case 'f': // HVP
    case 'g': // TBC
    case 'h': // SM
    case 'l': // RM
    case 'm': // SGR
    case 'n': // DSR
    case 'r': // DECSTBM
    case 's': // save cursor
    case 'u': // restore cursor
    case '`': // HPA
      vt100_state = -1;
      break;
    case '?': // DPM
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      vt100_args[vt100_argp] *= 10;
      vt100_args[vt100_argp] += c - '0';
      break;
    }
    break;
  case VT_STATE_CSI_DROPCHAR:
    vt100_state = -1;
    break;
  }
  if(vt100_state < 0) {
    vt100_escape = 0;
  }
}

void add_tchar(char c) {
  if(vt100_escape) return vtchar(c);
  switch(c) {
  case '\n':
    t_cx = 0;
    t_cy++;
    break;
  case 0: break;
  case 0x1b:
    vt100_escape = 1;
    vt100_state = VT_STATE_READ1;
    break;
  case '\b':
    if(t_cx > 0) {
      set_tchar(t_cx - 1, t_cy, ' ');
      t_cx--;
    }
    break;
  default:
    if(c < ' ') break;
    set_tchar(t_cx++, t_cy, c);
    break;
  }
  if(t_cx >= t_width) {
    t_cx = 0;
    t_cy++;
  }
  if(t_cy >= t_height) {
    t_cy = t_height - 1;
    scroll_term();
  }
}
