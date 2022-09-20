#ifndef FUNCTIONOBJECT__H
#define FUNCTIONOBJECT__H
class FunctionObject {
  public:
    FunctionObject(){};
    virtual ~FunctionObject(){};
    virtual double operator() (double *x, double *p) {return 0;};
    virtual Bool_t isValid() { return kFALSE; };
    // void SetBaseTemplate(TH1 *inh) {baseTemplate=inh;};
  // protected:
  //   TH1 *baseTemplate; //! Not stored by the function, do not delete
};
// double FunctionObject::operator()(double *x, double *p) {
//   Double_t bc = baseTemplate->GetBinContent(baseTemplate->FindBin(x[0]));
//   return p[0]*bc + p[1]*(1 + 2*p[2]*TMath::Cos(2*x[0]) + 2*p[3]*TMath::Cos(3*x[0]));
// };

/*
class FunctionObject {
  public:
    FunctionObject():baseTemplate(0){};
    FunctionObject(TH1 *inh):baseTemplate(inh){};
    ~FunctionObject(){};
    double operator() (double *x, double *p);
    void SetBaseTemplate(TH1 *inh) {baseTemplate=inh;};
  protected:
    TH1 *baseTemplate; //! Not stored by the function, do not delete
};
double FunctionObject::operator()(double *x, double *p) {
  Double_t bc = baseTemplate->GetBinContent(baseTemplate->FindBin(x[0]));
  return p[0]*bc + p[1]*(1 + 2*p[2]*TMath::Cos(2*x[0]) + 2*p[3]*TMath::Cos(3*x[0]));
};
*/
#endif
