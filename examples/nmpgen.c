/* gen.c  03/19/93
 * Generate cascased nmos inverters to test simulators
 */
#include <stdio.h>
void main(argc,argv)
int argc;
const char *argv[];
{
 unsigned count, ii;
 (void)sscanf(argv[1], "%u",  &count);
 printf("%u cascaded NMOS inverters\n", count);
 for (ii = 1;  ii <= count;  ii++){
    int in = ii*2-1;
    int out = ii*2+1;
    int pwr = ii*2;
    printf("vd%u %u 0 5\n", ii, pwr);
    printf("md%u  %u %u %u %u  modeld w=10u l=2u\n", ii, out, in, 0, 0);
    printf("ml%u  %u %u %u %u  modell w=2u l=2u\n", ii, pwr, pwr, out, 0);
 }
 printf("vin 1 0 .8\n");
 printf(".MODEL MODELD NMOS (level=2 KP=28U VTO=0.7 LAMBDA=0.01 GAMMA=0.9 PHI=0.5)\n");
 printf(".MODEL MODELL NMOS (level=2 KP=28U VTO=0.7 LAMBDA=0.01 GAMMA=0.9 PHI=0.5)\n");
 printf(".PRINT OP iter(0) V(nodes)\n");
 printf(".op\n");
 printf(".options acct\n");
 printf(".end\n");
}
