#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char wp_expr[50];   // expression
  uint32_t wp_value;  // value calculated
  bool trig;
} WP;

#endif
