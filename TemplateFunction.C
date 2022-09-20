#ifndef TEMPLATE_FUNCTION__C
#define TEMPLATE_FUNCTION__C
#include "FunctionObject.h"
#include "TMath.h"
#include "TH1.h"
class TemplateFunction: public FunctionObject {
 public:
   TemplateFunction():baseTemplate(0) {};
   TemplateFunction(TH1 *inh):baseTemplate(inh){};
   ~TemplateFunction(){};
   double operator() (double *x, double *p);
   Bool_t isValid() { return baseTemplate?kTRUE:kFALSE;};
   void SetBaseTemplate(TH1 *inh) {baseTemplate=inh;};
 protected:
   TH1 *baseTemplate; //! Not stored by the function, do not delete
};
double TemplateFunction::operator()(double *x, double *p) {
  Double_t bc = baseTemplate->GetBinContent(baseTemplate->FindBin(x[0]));
  return p[0]*bc + p[1]*(1 + 2*p[2]*TMath::Cos(2*x[0]) + 2*p[3]*TMath::Cos(3*x[0]));
};
#endif
