#define VbbHcc_selector_cxx

#include <math.h>

#include "TList.h"
#include "TParameter.h"
#include "TLorentzVector.h"

#include "VbbHcc_selector.h"
#include "Global.h"
#include "Obj.cxx"

#include "correction.h"

///////////////////////////////////////
//using particle net tagger
///////////////////////////////////////
//pQCD cut from AN2022_031_v3.pdf table 11, page 25
#if defined(DATA_2016) || defined(MC_2016)
//medium
//const float XccCut = 0.9743;//2016 post
const float XbbCut = 0.9735;//2016 post
//loose WP
const float XccCut = 0.9252;//2016 post
//const float pQCDcut = 0.0681;
//const float pQCDcut = 0.0741;
const float deepFlavBCut = 0.2489;
#endif

#if defined(DATA_2016PRE) || defined(MC_2016PRE)
//medium
//const float XccCut = 0.9751;
const float XbbCut = 0.9737;
//loose WP
const float XccCut = 0.9252;
//const float pQCDcut = 0.0541;
//const float pQCDcut = 0.0741;
const float deepFlavBCut = 0.2598;
#endif

#if defined(DATA_2017) || defined(MC_2017)
//medium
//const float  XccCut = 0.9765; 
const float  XbbCut = 0.9714;
//loose wp
const float  XccCut = 0.9347; 
//const float pQCDcut = 0.0541;
//const float pQCDcut = 0.0741;
const float deepFlavBCut = 0.3040;
#endif
#if defined(DATA_2018) || defined(MC_2018)
//medium
//const float XccCut = 0.9777;
const float XbbCut = 0.9734;
//const float pQCDcut = 0.0741;
//loose
const float XccCut = 0.9368;
const float deepFlavBCut = 0.2783;
#endif
const float pQCDcut = 0.0741;

//////////////////////////////////////////
// using correction sets for JES
//////////////////////////////////////////
#if defined(MC_2016PRE) || defined(DATA_2016PRE)
auto correctionSet_msd = correction::CorrectionSet::from_file("CalibData/jme/2016preVFP_UL/msdcorr_2016.json");
auto correctionSet_btag = correction::CorrectionSet::from_file("CalibData/btagging_2016preVFP_UL.json.gz");
#endif
#if defined(MC_2016) || defined(DATA_2016)
auto correctionSet_msd = correction::CorrectionSet::from_file("CalibData/jme/2016preVFP_UL/msdcorr_2016.json");
auto correctionSet_btag = correction::CorrectionSet::from_file("CalibData/btagging_2016_UL.json.gz");
#endif
//2017 does not have JER for AK8
#if defined(MC_2017) || defined(DATA_2017)
auto correctionSet_msd = correction::CorrectionSet::from_file("CalibData/jme/2017_UL/msdcorr_2017.json");
auto correctionSet_btag = correction::CorrectionSet::from_file("CalibData/btagging_2017_UL.json.gz");
#endif
#if defined(MC_2018) || defined(DATA_2018)
auto correctionSet_msd = correction::CorrectionSet::from_file("CalibData/jme/2018_UL/msdcorr_2018.json");
auto correctionSet_btag = correction::CorrectionSet::from_file("CalibData/btagging_2018_UL.json.gz");
#endif


VbbHcc_selector::~VbbHcc_selector() {
  /*
  if(h_ZccHcc_PN_med) delete h_ZccHcc_PN_med;
  if(h_ZccHcc_PN_med_zmass_deltaPhi) delete h_ZccHcc_PN_med_zmass_deltaPhi;
  if(h_ZccHcc_PN_med_xccWeight) delete h_ZccHcc_PN_med_xccWeight;
  if(h_ZccHcc_PN_med_qcdCR) delete h_ZccHcc_PN_med_qcdCR;
  if(h_ZccHcc_PN_med_topCR_pass) delete h_ZccHcc_PN_med_topCR_pass;
  if(h_ZccHcc_PN_med_VjetCR_pass) delete h_ZccHcc_PN_med_VjetCR_pass;
  if(h_VHcc_PN_med) delete h_VHcc_PN_med;
  if(h_VHcc_PN_med_zmass_deltaPhi) delete h_VHcc_PN_med_zmass_deltaPhi;
  if(h_VHcc_PN_med_qcdCR) delete h_VHcc_PN_med_qcdCR;
  if(h_VHcc_PN_med_topCR_pass) delete h_VHcc_PN_med_topCR_pass;
  if(h_VHcc_PN_med_VjetCR_pass) delete h_VHcc_PN_med_VjetCR_pass;

  if(h_jesUnc) delete h_jesUnc;
  */
} 

void VbbHcc_selector::SlaveBegin(Reader* r) {
  
  int seed(1000);
  if (m_year == "2016PRE") seed = 2016;
  if (m_year == "2016") seed = 20166;
  if (m_year == "2017") seed = 2017;
  if (m_year == "2018") seed = 2018;
  m_random_generator = std::mt19937(seed*1000); 
  
  h_evt_all = new TH1D("Nevt_all_VbbHcc_boosted","",4,-1.5,2.5) ; //This stores events before any filter or cut, bin 1: total negative weight events, bin 2: total positive weight events, bin 3: total event weighted by genWeight (= bin2 - bin1, if genWeight are always -1,1
  h_evt = new TH1D("Nevt_VbbHcc_boosted","",4,-1.5,2.5) ; //bin 1: total negative weight events, bin 2: total positive weight events, bin 3: total event weighted by genWeight (= bin2 - bin1, if genWeight are always -1,1
  h_evtW_ZccHcc = new TH1D("EvtW_ZccHcc","",2000,-10,10);
  h_trigW_ZccHcc = new TH1D("TrigW_ZccHcc","",1000,0,10);
  h_tagW_ZccHcc = new TH1D("TagW_ZccHcc","",1000,0,10);
  h_evtW_VHcc = new TH1D("EvtW_VHcc","",2000,-10,10);
  h_trigW_VHcc = new TH1D("TrigW_VHcc","",1000,0,10);
  h_tagW_VHcc = new TH1D("TagW_VHcc","",1000,0,10);

  h_l1_SF = new TH1D("l1preW", "", 2000, -10, 10);
  h_pu_SF = new TH1D("puSF", "", 2000, -10, 10);
  h_jes_pt_SF = new TH1D("jes_pt_SF", "", 2000, -10, 10);
  h_jes_mass_SF = new TH1D("jes_mass_SF", "", 2000, -10, 10);
  h_jer_SF = new TH1D("jer_SF", "", 2000, -10, 10);
  h_wcorr = new TH1D("wcorr", "", 2000, -10, 10);
  h_gen_weight = new TH1D("gen_weight", "", 2000, -10, 10);
  
  h_bStatus_noMother = new TH1D("bStatus_noMother","",100,0,100) ; 
  h_bStatus_hasMother = new TH1D("bStatus_hasMother","",100,0,100) ; 
  h_nb = new TH1D("nb","",20,0,20) ; 
  h_nB = new TH1D("nB","",20,0,20) ; 
  h_nbB = new TH1D("nbB","",20,0,20) ; 
  h_nb_1 = new TH1D("nb_1","",20,0,20) ; 
  h_nB_1 = new TH1D("nB_1","",20,0,20) ; 
  h_nbB_1 = new TH1D("nbB_1","",20,0,20) ; 

  h_Pileup_nTrueInt = new TH1D("Pileup_nTrueInt", "", 99, 0, 99);
  h_Pileup_nTrueInt_scaled = new TH1D("Pileup_nTrueInt_scaled", "", 99, 0, 99);
  h_PV_npvsGood = new TH1D("PV_npvsGood", "", 99, 0, 99);
  h_PV_npvsGood_scaled = new TH1D("PV_npvsGood_scaled", "", 99, 0, 99);
  
  h_pt_rho_n2b1 = new TH3D("pt_rho_n2b1","",70,300,1000,75,-6,-1.5,100,0,1);
  h_jet_mass = new TH1D("jet_mass","",2000,0,2000);
  h_jet_pt = new TH1D("jet_pt", "", 2000, 0, 2000);

  h_MET_beforeCut = new TH1D("MET_beforeCut","", 1000, 0, 1000);
  h_MET_afterCut  = new TH1D("MET_afterCut", "", 1000, 0, 1000);

  h_jet_pt_afterSel = new TH1D("jet_pt_afterSel", "", 2000, 0, 2000);
  h_jet_mass_afterSel = new TH1D("jet_mass_afterSel", "", 2000, 0, 2000);
  h_jet_mass_beforeWcorr = new TH1D("jet_mass_beforeWcorr", "", 2000, 0, 2000);
  h_jet_mass_afterWcorr = new TH1D("jet_mass_afterWcorr", "", 2000, 0, 2000);

  h_Xcc_vs_pQCD = new TH2D("Xcc_vs_pQCD", "", 1000, 0, 1, 1000, 0, 1);
  h_Xcc_vs_pQCD_beforeCuts = new TH2D("Xcc_vs_pQCD_beforeCuts", "", 1000, 0, 1, 1000, 0, 1);
  h_Xcc_vs_pQCD_raw = new TH2D("Xcc_vs_pQCD_raw", "", 1000, 0, 1, 1000, 0, 1);
  
  h_cutFlow_ZccHcc_PN_med = new TH1D("CutFlow_ZccHcc_boosted_PN_med","",15,0,15) ;
  h_cutFlow_ZccHcc_PN_med->GetXaxis()->SetBinLabel(1,"Total");
  h_cutFlow_ZccHcc_PN_med->GetXaxis()->SetBinLabel(2,"Lumi");
  h_cutFlow_ZccHcc_PN_med->GetXaxis()->SetBinLabel(3,"EleVeto");
  h_cutFlow_ZccHcc_PN_med->GetXaxis()->SetBinLabel(4,"MuonVeto");
  h_cutFlow_ZccHcc_PN_med->GetXaxis()->SetBinLabel(5,"TauVeto");
  h_cutFlow_ZccHcc_PN_med->GetXaxis()->SetBinLabel(6,"Jet kin");
  h_cutFlow_ZccHcc_PN_med->GetXaxis()->SetBinLabel(7,"Jet rho");
  h_cutFlow_ZccHcc_PN_med->GetXaxis()->SetBinLabel(8,"2 jets, ptCut");
  h_cutFlow_ZccHcc_PN_med->GetXaxis()->SetBinLabel(9,"Jet Hcc");
  h_cutFlow_ZccHcc_PN_med->GetXaxis()->SetBinLabel(10,"Jet Zcc");
  h_cutFlow_ZccHcc_PN_med->GetXaxis()->SetBinLabel(11,"Extra jets");
  h_cutFlow_ZccHcc_PN_med->GetXaxis()->SetBinLabel(12,"pT_miss");
  h_cutFlow_ZccHcc_PN_med->GetXaxis()->SetBinLabel(13,"Trigger");
  h_cutFlow_ZccHcc_PN_med->GetXaxis()->SetBinLabel(14,"DeltaPhi");
  h_cutFlow_ZccHcc_PN_med->GetXaxis()->SetBinLabel(15,"ZMass");

  h_cutFlow_VHcc_PN_med = new TH1D("CutFlow_VHcc_boosted_PN_med","",15,0,15) ;
  h_cutFlow_VHcc_PN_med->GetXaxis()->SetBinLabel(1,"Total");
  h_cutFlow_VHcc_PN_med->GetXaxis()->SetBinLabel(2,"Lumi");
  h_cutFlow_VHcc_PN_med->GetXaxis()->SetBinLabel(3,"EleVeto");
  h_cutFlow_VHcc_PN_med->GetXaxis()->SetBinLabel(4,"MuonVeto");
  h_cutFlow_VHcc_PN_med->GetXaxis()->SetBinLabel(5,"TauVeto");
  h_cutFlow_VHcc_PN_med->GetXaxis()->SetBinLabel(6,"Jet kin");
  h_cutFlow_VHcc_PN_med->GetXaxis()->SetBinLabel(7,"Jet rho");
  h_cutFlow_VHcc_PN_med->GetXaxis()->SetBinLabel(8,"2 jets, ptCut");
  h_cutFlow_VHcc_PN_med->GetXaxis()->SetBinLabel(9,"Jet Vqq");
  h_cutFlow_VHcc_PN_med->GetXaxis()->SetBinLabel(10,"Jet Hcc");
  h_cutFlow_VHcc_PN_med->GetXaxis()->SetBinLabel(11,"Extra jets");
  h_cutFlow_VHcc_PN_med->GetXaxis()->SetBinLabel(12,"pT_miss");
  h_cutFlow_VHcc_PN_med->GetXaxis()->SetBinLabel(13,"Trigger");
  h_cutFlow_VHcc_PN_med->GetXaxis()->SetBinLabel(14,"DeltaPhi");
  h_cutFlow_VHcc_PN_med->GetXaxis()->SetBinLabel(15,"VMass");
  int nLHEScaleWeight=0;
  if (m_scaleUnc=="scale") nLHEScaleWeight = 9;

  /// This fix is put here to avoid an issue where they weren't set properly
  /// with data. For cases of data, we just want them to be 0 and 0.
#if defined(DATA_2016) || defined(DATA_2017) || defined(DATA_2018)
  m_iPdfStart = 0;
  m_iPdfStop = 0;
#endif
  
  h_ZccHcc_PN_med = new VHBoostedPlots("ZccHcc_boosted_PN_med",m_iPdfStart,m_iPdfStop,nLHEScaleWeight);
  h_ZccHcc_PN_med_zmass_deltaPhi = new VHBoostedPlots("ZccHcc_boosted_PN_med_zmass_deltaPhi");
  h_ZccHcc_PN_med_xccWeight = new VHBoostedPlots("ZccHcc_boosted_PN_med_xccWeight");
  h_ZccHcc_PN_med_qcdCR = new VHBoostedPlots("ZccHcc_boosted_PN_med_qcdCR",m_iPdfStart,m_iPdfStop,nLHEScaleWeight);
  h_ZccHcc_PN_med_topCR_pass = new VHBoostedPlots("ZccHcc_boosted_PN_med_topCR_pass",m_iPdfStart,m_iPdfStop,nLHEScaleWeight);
  h_ZccHcc_PN_med_VjetCR_pass = new VHBoostedPlots("ZccHcc_boosted_PN_med_VjetCR_pass");
  h_VHcc_PN_med = new VHBoostedPlots("VHcc_boosted_PN_med",m_iPdfStart,m_iPdfStop,nLHEScaleWeight);
  h_VHcc_PN_med_zmass_deltaPhi = new VHBoostedPlots("VHcc_boosted_PN_med_vmass_deltaPhi");
  h_VHcc_PN_med_qcdCR = new VHBoostedPlots("VHcc_boosted_PN_med_qcdCR",m_iPdfStart,m_iPdfStop,nLHEScaleWeight);
  h_VHcc_PN_med_topCR_pass = new VHBoostedPlots("VHcc_boosted_PN_med_topCR_pass",m_iPdfStart,m_iPdfStop,nLHEScaleWeight);
  h_VHcc_PN_med_VjetCR_pass = new VHBoostedPlots("VHcc_boosted_PN_med_VjetCR_pass");

  // Added by Peter for data-driven Bckg Estimation (Apr 5, 2025)
  h_VHcc_PN_med_qcdEnriched_topCR = new VHBoostedPlots("VHcc_boosted_PN_med_qcdEnriched_topCR",
						       m_iPdfStart, m_iPdfStop, nLHEScaleWeight);
  
#if defined(MC_VZ)
  h_ZccHcc_PN_med_VZcc = new VHBoostedPlots("ZccHcc_boosted_PN_med_VZcc",m_iPdfStart,m_iPdfStop,nLHEScaleWeight);
  h_ZccHcc_PN_med_VZbb = new VHBoostedPlots("ZccHcc_boosted_PN_med_VZbb",m_iPdfStart,m_iPdfStop,nLHEScaleWeight);
  h_ZccHcc_PN_med_VZother = new VHBoostedPlots("ZccHcc_boosted_PN_med_VZother",m_iPdfStart,m_iPdfStop,nLHEScaleWeight); 
  h_ZccHcc_PN_med_qcdCR_VZcc = new VHBoostedPlots("ZccHcc_boosted_PN_med_qcdCR_VZcc",m_iPdfStart,m_iPdfStop,nLHEScaleWeight);
  h_ZccHcc_PN_med_qcdCR_VZbb = new VHBoostedPlots("ZccHcc_boosted_PN_med_qcdCR_VZbb",m_iPdfStart,m_iPdfStop,nLHEScaleWeight);
  h_ZccHcc_PN_med_qcdCR_VZother = new VHBoostedPlots("ZccHcc_boosted_PN_med_qcdCR_VZother",m_iPdfStart,m_iPdfStop,nLHEScaleWeight); 
  h_ZccHcc_PN_med_topCR_pass_VZcc = new VHBoostedPlots("ZccHcc_boosted_PN_med_topCR_pass_VZcc",m_iPdfStart,m_iPdfStop,nLHEScaleWeight);
  h_ZccHcc_PN_med_topCR_pass_VZbb = new VHBoostedPlots("ZccHcc_boosted_PN_med_topCR_pass_VZbb",m_iPdfStart,m_iPdfStop,nLHEScaleWeight);
  h_ZccHcc_PN_med_topCR_pass_VZother = new VHBoostedPlots("ZccHcc_boosted_PN_med_topCR_pass_VZother",m_iPdfStart,m_iPdfStop,nLHEScaleWeight); 
  h_VHcc_PN_med_VZcc = new VHBoostedPlots("VHcc_boosted_PN_med_VZcc",m_iPdfStart,m_iPdfStop,nLHEScaleWeight);
  h_VHcc_PN_med_VZbb = new VHBoostedPlots("VHcc_boosted_PN_med_VZbb",m_iPdfStart,m_iPdfStop,nLHEScaleWeight);
  h_VHcc_PN_med_VZother = new VHBoostedPlots("VHcc_boosted_PN_med_VZother",m_iPdfStart,m_iPdfStop,nLHEScaleWeight); 
  h_VHcc_PN_med_qcdCR_VZcc = new VHBoostedPlots("VHcc_boosted_PN_med_qcdCR_VZcc",m_iPdfStart,m_iPdfStop,nLHEScaleWeight);
  h_VHcc_PN_med_qcdCR_VZbb = new VHBoostedPlots("VHcc_boosted_PN_med_qcdCR_VZbb",m_iPdfStart,m_iPdfStop,nLHEScaleWeight);
  h_VHcc_PN_med_qcdCR_VZother = new VHBoostedPlots("VHcc_boosted_PN_med_qcdCR_VZother",m_iPdfStart,m_iPdfStop,nLHEScaleWeight); 
  h_VHcc_PN_med_topCR_pass_VZcc = new VHBoostedPlots("VHcc_boosted_PN_med_topCR_pass_VZcc",m_iPdfStart,m_iPdfStop,nLHEScaleWeight);
  h_VHcc_PN_med_topCR_pass_VZbb = new VHBoostedPlots("VHcc_boosted_PN_med_topCR_pass_VZbb",m_iPdfStart,m_iPdfStop,nLHEScaleWeight);
  h_VHcc_PN_med_topCR_pass_VZother = new VHBoostedPlots("VHcc_boosted_PN_med_topCR_pass_VZother",m_iPdfStart,m_iPdfStop,nLHEScaleWeight); 
#endif

  //test H and Z mass from MC truth
  h_test_MZ = new TH1D("MZbb_MCtruth","",200,0,200);
  h_test_MH = new TH1D("MHcc_MCtruth","",200,0,200);
  h_NextraJet = new TH1D("NextraJet","",10,0,10);
  h_NextraJet_ZccHcc = new TH1D("NextraJet_ZccHcc","",10,0,10);
  h_NextraJet_VHcc = new TH1D("NextraJet_VHcc","",10,0,10);
  h_triggerCheck = new TH1D("TriggerCheck","",3,0,3);
  h_ljpt = new TH1D("h_ljpt","",3000,0,3000);
  h_ljpt_gen = new TH1D("h_ljpt_gen","",3000,0,3000);

  h_jesUnc = new JESUncPlots("JESUnc");

  h_VZtype = new TH1D("VZtype","",10,1,11);
  h_VZtype->GetXaxis()->SetBinLabel(1,"All leptonic (neu)");
  h_VZtype->GetXaxis()->SetBinLabel(2,"Semi leptonic (neu)");
  h_VZtype->GetXaxis()->SetBinLabel(3,"qqcc");
  h_VZtype->GetXaxis()->SetBinLabel(4,"qccc");
  h_VZtype->GetXaxis()->SetBinLabel(5,"cccc");
  h_VZtype->GetXaxis()->SetBinLabel(6,"bbcc");
  h_VZtype->GetXaxis()->SetBinLabel(7,"qqbb");
  h_VZtype->GetXaxis()->SetBinLabel(8,"qcbb");
  h_VZtype->GetXaxis()->SetBinLabel(9,"bbbb");
  h_VZtype->GetXaxis()->SetBinLabel(10,"qqqq");
 
  ////////////////////////////////////
  //Add histograms to fOutput so they can be saved in Processor::SlaveTerminate
  /////////////////////////////////////
  std::vector<TH1*> tmp = h_ZccHcc_PN_med->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_ZccHcc_PN_med_zmass_deltaPhi->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);

  tmp = h_ZccHcc_PN_med_xccWeight->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  
  tmp = h_ZccHcc_PN_med_qcdCR->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
 
  tmp = h_ZccHcc_PN_med_topCR_pass->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  
  tmp = h_ZccHcc_PN_med_VjetCR_pass->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
   

  tmp = h_VHcc_PN_med->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VHcc_PN_med_zmass_deltaPhi->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  
  tmp = h_VHcc_PN_med_qcdCR->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
 
  tmp = h_VHcc_PN_med_topCR_pass->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);

  tmp = h_VHcc_PN_med_VjetCR_pass->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);

  tmp = h_VHcc_PN_med_qcdEnriched_topCR->returnHisto();
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  
#if defined(MC_VZ)
  tmp = h_ZccHcc_PN_med_VZcc->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_ZccHcc_PN_med_VZbb->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_ZccHcc_PN_med_VZother->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_ZccHcc_PN_med_topCR_pass_VZcc->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_ZccHcc_PN_med_topCR_pass_VZbb->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_ZccHcc_PN_med_topCR_pass_VZother->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_ZccHcc_PN_med_qcdCR_VZcc->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_ZccHcc_PN_med_qcdCR_VZbb->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_ZccHcc_PN_med_qcdCR_VZother->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VHcc_PN_med_VZcc->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VHcc_PN_med_VZbb->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VHcc_PN_med_VZother->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VHcc_PN_med_qcdCR_VZcc->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VHcc_PN_med_qcdCR_VZbb->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VHcc_PN_med_qcdCR_VZother->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VHcc_PN_med_topCR_pass_VZcc->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VHcc_PN_med_topCR_pass_VZbb->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VHcc_PN_med_topCR_pass_VZother->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
#endif
  tmp = h_jesUnc->returnHisto();
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);


  r->GetOutputList()->Add(h_evt_all) ;
  r->GetOutputList()->Add(h_evt) ;
  r->GetOutputList()->Add(h_evtW_ZccHcc) ;
  r->GetOutputList()->Add(h_trigW_ZccHcc) ;
  r->GetOutputList()->Add(h_tagW_ZccHcc) ;
  r->GetOutputList()->Add(h_evtW_VHcc) ;
  r->GetOutputList()->Add(h_trigW_VHcc) ;
  r->GetOutputList()->Add(h_tagW_VHcc) ;
  r->GetOutputList()->Add(h_bStatus_noMother) ;
  r->GetOutputList()->Add(h_bStatus_hasMother) ;
  r->GetOutputList()->Add(h_nb) ;
  r->GetOutputList()->Add(h_nB) ;
  r->GetOutputList()->Add(h_nbB) ;
  r->GetOutputList()->Add(h_nb_1) ;
  r->GetOutputList()->Add(h_nB_1) ;
  r->GetOutputList()->Add(h_nbB_1) ;
  r->GetOutputList()->Add(h_pt_rho_n2b1) ;
  r->GetOutputList()->Add(h_jet_mass) ;
  r->GetOutputList()->Add(h_jet_pt) ;
  r->GetOutputList()->Add(h_cutFlow_ZccHcc_PN_med) ;
  r->GetOutputList()->Add(h_cutFlow_VHcc_PN_med) ;
  r->GetOutputList()->Add(h_test_MZ);
  r->GetOutputList()->Add(h_test_MH);
  r->GetOutputList()->Add(h_NextraJet);
  r->GetOutputList()->Add(h_NextraJet_ZccHcc);
  r->GetOutputList()->Add(h_NextraJet_VHcc);
  r->GetOutputList()->Add(h_triggerCheck);
  r->GetOutputList()->Add(h_ljpt);
  r->GetOutputList()->Add(h_ljpt_gen);
  r->GetOutputList()->Add(h_VZtype);

  r->GetOutputList()->Add(h_Pileup_nTrueInt);
  r->GetOutputList()->Add(h_Pileup_nTrueInt_scaled);
  r->GetOutputList()->Add(h_PV_npvsGood);
  r->GetOutputList()->Add(h_PV_npvsGood_scaled);

  r->GetOutputList()->Add(h_MET_beforeCut);
  r->GetOutputList()->Add(h_MET_afterCut);
  r->GetOutputList()->Add(h_jet_pt_afterSel);
  r->GetOutputList()->Add(h_jet_mass_afterSel);
  r->GetOutputList()->Add(h_jet_mass_beforeWcorr);
  r->GetOutputList()->Add(h_jet_mass_afterWcorr);

  r->GetOutputList()->Add(h_Xcc_vs_pQCD);
  r->GetOutputList()->Add(h_Xcc_vs_pQCD_beforeCuts);
  r->GetOutputList()->Add(h_Xcc_vs_pQCD_raw);
  
  r->GetOutputList()->Add(h_l1_SF);
  r->GetOutputList()->Add(h_pu_SF);
  r->GetOutputList()->Add(h_jer_SF);
  r->GetOutputList()->Add(h_jes_pt_SF);
  r->GetOutputList()->Add(h_jes_mass_SF);
  r->GetOutputList()->Add(h_wcorr);
  r->GetOutputList()->Add(h_gen_weight);
}

#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018) 
std::vector<std::vector<int> > VbbHcc_selector::DauIdxs_ZH(Reader* r) {
  //vector to store indexes of H and Z daughters
  std::vector<std::vector<int> > dauIdxs;
  std::vector<int> dauIdxsZ;
  std::vector<int> dauIdxsH;
  //loop over genpart
  for(unsigned i = 0; i < *(r->nGenPart); ++i) {
    int mIdx = (r->GenPart_genPartIdxMother)[i];
    //if(mIdx>-1) {
      //std::cout << "\n" << (r->GenPart_pdgId)[mIdx] ;
    //}
    //b quarks from Z
    if(fabs((r->GenPart_pdgId)[i])==5 && mIdx>-1 && (r->GenPart_pdgId)[mIdx]==23) dauIdxsZ.push_back(i);
    //c quarks from H
    if(fabs((r->GenPart_pdgId)[i])==4 && mIdx>-1 && (r->GenPart_pdgId)[mIdx]==25) dauIdxsH.push_back(i);
  }
  dauIdxs.push_back(dauIdxsZ);
  dauIdxs.push_back(dauIdxsH);
  return dauIdxs;
}

int VbbHcc_selector::VZDecayMode(Reader* r) {
  //1. Find all quarks from VZ decays, WW count as qqqq since there is no real W->bb or W->cc
  //2. Define a combination number for decay mode, dcMode
  //3. If a quarks from V decay found, multiply dcMode by 3, 4, or 5 depending on the flavor 
  //4. We have 8 VZ hadronic final states: qqcc qccc cccc bbcc qqbb qcbb bbbb qqqq corresponding to dcMode = 3^2*4^2, 3*4*4^2, 4^4, 5^2*4^2, 3^2*5^2, 3*4*5^2, 5^4, 3^4 (qccc and qcbb is for WZ)
  //loop over genpart
  int dcMode = 1;
  std::string partFromZ;
  for(unsigned i = 0; i < *(r->nGenPart); ++i) {
    int mIdx = (r->GenPart_genPartIdxMother)[i];
    //quarks from V
    if(mIdx>-1 && ((r->GenPart_pdgId)[mIdx]==23 || fabs((r->GenPart_pdgId)[mIdx])==24)) {
      unsigned flav = fabs((r->GenPart_pdgId)[i]) ;
      int status = (r->GenPart_status)[i] ;
      partFromZ = partFromZ + "(" + std::to_string(flav) + ", " + std::to_string(status) + ") "; 
      if (flav <= 3) dcMode *= 3;
      if (flav == 4 || flav == 5) dcMode *= flav;
    }
  }
  std::set<int> numbers = {3*3*4*4, 3*4*4*4, 4*4*4*4, 5*5*4*4, 3*3*5*5, 3*4*5*5, 5*5*5*5, 3*3*3*3};
  if (false && numbers.find(dcMode) == numbers.end()) std::cout << "\n =========== \n" << dcMode << " " << partFromZ;  
  return dcMode;
}

int VbbHcc_selector::VZDecayModeGroup(int VZtype) {
  //if (VZtype==3*3*4*4) h_HMass_VZqqcc->Fill(H.m_lvec.M(),w);
  //if (VZtype==3*4*4*4) h_HMass_VZqccc->Fill(H.m_lvec.M(),w);
  //if (VZtype==4*4*4*4) h_HMass_VZcccc->Fill(H.m_lvec.M(),w);
  //if (VZtype==5*5*4*4) h_HMass_VZbbcc->Fill(H.m_lvec.M(),w);
  //if (VZtype==3*3*5*5) h_HMass_VZqqbb->Fill(H.m_lvec.M(),w);
  //if (VZtype==3*4*5*5) h_HMass_VZqcbb->Fill(H.m_lvec.M(),w);
  //if (VZtype==5*5*5*5) h_HMass_VZbbbb->Fill(H.m_lvec.M(),w);
  //if (VZtype==3*3*3*3) h_HMass_VZqqqq->Fill(H.m_lvec.M(),w);
  if (VZtype==3*3*4*4 || VZtype==3*4*4*4 || VZtype==4*4*4*4 || VZtype==5*5*4*4) return 0;
  if (VZtype==3*3*5*5 || VZtype==3*4*5*5 || VZtype==5*5*5*5) return 1;
  return 2;
}

#endif

bool VbbHcc_selector::sortbysecdesc(const std::pair<int,float> &a, const std::pair<int,float> &b)
{
  return a.second>b.second;
}

bool VbbHcc_selector::sortbysecdesc1(const std::pair<std::vector<int>,float> &a, const std::pair<std::vector<int>,float> &b)
{
  return a.second>b.second;
}

std::vector<JetObj> VbbHcc_selector::NextraJet(std::vector<JetObjBoosted>& fatJets, std::vector<JetObj>& jets) {
  std::vector<JetObj> extraJets;
  for(auto jet:jets) {
    float minDr = 10;
    for(auto fatJet:fatJets) {
      float dR = fatJet.m_lvec.DeltaR(jet.m_lvec);
      if (dR <= minDr) minDr = dR;
    }
    if (minDr >= 0.8) extraJets.push_back(jet);
  }
  return extraJets;
}

void VbbHcc_selector::Process(Reader* r) {

  //Weights
  float genWeight = 1.;
  float puSF = 1.;
  float puSF_new = 1.0f;
  float l1preW = 1.;
  int nB(0);
  int nb(0);
#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018)
  if (*(r->genWeight) < 0) genWeight = -1.;
  if (*(r->genWeight) == 0) genWeight = 0;
  if (m_centralGenWeight != 0) genWeight = *(r->genWeight)/m_centralGenWeight; //use central gen weight to normalize gen weight
  
  if (genWeight < 0) h_evt_all->Fill(-1) ;
  if (genWeight == 0) h_evt_all->Fill(0) ;
  if (genWeight > 0) h_evt_all->Fill(1) ;
  h_evt_all->Fill(2,genWeight);
  
  puSF = PileupSF(*(r->Pileup_nTrueInt));
  puSF_new = new_PileupSF(*(r->Pileup_nTrueInt));
#endif

#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017)
  l1preW = *(r->L1PreFiringWeight_Nom);
  //std::cout << "\nPrefiring: " << l1preW;
  if (m_l1prefiringType == "l1prefiringu") l1preW = *(r->L1PreFiringWeight_Up);
  if (m_l1prefiringType == "l1prefiringd") l1preW = *(r->L1PreFiringWeight_Dn);
#endif

  float evtW = 1. ;
  float evtW_noPU = 1.0f;
  if (!m_isData) {
    evtW *= genWeight*puSF*l1preW;
    evtW_noPU *= genWeight * l1preW;
  }

  h_l1_SF->Fill(l1preW);
  h_pu_SF->Fill(puSF);
  h_gen_weight->Fill(genWeight);
  
  h_cutFlow_ZccHcc_PN_med->Fill(0.5,evtW);
  h_cutFlow_VHcc_PN_med->Fill(0.5,evtW);

#if defined(DATA_2016PRE) || defined(DATA_2016) || defined(DATA_2017) || defined(DATA_2018)
  h_evt->Fill(-1) ;
  if (!m_lumiFilter.Pass(*(r->run),*(r->luminosityBlock))) return;
  h_evt->Fill(1) ;
#endif

  // Record the nTrueInt which is in MC only. Additionally,
  // Record the version of NPVs_good where the weight is taken into account.
#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018) 
  h_Pileup_nTrueInt->Fill(*(r->Pileup_nTrueInt), 1.0f); // don't want PU SF here
  h_Pileup_nTrueInt_scaled->Fill(*(r->Pileup_nTrueInt), puSF_new); // we want the PU SF here
  h_PV_npvsGood_scaled->Fill(*(r->PV_npvsGood), puSF_new);  // includes PU SF
#endif

  // In both MC and data, we also want to see how the NPVs_good
  // looks without the scale factor (as a before picture).
  h_PV_npvsGood->Fill(*(r->PV_npvsGood), 1.0f);  // removes PU SF

  
  h_cutFlow_ZccHcc_PN_med->Fill(1.5,evtW);
  h_cutFlow_VHcc_PN_med->Fill(1.5,evtW);

  //=============Do some MC truth event analysis, like finding which Z decay mode is===========
  int VZtype(-1); 
  int VZtypeGroup(-1); 
#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018)
//to save time, only run on VZ sample
#if defined(MC_VZ)
  VZtype = VZDecayMode(r);
  VZtypeGroup = VZDecayModeGroup(VZtype);
  if (VZtype==1) h_VZtype->Fill(1); //all leptonic including neutrino
  //all hadronic
  else if (VZtype==3*3*4*4) h_VZtype->Fill(3);
  else if (VZtype==3*4*4*4) h_VZtype->Fill(4);
  else if (VZtype==4*4*4*4) h_VZtype->Fill(5);
  else if (VZtype==5*5*4*4) h_VZtype->Fill(6);
  else if (VZtype==3*3*5*5) h_VZtype->Fill(7);
  else if (VZtype==3*4*5*5) h_VZtype->Fill(8);
  else if (VZtype==5*5*5*5) h_VZtype->Fill(9);
  else if (VZtype==3*3*3*3) h_VZtype->Fill(10);
  else h_VZtype->Fill(2); //semi leptonic
#endif
#endif
  
  //=============Get objects============= 
  std::string year_str = "2016postVFP";
#if defined(MC_2016PRE) || defined(DATA_2016B) || defined(DATA_2016C) || defined(DATA_2016D) || defined(DATA_2016E)
  year_str = "2016preVFP";
#elif defined(MC_2017) || defined(DATA_2017)
  year_str = "2017";
#elif defined(MC_2018) || defined(DATA_2018)
  year_str = "2018";
#endif

  std::vector<LepObj> eles_jetOverlap ;
  std::vector<LepObj> eles_lepVeto ;
  float elecVeto_w = 1.0f;
  for (unsigned int i = 0 ; i < *(r->nElectron) ; ++i) {
    
    float etaSC = (r->Electron_eta)[i]+(r->Electron_deltaEtaSC[i]) ;
    LepObj ele((r->Electron_pt)[i],(r->Electron_eta)[i],etaSC,(r->Electron_phi)[i],(r->Electron_mass)[i],i,(r->Electron_charge)[i],0) ;

    float dz = (r->Electron_dz)[i]; 
    float dxy = (r->Electron_dxy)[i]; //dxy=d0
    if ((fabs(etaSC) < 1.4442) && (fabs(dz) > CUTS.Get<float>("ele_dz_b") || fabs(dxy) > CUTS.Get<float>("ele_d0_b"))) continue;
    if ((fabs(etaSC) >= 1.4442) && (fabs(dz) > CUTS.Get<float>("ele_dz_e") || fabs(dxy) > CUTS.Get<float>("ele_d0_e"))) continue;
    int eleId = r->Electron_cutBased[i] ; //cut-based ID Fall17 V2 (0:fail, 1:veto, 2:loose, 3:medium, 4:tight)
    //electron for electron jet overlap removal
    if (ele.m_lvec.Pt() > CUTS.Get<float>("lep_jetOverlap_pt") && fabs(ele.m_lvec.Eta()) < CUTS.Get<float>("ele_eta")) {
      if (eleId >= 4) { //tightId for jet removal
        eles_jetOverlap.push_back(ele) ;
      }
    }
    if (ele.m_lvec.Pt() > CUTS.Get<float>("lep_veto_pt") && fabs(ele.m_lvec.Eta()) < CUTS.Get<float>("ele_eta")) {
      if (eleId >= 2) { //loose for lepton veto
        eles_lepVeto.push_back(ele) ;
      }
    }

    float abseta = abs((r->Electron_eta)[i]);
    float pt = (r->Electron_pt)[i];

    auto map = m_corrPtrElec->at("UL-Electron-ID-SF");
    auto sf = 1.0;
    if (pt > 10) sf = map->evaluate({year_str, m_eleUncType, "Loose", abseta, pt});
    //h_elec_id_sf->Fill(sf);
    elecVeto_w *= sf;
  }

#if defined(MC_2016) || defined(MC_2017) || defined(MC_2018)
  evtW *= elecVeto_w;
#endif

  std::vector<LepObj> muons_jetOverlap ;
  std::vector<LepObj> muons_lepVeto ;
  float muonVeto_w = 1.0f;
  for (unsigned int i = 0 ; i < *(r->nMuon) ; ++i) {
    LepObj muon((r->Muon_pt)[i],(r->Muon_eta)[i],-1,(r->Muon_phi)[i],(r->Muon_mass)[i],i,(r->Muon_charge)[i],(r->Muon_pfRelIso04_all)[i]) ;
    //float sf_rc = MuonRcSF(r, muon, 13) ;
    float sf_rc = 1;
    muon.m_lvec.SetPtEtaPhiM(muon.m_lvec.Pt()*sf_rc,muon.m_lvec.Eta(),muon.m_lvec.Phi(),muon.m_lvec.M()) ;
    //muon for muon jet overlap removal
    if (muon.m_lvec.Pt() > CUTS.Get<float>("lep_jetOverlap_pt") && fabs(muon.m_lvec.Eta()) < CUTS.Get<float>("lep_jetOverlap_eta")) {
      if (r->Muon_mediumId[i] > 0 && r->Muon_pfRelIso04_all[i] < CUTS.Get<float>("muon_iso")) {
        muons_jetOverlap.push_back(muon) ;
      }
    }
    if (muon.m_lvec.Pt() > CUTS.Get<float>("lep_veto_pt") && fabs(muon.m_lvec.Eta()) < CUTS.Get<float>("muon_eta")) {
      if (r->Muon_looseId[i] > 0 && r->Muon_pfRelIso04_all[i] < CUTS.Get<float>("muon_iso_0p25")) {
        muons_lepVeto.push_back(muon) ;
      }
    }

    float abseta = abs((r->Muon_eta)[i]);
    float pt = (r->Muon_pt)[i];
    //h_muon_pt->Fill(pt);

    float sf = 1.0f;

    bool in_eta_range = abseta < 2.4;
    float sf_reco = 1.0f;
    if (in_eta_range && pt > 40){
      auto map_reco = m_corrPtrMuon->at("NUM_TrackerMuons_DEN_genTracks");
      sf_reco = map_reco->evaluate({abseta, pt, m_muonUncType});
    }
    //h_muon_reco_sf->Fill(sf_reco);

    float sf_ID = 1.0f;
    if (in_eta_range && pt > 15) {
      auto map_ID = m_corrPtrMuon->at("NUM_LooseID_DEN_TrackerMuons");
      sf_ID = map_ID->evaluate({abseta, pt, m_muonUncType});
    }
    //h_muon_id_sf->Fill(sf_ID);

    float sf_iso = 1.0f;
    if (in_eta_range && pt > 15) {
      auto map_iso = m_corrPtrMuon->at("NUM_LooseRelIso_DEN_LooseID");
      float sf_iso = map_iso->evaluate({abseta, pt, m_muonUncType});
    }
    //h_muon_iso_sf->Fill(sf_iso);

    sf = sf_reco * sf_ID * sf_iso;
    muonVeto_w *= sf;
  }

#if defined(MC_2016) || defined(MC_2017) || defined(MC_2018)
  evtW *= muonVeto_w;
#endif
  
  std::vector<LepObj> taus_lepVeto ;
  for (unsigned int i = 0 ; i < *(r->nTau) ; ++i) {
    LepObj tau((r->Tau_pt)[i],(r->Tau_eta)[i],-1,(r->Tau_phi)[i],(r->Tau_mass)[i],i,(r->Tau_charge)[i],(r->Tau_chargedIso)[i]) ;
    //int wp = 32;
    //bool passTauId = (((r->Tau_idDeepTau2017v2p1VSe)[i] >= wp) && ((r->Tau_idDeepTau2017v2p1VSjet)[i] >= wp) && ((r->Tau_idDeepTau2017v2p1VSmu)[i] >= wp));
    bool passTauId = ((((r->Tau_idDeepTau2017v2p1VSe)[i] & (1<<3)) != 0) && (((r->Tau_idDeepTau2017v2p1VSjet)[i] & (1<<3)) !=0) && (((r->Tau_idDeepTau2017v2p1VSmu)[i] & (1<<3)) !=0));
    //std::cout << "\n Tau id: " << std::bitset<8>((r->Tau_idDeepTau2017v2p1VSe)[i]) << " " << std::bitset<8>((r->Tau_idDeepTau2017v2p1VSjet)[i]) << " " << std::bitset<8>((r->Tau_idDeepTau2017v2p1VSmu)[i]) << " " << passTauId;
    if (tau.m_lvec.Pt() > CUTS.Get<float>("tau_veto_pt") && fabs(tau.m_lvec.Eta()) < CUTS.Get<float>("tau_eta") &&
        (r->Tau_decayMode)[i] != 5 && (r->Tau_decayMode)[i] != 6 && (r->Tau_decayMode)[i] != 7 && passTauId)
        //((((r->Tau_idDeepTau2017v2p1VSe)[i] & (1<<5)) != 0) || (((r->Tau_idDeepTau2017v2p1VSjet)[i] & (1<<5)) !=0) || (((r->Tau_idDeepTau2017v2p1VSmu)[i] & (1<<5)) !=0)))
      taus_lepVeto.push_back(tau);
  }
 
   // == For purposes of calculating JER, we need to get any GenJetsAK8 ==
  // == before we go on to get the reco AK8 jets.                      ==
  std::vector<JetObjBoosted> genJetAK8;
  double MC = 1.0;
  double btagWeight = 1.0;
  double btagWeightUp = 1.0;
  double btagWeightDn = 1.0;

#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018)
  for (unsigned int i = 0; i < *(r->nGenJetAK8); ++i) {
    int jetFlav = (r->GenJetAK8_hadronFlavour)[i];
    JetObjBoosted jet(
      (r->GenJetAK8_pt)[i], (r->GenJetAK8_eta)[i], (r->GenJetAK8_phi)[i], // pT, eta, phi
      (r->GenJetAK8_mass)[i], jetFlav, // mass & jet flavor
      -1.0, -1.0, -1.0, 
      -1.0, -1.0,        
      -1.0, -1.0,-1.0,    
      -1, -1, -1, -1, -1);  // remaining values
    genJetAK8.push_back(jet);

    //h_genJet_mass->Fill((r->GenJetAK8_mass)[i]);
  }
#endif

  //ak04 gen jets
  std::vector<JetObjBoosted> genJetAK4;
#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018)
  for (unsigned int i = 0; i < *(r->nGenJet); ++i) {
    int jetFlav = (r->GenJet_hadronFlavour)[i];
    JetObjBoosted jet(
      (r->GenJet_pt)[i], (r->GenJet_eta)[i], (r->GenJet_phi)[i], // pT, eta, phi              \
                                                                                               
      (r->GenJet_mass)[i], jetFlav, // mass & jet flavor                                       
      -1.0, -1.0, -1.0,
      -1.0, -1.0,
      -1.0, -1.0,-1.0,
      -1, -1, -1, -1, -1);  // remaining values                                                
    genJetAK4.push_back(jet);
  }
  //std::cout << "Got past gen jet AK4 loop..." << std::endl;                                  
#endif

  //ak08 jets
  std::vector<JetObjBoosted> jets ;
  for (unsigned int i = 0 ; i < *(r->nFatJet) ; ++i) {
    int jetFlav = -1 ;
#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018)
    jetFlav = (r->FatJet_hadronFlavour)[i];
#endif
    float totXcc = (r->FatJet_particleNetMD_Xcc)[i]+(r->FatJet_particleNetMD_QCD)[i];
    float totXbb = (r->FatJet_particleNetMD_Xbb)[i]+(r->FatJet_particleNetMD_QCD)[i];
    float Xcc = -1; 
    float Xbb = -1;
    if(totXcc>0) Xcc = (r->FatJet_particleNetMD_Xcc)[i]/(totXcc);
    if(totXbb>0) Xbb = (r->FatJet_particleNetMD_Xbb)[i]/(totXbb);

    float jetPt = (r->FatJet_pt)[i];
//TEMP
#if defined(POSTPROC)
    jetPt = (r->FatJet_pt_nom)[i];
#endif
    //std::cout << "\n Jet pt: " << jetPt;
#if defined(POSTPROC) && (defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018))
    if (m_jetmetSystType == "jesu") jetPt = (r->FatJet_pt_jesTotalUp)[i];
    if (m_jetmetSystType == "jesd") jetPt = (r->FatJet_pt_jesTotalDown)[i];
    if (m_jetmetSystType == "jer0u") jetPt = (r->FatJet_pt_jer0Up)[i];
    if (m_jetmetSystType == "jer0d") jetPt = (r->FatJet_pt_jer0Down)[i];
    if (m_jetmetSystType == "jer1u") jetPt = (r->FatJet_pt_jer1Up)[i];
    if (m_jetmetSystType == "jer1d") jetPt = (r->FatJet_pt_jer1Down)[i];
    if (m_jetmetSystType == "jer2u") jetPt = (r->FatJet_pt_jer2Up)[i];
    if (m_jetmetSystType == "jer2d") jetPt = (r->FatJet_pt_jer2Down)[i];
    if (m_jetmetSystType == "jer3u") jetPt = (r->FatJet_pt_jer3Up)[i];
    if (m_jetmetSystType == "jer3d") jetPt = (r->FatJet_pt_jer3Down)[i];
    if (m_jetmetSystType == "jer4u") jetPt = (r->FatJet_pt_jer4Up)[i];
    if (m_jetmetSystType == "jer4d") jetPt = (r->FatJet_pt_jer4Down)[i];
#endif
    
    // Calculate the JES & unc to use. The new recommendation is you need to calculate the values
    // for the individual AK4 subjects and then use this for the AK8 jet. NOTE: The only correction
    // that uses jet area and rho is the L1 correction which is no longer actually used. No matter
    // what we pass, L1 will return 1.0 as a factor. Thus, we do not need a workaround for the fact
    // that subjets do not provide jet area.

    // Get the subjets, run the JES+JER corrections over them,
    // and then use them to calculate the "corrected" SD mass.
    Int_t idx1 = (r->FatJet_subJetIdx1)[i];
    Int_t idx2 = (r->FatJet_subJetIdx2)[i];

    int nsub = 0;
    if (idx1 >= 0) nsub++;
    if (idx2 >= 0) nsub++;
    if (nsub < 2) continue;

    /// PLOT CODE GOES HERE

    std::map<std::string, float> subMap1;
    std::map<std::string, float> subMap2;

    float dummy_area = 0.5f * M_PI * 0.4 * 0.4;
    float rho = *(r->fixedGridRhoFastjetAll);

    subMap1["area"] = dummy_area;
    subMap2["area"] = dummy_area;
    subMap1["rho"] = rho;
    subMap2["rho"] = rho;

    subMap1["pt_raw"] = (r->SubJet_pt)[idx1] * (1 - (r->SubJet_rawFactor)[idx1]);
    subMap2["pt_raw"] = (r->SubJet_pt)[idx2] * (1 - (r->SubJet_rawFactor)[idx2]);
    subMap1["eta"] = (r->SubJet_eta)[idx1];
    subMap2["eta"] = (r->SubJet_eta)[idx2];

    double JES_sub1_pt = CalculateJES(subMap1, "pt", m_jetmetSystType, m_isData);
    double JES_sub2_pt = CalculateJES(subMap2, "pt", m_jetmetSystType, m_isData);
    double JES_sub1_m  = CalculateJES(subMap1, "mass", m_jetmetSystType, m_isData);
    double JES_sub2_m  = CalculateJES(subMap2, "mass", m_jetmetSystType, m_isData);

    h_jes_pt_SF->Fill(JES_sub1_pt, evtW);
    h_jes_pt_SF->Fill(JES_sub2_pt, evtW);
    h_jes_mass_SF->Fill(JES_sub1_m, evtW);
    h_jes_mass_SF->Fill(JES_sub2_m, evtW);
    
    double m_raw_1 = (r->SubJet_mass)[idx1] * (1 - (r->SubJet_rawFactor)[idx1]);
    double m_raw_2 = (r->SubJet_mass)[idx2] * (1 - (r->SubJet_rawFactor)[idx2]);
    double corrM_1 = m_raw_1 * JES_sub1_m;
    double corrM_2 = m_raw_2 * JES_sub2_m;
    double corrPt_1 = subMap1["pt_raw"] * JES_sub1_pt;
    double corrPt_2 = subMap2["pt_raw"] * JES_sub2_pt;

    subMap1["pt"] = corrPt_1; subMap1["mass"] = corrM_1; subMap1["phi"] = (r->SubJet_phi)[idx1];
    subMap2["pt"] = corrPt_2; subMap2["mass"] = corrM_2; subMap2["phi"] = (r->SubJet_phi)[idx2];

    // Calculate the JER corrections. This is only done for MC.
    // It can be done using the exact same correction set.
    // Do not apply JER for 2017 for nwo since it does not  have JER in correction set.

    
#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2018)
    //float tmp = 1.0f;
    float cJER_1 = CalculateJER(subMap1, genJetAK4, "pt", m_jetmetSystType);
    float cJER_2 = CalculateJER(subMap2, genJetAK4, "pt", m_jetmetSystType);
#else
    //float tmp = 1.0f;
    float cJER_1 = 1.0f;
    float cJER_2 = 1.0f;
#endif
    
    //if JER negative or zero do nothing                                                       
    if (cJER_1 <= 0) cJER_1 = 1.0f;
    if (cJER_2 <= 0) cJER_2 = 1.0f;
    corrPt_1 *= cJER_1;
    corrPt_2 *= cJER_2;

    h_jer_SF->Fill(cJER_1, evtW);
    h_jer_SF->Fill(cJER_1, evtW);
    
    // Calculate the SD mass using the di-subjet mass.                                         
    TLorentzVector sub_vec_1, sub_vec_2;
    sub_vec_1.SetPtEtaPhiM(corrPt_1, subMap1["eta"], subMap1["phi"], corrM_1);
    sub_vec_2.SetPtEtaPhiM(corrPt_2, subMap2["eta"], subMap2["phi"], corrM_2);

    // NOTE: There are cases where we only have one subjet.                                    
    // This is meant to take care of those cases. In those                                     
    // cases, the subjet's mass is equals to the AK8 m_sd.                                     
    TLorentzVector jetak8 = sub_vec_1 + sub_vec_2;
    //if (idx1 < 0) jetak8 = sub_vec_2;                                                        
    //if (idx2 < 0) jetak8 = sub_vec_1;                                                        
    float corrM = jetak8.M();
    float corrPt = jetak8.Pt();
    
    auto msdCorr = correctionSet_msd->at("msdfjcorr"); //this is correction applied to JEC correct mass
    float wmass_corr = 1;
    if ((r->FatJet_pt)[i]>0) wmass_corr = msdCorr->evaluate({(r->FatJet_msoftdrop)[i]/(r->FatJet_pt)[i],log((r->FatJet_pt)[i]),(r->FatJet_eta)[i]});
    //std::cout << "\n wmass_corr: " << wmass_corr << " " << corrM << " ";
    h_jet_mass_beforeWcorr->Fill(corrM, evtW);
    corrM *= wmass_corr;
    h_jet_mass_afterWcorr->Fill(corrM, evtW);
    h_wcorr->Fill(wmass_corr);
    //std::cout << corrM << std::endl;

    h_jet_mass->Fill(corrM);
    h_jet_pt->Fill(corrPt);

    JetObjBoosted jet(corrPt,(r->FatJet_eta)[i],(r->FatJet_phi)[i],corrM,jetFlav,
        (r->FatJet_btagDDCvB)[i],(r->FatJet_btagDDCvL)[i], (r->FatJet_btagDDBvL)[i], 
        (r->FatJet_deepTagMD_ZHccvsQCD)[i],(r->FatJet_deepTagMD_ZbbvsQCD)[i],
        Xcc,Xbb,(r->FatJet_particleNetMD_QCD)[i],
        (r->FatJet_particleNet_TvsQCD)[i],
        (r->FatJet_particleNet_WvsQCD)[i],
        (r->FatJet_particleNet_ZvsQCD)[i],
        (r->FatJet_n2b1)[i], -1) ;
    if(jet.IsLepton(eles_jetOverlap,0.8) || jet.IsLepton(muons_jetOverlap,0.8)) continue;
    if(corrPt > CUTS.Get<float>("jet_pt_ak08") && fabs((r->FatJet_eta)[i]) < CUTS.Get<float>("jet_eta_ak08")){
      jets.push_back(jet) ;
      h_jet_mass_afterSel->Fill(jet.m_lvec.M(), evtW);
      h_jet_pt_afterSel->Fill(jet.m_lvec.Pt(), evtW);

      h_Xcc_vs_pQCD_beforeCuts->Fill(Xcc, (r->FatJet_particleNetMD_QCD)[i], evtW);
    }
  }

  //Fill fat jet pt, rho, n2b1 to identify c_26 cut
  for(unsigned iJ = 0; iJ < jets.size();++iJ) {
    h_jet_mass->Fill(jets[iJ].m_lvec.M(),evtW);
    if(jets[iJ].m_lvec.M()>40) h_pt_rho_n2b1->Fill(jets[iJ].m_lvec.Pt(),jets[iJ].m_rho,jets[iJ].m_n2b1,evtW);
  }

  //ak04 jets
  std::vector<JetObj> ak4Jets;
  int nBjet(0); 
  for (unsigned int i = 0 ; i < *(r->nJet) ; ++i) {
    
    int jetFlav = -1 ;
#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018)
    jetFlav = (r->Jet_hadronFlavour)[i];
#endif 
    
    float jetPt = (r->Jet_pt)[i];

    JetObj jet(jetPt,(r->Jet_eta)[i],(r->Jet_phi)[i],(r->Jet_mass)[i],jetFlav,(r->Jet_btagDeepFlavB)[i],(r->Jet_puIdDisc)[i]) ;

    if (jet.m_lvec.Pt() < CUTS.Get<float>("jet_pt") || fabs(jet.m_lvec.Eta()) > CUTS.Get<float>("jet_eta")) continue ;
    
    if (jet.IsLepton(eles_jetOverlap)) continue ;
    if (jet.IsLepton(muons_jetOverlap)) continue ;
    
    if (r->Jet_jetId[i] < 2 ) continue ;
    
    if (jet.m_lvec.Pt()<50 && jet.m_lvec.Pt()>30 && (r->Jet_puIdDisc)[i]<0.61 ) continue;
	  
    ak4Jets.push_back(jet) ;
    if (jet.m_deepFlavB > deepFlavBCut) nBjet += 1;
  }

  if(ak4Jets.size()>0) h_ljpt->Fill(ak4Jets[0].m_lvec.Pt(),genWeight);
 
  bool passLepVeto(false);
  if (eles_lepVeto.size()==0) {
    h_cutFlow_ZccHcc_PN_med->Fill(2.5,evtW);
    h_cutFlow_VHcc_PN_med->Fill(2.5,evtW);
    if(muons_lepVeto.size()==0) {
      h_cutFlow_ZccHcc_PN_med->Fill(3.5,evtW);
      h_cutFlow_VHcc_PN_med->Fill(3.5,evtW);
      if (taus_lepVeto.size()==0) {
        h_cutFlow_ZccHcc_PN_med->Fill(4.5,evtW);
        h_cutFlow_VHcc_PN_med->Fill(4.5,evtW);
        passLepVeto = true;
      }
    }
  }

  //////////////////////////////////////////
  //HBoosted
  //////////////////////////////////////////
  //see page. 29 of AN-2019-048 for details
  //find a jet with highest CvL and with pt > 450
  //jet pt and eta cut
  std::vector<unsigned> idx_tmps;
  for(unsigned i = 0; i < jets.size();++i) {
    if (jets[i].m_lvec.Pt()> CUTS.Get<float>("jet_pt_ak08") && jets[i].m_lvec.M()>40) idx_tmps.push_back(i);
  }
  
  //jet rho cut
  std::vector<unsigned> idx_tmps_1;
  //consider jets passing kinematic cuts
  if (idx_tmps.size()>0) {
    h_cutFlow_ZccHcc_PN_med->Fill(5.5,evtW);
    h_cutFlow_VHcc_PN_med->Fill(5.5,evtW);
    for (auto i:idx_tmps) {
      if(jets[i].m_rho > -6 && jets[i].m_rho < -2.1) idx_tmps_1.push_back(i);
    }
  }

  ////////////////////////////////////////////////////
  //Trigger results, will be applied in selection
  ////////////////////////////////////////////////////
  bool trigger(false); 
#if defined(MC_2016PRE) || defined(MC_2016) || defined(DATA_2016PRE) || defined(DATA_2016)
   trigger =  *(r->HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p20) ||
     *(r->HLT_AK8PFJet360_TrimMass30) ||
     *(r->HLT_AK8PFJet450) ||
     *(r->HLT_PFJet450) ||
     *(r->HLT_AK8PFHT700_TrimR0p1PT0p03Mass50) ||
     *(r->HLT_PFHT900);
   h_triggerCheck->Fill(0.5);
  #if !defined(DATA_2016H)
   trigger = trigger || *(r->HLT_PFHT800);
   h_triggerCheck->Fill(1.5);
  #endif
#endif

#if defined(MC_2017) || defined(DATA_2017)
   trigger = (*(r->HLT_AK8PFJet500) ||
     *(r->HLT_PFJet500) ||
     *(r->HLT_PFHT1050));
   h_triggerCheck->Fill(0.5);
  #if defined(MC_2017) || !defined(DATA_2017B)
   trigger = trigger || 
     *(r->HLT_AK8PFJet400_TrimMass30) ||
     *(r->HLT_AK8PFHT800_TrimMass50);
   h_triggerCheck->Fill(1.5);
  #endif
  #if defined(MC_2017) || defined(DATA_2017F)
   trigger = trigger || *(r->HLT_AK8PFJet330_PFAK8BTagCSV_p17);
   h_triggerCheck->Fill(2.5);
  #endif
#endif

#if defined(MC_2018) || defined(DATA_2018)
   trigger = *(r->HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_np4) ||
     *(r->HLT_AK8PFJet400_TrimMass30) ||
     *(r->HLT_AK8PFJet500) ||
     *(r->HLT_PFJet500) ||
     *(r->HLT_AK8PFHT800_TrimMass50) ||
     *(r->HLT_PFHT1050);
   h_triggerCheck->Fill(0.5);
#endif

  ////////////////////////
  //MET cut///////////////
  ////////////////////////
  bool passMET(false);
  h_MET_beforeCut->Fill(*(r->MET_pt), evtW);
  if (*(r->MET_pt)<140) {
    passMET=true;
    h_MET_afterCut->Fill(*(r->MET_pt), evtW);
  }
    
  //jet n2b1 cut 
  std::vector<unsigned> idx_tmps_2;
  std::vector<std::pair<int,float> > idx_pn_cc_tmp;
  //consider jets passing kine and rho
  if (idx_tmps_1.size()>0) {
    //FIXME: fix other cutflow
    h_cutFlow_ZccHcc_PN_med->Fill(6.5,evtW);
    h_cutFlow_VHcc_PN_med->Fill(6.5,evtW);
    for (auto i:idx_tmps_1) {
      //std::cout << "\n" << jets[i].m_lvec.Pt() << " " << jets[i].m_rho << " " << Getn2b1Cut(jets[i].m_rho,jets[i].m_lvec.Pt());
      //if(jets[i].m_n2b1 < Getn2b1Cut(jets[i].m_rho,jets[i].m_lvec.Pt())) idx_tmps_2.push_back(i);
      if(true) {
        idx_tmps_2.push_back(i);
        idx_pn_cc_tmp.push_back({i,jets[i].m_PN_Xcc});
      }
    }
  }

  //only take 2 leading jets
  std::vector<std::pair<int,float> > idx_pn_cc;
  for(int i = 0; i < idx_pn_cc_tmp.size(); ++i) {
    idx_pn_cc.push_back(idx_pn_cc_tmp[i]);
    if (i == 1) break;
  }

  //if (idx_pn_cc.size() >= 2) {
  //  std::cout << "\n Pt: " <<
  //    jets[idx_pn_cc[0].first].m_lvec.Pt() <<
  //    " " << jets[idx_pn_cc[1].first].m_lvec.Pt();
  //}

  // Sort the jets based on their PN cc score.
  sort(idx_pn_cc.begin(),idx_pn_cc.end(),sortbysecdesc);
  //if (idx_pn_cc.size() >= 2) {
  //  std::cout << "\n";
  //  for(auto i : idx_pn_cc) std::cout << " (" << i.first << " " << i.second << ") ";
  //}


  // Make sure there are at least enough jets & we passed the lepton veto stage
  if (passLepVeto && (idx_pn_cc.size() >= 2)) { 

    //now do ambugity resolve using DH method
    //calculate the DH 
    std::vector<std::pair<std::vector<int>,float>> dhs_idx_cand;
    float dh=DH(jets[idx_pn_cc[0].first].m_lvec.M(),jets[idx_pn_cc[1].first].m_lvec.M());//mH,mZ
    std::vector<int> tmp = {idx_pn_cc[0].first,idx_pn_cc[1].first};
    dhs_idx_cand.push_back(std::make_pair(tmp,dh));//H,Z
    
    dh=DH(jets[idx_pn_cc[1].first].m_lvec.M(),jets[idx_pn_cc[0].first].m_lvec.M());//mH,mZ
    tmp = {idx_pn_cc[1].first,idx_pn_cc[0].first};
    dhs_idx_cand.push_back(std::make_pair(tmp,dh));//H,Z

    // Pick the candidate that has the smallest DH
    sort(dhs_idx_cand.begin(),dhs_idx_cand.end(),sortbysecdesc1);
    int idx_H = dhs_idx_cand.back().first[0]; // first jet = H
    int idx_Z = dhs_idx_cand.back().first[1]; // second jet = Z/V

    
    // Get the tagging scale factor & add it to the event weight.
    float tagW = 1.0;
    auto jet_H = std::make_pair(jets[idx_H],false);
    if(jets[idx_H].m_PN_Xcc >= XccCut) jet_H.second = true;
    auto jet_Z = std::make_pair(jets[idx_Z],false);
    if(jets[idx_Z].m_PN_Xcc >= XccCut) jet_Z.second = true;
#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018) 
    tagW = CalCtagWeightBoosted(jet_H,jet_Z,m_hfTagUncType);
#endif  
    float evtW_tag = evtW*tagW;

    // Get the trigger scale factor
    float trigSF=1;
    float jetPtMax = std::max(jets[idx_Z].m_lvec.Pt(),jets[idx_H].m_lvec.Pt());
    float jetPtMin = std::max(jets[idx_Z].m_lvec.Pt(),jets[idx_H].m_lvec.Pt());
#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018) 
    trigSF = GetTrigSF(jetPtMax,jetPtMin, m_trigUncType);
#endif  

    //std::cout << "\n Weights: " << evtW << " " << tagW << " " << evtW_tag << " " << trigSF ;

    // Construct the boson candidate objets from the proper jets
    ZObj Z(jets[idx_Z]);
    HObj H(jets[idx_H]);
    std::vector<JetObjBoosted> jet_ZccHcc{jets[idx_Z], jets[idx_H]};
    
    // Get the number of extra jets
    std::vector<JetObjBoosted> tmp1{jets[idx_Z],jets[idx_H]};
    std::vector<JetObj> extJets = NextraJet(tmp1, ak4Jets);
    int nExtraJet = extJets.size();

    // Get the btag weight for these extra jets
    float w_btag(1.0);   
#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018) 
    w_btag = CalBtagWeight(extJets,CUTS.GetStr("jet_main_btagWP"),m_btagUncType); 
    //std::cout << "\n " << w_btag;
#endif  

    // Get the number of bjets of extra jets
    int nBjet_extJets(0);
    for(auto jet : extJets)  if (jet.m_deepFlavB > deepFlavBCut) nBjet_extJets += 1;
    
    int nBjet_extJets_cut(0);

    //deltaPhi MET and jets
    float min_dPhiJetMet(1000);
    TLorentzVector lvec_met;
    lvec_met.SetPtEtaPhiM(*(r->MET_pt),0,*(r->MET_phi),0);
    for(auto jet: ak4Jets) {
      float tmp = fabs(jet.m_lvec.DeltaPhi(lvec_met));
      if (tmp < min_dPhiJetMet) min_dPhiJetMet = tmp;
    }

    float evtW_tag_trig = evtW_tag*trigSF;
    float evtW_tag_btag_trig = evtW_tag*trigSF*w_btag;
    
    /////////////////////////////////////////
    // Channel 1 - ZccHcc
    /////////////////////////////////////////

    // Check to make sure that both selected jets pass a pT > 450 GeV requirement
    if (jets[idx_Z].m_lvec.Pt()>450 && jets[idx_H].m_lvec.Pt()>450) {

      h_cutFlow_ZccHcc_PN_med->Fill(7.5,evtW); // ZccHcc - passes jet pT cuts
      h_ZccHcc_PN_med->h_ccTagDis_beforeCut->Fill(jets[idx_H].m_PN_Xcc,evtW_tag);

      // Check to see if the Higgs candidate passes the tagging cut
      // This is the start of the signal region (SR)
      if (jets[idx_H].m_PN_Xcc>=XccCut) {
	
        //////////////////////
        //evtW_tag = evtW; //////////////////////////////////////////////////////////????FIXME HERE!!!!!!
        h_cutFlow_ZccHcc_PN_med->Fill(8.5,evtW_tag);

	// Check the extra criteria we're interested in
        if(nExtraJet < 2 && passMET && trigger) {
          float xcc_weight = 1.0 ;
          float tagWtmp = 1.0;
#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018) 
          xcc_weight = Get_xccbb_weight(jets,idx_H,XbbCut,XccCut,"xcc");
          auto jet_H = std::make_pair(jets[idx_H].m_lvec.Pt(),true);
          tagWtmp = CalTagWeightBoosted_1jet(jet_H, jets[idx_H].m_flav, "xcc", m_hfTagUncType);
#endif
          //float w = evtW*trigSF*tagWtmp*xcc_weight;
          float w = evtW*trigSF*tagW*xcc_weight;
          
          //std::cout << "\n Weights 1: " << evtW << " " << trigSF << " " << tagWtmp << " " << xcc_weight ;

          h_ZccHcc_PN_med_xccWeight->Fill(H,Z,VZtype,w);
          h_ZccHcc_PN_med_xccWeight->FillJets(jet_ZccHcc,w);
        }//end-extra-criteria

        //std::vector<JetObjBoosted> jet_ZccHcc{jets[idx_Z], jets[idx_H]};
        h_ZccHcc_PN_med->h_bbTagDis_beforeCut->Fill(jets[idx_Z].m_PN_Xcc,evtW_tag); 
        if (jets[idx_Z].m_PN_Xcc>=XccCut) {
          h_cutFlow_ZccHcc_PN_med->Fill(9.5,evtW_tag);
          h_NextraJet_ZccHcc->Fill(nExtraJet,evtW_tag);
	  
          ///////////////////////////////////
          //>>>>>>>>>>>>SR ZccHcc>>>>>>>>>>>>>>>>>>>
          ///////////////////////////////////
          h_ZccHcc_PN_med->h_NextraJet_beforeCut->Fill(nExtraJet,evtW_tag); 
          h_ZccHcc_PN_med->h_MET_beforeCut_1->Fill(*(r->MET_pt),evtW_tag);
          //top quarks suppression
          
          if (passMET && trigger && nExtraJet>=2) {
            h_ZccHcc_PN_med->h_nBjet_failNextraJetCut->Fill(nBjet,evtW_tag_trig);
            h_ZccHcc_PN_med->h_nBjetExtraJet_failNextraJetCut->Fill(nBjet_extJets,evtW_tag_trig);
            h_ZccHcc_PN_med->h_DPhiJetMet_failNextraJetCut->Fill(min_dPhiJetMet,evtW_tag_trig); 
          }

          if (nExtraJet < 2) {
            h_cutFlow_ZccHcc_PN_med->Fill(10.5,evtW_tag);
            //now fill histograms
            h_ZccHcc_PN_med->h_MET_beforeCut->Fill(*(r->MET_pt),evtW_tag);
            if (passMET) {
              h_cutFlow_ZccHcc_PN_med->Fill(11.5,evtW_tag);
              if (trigger) {
                h_evtW_ZccHcc->Fill(evtW);
                h_tagW_ZccHcc->Fill(tagW);
                h_trigW_ZccHcc->Fill(trigSF);
                h_cutFlow_ZccHcc_PN_med->Fill(12.5,evtW_tag_trig);
                h_ZccHcc_PN_med->Fill(H,Z,VZtype,evtW_tag_trig);
                h_ZccHcc_PN_med->FillJets(jet_ZccHcc,evtW_tag_trig);
                h_ZccHcc_PN_med->h_bbTagDis->Fill(jets[idx_Z].m_PN_Xcc,evtW_tag_trig);
                h_ZccHcc_PN_med->h_ccTagDis->Fill(jets[idx_H].m_PN_Xcc,evtW_tag_trig);
                h_ZccHcc_PN_med->h_MET->Fill(*(r->MET_pt),evtW_tag_trig);
                h_ZccHcc_PN_med->h_nBjet_passNextraJetCut->Fill(nBjet,evtW_tag_trig);
                h_ZccHcc_PN_med->h_nBjetExtraJet_passNextraJetCut->Fill(nBjet_extJets,evtW_tag_trig);
                h_ZccHcc_PN_med->h_DPhiJetMet_passNextraJetCut->Fill(min_dPhiJetMet,evtW_tag_trig);

                //PDF,SCALE
#if (defined(MC_2016) || defined(MC_2016PRE) || defined(MC_2017) || defined(MC_2018)) && !defined(MC_VV_LO)
                h_ZccHcc_PN_med->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
                //scaleunc
                if (m_scaleUnc=="scale") h_ZccHcc_PN_med->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
#if defined(MC_VZ)                
                if (VZtypeGroup == 0) {
                  h_ZccHcc_PN_med_VZcc->Fill(H,Z,VZtype,evtW_tag_trig);
                  h_ZccHcc_PN_med_VZcc->FillJets(jet_ZccHcc,evtW_tag_trig);
                  h_ZccHcc_PN_med_VZcc->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
                  if (m_scaleUnc=="scale") h_ZccHcc_PN_med_VZcc->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
                }
                else if (VZtypeGroup == 1) {
                  h_ZccHcc_PN_med_VZbb->Fill(H,Z,VZtype,evtW_tag_trig);
                  h_ZccHcc_PN_med_VZbb->FillJets(jet_ZccHcc,evtW_tag_trig);
                  h_ZccHcc_PN_med_VZbb->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
                  if (m_scaleUnc=="scale") h_ZccHcc_PN_med_VZbb->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
                }
                else {
                  h_ZccHcc_PN_med_VZother->Fill(H,Z,VZtype,evtW_tag_trig);
                  h_ZccHcc_PN_med_VZother->FillJets(jet_ZccHcc,evtW_tag_trig);
                  h_ZccHcc_PN_med_VZother->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
                  if (m_scaleUnc=="scale") h_ZccHcc_PN_med_VZother->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
                }
#endif
#endif
                float deltaPhi = jets[idx_Z].m_lvec.DeltaPhi(jets[idx_H].m_lvec);
                if (deltaPhi > 2.5) {
                  h_cutFlow_ZccHcc_PN_med->Fill(13.5,evtW_tag_trig);
                  if (jets[idx_Z].m_lvec.M() < 120 && jets[idx_Z].m_lvec.M() > 60) 
                  {
                    h_cutFlow_ZccHcc_PN_med->Fill(14.5,evtW_tag_trig);
                    h_ZccHcc_PN_med_zmass_deltaPhi->Fill(H,Z,VZtype,evtW_tag_trig);
                    h_ZccHcc_PN_med_zmass_deltaPhi->FillJets(jet_ZccHcc,evtW_tag_trig);
                  }//end-Zmass-cut
                }//end-dPhi-cut
              }//end-trigger-cut
            }//end-MET-cut
          }//end-extraJet-cut
        }//end-Z-Xcc-cut
      } //end-SR (H candidate passes Xcc cut)
      
      // Fail CR #1 - This fail region is defined by having our
      // Higgs candidate FAILING the Xcc tagging cut
      // BUT the Z candidate PASSES the Xcc tagging cut
      if (jets[idx_H].m_PN_Xcc<XccCut && jets[idx_Z].m_PN_Xcc>=XccCut) {

	// Check the MET and trigger cuts
	if(passMET && trigger) {

	  // Check the cut on number of extra jets
          if (nExtraJet < 2) {

	    // Fill the appropriate plots
            h_ZccHcc_PN_med_qcdCR->Fill(H,Z,VZtype,evtW_tag_trig);
            h_ZccHcc_PN_med_qcdCR->FillJets(jet_ZccHcc,evtW_tag_trig);
            h_ZccHcc_PN_med_qcdCR->h_bbTagDis->Fill(jets[idx_Z].m_PN_Xcc,evtW_tag_trig);
            h_ZccHcc_PN_med_qcdCR->h_ccTagDis->Fill(jets[idx_H].m_PN_Xcc,evtW_tag_trig);
            h_ZccHcc_PN_med_qcdCR->h_MET->Fill(*(r->MET_pt),evtW_tag_trig);
            //PDF,SCALE
#if (defined(MC_2016) || defined(MC_2016PRE) || defined(MC_2017) || defined(MC_2018)) && !defined(MC_VV_LO)
            //pdfunc
            h_ZccHcc_PN_med_qcdCR->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
            //scaleunc
            if (m_scaleUnc=="scale") h_ZccHcc_PN_med_qcdCR->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
#if defined(MC_VZ)                
            if (VZtypeGroup == 0) {
              h_ZccHcc_PN_med_qcdCR_VZcc->Fill(H,Z,VZtype,evtW_tag_trig);
              h_ZccHcc_PN_med_qcdCR_VZcc->FillJets(jet_ZccHcc,evtW_tag_trig);
              h_ZccHcc_PN_med_qcdCR_VZcc->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
              if (m_scaleUnc=="scale") h_ZccHcc_PN_med_qcdCR_VZcc->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
            }
            else if (VZtypeGroup == 1) {
              h_ZccHcc_PN_med_qcdCR_VZbb->Fill(H,Z,VZtype,evtW_tag_trig);
              h_ZccHcc_PN_med_qcdCR_VZbb->FillJets(jet_ZccHcc,evtW_tag_trig);
              h_ZccHcc_PN_med_qcdCR_VZbb->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
              if (m_scaleUnc=="scale") h_ZccHcc_PN_med_qcdCR_VZbb->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
            }
            else {
              h_ZccHcc_PN_med_qcdCR_VZother->Fill(H,Z,VZtype,evtW_tag_trig);
              h_ZccHcc_PN_med_qcdCR_VZother->FillJets(jet_ZccHcc,evtW_tag_trig);
              h_ZccHcc_PN_med_qcdCR_VZother->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
              if (m_scaleUnc=="scale") h_ZccHcc_PN_med_qcdCR_VZother->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
            }
#endif

#endif
          }//end-extraJet-cut
        }//end-MET-trigger-cuts
      } //end fail CR 
    }//end ZccHcc


    /////////////////////////////////////////
    // Control Region 1 - Top (ZccHcc)    
    ///////////////////////////////////////// 

    // This checks the Top CR for ZccHcc. This takes the same criteria
    // as our signal region but inverts the NextraJet and Nbjet cuts.
    if (jets[idx_Z].m_lvec.Pt()>350 && jets[idx_H].m_lvec.Pt()>350) {

      // Invert the nExtraJet cut and check others
      if (nExtraJet >= 2 && passMET && trigger) {

        // Check that both of the jets pass the tagging cut
	if (jets[idx_H].m_PN_Xcc>=XccCut && jets[idx_Z].m_PN_Xcc>=XccCut) {
	  
          h_ZccHcc_PN_med_topCR_pass->h_ccPN_TopvsQCD->Fill(jets[idx_H].m_PN_TopvsQCD,evtW_tag_btag_trig);
          h_ZccHcc_PN_med_topCR_pass->h_bbPN_TopvsQCD->Fill(jets[idx_Z].m_PN_TopvsQCD,evtW_tag_btag_trig);
          h_ZccHcc_PN_med_topCR_pass->h_DPhiJetMet_failNextraJetCut->Fill(min_dPhiJetMet,evtW_tag_btag_trig);
          h_ZccHcc_PN_med_topCR_pass->h_nBjet_failNextraJetCut->Fill(nBjet,evtW_tag_btag_trig);
          h_ZccHcc_PN_med_topCR_pass->h_nBjetExtraJet_failNextraJetCut->Fill(nBjet_extJets,evtW_tag_btag_trig);
          //top CR 
          //if(nBjet_extJets > nBjet_extJets_cut && jets[idx_Z].m_PN_TopvsQCD > 0.02) 
          if(nBjet_extJets > nBjet_extJets_cut) {
            h_ZccHcc_PN_med_topCR_pass->Fill(H,Z,VZtype,evtW_tag_btag_trig);
            h_ZccHcc_PN_med_topCR_pass->FillJets(jet_ZccHcc,evtW_tag_btag_trig);
            h_ZccHcc_PN_med_topCR_pass->h_ccTagDis->Fill(jets[idx_H].m_PN_Xcc,evtW_tag_btag_trig);
            h_ZccHcc_PN_med_topCR_pass->h_bbTagDis->Fill(jets[idx_Z].m_PN_Xcc,evtW_tag_btag_trig);
            //PDF,SCALE
#if (defined(MC_2016) || defined(MC_2016PRE) || defined(MC_2017) || defined(MC_2018)) && !defined(MC_VV_LO)
            //pdfunc
            h_ZccHcc_PN_med_topCR_pass->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_btag_trig);
            //scaleunc
            if (m_scaleUnc=="scale") h_ZccHcc_PN_med_topCR_pass->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_btag_trig);
#if defined(MC_VZ)                
            if (VZtypeGroup == 0) {
              h_ZccHcc_PN_med_topCR_pass_VZcc->Fill(H,Z,VZtype,evtW_tag_trig);
              h_ZccHcc_PN_med_topCR_pass_VZcc->FillJets(jet_ZccHcc,evtW_tag_trig);
              h_ZccHcc_PN_med_topCR_pass_VZcc->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
              if (m_scaleUnc=="scale") h_ZccHcc_PN_med_topCR_pass_VZcc->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
            }
            else if (VZtypeGroup == 1) {
              h_ZccHcc_PN_med_topCR_pass_VZbb->Fill(H,Z,VZtype,evtW_tag_trig);
              h_ZccHcc_PN_med_topCR_pass_VZbb->FillJets(jet_ZccHcc,evtW_tag_trig);
              h_ZccHcc_PN_med_topCR_pass_VZbb->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
              if (m_scaleUnc=="scale") h_ZccHcc_PN_med_topCR_pass_VZbb->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
            }
            else {
              h_ZccHcc_PN_med_topCR_pass_VZother->Fill(H,Z,VZtype,evtW_tag_trig);
              h_ZccHcc_PN_med_topCR_pass_VZother->FillJets(jet_ZccHcc,evtW_tag_trig);
              h_ZccHcc_PN_med_topCR_pass_VZother->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
              if (m_scaleUnc=="scale") h_ZccHcc_PN_med_topCR_pass_VZother->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
            }
#endif

#endif
          }
          //Vjet CR
          h_ZccHcc_PN_med_VjetCR_pass->h_ccPN_TopvsQCD->Fill(jets[idx_H].m_PN_TopvsQCD,evtW_tag_btag_trig);
          h_ZccHcc_PN_med_VjetCR_pass->h_bbPN_TopvsQCD->Fill(jets[idx_Z].m_PN_TopvsQCD,evtW_tag_btag_trig);
          h_ZccHcc_PN_med_VjetCR_pass->h_ccPN_WvsQCD->Fill(jets[idx_H].m_PN_WvsQCD,evtW_tag_btag_trig);
          h_ZccHcc_PN_med_VjetCR_pass->h_bbPN_WvsQCD->Fill(jets[idx_Z].m_PN_WvsQCD,evtW_tag_btag_trig);
          h_ZccHcc_PN_med_VjetCR_pass->h_ccPN_ZvsQCD->Fill(jets[idx_H].m_PN_ZvsQCD,evtW_tag_btag_trig);
          h_ZccHcc_PN_med_VjetCR_pass->h_bbPN_ZvsQCD->Fill(jets[idx_Z].m_PN_ZvsQCD,evtW_tag_btag_trig);
          h_ZccHcc_PN_med_VjetCR_pass->h_DPhiJetMet_failNextraJetCut->Fill(min_dPhiJetMet,evtW_tag_btag_trig);
          h_ZccHcc_PN_med_VjetCR_pass->h_nBjet_failNextraJetCut->Fill(nBjet,evtW_tag_btag_trig);
          h_ZccHcc_PN_med_VjetCR_pass->h_nBjetExtraJet_failNextraJetCut->Fill(nBjet_extJets,evtW_tag_btag_trig);
          if(nBjet_extJets == nBjet_extJets_cut) {
            h_ZccHcc_PN_med_VjetCR_pass->Fill(H,Z,VZtype,evtW_tag_btag_trig);
            h_ZccHcc_PN_med_VjetCR_pass->FillJets(jet_ZccHcc,evtW_tag_btag_trig);
            h_ZccHcc_PN_med_VjetCR_pass->h_ccTagDis->Fill(jets[idx_H].m_PN_Xcc,evtW_tag_btag_trig);
            h_ZccHcc_PN_med_VjetCR_pass->h_bbTagDis->Fill(jets[idx_Z].m_PN_Xcc,evtW_tag_btag_trig);
          }//end-extraBjets-cut
        }//end-tagging-cuts
      }//end-NextraJet-MET-trigger-cuts
    }//end-Top-CR


    /////////////////////////////////////////
    // Channel 2 - VqqHcc
    ///////////////////////////////////////// 

    // In this channel, we look at Vqq that is NOT Zcc.
    // This means we re-label anything with Z as V
    // for a more general vector boson V.
    
    
    idx_H = idx_pn_cc[0].first; //assign leading PN_cc for idx_H
    int idx_V = idx_pn_cc[1].first; 
    ZObj V = ZObj(jets[idx_V]);
    H = HObj(jets[idx_H]);
    
    std::vector<JetObjBoosted> jet_VHcc{jets[idx_V], jets[idx_H]};
    ///////////////////////////////////////////////////////////////////////FIXME HERE////////////////////
    //evtW_tag = evtW;

    // Check that the Higgs boson passes a pT > 450 GeV requirement
    // and V boson passes a pT > 200 GeV requirement
    // UPDATE(Apr 2, 2025): move pT(V) cut to 350 GeV
    // UPDATE(Apr 5, 2025): changed back since we saw no improvement
    if (jets[idx_V].m_lvec.Pt() > 200 && jets[idx_H].m_lvec.Pt() > 450) {

      h_cutFlow_VHcc_PN_med->Fill(7.5,evtW); // passes pT cuts

      // ??? I don't know purpose of this - Peter 
      if (jets[idx_V].m_PN_Xcc < XccCut)
        h_VHcc_PN_med->h_pQCD_beforeCut->Fill(jets[idx_V].m_PN_pQCD,evtW_tag); 
      if (jets[idx_V].m_PN_pQCD< pQCDcut)
        h_VHcc_PN_med->h_bbTagDis_beforeCut->Fill(jets[idx_V].m_PN_Xcc,evtW_tag);

      // Check that the V boson FAILS our Xcc cut
      // but also PASSES our 2-prong cut
      // (This is the definition of the VqqHcc region)
      if (jets[idx_V].m_PN_Xcc < XccCut && jets[idx_V].m_PN_pQCD < pQCDcut) { //tag V-jet, 2-prong cut 

	h_cutFlow_VHcc_PN_med->Fill(8.5,evtW_tag); // passes cuts
        h_VHcc_PN_med->h_ccTagDis_beforeCut->Fill(jets[idx_H].m_PN_Xcc,evtW_tag); 

        // Check that the Higgs candidate PASSES the Xcc cut
	// This defines our Signal Region (SR)
	if (jets[idx_H].m_PN_Xcc>XccCut) {
	  
          h_cutFlow_VHcc_PN_med->Fill(9.5,evtW_tag);
          h_NextraJet_VHcc->Fill(nExtraJet,evtW_tag);
	  
          ///////////////////////////////////
          //>>>>>>>>>>>>SR VHcc>>>>>>>>>>>>>>>>>>>
            ///////////////////////////////////
          h_VHcc_PN_med->h_NextraJet_beforeCut->Fill(nExtraJet,evtW_tag); 
          h_VHcc_PN_med->h_MET_beforeCut_1->Fill(*(r->MET_pt),evtW_tag);

	  //Top quarks suppression - we shouldn't have many extra jets
          if (nExtraJet < 2) {

	    h_cutFlow_VHcc_PN_med->Fill(10.5,evtW_tag); // passes-nExtraJet

	    //now fill histograms
            h_VHcc_PN_med->h_MET_beforeCut->Fill(*(r->MET_pt),evtW_tag);
	    
            // Check for the MET and trigger cuts
	    if (passMET) {
              h_cutFlow_VHcc_PN_med->Fill(11.5,evtW_tag);
	      
              if (trigger) {
		
                //float evtW_tag_trig = evtW_tag*trigSF;
                h_evtW_VHcc->Fill(evtW);
                h_tagW_VHcc->Fill(tagW);
                h_trigW_VHcc->Fill(trigSF);
                h_cutFlow_VHcc_PN_med->Fill(12.5,evtW_tag_trig);
                h_VHcc_PN_med->Fill(H,V,VZtype,evtW_tag_trig);
                h_VHcc_PN_med->FillJets(jet_VHcc,evtW_tag_trig);
                h_VHcc_PN_med->h_bbTagDis->Fill(jets[idx_V].m_PN_Xcc,evtW_tag_trig);
                h_VHcc_PN_med->h_ccTagDis->Fill(jets[idx_H].m_PN_Xcc,evtW_tag_trig);
                h_VHcc_PN_med->h_MET->Fill(*(r->MET_pt),evtW_tag_trig);
                //PDF,SCALE
#if (defined(MC_2016) || defined(MC_2016PRE) || defined(MC_2017) || defined(MC_2018)) && !defined(MC_VV_LO)
                //pdfunc
                h_VHcc_PN_med->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
                //scaleunc
                if (m_scaleUnc=="scale") h_VHcc_PN_med->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
#if defined(MC_VZ)                
                if (VZtypeGroup == 0) {
                  h_VHcc_PN_med_VZcc->Fill(H,V,VZtype,evtW_tag_trig);
                  h_VHcc_PN_med_VZcc->FillJets(jet_VHcc,evtW_tag_trig);
                  h_VHcc_PN_med_VZcc->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
                  if (m_scaleUnc=="scale") h_VHcc_PN_med_VZcc->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
                }
                else if (VZtypeGroup == 1) {
                  h_VHcc_PN_med_VZbb->Fill(H,V,VZtype,evtW_tag_trig);
                  h_VHcc_PN_med_VZbb->FillJets(jet_VHcc,evtW_tag_trig);
                  h_VHcc_PN_med_VZbb->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
                  if (m_scaleUnc=="scale") h_VHcc_PN_med_VZbb->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
                }
                else {
                  h_VHcc_PN_med_VZother->Fill(H,V,VZtype,evtW_tag_trig);
                  h_VHcc_PN_med_VZother->FillJets(jet_VHcc,evtW_tag_trig);
                  h_VHcc_PN_med_VZother->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
                  if (m_scaleUnc=="scale") h_VHcc_PN_med_VZother->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
                }
#endif

#endif
                float deltaPhi = jets[idx_V].m_lvec.DeltaPhi(jets[idx_H].m_lvec);
                if (deltaPhi > 2.5) {
                  h_cutFlow_VHcc_PN_med->Fill(13.5,evtW_tag_trig);
                  if (jets[idx_V].m_lvec.M() < 120 && jets[idx_V].m_lvec.M() > 60) 
                  {
                    h_cutFlow_VHcc_PN_med->Fill(14.5,evtW_tag_trig);
                    h_VHcc_PN_med_zmass_deltaPhi->Fill(H,V,VZtype,evtW_tag_trig);
                    h_VHcc_PN_med_zmass_deltaPhi->FillJets(jet_VHcc,evtW_tag_trig);
                  }//end-Vmass-cut
                }//end-deltaPhi-cut
              }//end-trigger-cut
            }//end-MET-cut
          }//end-extra-jets-cut

	  /////////////////////////////////////
	  // Control Region #2 - Top (VqqHcc)
	  /////////////////////////////////////
	  // We get the Top CR for VqqHcc by inverting
	  // the NextraJet and Nbjet cuts
	  else { //nExtraJet >= 2

	    // Check the MET & trigger cuts
            if (passMET && trigger) {
              //topCR
              h_VHcc_PN_med_topCR_pass->h_ccPN_TopvsQCD->Fill(jets[idx_H].m_PN_TopvsQCD,evtW_tag_btag_trig);
              h_VHcc_PN_med_topCR_pass->h_bbPN_TopvsQCD->Fill(jets[idx_Z].m_PN_TopvsQCD,evtW_tag_btag_trig);
              h_VHcc_PN_med_topCR_pass->h_DPhiJetMet_failNextraJetCut->Fill(min_dPhiJetMet,evtW_tag_btag_trig);
              h_VHcc_PN_med_topCR_pass->h_nBjet_failNextraJetCut->Fill(nBjet,evtW_tag_btag_trig);
              h_VHcc_PN_med_topCR_pass->h_nBjetExtraJet_failNextraJetCut->Fill(nBjet_extJets,evtW_tag_btag_trig);
              //if(nBjet_extJets > nBjet_extJets_cut && jets[idx_Z].m_PN_TopvsQCD > 0.02) 
              if(nBjet_extJets > nBjet_extJets_cut) {
                h_VHcc_PN_med_topCR_pass->Fill(H,V,VZtype,evtW_tag_btag_trig);
                h_VHcc_PN_med_topCR_pass->FillJets(jet_VHcc,evtW_tag_btag_trig);
                h_VHcc_PN_med_topCR_pass->h_bbTagDis->Fill(jets[idx_Z].m_PN_Xcc,evtW_tag_btag_trig);
                h_VHcc_PN_med_topCR_pass->h_ccTagDis->Fill(jets[idx_H].m_PN_Xcc,evtW_tag_btag_trig);
                //PDF,SCALE
#if (defined(MC_2016) || defined(MC_2016PRE) || defined(MC_2017) || defined(MC_2018)) && !defined(MC_VV_LO)
                //pdfunc
                h_VHcc_PN_med_topCR_pass->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_btag_trig);
                //scaleunc
                if (m_scaleUnc=="scale") h_VHcc_PN_med_topCR_pass->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_btag_trig);
#if defined(MC_VZ)                
                if (VZtypeGroup == 0) {
                  h_VHcc_PN_med_topCR_pass_VZcc->Fill(H,V,VZtype,evtW_tag_trig);
                  h_VHcc_PN_med_topCR_pass_VZcc->FillJets(jet_VHcc,evtW_tag_trig);
                  h_VHcc_PN_med_topCR_pass_VZcc->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
                  if (m_scaleUnc=="scale") h_VHcc_PN_med_topCR_pass_VZcc->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
                }
                else if (VZtypeGroup == 1) {
                  h_VHcc_PN_med_topCR_pass_VZbb->Fill(H,V,VZtype,evtW_tag_trig);
                  h_VHcc_PN_med_topCR_pass_VZbb->FillJets(jet_VHcc,evtW_tag_trig);
                  h_VHcc_PN_med_topCR_pass_VZbb->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
                  if (m_scaleUnc=="scale") h_VHcc_PN_med_topCR_pass_VZbb->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
                }
                else {
                  h_VHcc_PN_med_topCR_pass_VZother->Fill(H,V,VZtype,evtW_tag_trig);
                  h_VHcc_PN_med_topCR_pass_VZother->FillJets(jet_VHcc,evtW_tag_trig);
                  h_VHcc_PN_med_topCR_pass_VZother->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
                  if (m_scaleUnc=="scale") h_VHcc_PN_med_topCR_pass_VZother->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
                }
#endif

#endif
              }//end-Nbjet-cut
	      
              //Vjet CR
              h_VHcc_PN_med_VjetCR_pass->h_ccPN_WvsQCD->Fill(jets[idx_H].m_PN_WvsQCD,evtW_tag_btag_trig);
              h_VHcc_PN_med_VjetCR_pass->h_bbPN_WvsQCD->Fill(jets[idx_Z].m_PN_WvsQCD,evtW_tag_btag_trig);
              h_VHcc_PN_med_VjetCR_pass->h_ccPN_ZvsQCD->Fill(jets[idx_H].m_PN_ZvsQCD,evtW_tag_btag_trig);
              h_VHcc_PN_med_VjetCR_pass->h_bbPN_ZvsQCD->Fill(jets[idx_Z].m_PN_ZvsQCD,evtW_tag_btag_trig);
              h_VHcc_PN_med_VjetCR_pass->h_DPhiJetMet_failNextraJetCut->Fill(min_dPhiJetMet,evtW_tag_btag_trig);
              h_VHcc_PN_med_VjetCR_pass->h_nBjet_failNextraJetCut->Fill(nBjet,evtW_tag_btag_trig);
              h_VHcc_PN_med_VjetCR_pass->h_nBjetExtraJet_failNextraJetCut->Fill(nBjet_extJets,evtW_tag_btag_trig);
              if(nBjet_extJets <= nBjet_extJets_cut) {
                h_VHcc_PN_med_VjetCR_pass->Fill(H,V,VZtype,evtW_tag_btag_trig);
                h_VHcc_PN_med_VjetCR_pass->FillJets(jet_VHcc,evtW_tag_btag_trig);
                h_VHcc_PN_med_VjetCR_pass->h_bbTagDis->Fill(jets[idx_Z].m_PN_Xcc,evtW_tag_btag_trig);
                h_VHcc_PN_med_VjetCR_pass->h_ccTagDis->Fill(jets[idx_H].m_PN_Xcc,evtW_tag_btag_trig);
              }//end-nbjet-cut
            }//end-MET-trigger-cuts
          }//end-top-CR(VHcc)
        }//end-SR VHcc (H-Xcc-cut)

	// Fail CR #2 - This fail region is defined by
        // Higgs candidate FAILING the Xcc tagging cut 
        if (jets[idx_H].m_PN_Xcc<XccCut) {

          // pass MET & trigger cuts
	  if(passMET && trigger) { 
            if (nExtraJet < 2) {
              h_VHcc_PN_med_qcdCR->Fill(H,V,VZtype,evtW_tag_trig);
              h_VHcc_PN_med_qcdCR->FillJets(jet_VHcc,evtW_tag_trig);
              h_VHcc_PN_med_qcdCR->h_bbTagDis->Fill(jets[idx_Z].m_PN_Xcc,evtW_tag_trig);
              h_VHcc_PN_med_qcdCR->h_ccTagDis->Fill(jets[idx_H].m_PN_Xcc,evtW_tag_trig);
              h_VHcc_PN_med_qcdCR->h_MET->Fill(*(r->MET_pt),evtW_tag_trig);
              //PDF,SCALE
#if (defined(MC_2016) || defined(MC_2016PRE) || defined(MC_2017) || defined(MC_2018)) && !defined(MC_VV_LO)
              //pdfunc
              h_VHcc_PN_med_qcdCR->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
              //scaleunc
              if (m_scaleUnc=="scale") h_VHcc_PN_med_qcdCR->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
#if defined(MC_VZ)                
              if (VZtypeGroup == 0) {
                h_VHcc_PN_med_qcdCR_VZcc->Fill(H,V,VZtype,evtW_tag_trig);
                h_VHcc_PN_med_qcdCR_VZcc->FillJets(jet_VHcc,evtW_tag_trig);
                h_VHcc_PN_med_qcdCR_VZcc->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
                if (m_scaleUnc=="scale") h_VHcc_PN_med_qcdCR_VZcc->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
              }
              else if (VZtypeGroup == 1) {
                h_VHcc_PN_med_qcdCR_VZbb->Fill(H,V,VZtype,evtW_tag_trig);
                h_VHcc_PN_med_qcdCR_VZbb->FillJets(jet_VHcc,evtW_tag_trig);
                h_VHcc_PN_med_qcdCR_VZbb->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
                if (m_scaleUnc=="scale") h_VHcc_PN_med_qcdCR_VZbb->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
              }
              else {
                h_VHcc_PN_med_qcdCR_VZother->Fill(H,V,VZtype,evtW_tag_trig);
                h_VHcc_PN_med_qcdCR_VZother->FillJets(jet_VHcc,evtW_tag_trig);
                h_VHcc_PN_med_qcdCR_VZother->FillPdfScaleUnc(r,H,m_iPdfStart,m_iPdfStop,0,evtW_tag_trig);
                if (m_scaleUnc=="scale") h_VHcc_PN_med_qcdCR_VZother->FillPdfScaleUnc(r,H,0,0,*(r->nLHEScaleWeight),evtW_tag_trig);
              }
#endif

#endif
            }
          }//end-MET-trigger-cuts
        } //end fail CR
	
      }//end-V-tag-cut

      h_Xcc_vs_pQCD->Fill(jets[idx_V].m_PN_Xcc, jets[idx_V].m_PN_pQCD, evtW_tag);

      // ////////////////////////////////////////////////////
      // Control Region - Top QCD Enchriced
      // ////////////////////////////////////////////////////
      // This is used for data-driven QCD bckg estimation.
      // We have the V-jet pass the cut instead of failing it
      // Apr 10, 2025: new idea - < XccCut, > pQCDcut
      if (jets[idx_V].m_PN_Xcc < XccCut && jets[idx_V].m_PN_pQCD > pQCDcut)
      {
        // Fill the tag scores before cut.
	h_VHcc_PN_med_qcdEnriched_topCR->h_ccTagDis_beforeCut->Fill(jets[idx_H].m_PN_Xcc, evtW_tag);

	// Check that the Higgs candidate PASSES the Xcc cut.
	// This keeps us in the top CR for VHcc.
	if (jets[idx_H].m_PN_Xcc > XccCut)
	{
          // For the top CR, we want to "invert" the nExtraJets cut.
	  // Thus, instead of having few, we want "many."
	  if (nExtraJet >= 2)
	  {
            // Check the MET & trigger cuts
	    if (passMET && trigger)
	    {
              // For purposes of bckg estimation, we will skip other fillings
	      // related to VV, PN, etc. We just want to be able to calculate
	      // the m_sd distribution for this inverted Xcc range.
              // We can later re-add those plots here...

	      // Check that we have the expected number of extra b-jets
	      // NOTE: This is the FINAL cut.
	      if (nBjet_extJets > nBjet_extJets_cut)
	      {

                // Fill in the values we want.
		h_VHcc_PN_med_qcdEnriched_topCR->Fill(H, V, VZtype, evtW_tag_btag_trig);
		h_VHcc_PN_med_qcdEnriched_topCR->FillJets(jet_VHcc, evtW_tag_btag_trig);
		
	      }//end-nBjet-extra-cut
	    }//end-MET-trigger-cuts
	  }//end-extraJet-cut (>= 2)
	}//end-H-Xcc-cut (PASSES)
      }//end-inverted-V-tag-cuts (FAIL V-tag)
      
    }//end-VqqHcc (V&H pT cuts)
    
  }//end-leptonVeto & nJet check
  
} //end Process



void VbbHcc_selector::Terminate(TList* mergedList, std::string outFileName) {
  
  /*TList* aList = new TList() ;
  TParameter<double>* a = new TParameter<double>("lep_eta",CUTS.Get<float>("lep_eta")) ;
  aList->Add(a) ;
  a = new TParameter<double>("lep_pt",CUTS.Get<float>("lep_pt")) ;
  aList->Add(a) ;
  
  TFile* f = new TFile(outFileName.c_str(), "UPDATE") ;
  aList->Write("VbbHcc_selectorCuts",TObject::kSingleKey) ;

  f->Close() ;
  */
}
