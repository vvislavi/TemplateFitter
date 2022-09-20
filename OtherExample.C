#include "TemplateFitter.cxx"
#include "Dummy2DFunction.C"
#include "TCanvas.h"
TH2 *h2;
// void Init() {
//   TFile *tf = new TFile("h2.root");
//   h2 = (TH2*)tf->Get("h2");
// };
Double_t xRange = 50;
Double_t yRange = 20;
Int_t Npx = 200;
Int_t Npy = 113;
TH1 *generateDistribution() {
  FunctionObject *fobj = new Dummy2DFunction();
  TF1 *f1 = new TF2("genFunc",fobj,0,xRange,0,yRange,2);
  f1->SetParameters(7.,5.);
  f1->SetNpx(Npx);
  ((TF2*)f1)->SetNpy(Npy);
  TH1 *reth = (TH2*)f1->GetHistogram();
  reth->Sumw2();
  reth->SetName("returnHist");
  reth->SetDirectory(0);
  return reth;
}
void OtherExample() {
  //Load projections
  // Init();
  TH1 *h2 = generateDistribution();
  h2->Draw("colz");
  new TCanvas();
  //Initialize fitter with given projections
  TemplateFitter *ft = new TemplateFitter(h2);
  //Setting up variable ( = delta phi, or just "x"):
  ft->AddVariable("x","x",0,10);
  ft->AddVariable("y","y",0,yRange);
  //Setting up arguments: name, title, initial value, minimum, maximum
  ft->AddParameter("xPar","xPar",3,0,100000);
  ft->AddParameter("yPar","yPar",2,0,200000);
  //Construct fit function
  FunctionObject *fobj = new Dummy2DFunction();
  ft->SetFitFunction(fobj);
  //Perform fit:
  printf("About to fit\n");
  Int_t dummy = ft->Fit(kTRUE); //Do not draw performance at this point. Return value of Fit() is false if no base is set.
  // dummy = ft->Fit(kTRUE);
  if(!dummy) return;
  Double_t v21 = ft->getVal(1); //0 for G, 1 for F, 2 for v2, 3 for v3
  Double_t v21e= ft->getErr(1);
  printf("V2 values from fit:\n");
  printf("No rebin: V2 = %f +- %f\n",v21,v21e);
}
