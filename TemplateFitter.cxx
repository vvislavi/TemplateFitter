#include "TemplateFitter.h"
using namespace RooFit;
TemplateFitter::TemplateFitter():
  dataH(0),
  totFunc(0),
  fParList(0)
{
};
TemplateFitter::~TemplateFitter() {
  delete dataH;
  // delete tmplH;
  delete totFunc;
  delete fParList;
};
Bool_t TemplateFitter::Fit(Bool_t drawFit) {
  if(!f_Dim) {printf("Input histogram has dimension 0, cannot fit...\n"); return 0; };
  if(!fParList || !fParList->GetEntries()) {printf("No parameters specified. You probably forgot to call AddParameter().\n"); return 0; };
  Int_t l_nDim = fVarList->GetEntries();
  if(l_nDim!=f_Dim) {printf("Number of variables (%i) is not the same as number of histogram dimensions (%i). Won't fit.\n",l_nDim,f_Dim); return 0;};
  if(!rescaleHistogram(kFALSE)) {printf("Could not rescale the histogram. Quitting...\n"); return 0; };
  //Figure out dimensions & define variables
  RooRealVar x, y, z;
  Double_t xmin=0,xmax=0,ymin=0,ymax=0,zmin=0,zmax=0;
  if(l_nDim>0) {
    x = *((RooRealVar*)fVarList->At(0));
    xmin=x.getMin(); xmax=x.getMax();
  }
  if(l_nDim>1) {
    y = *((RooRealVar*)fVarList->At(1));
    ymin=y.getMin(); ymax=y.getMax();
  };
  if(l_nDim>2) {
    z = *((RooRealVar*)fVarList->At(2));
    zmin=z.getMin();
    zmax=z.getMax();
  };
  if(!SetupFF()) return kFALSE;
  totFunc->SetRange(xmin,xmax,ymin,ymax,zmin,zmax);
  RooArgList lVarList;
  for(Int_t i=0;i<l_nDim;i++) lVarList.add(*((RooRealVar*)fVarList->At(i)));
  RooDataHist dsig("dsig", "dsig", lVarList, Import(*dataH,kFALSE));
  RooArgList lArgList;
  for(Int_t i=0;i<fParList->GetEntries(); i++) lArgList.add(*((RooRealVar*)fParList->At(i)));
  //Set up the function
  RooAbsReal *frfn;
  if(l_nDim==1) frfn = bindFunction((TF1*)totFunc,x,lArgList);
  else if(l_nDim==2) frfn = bindFunction((TF2*)totFunc,x,y,lArgList);
  else if(l_nDim==3) frfn = bindFunction((TF3*)totFunc,x,y,z,lArgList);
  else { printf("Currently, only 1-3 dimensions are supported.\n"); return kFALSE; };
  //Perform fit
  frfn->chi2FitTo(dsig);//signal is divided by bin width at this point
  return 1;
};
void TemplateFitter::SetFitFunction(FunctionObject *fobj) {
  if(!fobj) {printf("Null pointer for function object passed! Not doing anything this time...\n"); return; };
  if(!fobj->isValid()) {printf("Fit function is not valid! Check isValid() method in your function implementation...\n"); return; };
  f_FObj = fobj; //Just a shallow copy
};
void TemplateFitter::AddParameter(TString lName, TString lTitle, Double_t l_val, Double_t l_min, Double_t l_max) {
  if(!fParList) {
    fParList = new TList();
    fParList->SetOwner(kTRUE);
  };
  RooRealVar *l_Var = new RooRealVar(lName,lTitle,l_val,l_min,l_max);
  fParList->Add(l_Var);
}
void TemplateFitter::AddVariable(TString lName, TString lTitle, Double_t l_min, Double_t l_max) {
  if(!fVarList) {
    fVarList = new TList();
    fVarList->SetOwner(kTRUE);
  };
  RooRealVar *l_Var = new RooRealVar(lName,lTitle,l_min,l_max);
  fVarList->Add(l_Var);
}
Bool_t TemplateFitter::SetConst(Int_t ind, Bool_t isConst) {
  if(ind>=fParList->GetEntries()) {
    printf("Could not find the parameter requested (asked for var %i, while there are only %i variables)!\n",ind,fParList->GetEntries());
    if(ind==fParList->GetEntries()) printf("Are you sure you did not forget that parameters start with index 0?\n");
    return kFALSE;
  };
  RooRealVar *vr = (RooRealVar*)fParList->At(ind);
  vr->setConstant(isConst);
  return kTRUE;
};
Bool_t TemplateFitter::SetConst(Int_t ind, Double_t cVal) {
  if(ind>=fParList->GetEntries()) {
    printf("Could not find the parameter requested (asked for var %i, while there are only %i variables)!\n",ind,fParList->GetEntries());
    if(ind==fParList->GetEntries()) printf("Are you sure you did not forget that parameters start with index 0?\n");
    return kFALSE;
  };
  RooRealVar *vr = (RooRealVar*)fParList->At(ind);
  vr->setVal(cVal);
  vr->setConstant(kTRUE);
  return kTRUE;
};
Int_t TemplateFitter::getDimension() {
  if(!dataH) return 0;
  Int_t rDim = 0;
  if(dynamic_cast<TH3*>(dataH)) rDim=3;
  else if(dynamic_cast<TH2*>(dataH)) rDim=2;
  else if(dynamic_cast<TH1*>(dataH)) rDim=1;
  return rDim;
}
Bool_t TemplateFitter::rescaleHistogram(Bool_t divide) {
  if(!f_Dim) getDimension();
  if(!f_Dim) return 0;
  Int_t fNx = getAxNbins(dataH->GetXaxis());
  Int_t fNy = getAxNbins(dataH->GetYaxis());
  Int_t fNz = getAxNbins(dataH->GetZaxis());
  Double_t scale = 1;
  if(!fNx) scale*=dataH->GetXaxis()->GetBinWidth(1);
  if(!fNy && f_Dim>1) scale*=dataH->GetYaxis()->GetBinWidth(1);
  if(!fNz && f_Dim>2) scale*=dataH->GetZaxis()->GetBinWidth(1);
  if(!fNx && !fNy && !fNz) { dataH->Scale(divide?(1./scale):scale); return kTRUE; }; //In case all bin are homogenious in their dimensions, then just scale the whole histogram
  for(Int_t i=1;i<=dataH->GetNbinsX();i++) {
    Double_t x_scale=dataH->GetXaxis()->GetBinWidth(i);
    if(f_Dim<2) ScaleBin(dataH,divide?(1./x_scale):x_scale,i);
    else {
      for(Int_t j=1;j<=dataH->GetNbinsY();j++){
        Double_t xy_scale=x_scale*dataH->GetYaxis()->GetBinWidth(j);
        if(f_Dim<3) ScaleBin(dataH,divide?(1./xy_scale):xy_scale,i,j);
        else {
          for(Int_t k=1;k<=dataH->GetNbinsZ();k++) {
            Double_t xyz_scale=xy_scale*dataH->GetZaxis()->GetBinWidth(k);
            ScaleBin(dataH,divide?(1./xyz_scale):xyz_scale,i,j,k);
          }
        }
      }
    }
  }
  return kTRUE;
}
void TemplateFitter::ScaleBin(TH1 *inh, Double_t scale, Int_t bx, Int_t by, Int_t bz) {
  Int_t binNo = inh->GetBin(bx,by,bz);
  inh->SetBinContent(binNo,inh->GetBinContent(binNo)*scale);
  inh->SetBinError(binNo,inh->GetBinError(binNo)*scale);
}
Bool_t TemplateFitter::SetupFF() {
  if(!f_FObj) {printf("FunctionObject not set! You probably forgot to call SetFitFunction()...\n"); return kFALSE; };
  if(totFunc) delete totFunc;
  totFunc = new TF1("total_FitFunction",f_FObj,0,10,fParList->GetEntries());
  return kTRUE;
}
