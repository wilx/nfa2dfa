#include <iostream>
#include <cstdio>

extern int yyparse (void);

int main(void)
{
   return yyparse();
}
