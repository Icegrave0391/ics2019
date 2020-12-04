#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
/* Initialize a new watchpoint (update `head` pointer) */
WP * new_wp(){
  if(!free_){
    printf("[new_wp failed] wp_pool used up.\n");
    assert(0);
    return NULL;
  }
  // update free_
  WP * new = free_;
  free_ = free_ -> next;
  // update head
  new -> next = head;
  head = new;
  return new;
}

void free_wp(WP* wp){
  // activated wp list (head)
  if(wp == head){ // free head
    head = head -> next;
    wp -> next = free_;
    free_ = wp;
    return;
  }
  else{
    WP *p = head;
    while(p -> next){
      if (p -> next == wp){ // free wp
        p -> next = wp -> next;
        wp -> next = free_;
        free_ = wp;
        return;
      }
      p = p -> next;
    }
  }
  printf("[free_wp failed] WP %d is not in head list.\n", wp->NO);
}

/* Polling each watchpoint in the activated list (head list) */
int polling_activated_wp(){
  int num_trig = 0;
  WP * p = head;
  while(p){
    bool success = true;
    uint32_t new_val = expr(p->wp_expr, &success);
    Assert(success, "[expr error]Wrong expression: %s for watchpoint %d", p->wp_expr, p->NO);
    if (new_val != p->wp_value){
      printf("Hardware watchpoint %d: %s\n", p->NO, p->wp_expr);
      printf("Old value = 0x%08x(%u)\n", p->wp_value, p->wp_value);
      printf("New value = 0x%08x(%u)\n", new_val, new_val);
      num_trig += 1;
      // update wp
      p -> wp_value = new_val;
    }
    p = p -> next;
  }
  return num_trig;
}

void description_wp(){
  if(!head){

  }
}