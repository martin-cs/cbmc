
int main()
{
  while(0) {} // main.0: 0

  do {} while(0); // main.1: 0

  L: if (0) goto L; // main.2: 0
}

