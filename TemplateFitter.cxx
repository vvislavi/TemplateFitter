#include "TemplateFitter.h"
using namespace RooFit;
TemplateFitter::TemplateFitter():
  dataH(0),
  // tmplH(0),
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
  if(!totFunc) {printf("No fit function set -- you probably forgot to call SetFitFunction(FunctionObject*). Not fitting...\n"); return 0; };
  if(!fParList || !fParList->GetEntries()) {printf("No parameters specified. You probably forgot to call AddParameter().\n"); return 0; };
  Int_t l_nDim = fVarList->GetEntries();
  if(l_nDim!=f_Dim) {printf("Number of variables (%i) is not the same as number of histogram dimensions (%i). Won't fit.\n",l_nDim,f_Dim); return 0;};
  if(!rescaleHistogram(kFALSE)) {printf("Could not rescale the histogram. Quitting...\n"); return 0; };
  //To be checked for 2D!
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
  totFunc->SetRange(xmin,xmax,ymin,ymax,zmin,zmax);
  // RooRealVar x("x", "x", -TMath::Pi()/2, 0.75*TMath::TwoPi());
  //Need RooVarList for histogram to get the right dimension. This is very stupid, b/c for function binding, this doesn't work.
  RooArgList lVarList;
  for(Int_t i=0;i<l_nDim;i++) lVarList.add(*((RooRealVar*)fVarList->At(i)));
  RooDataHist dsig("dsig", "dsig", lVarList, Import(*dataH,kFALSE));
  // if(l_nDim==1)
  // else if(l_nDim==2) dsig("dsig", "dsig", x, y, Import(*dataH));
  // else if(l_nDim==3) dsig("dsig", "dsig", x, y, z, Import(*dataH));
  //And go back to original unscaled distribution
  //To be checked for 2D! dataH->Scale(1./dataH->GetBinWidth(1));
  //Set up the argument ( = parameter) list
  RooArgList lArgList;
  for(Int_t i=0;i<fParList->GetEntries(); i++) lArgList.add(*((RooRealVar*)fParList->At(i)));
  //Set up the function
  RooAbsReal *frfn;
  if(l_nDim==1) frfn = bindFunction((TF1*)totFunc,x,lArgList);
  else if(l_nDim==2) frfn = bindFunction((TF2*)totFunc,x,y,lArgList);
  else if(l_nDim==3) frfn = bindFunction((TF3*)totFunc,x,y,z,lArgList);

  //Perform fit
  frfn->chi2FitTo(dsig);//signal is divided by bin width at this point
  if(!drawFit) return 1;
  // for drawing, need to use unscaled distribution. Don't know how this works for 2D dists just yet
  RooDataHist dsig2("dsig2", "dsig2", lVarList, Import(*dataH,kFALSE));

  TH2* ph2 = x.createHistogram("x vs y pdf",y);
  frfn->fillHistogram(ph2,RooArgList(x,y)) ;
  ph2->Draw("colz");
/*
  auto frame = x.frame(Title("No template uncertainties"));
  dsig.plotOn(frame);
  dsig2.plotOn(frame);
  frame->Draw();
  */
  return 1;
};
void TemplateFitter::SetFitFunction(FunctionObject *fobj) {
  if(!fobj) {printf("Null pointer for function object passed! Not doing anything this time...\n"); return; };
  if(!fobj->isValid()) {printf("Fit function is not valid! Check isValid() method in your function implementation...\n"); return; };
  if(totFunc) delete totFunc;
  totFunc = new TF1("total_FitFunction",fobj,0,10,2);
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
  if(!fNx && !fNy && !fNz) {dataH->Scale(divide?(1./scale):scale); return kTRUE; }; //In case all bin are homogenious in their dimensions, then just scale the whole histogram
  for(Int_t i=1;i<=dataH->GetNbinsX();i++) {
    Double_t x_scale=scale;
    if(fNx) x_scale*=dataH->GetXaxis()->GetBinWidth(i);
    if(f_Dim<2) ScaleBin(dataH,divide?(1./x_scale):x_scale,i);
    else {
      for(Int_t j=1;j<=dataH->GetNbinsY();j++){
        Double_t xy_scale=x_scale;
        if(fNy) xy_scale*=dataH->GetYaxis()->GetBinWidth(j);
        if(f_Dim<3) ScaleBin(dataH,divide?(1./xy_scale):xy_scale,i,j);
        else {
          for(Int_t k=1;k<=dataH->GetNbinsZ();k++) {
            Double_t xyz_scale=xy_scale;
            if(fNz) xyz_scale*=dataH->GetZaxis()->GetBinWidth(k);
            ScaleBin(dataH,divide?(1./xyz_scale):xyz_scale,i,j,k);
          }
        }
      }
    }
  }
  return kTRUE;
}
void TemplateFitter::ScaleBin(TH1 *inh, Double_t scale, Int_t bx, Int_t by, Int_t bz) {
  Int_t binNo = inh->FindBin(bx,by,bz);
  inh->SetBinContent(binNo,inh->GetBinContent(binNo)*scale);
  inh->SetBinError(binNo,inh->GetBinError(binNo)*scale);
}
