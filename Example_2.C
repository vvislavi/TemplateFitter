#include "TemplateFitter.cxx"
#include "TemplateFunction.C"
TH1 *lm, *hm;
void Init() {
  TFile *tf = new TFile("dummy.root");
  lm = (TH1*)tf->Get("proj_TPCFMDC_1D_60_100");
  hm = (TH1*)tf->Get("proj_TPCFMDC_1D_0_20");
};
void Example_2() {
  //Load projections
  Init();
  //Initialize fitter with given projections
  TemplateFitter *ft = new TemplateFitter(hm);
  //Setting up variable ( = delta phi, or just "x"):
  ft->AddVariable("x", "x", -TMath::Pi()/2, 0.75*TMath::TwoPi());
  //Setting up arguments: name, title, initial value, minimum, maximum
  ft->AddParameter("Ga","Ga",1,0,10000);
  ft->AddParameter("Fa","Fa",1,0,10000);
  ft->AddParameter("v2","v2",0.1,0,0.5);
  ft->AddParameter("v3","v3",0.1,0,0.5);
  //Construct fit function
  FunctionObject *fobj = new TemplateFunction(lm);
  ft->SetFitFunction(fobj);
  //Perform fit:
  printf("About to fit\n");
  Int_t dummy = ft->Fit(0); //Do not draw performance at this point. Return value of Fit() is false if no base is set.
  if(!dummy) return;
  Double_t v21 = ft->getVal(2); //0 for G, 1 for F, 2 for v2, 3 for v3
  Double_t v21e= ft->getErr(2);
  Double_t v31 = ft->getVal(3); //0 for G, 1 for F, 2 for v2, 3 for v3
  Double_t v31e= ft->getErr(3);
  printf("Values from fit:\n");
  printf("V2 = %f +- %f\n",v21,v21e);
  printf("V3 = %f +- %f\n",v31,v31e);
  printf("Warning! These results were obtained using dummy distributions!\n");
}
