#include <pthread.h>

#include "gui.h"
#include "evloop.h"
#include "term.h"

int main(int argc, char ** argv) {
  init_x();
  init_term();
  pthread_t thread_term, thread_gui;
  pthread_create(&thread_gui, NULL, x_main_loop, NULL);
  pthread_create(&thread_term, NULL, term_main_loop, NULL);
  pthread_join(thread_gui, NULL);
  pthread_cancel(thread_term);
  close_term();
  close_x();
}
