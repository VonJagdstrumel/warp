#include <stdio.h>
#include <unistd.h>

int main() {
  puts("Test...");
  sleep(2);
  char mystring[100];
  FILE* pFile = fopen("res/res.txt", "r");
  fgets(mystring, 100, pFile);
  puts(mystring);
  fclose(pFile);
  return 0;
}
