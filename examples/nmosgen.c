/* gen.c  03/19/93
 * Generate cascased nmos inverters to test simulators
 */
#include <libc.h>
void main(argc,argv)
int argc;
const char *argv[];
{
 unsigned count, ii;
 (void)sscanf(argv[1], "%u",  &count);
 printf("%u cascaded NMOS inverters\n", count);
 for (ii = 1;  ii <= count;  ii++){
    int jj = ii+1;
    int kk = jj+1;
    printf("md%u  %u %u %u %u  modeld w=10u l=2u\n", ii, kk, jj, 0, 0);
    printf("ml%u  %u %u %u %u  modell w=2u l=2u\n", ii, 1, 1, kk, 0);
 }
 printf("vdd 1 0 5\n");
 printf("vin 2 0 .8\n");
 printf(".MODEL MODELD NMOS (level=2 KP=28U VTO=0.7 LAMBDA=0.01 GAMMA=0.9 PHI=0.5)\n");
 printf(".MODEL MODELL NMOS (level=2 KP=28U VTO=0.7 LAMBDA=0.01 GAMMA=0.9 PHI=0.5)\n");
 printf(".PRINT OP iter(0) V(nodes)\n");
 printf(".op\n");
 printf(".options acct\n");
 printf(".end\n");
}
