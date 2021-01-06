#include "Prob.h"
#include <stdlib.h>
#include <math.h>

 int poisson(double mean) {
	 float u = (float)rand() / RAND_MAX;
	 int i = 0;
	 double eVal = pow(2.718, -mean);
	 double k = 1;
	 double l = eVal;
	 while (l < u) {
		 i++;
		 k *= mean / i;
		 l += eVal * k;
	 }
	 return i;
}