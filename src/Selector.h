#ifndef Selector_h
#define Selector_h

#include "Reader.h"
#include "Obj.cxx"
#include "Plots.cxx"
#include "TLorentzVector.h"

#include "BTagCalibrationStandalone.h"
//#include "BTagCalibrationStandalone.cpp"
#include "LumiMaskFilter.h"

#include "TFile.h"
#include "TH2F.h"
#include "TGraphAsymmErrors.h"

#include "TRandom3.h"
#include <math.h>

#include <correction.h>

//Base class for all selectors
class Selector
{
  public:
    //const member needs to be initialized in intialisation list meaning m_isData(isData)
    Selector() {} ;
    virtual ~Selector() {} ;
    // These methods are called at the corresponding stage of processing of TSelector
  virtual void SlaveBegin(Reader* r) {} ;
  virtual void Process(Reader* r) ;
  virtual void SlaveTerminate(Reader* r) {} ;
  virtual void Terminate(TList* mergedList, std::string outFileName) {} ; //outFileName is used to write parameter, like cuts, to output file
  virtual void SetRandom() ;
  virtual void SetDataInfo(bool isData, std::string year) {m_isData = isData ; m_year = year ; } ;
  virtual void SetCentralGenWeight(double centralGenWeight) {m_centralGenWeight = centralGenWeight;}; //this is the central gen weight used to normalize the gen weight. This is useful when the absolute value of gen weight is not always the same like in sherpa sample.  
  virtual void SetBtagCalib(std::string csvFileName, std::string effFileName, std::string btagUncType) ;
  virtual void SetEleEffCorr(std::vector<std::string> fName_trig, std::string fName_recSF, std::string fName_IDSF, std::vector<float> w_trig, std::string eleUncType) ;
  virtual void SetMuonEffCorr(std::vector<std::string> fName_trig, std::vector<std::string> fName_ID, std::vector<std::string> fName_iso, std::vector<float> w_trig, std::vector<float> w_ID, std::vector<float> w_iso, std::string muonUncType) ;
  virtual void SetLumiMaskFilter(std::string fName_lumiMaskFilter);
  virtual void SetPileupSF(std::string fName_puSF);
  virtual void SetXbbXccEff(std::string fName_xbb_xcc_eff);
  virtual void SetXbbXccEff1D(std::string fName_xbb_xcc_eff);
  virtual void SetTriggerSF(std::string fName_triggerSF);

  virtual void SetJECcorr(std::string jsonFileName);
  virtual void SetMSDcorr(std::string jsonFileName);
  
  virtual float PileupSF(int nTrueInt);
  virtual std::vector<float> GetSF_2DHist(float x, float y, std::vector<TH2F*> h, std::vector<float> w);
  virtual float CalBtagWeight(std::vector<JetObj>& jets, std::string jet_main_bTagWP="deepCSVT", std::string uncType="central") ;
  virtual float CalTagWeightBoosted_1jet(std::pair<float,bool> jet, int jet_flav, std::string tagType, std::string uncType);
  virtual float CalBtagWeightBoosted(std::pair<float,bool> jet_bb, std::pair<float,bool> jet_cc, std::string uncType);
  virtual float CalCtagWeightBoosted(std::pair<JetObjBoosted,bool> jet_1, std::pair<JetObjBoosted,bool> jet_2, std::string uncType);
  virtual float CalEleSF(LepObj e1, LepObj e2);
  virtual float CalSingleEleSF(LepObj e1);
  virtual float CalMuonSF_id_iso(LepObj e1, LepObj e2);
  virtual float CalSingleMuonSF_id_iso(LepObj e1);
  virtual float CalTrigSF(int id, LepObj lep1, LepObj lep2, TLorentzVector trigObj, TH1D* h_dR1_trig, TH1D* h_dR2_trig, TH1D* h_pt1_trig, TH1D* h_pt2_trig) ;
  virtual float CalTrigSF_singleLepton(int id, LepObj lep1, TLorentzVector trigObj);
  //virtual TLorentzVector GetTrigObj(Reader* r, unsigned id, unsigned bits, float ptThresh) ;
  virtual void SetPUjetidCalib(std::string fName_PUjetID_SF,std::string fName_PUjetID_eff,std::string jetPUidUncType);
  virtual float PUjetWeight(std::vector<JetObj>& jets) ;

  virtual float Get_xccbb_weight(std::vector<JetObjBoosted>& fatJets, int idx_excludedJet, float XbbCut, float XccCut, std::string type);
  virtual float GetTrigSF(float jetPt) ;
  virtual float GetTrigSF(float jetPt1, float jetPt2) ; //Pt1 > Pt2

#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018)
  //virtual unsigned MatchGenLep(Reader* r, LepObj lep, int pdgId) ;
#endif
  //virtual float MuonRcSF(Reader* r, LepObj lep, int pdgId) ;

  virtual void SetJetMetSyst(std::string jetmetsystType) {m_jetmetSystType = jetmetsystType;};
  virtual void SetHFtagUncType(std::string hfTagUncType) {m_hfTagUncType = hfTagUncType;};
  virtual void SetL1prefiring(std::string l1prefiringType) {m_l1prefiringType = l1prefiringType;};
  virtual void SetPdfScaleSyst(std::string pdfScaleSystType) {
    m_nScale = 0;
    if (pdfScaleSystType=="scale") m_nScale=9;
    m_iPdfStart = 0;
    m_iPdfStop = 0;
    if (pdfScaleSystType == "pdfg0") {m_iPdfStart=0; m_iPdfStop=35;}
    if (pdfScaleSystType == "pdfg1") {m_iPdfStart=35; m_iPdfStop=70;}
    if (pdfScaleSystType == "pdfg2") {m_iPdfStart=70; m_iPdfStop=103;}
  };

  virtual void SetModelUnc(std::string modelUncType) {m_modelUncType=modelUncType; };
  
  virtual float DH(float mH,float mZ) {
    float k(125./91);
    return fabs(mH-k*mZ)/sqrt(1+k*k);
  }

  virtual void Setn2b1Cut(std::string fName_n2b1_cut);
  virtual float Getn2b1Cut(float pt, float rho);

  virtual float CalculateJES(std::map<std::string, float> jet_info, std::string var, std::string systType, bool isData, JESUncPlots* plots = NULL);
  virtual float CalculateJER(std::map<std::string, float> jet_info, std::vector<JetObjBoosted> genjets, std::string var, std::string systType);

  /*#if defined(MC_2016PRE) || defined(DATA_2016)
static auto correctionSet = correction::CorrectionSet::from_file("CalibData/jme/2016preVFP_UL/fatJet_jerc.json.gz");
static auto correctionSet_msd = correction::CorrectionSet::from_file("CalibData/jme/2016preVFP_UL/msdcorr_2016.json");
#endif
#if defined(MC_2016) || defined(DATA_2016)
static auto correctionSet = correction::CorrectionSet::from_file("CalibData/jme/2016postVFP_UL/fatJet_jerc.json.gz");
static auto correctionSet_msd = correction::CorrectionSet::from_file("CalibData/jme/2016postVFP_UL/msdcorr_2016.json");
#endif
#if defined(MC_2017) || defined(DATA_2017)
auto correctionSet = correction::CorrectionSet::from_file("CalibData/jme/2017_UL/fatJet_jerc.json.gz");
auto correctionSet_msd = correction::CorrectionSet::from_file("CalibData/jme/2017_UL/msdcorr_2017.json");
#endif
#if defined(MC_2018) || defined(DATA_2018)
auto correctionSet = correction::CorrectionSet::from_file("CalibData/jme/2018_UL/fatJet_jerc.json.gz");
auto correctionSet_msd = correction::CorrectionSet::from_file("CalibData/jme/2018_UL/msdcorr_2018.json");
#endif
  */
  
  double m_centralGenWeight;

  bool m_isData ;
  std::string m_year ;

  TRandom3* m_rand ;
  //for btagging SFs
  TFile* m_btagEffFile ;
  //BTagCalibration m_btagCal ;
  //BTagCalibrationReader m_btagReader ;
  LumiMaskFilter m_lumiFilter ;

  std::string m_jec_corFilename;
  std::string m_msd_corFilename;

  //correction::CorrectionSet m_jec_corrSet;
  //correction::CorrectionSet m_msd_corrSet;

  //for electron SFs
  std::vector<TH2F*> m_hSF_eleTrig ;
  TH2F* m_hSF_eleRec ;
  TH2F* m_hSF_eleID ;
  std::vector<float> m_eleTrig_w ;
  
  std::vector<TH2F*> m_hSF_muonTrig ;
  std::vector<TH2F*> m_hSF_muonIso ;
  std::vector<TH2F*> m_hSF_muonID ;
  std::vector<float> m_muonTrig_w ;
  std::vector<float> m_muonIso_w ;
  std::vector<float> m_muonID_w ;

  std::unique_ptr<correction::CorrectionSet> m_corrPtrBtag;
  std::unique_ptr<correction::CorrectionSet> m_corrPtrJEC;
  std::unique_ptr<correction::CorrectionSet> m_corrPtrMSD;
  
  std::map<std::string, std::vector<TH1D*> > m_hEff1D_xbb_xcc; //eff vs. pt, use in tagging weight 
  std::map<std::string, std::vector<TH2D*> > m_hEff_xbb_xcc; //eff vs. pt and eta used in QCD weight, FIXME actually it should be SF

  TGraphAsymmErrors* m_hTrig_SF;
  TH2D* m_hTrig_SF_2D;

  std::string m_btagUncType;
  std::string m_eleUncType;
  std::string m_muonUncType;
  std::string m_jetmetSystType;
  std::string m_l1prefiringType;
  std::string m_jetPUidUncType;
  std::string m_modelUncType;
  std::string m_hfTagUncType;

  unsigned m_nScale;
  unsigned m_iPdfStart;
  unsigned m_iPdfStop;

  TH1D* m_hSF_pu;

  TFile* PUjetID_SF;
  TFile* PUjetID_eff;

  TH2D* m_hn2b1;

};
#endif
