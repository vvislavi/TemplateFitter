#ifndef TEMPLATEFITTER__H
#define TEMPLATEFITTER__H
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooHistFunc.h"
#include "RooRealSumPdf.h"
#include "RooParamHistFunc.h"
#include "RooHistConstraint.h"
#include "RooProdPdf.h"
#include "RooPlot.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TNamed.h"
#include "TMath.h"
#include "TF1.h"
#include "TF2.h"
#include "RooTFnBinding.h"
#include "FunctionObject.h"
using namespace RooFit;
class TemplateFitter {
 public:
    TemplateFitter();
    TemplateFitter(TH1 *datah):dataH(0),totFunc(0),fParList(0),fVarList(0),f_Dim(0) { SetData(datah); };
    ~TemplateFitter();
    void SetData(TH1 *inh) {if(dataH) delete dataH; dataH = (TH1*)inh->Clone("l_dataH"); dataH->SetDirectory(0); f_Dim=getDimension(); };
    void SetFitFunction(FunctionObject *fobj);
    void AddParameter(TString lName, TString lTitle, Double_t l_val, Double_t l_min, Double_t l_max); //parameters
    void AddVariable(TString lName, TString lTitle, Double_t l_min, Double_t l_max); //variables ( = dimensions)
    // void ResetParList() {if(fArgList) delete fArgList; };
    // void SetBase(TH1 *inh) {if(tmplH) delete tmplH; tmplH = (TH2*)inh->Clone("l_tmplH"); tmplH->SetDirectory(0); };
    Bool_t Fit(Bool_t drawFit=kFALSE);
    Double_t getVal(Int_t ind) {return ((RooRealVar*)fParList->At(ind))->getVal();}; //0 for G, 1 for F, 2 for v2, 3 for v3
    Double_t getErr(Int_t ind) {return ((RooRealVar*)fParList->At(ind))->getError();};
  protected:
    TH1 *dataH;
    TF1 *totFunc;
    TList *fParList;
    TList *fVarList;
    Int_t f_Dim;
    Int_t getDimension();
    Bool_t rescaleHistogram(Bool_t divide=kFALSE);
    Int_t getAxNbins(TAxis *inax) {if(!inax) return 0; return inax->GetXbins()->fN; };
    void ScaleBin(TH1 *inh, Double_t scale, Int_t bx, Int_t by=0, Int_t bz=0);
};
#endif
