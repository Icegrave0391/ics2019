#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

void expr_test(void);
uint32_t eval(int p, int q, bool *success);
uint32_t isa_reg_str2val(const char *s, bool *success);
void reset_tokens();

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NEQ,
  /* TODO: Add more token types */
  TK_HEX, TK_DECI,
  TK_LP, TK_RP,
  TK_REG,
  TK_AND,
  TK_POINTER, TK_NEG,
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
  {"!=", TK_NEQ},       // not equal
  {"&&", TK_AND},       // and
  {"0[Xx][0-9a-fA-F]+", TK_HEX},    // hex number
  {"[0-9]+", TK_DECI},  // decimal number 
  {"\\(", TK_LP},       // left parenthesis
  {"\\)", TK_RP},       // right parenthesis
  {"\\$[a-zA-Z]{2,3}", TK_REG}
  
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
  // // TODO: comment this
  // expr_test();
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

void reset_tokens(){
  for(int i = 0; i < 32; i++){
    memset(tokens + i, 0, sizeof(Token));
  }
}

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
  reset_tokens();   // tricky: convenient for debug
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
        int last_tp = 0;
        if (nr_token){
          last_tp = tokens[nr_token - 1].type;
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
          case TK_EQ:
          case TK_NEQ:
          case TK_AND:
            tokens[nr_token].type = rules[i].token_type;
            if(rules[i].token_type == '-'){
              // determine whether sub_operator or negative
              if (!nr_token || (last_tp != TK_DECI && last_tp != TK_HEX && last_tp != TK_REG && last_tp != TK_RP)){
                  tokens[nr_token].type = TK_NEG;
                } 
            }
            else if(rules[i].token_type == '*'){
              // determine whether multi_operator or pointer
              if (!nr_token || (last_tp != TK_DECI && last_tp != TK_HEX && last_tp != TK_REG && last_tp != TK_RP)){
                  tokens[nr_token].type = TK_POINTER;
                } 
            }
            nr_token += 1;
            break;
          case TK_HEX:
          case TK_DECI:
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].type = rules[i].token_type;
            nr_token += 1;
            break;
          case TK_REG:  //$REG_NAME -> REG_NAME -> reg_name (lower)
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start + 1, substr_len - 1);
            for(int i = 0; i < substr_len -1; i++){
              if(tokens[nr_token].str[i] >= 'A' && tokens[nr_token].str[i] <= 'Z'){
                tokens[nr_token].str[i] += 'a' - 'A';
              }
            }
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

  uint32_t res = eval(0, nr_token - 1, success);
  return res;
}

/* A function to determine if the whole expression is in a parenthese */
bool check_parentheses(int p, int q){
  if(tokens[p].type != TK_LP || tokens[q].type != TK_RP){
    return false;
  }
  int flg = 0;
  for (int i = p+1; i <= q-1; i++){
    if(tokens[i].type == TK_LP){
      flg += 1;
    }
    else if(tokens[i].type == TK_RP){
      flg -= 1;
    }
    if(flg < 0){
      return false;
    }
  }
  if(flg == 0){
    return true;
  }
  return false;
}

/* evaluate the operator priority for token */
int get_token_priority(int token_type){
  // none-operator tokens
  if (token_type == TK_NOTYPE || token_type == TK_REG || token_type == TK_HEX ||
  token_type == TK_DECI){
    return 0;
  }
  if (token_type == TK_AND){
    return 1;
  }
  if (token_type == TK_EQ || token_type == TK_NEQ){
    return 2;
  }
  if (token_type == '+' || token_type == '-'){
    return 3;
  }
  if (token_type == '*' || token_type == '/'){
    return 4;
  }
  if (token_type == TK_LP || token_type == TK_RP){
    return 6;
  }
  if (token_type == TK_POINTER || token_type == TK_NEG){
    return 5;
  }
  return 0;
}

/* A function to find the main_operator in a expression*/
int find_main_op(int p, int q){
  // initialization
  int cur_prior = 99;
  int main_loc = p;
  int in_paren = 0;
  // iterate
  for (int i = p; i <= q; i++){
    if(tokens[i].type == TK_LP){
      in_paren += 1;
    }
    else if(tokens[i].type == TK_RP){
      in_paren -= 1;
    }
    if(in_paren) continue;

    int tok_prior = get_token_priority(tokens[i].type);
    if(tok_prior <= cur_prior && tok_prior){
      cur_prior = tok_prior;
      main_loc = i;
    }
  }
  return main_loc;
}


/* Handler for the bad cases during expression evaluation.
 * Just update success -> false and return 0.
 */
uint32_t fail_handler(int p, int q, bool *success){
  *success = false;
  /* bad expression */
  if (p > q){
    printf("Bad expression for location (%d, %d)\n", p, q);
    Token bad_t = tokens[p];
    if (bad_t.type == TK_HEX || bad_t.type == TK_DECI || bad_t.type == TK_REG){
      printf("\t->Bad token val: %s\n", bad_t.str);
    }
  }
  /* single token */
  if (p == q){
    printf("Bad single token for location %d\n", p);
  }
  return 0;
}

/* Eval the sub_expression from location p(start) to q(end) 
 * The result of the expression(if succeeded) will be returned.
 * :param *success: to store if the evaluation is successful.
 */
uint32_t eval(int p, int q, bool *success){
  if (p > q){
    /* Bad expression */
    return fail_handler(p, q, success);
  }
  else if (p == q){
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    Token single_t = tokens[p];
    uint32_t num_val = 0;
    if (single_t.type == TK_HEX){  // hex value
      sscanf(single_t.str, "%x", &num_val);
    }
    else if (single_t.type == TK_DECI){ // decimal value
      sscanf(single_t.str, "%d", &num_val);
    }
    else if (single_t.type == TK_REG){
      num_val = isa_reg_str2val(single_t.str, success);
    }
    else {
      return fail_handler(p, q, success);
    }
    return num_val;
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1, success);
  }
  else{
    /* Find main operator (operator with the lowest priority) */
    int main_op_loc = find_main_op(p, q);

    /* case unary operation (negative or pointer) */
    if (tokens[main_op_loc].type == TK_NEG){
      return -eval(main_op_loc + 1, q, success);
    }
    if (tokens[main_op_loc].type == TK_POINTER){
      return vaddr_read(eval(main_op_loc + 1, q, success), 4);
    }

    /* case binary operation */
    uint32_t val1 = eval(p, main_op_loc - 1, success);
    uint32_t val2 = eval(main_op_loc + 1, q, success);
    switch (tokens[main_op_loc].type){
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/':
        if(!val2){
          *success = false;
          return 0;
        }
        return val1 / val2;
      case TK_EQ: return val1 == val2;
      case TK_NEQ: return val1 != val2;
      case TK_AND: return val1 && val2;
      default: assert(0);
    }
  }
}



/* JUST FOR TEST */
void expr_test(void){
  bool success=true;
  uint32_t res;
  char exp1[20] = "0x75aF +     124";
  res =  expr(exp1, &success);
  Assert(success, "error1\n");
  Log("result1: %d\n", res);

  char exp2[20] = "66 + 13";
  res = expr(exp2, &success);
  Assert(success, "error2\n");
  Log("res2: %d\n", res);

  char exp3[50] = "(381 / 0x220 *   23) == (0x7a - 124)";
  res = expr(exp3, &success);
  Assert(success, "error3\n");
  Log("res3: %d\n", res);

  char exp4[50] = "$eax + 0x1";
  res = expr(exp4, &success);
  Assert(success, "error4\n");
  Log("res4: %d\n", res);
  
  char exp5[50] = "52 + -2";
  res = expr(exp5, &success);
  Assert(success, "error5\n");
  Log("res5: %d\n", res);

  char exp6[50] = "*(0x00100000 + 0x4)";
  res = expr(exp6, &success);
  Assert(success, "error6\n");
  Log("res6: %d\n", res); 
}