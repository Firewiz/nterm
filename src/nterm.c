#include "gui.h"
#include "evloop.h"

int main(int argc, char ** argv) {
  init_x();
  init_term();
  set_tchar(0, 0, 'W');
  set_tchar(1, 0, 'e');
  main_loop();
  close_term();
  close_x();
}
