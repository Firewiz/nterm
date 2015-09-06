#define DEFS
#include "term.h"
#undef DEFS

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

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
    tcsetattr(fds, TCSANOW, new_term_settings);
    close(0);
    close(1);
    close(2);
    dup(fds);
    dup(fds);
    dup(fds);
    execl("/bin/bash", NULL);
  }
}

void close_term() {
  free(terminal);
}

void set_tchar(int x, int y, char c) {
  terminal[x + (y * t_height)].c = c;
}
