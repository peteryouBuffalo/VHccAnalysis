#define VbbHcc_selector_unc_cxx

#include <math.h>

#include "TList.h"
#include "TParameter.h"
#include "TLorentzVector.h"

#include "VbbHcc_selector_unc.h"
#include "Global.h"
#include "Obj.cxx"

///////////////////////////////////////
//using particle net tagger
///////////////////////////////////////
#if defined(DATA_2016) || defined(MC_2016)
const float XccCut = 0.9743;
const float XbbCut = 0.9735;
#endif

#if defined(DATA_2016PRE) || defined(MC_2016PRE)
const float XccCut = 0.9751;
const float XbbCut = 0.9737;
#endif

#if defined(DATA_2017) || defined(MC_2017)
const float  XccCut = 0.9765; 
const float  XbbCut = 0.9714;
#endif
#if defined(DATA_2018) || defined(MC_2018)
const float XccCut = 0.9777;
const float XbbCut = 0.9734;
#endif

VbbHcc_selector_unc::~VbbHcc_selector_unc() {
  if(h_VbbHcc) delete h_VbbHcc;
  if(h_VbbHcc_twojets) delete h_VbbHcc_twojets;
  if(h_VbbHcc_select1) delete h_VbbHcc_select1;
  if(h_VbbHcc_select2) delete h_VbbHcc_select2;
  if(h_VbbHcc_select3) delete h_VbbHcc_select3;
  if(h_VbbHcc_select4) delete h_VbbHcc_select4;
  if(h_VbbHcc_PN_med) delete h_VbbHcc_PN_med;
  if(h_VbbHcc_PN_med_xccWeight) delete h_VbbHcc_PN_med_xccWeight;
  if(h_VbbHcc_PN_med_qcdCR) delete h_VbbHcc_PN_med_qcdCR;
  if(h_VbbHcc_PN_med_topCR_pass) delete h_VbbHcc_PN_med_topCR_pass;
  if(h_VbbHcc_PN_med_topCR_fail) delete h_VbbHcc_PN_med_topCR_fail;
  if(h_VbbHcc_PN_med_VR) delete h_VbbHcc_PN_med_VR;
  if(h_VbbHcc_PN_med_qcdCR1) delete h_VbbHcc_PN_med_qcdCR1;
  if(h_VbbHcc_PN_med_topCR1_pass) delete h_VbbHcc_PN_med_topCR1_pass;
  if(h_VbbHcc_PN_med_topCR1_fail) delete h_VbbHcc_PN_med_topCR1_fail;
  if(h_VbbHcc_qcd) delete h_VbbHcc_qcd;
  if(h_VbbHcc_qcd_1) delete h_VbbHcc_qcd_1;
  if(h_VbbHcc_qcd_2) delete h_VbbHcc_qcd_2;
  if(h_VbbHcc_qcd_3) delete h_VbbHcc_qcd_3;
  if(h_Hcc) delete h_Hcc;
  if(h_eff_xbb_bb) delete h_eff_xbb_bb;
  if(h_eff_xbb_cc) delete h_eff_xbb_cc;
  if(h_eff_xbb_light) delete h_eff_xbb_light;
  if(h_eff_xcc_bb) delete h_eff_xcc_bb;
  if(h_eff_xcc_cc) delete h_eff_xcc_cc;
  if(h_eff_xcc_light) delete h_eff_xcc_light;
} 

void VbbHcc_selector_unc::SlaveBegin(Reader* r) {
  h_evt_all = new TH1D("Nevt_all_VbbHcc_boosted","",4,-1.5,2.5) ; //This stores events before any filter or cut, bin 1: total negative weight events, bin 2: total positive weight events, bin 3: total event weighted by genWeight (= bin2 - bin1, if genWeight are always -1,1
  h_evt = new TH1D("Nevt_VbbHcc_boosted","",4,-1.5,2.5) ; //bin 1: total negative weight events, bin 2: total positive weight events, bin 3: total event weighted by genWeight (= bin2 - bin1, if genWeight are always -1,1
  h_bStatus_noMother = new TH1D("bStatus_noMother","",100,0,100) ; 
  h_bStatus_hasMother = new TH1D("bStatus_hasMother","",100,0,100) ; 
  h_nb = new TH1D("nb","",20,0,20) ; 
  h_nB = new TH1D("nB","",20,0,20) ; 
  h_nbB = new TH1D("nbB","",20,0,20) ; 
  h_nb_1 = new TH1D("nb_1","",20,0,20) ; 
  h_nB_1 = new TH1D("nB_1","",20,0,20) ; 
  h_nbB_1 = new TH1D("nbB_1","",20,0,20) ; 

  h_pt_rho_n2b1 = new TH3D("pt_rho_n2b1","",70,300,1000,75,-6,-1.5,100,0,1);
  h_jet_mass = new TH1D("jet_mass","",2000,0,2000);

  h_cutFlow = new TH1D("CutFlow_VbbHcc_boosted","",4,0,4) ;
  h_cutFlow->GetXaxis()->SetBinLabel(1,"Total");
  h_cutFlow->GetXaxis()->SetBinLabel(2,"Lumi");
  h_cutFlow->GetXaxis()->SetBinLabel(3,"Two jets+MET");
  h_cutFlow->GetXaxis()->SetBinLabel(4,"BCvL");

  //cut flow to select a boosted jet targetting Hcc
  h_cutFlow_hcc = new TH1D("CutFlow_Hcc_boosted","",7,0,7) ;
  h_cutFlow_hcc->GetXaxis()->SetBinLabel(1,"Total");
  h_cutFlow_hcc->GetXaxis()->SetBinLabel(2,"Lumi");
  h_cutFlow_hcc->GetXaxis()->SetBinLabel(3,"Jet kin");
  h_cutFlow_hcc->GetXaxis()->SetBinLabel(4,"pT_miss");
  h_cutFlow_hcc->GetXaxis()->SetBinLabel(5,"Jet rho");
  h_cutFlow_hcc->GetXaxis()->SetBinLabel(6,"Jet n2b1");
  h_cutFlow_hcc->GetXaxis()->SetBinLabel(7,"Jet DDCvL");
  
  //cut flow to select a boosted jet targetting VbbHcc using CvL and BvL chosing max CvL first 
  h_cutFlow_VbbHcc_select1 = new TH1D("CutFlow_VbbHcc_boosted_select1","",8,0,8) ;
  h_cutFlow_VbbHcc_select1->GetXaxis()->SetBinLabel(1,"Total");
  h_cutFlow_VbbHcc_select1->GetXaxis()->SetBinLabel(2,"Lumi");
  h_cutFlow_VbbHcc_select1->GetXaxis()->SetBinLabel(3,"Jet kin");
  h_cutFlow_VbbHcc_select1->GetXaxis()->SetBinLabel(4,"pT_miss");
  h_cutFlow_VbbHcc_select1->GetXaxis()->SetBinLabel(5,"Jet rho");
  h_cutFlow_VbbHcc_select1->GetXaxis()->SetBinLabel(6,"Jet n2b1");
  h_cutFlow_VbbHcc_select1->GetXaxis()->SetBinLabel(7,"Jet CvL");
  h_cutFlow_VbbHcc_select1->GetXaxis()->SetBinLabel(8,"Jet BvL");

  //cut flow to select a boosted jet targetting Hcc
  h_cutFlow_VbbHcc_select2 = new TH1D("CutFlow_VbbHcc_boosted_select2","",8,0,8) ;
  h_cutFlow_VbbHcc_select2->GetXaxis()->SetBinLabel(1,"Total");
  h_cutFlow_VbbHcc_select2->GetXaxis()->SetBinLabel(2,"Lumi");
  h_cutFlow_VbbHcc_select2->GetXaxis()->SetBinLabel(3,"Jet kin");
  h_cutFlow_VbbHcc_select2->GetXaxis()->SetBinLabel(4,"pT_miss");
  h_cutFlow_VbbHcc_select2->GetXaxis()->SetBinLabel(5,"Jet rho");
  h_cutFlow_VbbHcc_select2->GetXaxis()->SetBinLabel(6,"Jet n2b1");
  h_cutFlow_VbbHcc_select2->GetXaxis()->SetBinLabel(7,"Jet DT_MD_ZHccvsQCD");
  h_cutFlow_VbbHcc_select2->GetXaxis()->SetBinLabel(8,"Jet DT_MD_ZbbvsQCD");
  //cut flow to select a boosted jet targetting VbbHcc using CvL and BvL, choosing max BvL first 
  h_cutFlow_VbbHcc_select3 = new TH1D("CutFlow_VbbHcc_boosted_select3","",8,0,8) ;
  h_cutFlow_VbbHcc_select3->GetXaxis()->SetBinLabel(1,"Total");
  h_cutFlow_VbbHcc_select3->GetXaxis()->SetBinLabel(2,"Lumi");
  h_cutFlow_VbbHcc_select3->GetXaxis()->SetBinLabel(3,"Jet kin");
  h_cutFlow_VbbHcc_select3->GetXaxis()->SetBinLabel(4,"pT_miss");
  h_cutFlow_VbbHcc_select3->GetXaxis()->SetBinLabel(5,"Jet rho");
  h_cutFlow_VbbHcc_select3->GetXaxis()->SetBinLabel(6,"Jet n2b1");
  h_cutFlow_VbbHcc_select3->GetXaxis()->SetBinLabel(7,"Jet BvL");
  h_cutFlow_VbbHcc_select3->GetXaxis()->SetBinLabel(8,"Jet CvL");
  
  h_cutFlow_VbbHcc_select4 = new TH1D("CutFlow_VbbHcc_boosted_select4","",8,0,8) ;
  h_cutFlow_VbbHcc_select4->GetXaxis()->SetBinLabel(1,"Total");
  h_cutFlow_VbbHcc_select4->GetXaxis()->SetBinLabel(2,"Lumi");
  h_cutFlow_VbbHcc_select4->GetXaxis()->SetBinLabel(3,"Jet kin");
  h_cutFlow_VbbHcc_select4->GetXaxis()->SetBinLabel(4,"pT_miss");
  h_cutFlow_VbbHcc_select4->GetXaxis()->SetBinLabel(5,"Jet rho");
  h_cutFlow_VbbHcc_select4->GetXaxis()->SetBinLabel(6,"Jet n2b1");
  h_cutFlow_VbbHcc_select4->GetXaxis()->SetBinLabel(7,"Jet BvL");
  h_cutFlow_VbbHcc_select4->GetXaxis()->SetBinLabel(8,"Jet CvL");
  h_cutFlow_VbbHcc_PN_med = new TH1D("CutFlow_VbbHcc_boosted_PN_med","",10,0,10) ;
  h_cutFlow_VbbHcc_PN_med->GetXaxis()->SetBinLabel(1,"Total");
  h_cutFlow_VbbHcc_PN_med->GetXaxis()->SetBinLabel(2,"Lumi");
  h_cutFlow_VbbHcc_PN_med->GetXaxis()->SetBinLabel(3,"Jet kin");
  h_cutFlow_VbbHcc_PN_med->GetXaxis()->SetBinLabel(4,"Jet rho");
  h_cutFlow_VbbHcc_PN_med->GetXaxis()->SetBinLabel(5,"Jet n2b1");
  h_cutFlow_VbbHcc_PN_med->GetXaxis()->SetBinLabel(6,"Jet Xbb");
  h_cutFlow_VbbHcc_PN_med->GetXaxis()->SetBinLabel(7,"Jet Xcc");
  h_cutFlow_VbbHcc_PN_med->GetXaxis()->SetBinLabel(8,"Extra jets");
  h_cutFlow_VbbHcc_PN_med->GetXaxis()->SetBinLabel(9,"pT_miss");
  h_cutFlow_VbbHcc_PN_med->GetXaxis()->SetBinLabel(10,"Trigger");

  h_VbbHcc_twojets = new VHBoostedPlots("VbbHcc_boosted_twojets");
  h_VbbHcc = new VHBoostedPlots("VbbHcc_boosted");
  h_Hcc = new HBoostedPlots("Hcc_boosted");
  h_VbbHcc_select1 = new VHBoostedPlots("VbbHcc_boosted_select1");
  h_VbbHcc_select2 = new VHBoostedPlots("VbbHcc_boosted_select2");
  h_VbbHcc_select3 = new VHBoostedPlots("VbbHcc_boosted_select3");
  h_VbbHcc_select4 = new VHBoostedPlots("VbbHcc_boosted_select4");
  h_VbbHcc_qcd= new VHBoostedPlots("VbbHcc_boosted_qcd");
  h_VbbHcc_qcd_1= new VHBoostedPlots("VbbHcc_boosted_qcd_1");
  h_VbbHcc_qcd_2= new VHBoostedPlots("VbbHcc_boosted_qcd_2");
  h_VbbHcc_qcd_3= new VHBoostedPlots("VbbHcc_boosted_qcd_3");
  h_VbbHcc_PN_med = new VHBoostedPlots("VbbHcc_boosted_PN_med");
  h_VbbHcc_PN_med_xccWeight = new VHBoostedPlots("VbbHcc_boosted_PN_med_xccWeight");
  h_VbbHcc_PN_med_qcdCR = new VHBoostedPlots("VbbHcc_boosted_PN_med_qcdCR");
  h_VbbHcc_PN_med_topCR_pass = new VHBoostedPlots("VbbHcc_boosted_PN_med_topCR_pass");
  h_VbbHcc_PN_med_topCR_fail = new VHBoostedPlots("VbbHcc_boosted_PN_med_topCR_fail");
  h_VbbHcc_PN_med_VR = new VHBoostedPlots("VbbHcc_boosted_PN_med_VR");
  h_VbbHcc_PN_med_qcdCR1 = new VHBoostedPlots("VbbHcc_boosted_PN_med_qcdCR1");
  h_VbbHcc_PN_med_topCR1_pass = new VHBoostedPlots("VbbHcc_boosted_PN_med_topCR1_pass");
  h_VbbHcc_PN_med_topCR1_fail = new VHBoostedPlots("VbbHcc_boosted_PN_med_topCR1_fail");
  
  //test H and Z mass from MC truth
  h_test_MZ = new TH1D("MZbb_MCtruth","",200,0,200);
  h_test_MH = new TH1D("MHcc_MCtruth","",200,0,200);
  h_NextraJet = new TH1D("NextraJet","",10,0,10);
  h_triggerCheck = new TH1D("TriggerCheck","",3,0,3);
  h_ljpt = new TH1D("h_ljpt","",3000,0,3000);
  h_ljpt_gen = new TH1D("h_ljpt_gen","",3000,0,3000);
  
  h_eff_xbb_bb = new BoostedJetEffPlots("xbb_bb");
  h_eff_xbb_cc = new BoostedJetEffPlots("xbb_cc");
  h_eff_xbb_light = new BoostedJetEffPlots("xbb_light");
  h_eff_xcc_bb = new BoostedJetEffPlots("xcc_bb");
  h_eff_xcc_cc = new BoostedJetEffPlots("xcc_cc");
  h_eff_xcc_light = new BoostedJetEffPlots("xcc_light");

  ////////////////////////////////////
  //Add histograms to fOutput so they can be saved in Processor::SlaveTerminate
  /////////////////////////////////////
  std::vector<TH1*> tmp = h_VbbHcc->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VbbHcc_twojets->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VbbHcc_select1->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VbbHcc_select2->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VbbHcc_select3->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VbbHcc_select4->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VbbHcc_qcd->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VbbHcc_qcd_1->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VbbHcc_qcd_2->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VbbHcc_qcd_3->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VbbHcc_PN_med->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VbbHcc_PN_med_xccWeight->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VbbHcc_PN_med_qcdCR->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VbbHcc_PN_med_topCR_pass->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VbbHcc_PN_med_topCR_fail->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VbbHcc_PN_med_VR->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VbbHcc_PN_med_qcdCR1->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VbbHcc_PN_med_topCR1_pass->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_VbbHcc_PN_med_topCR1_fail->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_Hcc->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_eff_xbb_bb->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_eff_xbb_cc->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_eff_xbb_light->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_eff_xcc_bb->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_eff_xcc_cc->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_eff_xcc_light->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);

  r->GetOutputList()->Add(h_evt_all) ;
  r->GetOutputList()->Add(h_evt) ;
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
  r->GetOutputList()->Add(h_cutFlow) ;
  r->GetOutputList()->Add(h_cutFlow_hcc) ;
  r->GetOutputList()->Add(h_cutFlow_VbbHcc_select1) ;
  r->GetOutputList()->Add(h_cutFlow_VbbHcc_select2) ;
  r->GetOutputList()->Add(h_cutFlow_VbbHcc_select3) ;
  r->GetOutputList()->Add(h_cutFlow_VbbHcc_select4) ;
  r->GetOutputList()->Add(h_cutFlow_VbbHcc_PN_med) ;
  r->GetOutputList()->Add(h_test_MZ);
  r->GetOutputList()->Add(h_test_MH);
  r->GetOutputList()->Add(h_NextraJet);
  r->GetOutputList()->Add(h_triggerCheck);
  r->GetOutputList()->Add(h_ljpt);
  r->GetOutputList()->Add(h_ljpt_gen);
}

#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018) 
std::vector<std::vector<int> > VbbHcc_selector_unc::DauIdxs_ZH(Reader* r) {
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
#endif

bool VbbHcc_selector_unc::sortbysecdesc(const std::pair<int,float> &a, const std::pair<int,float> &b)
{
  return a.second>b.second;
}

bool VbbHcc_selector_unc::sortbysecdesc1(const std::pair<std::vector<int>,float> &a, const std::pair<std::vector<int>,float> &b)
{
  return a.second>b.second;
}

int VbbHcc_selector_unc::NextraJet(std::vector<JetObjBoosted>& fatJets, std::vector<JetObj>& jets) {
  int nJet(0);
  for(auto jet:jets) {
    float minDr = 10;
    for(auto fatJet:fatJets) {
      float dR = fatJet.m_lvec.DeltaR(jet.m_lvec);
      if (dR <= minDr) minDr = dR;
    }
    if (minDr >= 0.8) nJet+=1;
  }
  return nJet;
}


void VbbHcc_selector_unc::Process(Reader* r) {

  //Weights
  float genWeight = 1.;
  float puSF = 1.;
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
#endif

#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017)
  l1preW = *(r->L1PreFiringWeight_Nom);
  //std::cout << "\nPrefiring: " << l1preW;
  if (m_l1prefiringType == "l1prefiringu") l1preW = *(r->L1PreFiringWeight_Up);
  if (m_l1prefiringType == "l1prefiringd") l1preW = *(r->L1PreFiringWeight_Dn);
#endif

  float evtW = 1. ;
  if (!m_isData) evtW *= genWeight*puSF*l1preW;

  h_cutFlow->Fill(0.5,evtW);
  h_cutFlow_hcc->Fill(0.5,evtW);
  h_cutFlow_VbbHcc_select1->Fill(0.5,evtW);
  h_cutFlow_VbbHcc_select2->Fill(0.5,evtW);
  h_cutFlow_VbbHcc_select3->Fill(0.5,evtW);
  h_cutFlow_VbbHcc_select4->Fill(0.5,evtW);
  h_cutFlow_VbbHcc_PN_med->Fill(0.5,evtW);

#if defined(DATA_2016PRE) || defined(DATA_2016) || defined(DATA_2017) || defined(DATA_2018)
  h_evt->Fill(-1) ;
  if (!m_lumiFilter.Pass(*(r->run),*(r->luminosityBlock))) return;
  h_evt->Fill(1) ;
#endif
  
  h_cutFlow->Fill(1.5,evtW);
  h_cutFlow_hcc->Fill(1.5,evtW);
  h_cutFlow_VbbHcc_select1->Fill(1.5,evtW);
  h_cutFlow_VbbHcc_select2->Fill(1.5,evtW);
  h_cutFlow_VbbHcc_select3->Fill(1.5,evtW);
  h_cutFlow_VbbHcc_select4->Fill(1.5,evtW);
  h_cutFlow_VbbHcc_PN_med->Fill(1.5,evtW);

  //std::cout << "\n Data type: " << m_isData ;
  //std::cout << "\n Number of jets: " << *(r->nJet) ;
  //if (*(r->nJet)>0) std::cout << "\n First jet: " << (r->Jet_eta)[0] ;
  
  //=============Get objects============= 
  std::vector<LepObj> eles_jetOverlap ;
  for (unsigned int i = 0 ; i < *(r->nElectron) ; ++i) {
    
    float etaSC = (r->Electron_eta)[i]+(r->Electron_deltaEtaSC[i]) ;
    LepObj ele((r->Electron_pt)[i],(r->Electron_eta)[i],etaSC,(r->Electron_phi)[i],(r->Electron_mass)[i],i,(r->Electron_charge)[i],0) ;

    float dz = (r->Electron_dz)[i]; 
    float dxy = (r->Electron_dxy)[i]; //dxy=d0
    if ((fabs(etaSC) < 1.4442) && (fabs(dz) > CUTS.Get<float>("ele_dz_b") || fabs(dxy) > CUTS.Get<float>("ele_d0_b"))) continue;
    if ((fabs(etaSC) >= 1.4442) && (fabs(dz) > CUTS.Get<float>("ele_dz_e") || fabs(dxy) > CUTS.Get<float>("ele_d0_e"))) continue;
    int eleId = r->Electron_cutBased[i] ; //cut-based ID Fall17 V2 (0:fail, 1:veto, 2:loose, 3:medium, 4:tight)
    //electron for electron jet overlap removal
    if (ele.m_lvec.Pt() > CUTS.Get<float>("lep_jetOverlap_pt") && fabs(ele.m_lvec.Eta()) < CUTS.Get<float>("lep_jetOverlap_eta")) {
      if (eleId >= 4) { //tightId for jet removal
        eles_jetOverlap.push_back(ele) ;
      }
    }
  }

  std::vector<LepObj> muons_jetOverlap ;
  for (unsigned int i = 0 ; i < *(r->nMuon) ; ++i) {
    LepObj muon((r->Muon_pt)[i],(r->Muon_eta)[i],-1,(r->Muon_phi)[i],(r->Muon_mass)[i],i,(r->Muon_charge)[i],(r->Muon_pfRelIso04_all)[i]) ;
    //float sf_rc = MuonRcSF(r, muon, 13) ;
    float sf_rc = 1;
    muon.m_lvec.SetPtEtaPhiM(muon.m_lvec.Pt()*sf_rc,muon.m_lvec.Eta(),muon.m_lvec.Phi(),muon.m_lvec.M()) ;
    //muon for muon jet overlap removal
    if (muon.m_lvec.Pt() > CUTS.Get<float>("lep_jetOverlap_pt") && fabs(muon.m_lvec.Eta()) < CUTS.Get<float>("lep_jetOverlap_eta")) {
      if (r-> Muon_mediumId[i] > 0 && r->Muon_pfRelIso04_all[i] < CUTS.Get<float>("muon_iso")) {
        muons_jetOverlap.push_back(muon) ;
      }
    }
  }
  
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
    //std::cout << " " << jetPt;
    
    JetObjBoosted jet(jetPt,(r->FatJet_eta)[i],(r->FatJet_phi)[i],(r->FatJet_mass)[i],jetFlav,
        (r->FatJet_btagDDCvB)[i],(r->FatJet_btagDDCvL)[i], (r->FatJet_btagDDBvL)[i], 
        (r->FatJet_deepTagMD_ZHccvsQCD)[i],(r->FatJet_deepTagMD_ZbbvsQCD)[i],
        Xcc,Xbb,(r->FatJet_particleNetMD_QCD)[i],
        (r->FatJet_particleNet_TvsQCD)[i],
        (r->FatJet_particleNet_WvsQCD)[i],
        (r->FatJet_particleNet_ZvsQCD)[i],
        (r->FatJet_n2b1)[i], -1) ;
    if(jet.IsLepton(eles_jetOverlap) || jet.IsLepton(muons_jetOverlap)) continue;
    if((r->FatJet_pt)[i] > CUTS.Get<float>("jet_pt_ak08") && fabs((r->FatJet_eta)[i]) < CUTS.Get<float>("jet_eta_ak08")) jets.push_back(jet) ;
  }

  //Fill fat jet pt, rho, n2b1 to identify c_26 cut
  for(unsigned iJ = 0; iJ < jets.size();++iJ) {
    h_jet_mass->Fill(jets[iJ].m_lvec.M(),evtW);
    if(jets[iJ].m_lvec.M()>40) h_pt_rho_n2b1->Fill(jets[iJ].m_lvec.Pt(),jets[iJ].m_rho,jets[iJ].m_n2b1,evtW);
  }

  //ak04 jets
  std::vector<JetObj> ak4Jets;
  for (unsigned int i = 0 ; i < *(r->nJet) ; ++i) {
    
    int jetFlav = -1 ;
#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018)
    jetFlav = (r->Jet_hadronFlavour)[i];
#endif 
    
    float jetPt = (r->Jet_pt)[i];

    JetObj jet(jetPt,(r->Jet_eta)[i],(r->Jet_phi)[i],(r->Jet_mass)[i],jetFlav,(r->Jet_btagDeepB)[i],(r->Jet_puIdDisc)[i]) ;

    if (jet.m_lvec.Pt() < CUTS.Get<float>("jet_pt") || fabs(jet.m_lvec.Eta()) > CUTS.Get<float>("jet_eta")) continue ;
    
    if (jet.IsLepton(eles_jetOverlap)) continue ;
    if (jet.IsLepton(muons_jetOverlap)) continue ;
    
    if (r->Jet_jetId[i] < 2 ) continue ;
    
    if (jet.m_lvec.Pt()<50 && jet.m_lvec.Pt()>30 && (r->Jet_puIdDisc)[i]<0.61 ) continue;
	  
    ak4Jets.push_back(jet) ;
  }

  if(ak4Jets.size()>0) h_ljpt->Fill(ak4Jets[0].m_lvec.Pt(),genWeight);
 
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
    h_cutFlow_hcc->Fill(2.5,evtW);
    h_cutFlow_VbbHcc_select1->Fill(2.5,evtW);
    h_cutFlow_VbbHcc_select2->Fill(2.5,evtW);
    h_cutFlow_VbbHcc_select3->Fill(2.5,evtW);
    h_cutFlow_VbbHcc_select4->Fill(2.5,evtW);
    h_cutFlow_VbbHcc_PN_med->Fill(2.5,evtW);
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
  if (*(r->MET_pt)<140) passMET=true;
   
  //jet n2b1 cut 
  std::vector<unsigned> idx_tmps_2;
  //consider jets passing kine and rho
  if (idx_tmps_1.size()>0) {
    //FIXME: fix other cutflow
    h_cutFlow_hcc->Fill(4.5,evtW);
    h_cutFlow_VbbHcc_select1->Fill(4.5,evtW);
    h_cutFlow_VbbHcc_select2->Fill(4.5,evtW);
    h_cutFlow_VbbHcc_select3->Fill(4.5,evtW);
    h_cutFlow_VbbHcc_select4->Fill(4.5,evtW);
    h_cutFlow_VbbHcc_PN_med->Fill(3.5,evtW);
    for (auto i:idx_tmps_1) {
      //std::cout << "\n" << jets[i].m_lvec.Pt() << " " << jets[i].m_rho << " " << Getn2b1Cut(jets[i].m_rho,jets[i].m_lvec.Pt());
      if(jets[i].m_n2b1 < Getn2b1Cut(jets[i].m_rho,jets[i].m_lvec.Pt())) idx_tmps_2.push_back(i);
      //if(true) idx_tmps_2.push_back(i);
    }
  }

  //find jets with maximum BvL among jet passing kine, met, rho, and n2b1
  int idx_bb = -1;
  float max_Xbb = -10;
  bool filled = false;
  for(auto i:idx_tmps_2) {
    if(!filled) {
      h_cutFlow_VbbHcc_PN_med->Fill(4.5,evtW);
      filled=true;
    }
    if(jets[i].m_PN_Xbb>max_Xbb) {
      max_Xbb = jets[i].m_PN_Xbb;
      idx_bb=i;
    }
  }
  //find second jets with maximum CvL
  float max_Xcc = -10;
  int idx_cc = -1;
  for(auto i:idx_tmps_2) {
    if (i!=idx_bb) {
      if(jets[i].m_PN_Xcc > max_Xcc) {
        max_Xcc = jets[i].m_PN_Xcc;
        idx_cc = i;
      }
    }
  }
  
  //pioritize cc
  //find jets with maximum CvL among jet passing kine, met, rho, and n2b1
  int idx_cc_1 = -1;
  float max_Xcc_1 = -10;
  for(auto i:idx_tmps_2) {
    if(jets[i].m_PN_Xcc>max_Xcc_1) {
      max_Xcc_1 = jets[i].m_PN_Xcc;
      idx_cc_1=i;
    }
  }
  //find second jets with maximum CvL
  float max_Xbb_1 = -10;
  int idx_bb_1 = -1;
  for(auto i:idx_tmps_2) {
    if (i!=idx_cc_1) {
      if(jets[i].m_PN_Xbb > max_Xbb_1) {
        max_Xbb_1 = jets[i].m_PN_Xbb;
        idx_bb_1 = i;
      }
    }
  }

  ////////////////////////!!!!!!!!!!!!!!!!!!!!!!CHECK
  //test pioritize cc
  if(false) {
    idx_cc = idx_cc_1;
    idx_bb = idx_bb_1;
  }

  //number of extra jets
  int nExtraJet = -1;
  if(idx_bb>=0 && idx_cc>=0) {
    std::vector<JetObjBoosted> tmp{jets[idx_bb],jets[idx_cc]};
    nExtraJet = NextraJet(tmp, ak4Jets); 
  }

  //tagging scale
  float tagW(1.0);
  if(idx_bb >= 0 && idx_cc >=0) {
    auto jet_bb = std::make_pair(jets[idx_bb].m_lvec.Pt(),false);
    if(jets[idx_bb].m_PN_Xbb >= XbbCut) jet_bb.second = true;
    auto jet_cc = std::make_pair(jets[idx_cc].m_lvec.Pt(),false);
    if(jets[idx_cc].m_PN_Xcc >= XccCut) jet_cc.second = true;
    tagW = CalBtagWeightBoosted(jet_bb,jet_cc,m_hfTagUncType);
  }

  float evtW_tag = evtW*tagW;
  //float evtW_tag = evtW;

  if(idx_bb>=0) {
    float trigSF=1;
    if(idx_cc>=0) {
      h_VbbHcc_PN_med->h_bbTagDis_beforeCut->Fill(jets[idx_bb].m_PN_Xbb,evtW_tag);
      h_VbbHcc_PN_med->h_ccTagDis_beforeCut->Fill(jets[idx_cc].m_PN_Xcc,evtW_tag);
      float jetPtMax = std::max(jets[idx_bb].m_lvec.Pt(),jets[idx_cc].m_lvec.Pt());
#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018) 
      trigSF = GetTrigSF(jetPtMax);
#endif
    }
    if (jets[idx_bb].m_PN_Xbb>=XbbCut) {
      h_cutFlow_VbbHcc_PN_med->Fill(5.5,evtW_tag);
      if(idx_cc>=0) {
        std::vector<JetObjBoosted> jet_VbbZcc{jets[idx_bb], jets[idx_cc]};
        ZObj Z(jets[idx_bb]);
        HObj H(jets[idx_cc]);
        if (nExtraJet < 2 && passMET && trigger) {
          float xcc_weight = 1.0 ;
#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018) 
          xcc_weight = Get_xccbb_weight(jets,idx_bb,XbbCut,XccCut,"xcc");
#endif
          h_VbbHcc_PN_med_xccWeight->Fill(H,Z,evtW*xcc_weight*trigSF);
          h_VbbHcc_PN_med_xccWeight->FillJets(jet_VbbZcc,evtW*xcc_weight*trigSF);
        }
        if (jets[idx_cc].m_PN_Xcc>=XccCut) {
          h_cutFlow_VbbHcc_PN_med->Fill(6.5,evtW_tag);
          h_NextraJet->Fill(nExtraJet,evtW_tag);
          ///////////////////////////////////
          //>>>>>>>>>>>>SR>>>>>>>>>>>>>>>>>>>
          ///////////////////////////////////
          h_VbbHcc_PN_med->h_NextraJet_beforeCut->Fill(nExtraJet,evtW_tag); 
          h_VbbHcc_PN_med->h_MET_beforeCut_1->Fill(*(r->MET_pt),evtW_tag);
          //top quarks suppression
          if (nExtraJet < 2) {
            h_cutFlow_VbbHcc_PN_med->Fill(7.5,evtW_tag);
            //now fill histograms
            h_VbbHcc_PN_med->h_MET_beforeCut->Fill(*(r->MET_pt),evtW_tag);
            if (passMET) {
              h_cutFlow_VbbHcc_PN_med->Fill(8.5,evtW_tag);
              if (trigger) {
                h_nb_1->Fill(nb);
                h_nB_1->Fill(nB);
                h_nbB_1->Fill(nb+nB);
                h_cutFlow_VbbHcc_PN_med->Fill(9.5,evtW_tag*trigSF);
                h_VbbHcc_PN_med->Fill(H,Z,evtW_tag*trigSF);
                h_VbbHcc_PN_med->FillJets(jet_VbbZcc,evtW_tag*trigSF);
                h_VbbHcc_PN_med->h_bbTagDis->Fill(jets[idx_bb].m_PN_Xbb,evtW_tag*trigSF);
                h_VbbHcc_PN_med->h_ccTagDis->Fill(jets[idx_cc].m_PN_Xcc,evtW_tag*trigSF);
                h_VbbHcc_PN_med->h_MET->Fill(*(r->MET_pt),evtW_tag*trigSF);
              }
            }
          }
          //top CR
          else { //nExtraJet >= 2
            if (passMET && trigger) {
              h_VbbHcc_PN_med_topCR_pass->h_ccPN_TopvsQCD->Fill(jets[idx_cc].m_PN_TopvsQCD,evtW_tag*trigSF);
              h_VbbHcc_PN_med_topCR_pass->h_bbPN_TopvsQCD->Fill(jets[idx_bb].m_PN_TopvsQCD,evtW_tag*trigSF);
              if(jets[idx_cc].m_PN_TopvsQCD > 0.2 && jets[idx_bb].m_PN_TopvsQCD > 0.2) {
                h_VbbHcc_PN_med_topCR_pass->Fill(H,Z,evtW_tag*trigSF);
                h_VbbHcc_PN_med_topCR_pass->FillJets(jet_VbbZcc,evtW_tag*trigSF);
                h_VbbHcc_PN_med_topCR_pass->h_bbTagDis->Fill(jets[idx_bb].m_PN_Xbb,evtW_tag*trigSF);
                h_VbbHcc_PN_med_topCR_pass->h_ccTagDis->Fill(jets[idx_cc].m_PN_Xcc,evtW_tag*trigSF);
              }
            }
          }
        }
      }
    }
  }

  //validation and CR with PN
  if(idx_bb>=0 && idx_cc>=0) {
    //invert bb or cc
    ZObj Z(jets[idx_bb]);
    HObj H(jets[idx_cc]);
    std::vector<JetObjBoosted> jet_VbbZcc{jets[idx_bb], jets[idx_cc]};
    float trigSF = 1;
#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018) 
    float jetPtMax = std::max(jets[idx_bb].m_lvec.Pt(),jets[idx_cc].m_lvec.Pt());
    trigSF = GetTrigSF(jetPtMax);
#endif

    //CR
    if((jets[idx_bb].m_PN_Xbb>=0.9 && jets[idx_bb].m_PN_Xbb<XbbCut && jets[idx_cc].m_PN_Xcc>=XccCut) ||
        (jets[idx_cc].m_PN_Xcc>=0.9 && jets[idx_cc].m_PN_Xcc<XccCut && jets[idx_bb].m_PN_Xbb>=XbbCut)) {
      if(passMET && nExtraJet < 2 && trigger) {
        h_VbbHcc_PN_med_qcdCR->Fill(H,Z,evtW_tag*trigSF);
        h_VbbHcc_PN_med_qcdCR->FillJets(jet_VbbZcc,evtW_tag*trigSF);
        h_VbbHcc_PN_med_qcdCR->h_bbTagDis->Fill(jets[idx_bb].m_PN_Xbb,evtW_tag*trigSF);
        h_VbbHcc_PN_med_qcdCR->h_ccTagDis->Fill(jets[idx_cc].m_PN_Xcc,evtW_tag*trigSF);
      }
      if(passMET && trigger && nExtraJet >= 2) {
        if(jets[idx_cc].m_PN_TopvsQCD > 0.2 && jets[idx_bb].m_PN_TopvsQCD > 0.2) {
          h_VbbHcc_PN_med_topCR_fail->Fill(H,Z,evtW_tag*trigSF);
          h_VbbHcc_PN_med_topCR_fail->FillJets(jet_VbbZcc,evtW_tag*trigSF);
          h_VbbHcc_PN_med_topCR_fail->h_bbTagDis->Fill(jets[idx_bb].m_PN_Xbb,evtW_tag*trigSF);
          h_VbbHcc_PN_med_topCR_fail->h_ccTagDis->Fill(jets[idx_cc].m_PN_Xcc,evtW_tag*trigSF);
        }
      }
    }
    //VR
    if((jets[idx_bb].m_PN_Xbb>=0.85 && jets[idx_bb].m_PN_Xbb<0.9) &&
        (jets[idx_cc].m_PN_Xcc>=0.85 && jets[idx_cc].m_PN_Xcc<0.9)) {
      if(passMET && nExtraJet < 2 && trigger) {
        h_VbbHcc_PN_med_VR->Fill(H,Z,evtW_tag*trigSF);
        h_VbbHcc_PN_med_VR->FillJets(jet_VbbZcc,evtW_tag*trigSF);
        h_VbbHcc_PN_med_VR->h_bbTagDis->Fill(jets[idx_bb].m_PN_Xbb,evtW_tag*trigSF);
        h_VbbHcc_PN_med_VR->h_ccTagDis->Fill(jets[idx_cc].m_PN_Xcc,evtW_tag*trigSF);
      }
      //top in VR
      if(passMET && trigger && nExtraJet >= 2) {
        if(jets[idx_cc].m_PN_TopvsQCD > 0.2 && jets[idx_bb].m_PN_TopvsQCD > 0.2) {
          h_VbbHcc_PN_med_topCR1_pass->Fill(H,Z,evtW_tag*trigSF);
          h_VbbHcc_PN_med_topCR1_pass->FillJets(jet_VbbZcc,evtW_tag*trigSF);
          h_VbbHcc_PN_med_topCR1_pass->h_bbTagDis->Fill(jets[idx_bb].m_PN_Xbb,evtW_tag*trigSF);
          h_VbbHcc_PN_med_topCR1_pass->h_ccTagDis->Fill(jets[idx_cc].m_PN_Xcc,evtW_tag*trigSF);
        }
      }
    }
    //CR1 for validation region
    if((jets[idx_bb].m_PN_Xbb>=0.8 && jets[idx_bb].m_PN_Xbb<0.85 &&
          jets[idx_cc].m_PN_Xcc>=0.85 && jets[idx_cc].m_PN_Xcc<0.9) ||
        (jets[idx_cc].m_PN_Xcc>=0.8 && jets[idx_cc].m_PN_Xcc<0.85 &&
          jets[idx_bb].m_PN_Xbb>=0.85 && jets[idx_bb].m_PN_Xbb<0.9)) {
      if(passMET && nExtraJet < 2 && trigger) {
        h_VbbHcc_PN_med_qcdCR1->Fill(H,Z,evtW_tag*trigSF);
        h_VbbHcc_PN_med_qcdCR1->FillJets(jet_VbbZcc,evtW_tag*trigSF);
        h_VbbHcc_PN_med_qcdCR1->h_bbTagDis->Fill(jets[idx_bb].m_PN_Xbb,evtW_tag*trigSF);
        h_VbbHcc_PN_med_qcdCR1->h_ccTagDis->Fill(jets[idx_cc].m_PN_Xcc,evtW_tag*trigSF);
      }
      //top CR1
      if(passMET && trigger && nExtraJet >= 2) {
        if(jets[idx_cc].m_PN_TopvsQCD > 0.2 && jets[idx_bb].m_PN_TopvsQCD > 0.2) {
          h_VbbHcc_PN_med_topCR1_fail->Fill(H,Z,evtW_tag*trigSF);
          h_VbbHcc_PN_med_topCR1_fail->FillJets(jet_VbbZcc,evtW_tag*trigSF);
          h_VbbHcc_PN_med_topCR1_fail->h_bbTagDis->Fill(jets[idx_bb].m_PN_Xbb,evtW_tag*trigSF);
          h_VbbHcc_PN_med_topCR1_fail->h_ccTagDis->Fill(jets[idx_cc].m_PN_Xcc,evtW_tag*trigSF);
        }
      }
    }
  }

  ///////////////////////////
  //efficiency of b,c,ljet
  ///////////////////////////
  for(auto jet:jets) {
    bool doPass = jet.m_PN_Xbb > XbbCut ;
    switch (jet.m_flav) {
      case 4:
        h_eff_xbb_cc->Fill(jet, doPass, evtW);
        break;
      case 5:
        h_eff_xbb_bb->Fill(jet, doPass, evtW);
        break;
      default:
        h_eff_xbb_light->Fill(jet, doPass, evtW);
    }
    doPass = jet.m_PN_Xcc > XccCut ;
    switch (jet.m_flav) {
      case 4:
        h_eff_xcc_cc->Fill(jet, doPass, evtW);
        break;
      case 5:
        h_eff_xcc_bb->Fill(jet, doPass, evtW);
        break;
      default:
        h_eff_xcc_light->Fill(jet, doPass, evtW);
    }
  }

  
} //end Process



void VbbHcc_selector_unc::Terminate(TList* mergedList, std::string outFileName) {
  
  /*TList* aList = new TList() ;
  TParameter<double>* a = new TParameter<double>("lep_eta",CUTS.Get<float>("lep_eta")) ;
  aList->Add(a) ;
  a = new TParameter<double>("lep_pt",CUTS.Get<float>("lep_pt")) ;
  aList->Add(a) ;
  
  TFile* f = new TFile(outFileName.c_str(), "UPDATE") ;
  aList->Write("VbbHcc_selector_uncCuts",TObject::kSingleKey) ;

  f->Close() ;
  */
}
