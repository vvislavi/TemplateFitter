#ifndef DUMMY2DFUNCTION__C
#define DUMMY2DFUNCTION__C
#include "FunctionObject.h"
#include "TMath.h"
#include "TH1.h"
class Dummy2DFunction: public FunctionObject {
 public:
   Dummy2DFunction() {};
   ~Dummy2DFunction(){};
   double operator() (double *x, double *p);
   Bool_t isValid() { return kTRUE;};
};
double Dummy2DFunction::operator()(double *x, double *p) {
  return TMath::Sin(x[0]*p[0])+x[1]*p[1]+p[0]*x[0]*x[1];
};
#endif
