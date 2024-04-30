//testing=trivial 2006.07.17
#include <readline/readline.h>
#include <readline/history.h>
int main()
{ untested();
  char* line_read = readline("xx");
  add_history(line_read);
}
