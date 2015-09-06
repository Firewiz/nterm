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
    dup(fds);
    dup(fds);
    dup(fds);
    execl("/bin/bash", "bash", NULL);
  }
}

void *term_main_loop(void *arg) {
  fd_set rfds;
  char rbuf[64];
  while(1) {
    FD_ZERO(&rfds);
    FD_SET(fdm, &rfds);
    select(fdm + 1, &rfds, NULL, NULL, NULL);
    int nchars = read(fdm, rbuf, 64);
    int i;
    for(i = 0; i < nchars; i++) {
      add_tchar(rbuf[i]);
    }
    redraw();
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

void add_tchar(char c) {
  switch(c) {
  case '\n':
    t_cx = 0;
    t_cy++;
    break;
  case 0: break;
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
