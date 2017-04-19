//testing=trivial 2006.07.17
#include <stdio.h> //BUG// sometimes required by readline.h and history.h
#include <readline/readline.h>
#include <readline/history.h>
int main()
{
  char* line_read = readline("xx");
  add_history(line_read);
}
// vim:ts=8:sw=2:noet:
