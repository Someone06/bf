#include <stdio.h>

extern void bfMain();

void bfOut(char c) { putchar(c); }

char bfIn() { return getchar(); }

int main(int argc, char* argv[]) { bfMain(); }