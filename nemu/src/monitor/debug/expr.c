#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

void expr_test(void);
enum {
  TK_NOTYPE = 256, TK_EQ,
  /* TODO: Add more token types */
  TK_HEX, TK_DECI,
  TK_LP, TK_RP,
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"\\-", '-'},         // sub
  {"\\*", '*'},         // times
  {"\\/", '/'},         // divide
  {"0[Xx][0-9a-fA-F]+", TK_HEX},    // hex number
  {"[0-9]+", TK_DECI},  // decimal number 
  {"\\(", TK_LP},       // left parenthesis
  {"\\)", TK_RP},       // right parenthesis
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;
  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }

  expr_test();
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        if (substr_len >= 32){
          printf("Matched a redundant length.\n");
          assert(0);
        }
        if (nr_token >= 32){
          printf("Token number exceeded.\n");
          return false;
        }        

        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          case '+':
          case '-':
          case '*':
          case '/':
          case TK_LP:
          case TK_RP:
            tokens[nr_token].type = rules[i].token_type;
            nr_token += 1;
            break;
          case TK_HEX:
          case TK_DECI:
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].type = rules[i].token_type;
            nr_token += 1;
            break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  *success = true;
  return 0;
  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}

void expr_test(void){
  bool success;
  char exp1[20] = "0x75aF + 124";
  expr(exp1, &success);
  Assert(success, "error1\n");
  char exp2[20] = "66afa + 13";
  expr(exp2, &success);
  Assert(!success, "error2\n");
  char exp3[50] = "(381 / 0x220 *   23) == (0x7a - 124)";
  expr(exp3, &success);
  Assert(success, "error3\n");
}