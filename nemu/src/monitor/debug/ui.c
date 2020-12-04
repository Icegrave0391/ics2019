#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void isa_reg_display();
void cpu_exec(uint64_t);
uint32_t expr(char *e, bool *success);
uint32_t paddr_read(paddr_t addr, int len);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);
static int cmd_info(char *args);
static int cmd_si(char *args);
static int cmd_x(char *args);
static int cmd_watch(char *args);
static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "info", "Print the program state (info r: registers) and (info w: watchpoints)", cmd_info },
  { "si", "Step over the program by n steps", cmd_si },
  { "x", "Scan n memory units from addr[expr] (x N expr)", cmd_x },
  { "watch", "Set a watchpoint to an expression (x expr)", cmd_watch},
  /* TODO: Add more commands */
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_info(char *args){
  char *arg = strtok(NULL, " ");
  if (arg == NULL){
    printf("Use info r or info w instead!\n");
  }
  else if (strcmp(arg, "r") == 0){
    isa_reg_display();
  }
  else if (strcmp(arg, "w") == 0){
    description_wp();
  }
  return 0;
}

static int cmd_si(char *args){
  char *arg = strtok(NULL, " ");
  if (arg == NULL){
    cpu_exec(1);
  }
  else
  {
    int n;
    sscanf(arg, "%d", &n);
    if (n > 0){
      cpu_exec(n);
    }
  }
  return 0;
}

static int cmd_x(char *args){
  int n = 0;
  bool success = true;
  char exp[50];
  sscanf(args, "%d %s", &n, exp);
  paddr_t addr = expr(exp, &success);
  if (!(n > 0 && success)){
    printf("Usage: x <num> <expression (valid expr)>\n");
    return 0;
  }
  for(int i = 0; i < n; i++){
    paddr_t base = addr + 4 * i;
    printf("0x%08x: ", base);
    for(int o = 0; o < 4; o++){
      uint32_t data = paddr_read(base + o, 1);
      printf("%02x ", (unsigned char)(data));
    }
    printf("(0x%08x)\n", paddr_read(base, 4));
  }
  return 0;
}

static int cmd_watch(char *args){
  bool success = true;
  char exp[50];
  sscanf(args, "%s", exp);
  uint32_t val = expr(exp, &success);
  Assert(success, "Invaild expression %s.\n", exp);
  new_wp(exp, val);
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
