#define VbbHcc_triggerSel_cxx

#include <math.h>
#include "TList.h"
#include "TParameter.h"
#include "TLorentzVector.h"

#include "VbbHcc_triggerSel.h"
#include "Global.h"
#include "Obj.cxx"

VbbHcc_triggerSel::~VbbHcc_triggerSel() {
} 

void VbbHcc_triggerSel::SlaveBegin(Reader* r) {
  h_evt_all = new TH1D("Nevt_all_VbbHcc_boosted_1","",4,-1.5,2.5) ; //This stores events before any filter or cut, bin 1: total negative weight events, bin 2: total positive weight events, bin 3: total event weighted by genWeight (= bin2 - bin1, if genWeight are always -1,1
  h_evt = new TH1D("Nevt_VbbHcc_boosted_1","",4,-1.5,2.5) ; //bin 1: total negative weight events, bin 2: total positive weight events, bin 3: total event weighted by genWeight (= bin2 - bin1, if genWeight are always -1,1
  
  h_pt_msd_xbb_pass["HLT_Soup"] = new TH3D("pt_msd_xbb_pass_HLT_Soup","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xbb_pass["HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p20"] = new TH3D("pt_msd_xbb_pass_HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p20","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xbb_pass["HLT_AK8PFHT700_TrimR0p1PT0p03Mass50"] = new TH3D("pt_msd_xbb_pass_HLT_AK8PFHT700_TrimR0p1PT0p03Mass50","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xbb_pass["HLT_AK8PFJet360_TrimMass30"] = new TH3D("pt_msd_xbb_pass_HLT_AK8PFJet360_TrimMass30","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xbb_pass["HLT_PFJet450"] = new TH3D("pt_msd_xbb_pass_HLT_PFJet450","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xbb_pass["HLT_PFHT800"] = new TH3D("pt_msd_xbb_pass_HLT_PFHT800","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xbb_pass["HLT_PFHT900"] = new TH3D("pt_msd_xbb_pass_HLT_PFHT900","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xbb_pass["HLT_AK8PFJet450"] = new TH3D("pt_msd_xbb_pass_HLT_AK8PFJet450","",100,0,1000,30,0,300,10,0,1);
  
  h_pt_msd_xbb_pass["HLT_AK8PFHT800_TrimMass50_17"] = new TH3D("pt_msd_xbb_pass_HLT_AK8PFHT800_TrimMass50_17","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xbb_pass["HLT_AK8PFJet330_PFAK8BTagCSV_p17"] = new TH3D("pt_msd_xbb_pass_HLT_AK8PFJet330_PFAK8BTagCSV_p17","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xbb_pass["HLT_AK8PFJet400_TrimMass30_17"] = new TH3D("pt_msd_xbb_pass_HLT_AK8PFJet400_TrimMass30_17","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xbb_pass["HLT_AK8PFJet500_17"] = new TH3D("pt_msd_xbb_pass_HLT_AK8PFJet500_17","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xbb_pass["HLT_PFJet500_17"] = new TH3D("pt_msd_xbb_pass_HLT_PFJet500_17","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xbb_pass["HLT_PFHT1050_17"] = new TH3D("pt_msd_xbb_pass_HLT_PFHT1050_17","",100,0,1000,30,0,300,10,0,1);

  h_pt_msd_xbb_pass["HLT_AK8PFHT800_TrimMass50_18"] = new TH3D("pt_msd_xbb_pass_HLT_AK8PFHT800_TrimMass50_18","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xbb_pass["HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_np4"] = new TH3D("pt_msd_xbb_pass_HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_np4","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xbb_pass["HLT_AK8PFJet400_TrimMass30_18"] = new TH3D("pt_msd_xbb_pass_HLT_AK8PFJet400_TrimMass30_18","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xbb_pass["HLT_AK8PFJet500_18"] = new TH3D("pt_msd_xbb_pass_HLT_AK8PFJet500_18","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xbb_pass["HLT_PFJet500_18"] = new TH3D("pt_msd_xbb_pass_HLT_PFJet500_18","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xbb_pass["HLT_PFHT1050_18"] = new TH3D("pt_msd_xbb_pass_HLT_PFHT1050_18","",100,0,1000,30,0,300,10,0,1);
  
  /////////////////////////////////////
  //
  h_pt_msd_xcc_pass["HLT_Soup"] = new TH3D("pt_msd_xcc_pass_HLT_Soup","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xcc_pass["HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p20"] = new TH3D("pt_msd_xcc_pass_HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p20","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xcc_pass["HLT_AK8PFHT700_TrimR0p1PT0p03Mass50"] = new TH3D("pt_msd_xcc_pass_HLT_AK8PFHT700_TrimR0p1PT0p03Mass50","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xcc_pass["HLT_AK8PFJet360_TrimMass30"] = new TH3D("pt_msd_xcc_pass_HLT_AK8PFJet360_TrimMass30","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xcc_pass["HLT_PFJet450"] = new TH3D("pt_msd_xcc_pass_HLT_PFJet450","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xcc_pass["HLT_PFHT800"] = new TH3D("pt_msd_xcc_pass_HLT_PFHT800","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xcc_pass["HLT_PFHT900"] = new TH3D("pt_msd_xcc_pass_HLT_PFHT900","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xcc_pass["HLT_AK8PFJet450"] = new TH3D("pt_msd_xcc_pass_HLT_AK8PFJet450","",100,0,1000,30,0,300,10,0,1);
  
  h_pt_msd_xcc_pass["HLT_AK8PFHT800_TrimMass50_17"] = new TH3D("pt_msd_xcc_pass_HLT_AK8PFHT800_TrimMass50_17","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xcc_pass["HLT_AK8PFJet330_PFAK8BTagCSV_p17"] = new TH3D("pt_msd_xcc_pass_HLT_AK8PFJet330_PFAK8BTagCSV_p17","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xcc_pass["HLT_AK8PFJet400_TrimMass30_17"] = new TH3D("pt_msd_xcc_pass_HLT_AK8PFJet400_TrimMass30_17","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xcc_pass["HLT_AK8PFJet500_17"] = new TH3D("pt_msd_xcc_pass_HLT_AK8PFJet500_17","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xcc_pass["HLT_PFJet500_17"] = new TH3D("pt_msd_xcc_pass_HLT_PFJet500_17","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xcc_pass["HLT_PFHT1050_17"] = new TH3D("pt_msd_xcc_pass_HLT_PFHT1050_17","",100,0,1000,30,0,300,10,0,1);

  h_pt_msd_xcc_pass["HLT_AK8PFHT800_TrimMass50_18"] = new TH3D("pt_msd_xcc_pass_HLT_AK8PFHT800_TrimMass50_18","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xcc_pass["HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_np4"] = new TH3D("pt_msd_xcc_pass_HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_np4","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xcc_pass["HLT_AK8PFJet400_TrimMass30_18"] = new TH3D("pt_msd_xcc_pass_HLT_AK8PFJet400_TrimMass30_18","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xcc_pass["HLT_AK8PFJet500_18"] = new TH3D("pt_msd_xcc_pass_HLT_AK8PFJet500_18","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xcc_pass["HLT_PFJet500_18"] = new TH3D("pt_msd_xcc_pass_HLT_PFJet500_18","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xcc_pass["HLT_PFHT1050_18"] = new TH3D("pt_msd_xcc_pass_HLT_PFHT1050_18","",100,0,1000,30,0,300,10,0,1);
  //////////////////////////////////////
  //
  h_pt1_pt2_ht_pass["HLT_Soup"] = new TH3D("pt1_pt2_ht_pass_HLT_Soup","",100,0,1000,100,0,1000,100,0,5000);
  h_pt1_pt2_ht_pass["HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p20"] = new TH3D("pt1_pt2_ht_pass_HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p20","",100,0,1000,100,0,1000,100,0,5000);
  h_pt1_pt2_ht_pass["HLT_AK8PFHT700_TrimR0p1PT0p03Mass50"] = new TH3D("pt1_pt2_ht_pass_HLT_AK8PFHT700_TrimR0p1PT0p03Mass50","",100,0,1000,100,0,1000,100,0,5000);
  h_pt1_pt2_ht_pass["HLT_AK8PFJet360_TrimMass30"] = new TH3D("pt1_pt2_ht_pass_HLT_AK8PFJet360_TrimMass30","",100,0,1000,100,0,1000,100,0,5000);
  h_pt1_pt2_ht_pass["HLT_PFJet450"] = new TH3D("pt1_pt2_ht_pass_HLT_PFJet450","",100,0,1000,100,0,1000,100,0,5000);
  h_pt1_pt2_ht_pass["HLT_PFHT800"] = new TH3D("pt1_pt2_ht_pass_HLT_PFHT800","",100,0,1000,100,0,1000,100,0,5000);
  h_pt1_pt2_ht_pass["HLT_PFHT900"] = new TH3D("pt1_pt2_ht_pass_HLT_PFHT900","",100,0,1000,100,0,1000,100,0,5000);
  h_pt1_pt2_ht_pass["HLT_AK8PFJet450"] = new TH3D("pt1_pt2_ht_pass_HLT_AK8PFJet450","",100,0,1000,100,0,1000,100,0,5000);
  
  h_pt1_pt2_ht_pass["HLT_AK8PFHT800_TrimMass50_17"] = new TH3D("pt1_pt2_ht_pass_HLT_AK8PFHT800_TrimMass50_17","",100,0,1000,100,0,1000,100,0,5000);
  h_pt1_pt2_ht_pass["HLT_AK8PFJet330_PFAK8BTagCSV_p17"] = new TH3D("pt1_pt2_ht_pass_HLT_AK8PFJet330_PFAK8BTagCSV_p17","",100,0,1000,100,0,1000,100,0,5000);
  h_pt1_pt2_ht_pass["HLT_AK8PFJet400_TrimMass30_17"] = new TH3D("pt1_pt2_ht_pass_HLT_AK8PFJet400_TrimMass30_17","",100,0,1000,100,0,1000,100,0,5000);
  h_pt1_pt2_ht_pass["HLT_AK8PFJet500_17"] = new TH3D("pt1_pt2_ht_pass_HLT_AK8PFJet500_17","",100,0,1000,100,0,1000,100,0,5000);
  h_pt1_pt2_ht_pass["HLT_PFJet500_17"] = new TH3D("pt1_pt2_ht_pass_HLT_PFJet500_17","",100,0,1000,100,0,1000,100,0,5000);
  h_pt1_pt2_ht_pass["HLT_PFHT1050_17"] = new TH3D("pt1_pt2_ht_pass_HLT_PFHT1050_17","",100,0,1000,100,0,1000,100,0,5000);

  h_pt1_pt2_ht_pass["HLT_AK8PFHT800_TrimMass50_18"] = new TH3D("pt1_pt2_ht_pass_HLT_AK8PFHT800_TrimMass50_18","",100,0,1000,100,0,1000,100,0,5000);
  h_pt1_pt2_ht_pass["HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_np4"] = new TH3D("pt1_pt2_ht_pass_HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_np4","",100,0,1000,100,0,1000,100,0,5000);
  h_pt1_pt2_ht_pass["HLT_AK8PFJet400_TrimMass30_18"] = new TH3D("pt1_pt2_ht_pass_HLT_AK8PFJet400_TrimMass30_18","",100,0,1000,100,0,1000,100,0,5000);
  h_pt1_pt2_ht_pass["HLT_AK8PFJet500_18"] = new TH3D("pt1_pt2_ht_pass_HLT_AK8PFJet500_18","",100,0,1000,100,0,1000,100,0,5000);
  h_pt1_pt2_ht_pass["HLT_PFJet500_18"] = new TH3D("pt1_pt2_ht_pass_HLT_PFJet500_18","",100,0,1000,100,0,1000,100,0,5000);
  h_pt1_pt2_ht_pass["HLT_PFHT1050_18"] = new TH3D("pt1_pt2_ht_pass_HLT_PFHT1050_18","",100,0,1000,100,0,1000,100,0,5000);

  //////////////////////////////////////
  //
  //////////////////////////////////////
  h_pt_msd_xbb_tot["HLT_Soup"] = new TH3D("pt_msd_xbb_tot_HLT_Soup","",100,0,1000,30,0,300,10,0,1);
  h_pt_msd_xcc_tot["HLT_Soup"] = new TH3D("pt_msd_xcc_tot_HLT_Soup","",100,0,1000,30,0,300,10,0,1);
  h_pt1_pt2_ht_tot["HLT_Soup"] = new TH3D("pt1_pt2_ht_tot_HLT_Soup","",100,0,1000,100,0,1000,100,0,5000);


  ////////////////////////////////////
  //Add histograms to fOutput so they can be saved in Processor::SlaveTerminate
  /////////////////////////////////////
  r->GetOutputList()->Add(h_evt_all) ;
  r->GetOutputList()->Add(h_evt) ;
  for(auto it = h_pt_msd_xbb_pass.begin(); it != h_pt_msd_xbb_pass.end(); ++it) {
    it->second->Sumw2();
    r->GetOutputList()->Add(it->second);
  }
  for(auto it = h_pt_msd_xbb_tot.begin(); it != h_pt_msd_xbb_tot.end(); ++it) {
    it->second->Sumw2();
    r->GetOutputList()->Add(it->second);
  }
  for(auto it = h_pt_msd_xcc_pass.begin(); it != h_pt_msd_xcc_pass.end(); ++it) {
    it->second->Sumw2();
    r->GetOutputList()->Add(it->second);
  }
  for(auto it = h_pt1_pt2_ht_tot.begin(); it != h_pt1_pt2_ht_tot.end(); ++it) {
    it->second->Sumw2();
    r->GetOutputList()->Add(it->second);
  }
  for(auto it = h_pt1_pt2_ht_pass.begin(); it != h_pt1_pt2_ht_pass.end(); ++it) {
    it->second->Sumw2();
    r->GetOutputList()->Add(it->second);
  }
}

#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018) 
std::vector<std::vector<int> > VbbHcc_triggerSel::DauIdxs_ZH(Reader* r) {
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

bool VbbHcc_triggerSel::sortbysecdesc(const std::pair<int,float> &a, const std::pair<int,float> &b)
{
  return a.second>b.second;
}

bool VbbHcc_triggerSel::sortbysecdesc1(const std::pair<std::vector<int>,float> &a, const std::pair<std::vector<int>,float> &b)
{
  return a.second>b.second;
}

int VbbHcc_triggerSel::NextraJet(std::vector<JetObjBoosted>& fatJets, std::vector<JetObj>& jets) {
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

void VbbHcc_triggerSel::Process(Reader* r) {

#if !defined(POSTPROC)
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
  
  //if (genWeight < 0) h_evt_all->Fill(-1) ;
  //if (genWeight == 0) h_evt_all->Fill(0) ;
  //if (genWeight > 0) h_evt_all->Fill(1) ;
  //h_evt_all->Fill(2,genWeight);
  
  puSF = PileupSF(*(r->Pileup_nTrueInt));
#endif

#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017)
  l1preW = *(r->L1PreFiringWeight_Nom);
  //std::cout << "\nPrefiring: " << l1preW;
#endif

  float evtW = 1. ;
  if (!m_isData) evtW *= genWeight*puSF*l1preW;

#if defined(DATA_2016PRE) || defined(DATA_2016) || defined(DATA_2017) || defined(DATA_2018)
  //h_evt->Fill(-1) ;
  if (!m_lumiFilter.Pass(*(r->run),*(r->luminosityBlock))) return;
  //h_evt->Fill(1) ;
#endif
  
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
  std::vector<LepObj> muons_trigger;
  for (unsigned int i = 0 ; i < *(r->nMuon) ; ++i) {
    LepObj muon((r->Muon_pt)[i],(r->Muon_eta)[i],-1,(r->Muon_phi)[i],(r->Muon_mass)[i],i,(r->Muon_charge)[i],(r->Muon_pfRelIso04_all)[i]) ;
    //float sf_rc = MuonRcSF(r, muon, 13) ;
    float sf_rc = 1;
    muon.m_lvec.SetPtEtaPhiM(muon.m_lvec.Pt()*sf_rc,muon.m_lvec.Eta(),muon.m_lvec.Phi(),muon.m_lvec.M()) ;
    //muon for muon jet overlap removal
    if (r-> Muon_mediumId[i] > 0 && r->Muon_pfRelIso04_all[i] < CUTS.Get<float>("muon_iso")) {
      if (muon.m_lvec.Pt() > CUTS.Get<float>("lep_jetOverlap_pt") && fabs(muon.m_lvec.Eta()) < CUTS.Get<float>("lep_jetOverlap_eta")) {
        muons_jetOverlap.push_back(muon) ;
      }
    }
    //muon for trigger study
    if (r->Muon_looseId[i] > 0 && r->Muon_pfRelIso04_all[i] < CUTS.Get<float>("muon_iso_0p25")) {
      if (muon.m_lvec.Pt() > CUTS.Get<float>("lep_pt") && fabs(muon.m_lvec.Eta()) < CUTS.Get<float>("lep_eta")) {
        muons_trigger.push_back(muon) ;
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
    JetObjBoosted jet(jetPt,(r->FatJet_eta)[i],(r->FatJet_phi)[i],(r->FatJet_mass)[i],jetFlav,
        (r->FatJet_btagDDCvB)[i],(r->FatJet_btagDDCvL)[i], (r->FatJet_btagDDBvL)[i], 
        (r->FatJet_deepTagMD_ZHccvsQCD)[i],(r->FatJet_deepTagMD_ZbbvsQCD)[i],
        Xcc,Xbb,(r->FatJet_particleNetMD_QCD)[i],
        (r->FatJet_particleNet_TvsQCD)[i],
        (r->FatJet_particleNet_WvsQCD)[i],
        (r->FatJet_particleNet_ZvsQCD)[i],
        (r->FatJet_n2b1)[i], -1) ;
    if(jet.IsLepton(eles_jetOverlap,0.8) || jet.IsLepton(muons_jetOverlap,0.8)) continue;
    //if((r->FatJet_pt)[i] > CUTS.Get<float>("jet_pt_ak08") && fabs((r->FatJet_eta)[i]) < CUTS.Get<float>("jet_eta_ak08")) jets.push_back(jet) ;
    if(fabs((r->FatJet_eta)[i]) < CUTS.Get<float>("jet_eta_ak08")) jets.push_back(jet) ;
  }

  //Fill fat jet pt, rho, n2b1 to identify c_26 cut
  //for(unsigned iJ = 0; iJ < jets.size();++iJ) {
  //}

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

  //if(ak4Jets.size()>0) h_ljpt->Fill(ak4Jets[0].m_lvec.Pt(),genWeight);
 
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
  
  ////////////////////////////////////////////////////
  //Muon selection for trigger
  ////////////////////////////////////////////////////

  ////////////////////////////////////////////////////
  //Trigger results, will be applied in selection
  ////////////////////////////////////////////////////
  bool trigger(false); 
  std::map<std::string,bool> trig_results;
  //2016
  trig_results["HLT_Soup"] = false;
  trig_results["HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p20"] = false;
  trig_results["HLT_AK8PFHT700_TrimR0p1PT0p03Mass50"] = false;
  trig_results["HLT_AK8PFJet360_TrimMass30"] = false;
  trig_results["HLT_AK8PFJet450"] = false;
  trig_results["HLT_PFHT800"] = false;
  trig_results["HLT_PFHT900"] = false;
  trig_results["HLT_PFJet450"] = false;
  //2017
  trig_results["HLT_AK8PFHT800_TrimMass50_17"] = false;
  trig_results["HLT_AK8PFJet330_PFAK8BTagCSV_p17"] = false;
  trig_results["HLT_AK8PFJet400_TrimMass30_17"] = false;
  //check 420
  trig_results["HLT_AK8PFJet500_17"] = false;
  trig_results["HLT_PFJet500_17"] = false;
  trig_results["HLT_PFHT1050_17"] = false;
  //2018 
  trig_results["HLT_AK8PFHT800_TrimMass50_18"] = false;
  trig_results["HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_np4"] = false;
  trig_results["HLT_AK8PFJet400_TrimMass30_18"] = false;
  trig_results["HLT_AK8PFJet500_18"] = false;
  trig_results["HLT_PFJet500_18"] = false;
  trig_results["HLT_PFHT1050_18"] = false;
  
  bool trigger_muon(false);

#if defined(MC_2016PRE) || defined(MC_2016) || defined(DATA_2016PRE) || defined(DATA_2016)
   trigger =  *(r->HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p20) ||
     *(r->HLT_AK8PFJet360_TrimMass30) ||
     *(r->HLT_AK8PFJet450) ||
     *(r->HLT_PFJet450) ||
     *(r->HLT_AK8PFHT700_TrimR0p1PT0p03Mass50) ||
     *(r->HLT_PFHT900);
  trig_results["HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p20"] = *(r->HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p20);
  trig_results["HLT_AK8PFHT700_TrimR0p1PT0p03Mass50"] = *(r->HLT_AK8PFHT700_TrimR0p1PT0p03Mass50);
  trig_results["HLT_AK8PFJet360_TrimMass30"] = *(r->HLT_AK8PFJet360_TrimMass30);
  trig_results["HLT_AK8PFJet450"] = *(r->HLT_AK8PFJet450);
  trig_results["HLT_PFHT900"] = *(r->HLT_PFHT900);
  trig_results["HLT_PFJet450"] =  *(r->HLT_PFJet450);

  trigger_muon = *(r->HLT_IsoMu24) || *(r->HLT_IsoTkMu24) || *(r->HLT_Mu50) || *(r->HLT_TkMu50);
 
  #if !defined(DATA_2016H)
   trigger = trigger || *(r->HLT_PFHT800);
   trig_results["HLT_PFHT800"] = *(r->HLT_PFHT800);
  #endif
#endif

#if defined(MC_2017) || defined(DATA_2017)
   trigger = (*(r->HLT_AK8PFJet500) ||
     *(r->HLT_PFJet500) ||
     *(r->HLT_PFHT1050));
   trig_results["HLT_AK8PFJet500_17"] = *(r->HLT_AK8PFJet500);
  trig_results["HLT_PFJet500_17"] = *(r->HLT_PFJet500);
  trig_results["HLT_PFHT1050_17"] = *(r->HLT_PFHT1050);

  //trigger_muon = *(r->HLT_IsoMu27) || *(r->HLT_Mu50) || *(r->HLT_OldMu100) || *(r->HLT_TkMu100); 
  trigger_muon = *(r->HLT_IsoMu27) || *(r->HLT_Mu50) ; 

  #if defined(MC_2017) || !defined(DATA_2017B)
   trigger = trigger || 
     *(r->HLT_AK8PFJet400_TrimMass30) ||
     *(r->HLT_AK8PFHT800_TrimMass50);
   trig_results["HLT_AK8PFJet400_TrimMass30_17"] = *(r->HLT_AK8PFJet400_TrimMass30);
   trig_results["HLT_AK8PFHT800_TrimMass50_17"] = *(r->HLT_AK8PFHT800_TrimMass50);
  #endif
  #if defined(MC_2017) || defined(DATA_2017F)
   trigger = trigger || *(r->HLT_AK8PFJet330_PFAK8BTagCSV_p17);
  trig_results["HLT_AK8PFJet330_PFAK8BTagCSV_p17"] = *(r->HLT_AK8PFJet330_PFAK8BTagCSV_p17);
  #endif
#endif

#if defined(MC_2018) || defined(DATA_2018)
   trigger = *(r->HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_np4) ||
     *(r->HLT_AK8PFJet400_TrimMass30) ||
     *(r->HLT_AK8PFJet500) ||
     *(r->HLT_PFJet500) ||
     *(r->HLT_AK8PFHT800_TrimMass50) ||
     *(r->HLT_PFHT1050);
  trig_results["HLT_AK8PFHT800_TrimMass50_18"] = *(r->HLT_AK8PFHT800_TrimMass50);
  trig_results["HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_np4"] = *(r->HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_np4);
  trig_results["HLT_AK8PFJet400_TrimMass30_18"] = *(r->HLT_AK8PFJet400_TrimMass30);
  trig_results["HLT_AK8PFJet500_18"] = *(r->HLT_AK8PFJet500);
  trig_results["HLT_PFJet500_18"] = *(r->HLT_PFJet500) ;
  trig_results["HLT_PFHT1050_18"] = *(r->HLT_PFHT1050);
  
  trigger_muon = *(r->HLT_IsoMu24) || *(r->HLT_Mu50) || *(r->HLT_OldMu100) || *(r->HLT_TkMu100); 

#endif
  
  //all triggers
  trig_results["HLT_Soup"] = trigger;

////////////////////////////////////////////////
//Now calculate trigger eff
////////////////////////////////////////////////

  if(trigger_muon) {

//only apply at least one muon for data
#if defined(DATA_2016PRE) || defined(DATA_2016) || defined(DATA_2017) || defined(DATA_2018)
    if (muons_trigger.size()>0) {
#endif    

//now set trigger names
//2016      
#if defined(MC_2016PRE) || defined(MC_2016) || defined(DATA_2016PRE) || defined(DATA_2016)
      std::vector<std::string> trigNames;
      trigNames.push_back("HLT_Soup");
      trigNames.push_back("HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p20");
      trigNames.push_back("HLT_AK8PFHT700_TrimR0p1PT0p03Mass50");
      trigNames.push_back("HLT_AK8PFJet360_TrimMass30");
      trigNames.push_back("HLT_AK8PFJet450");
      trigNames.push_back("HLT_PFHT900");
      trigNames.push_back("HLT_PFJet450");
  #if !defined(DATA_2016H)
      trigNames.push_back("HLT_PFHT800");
  #endif
#endif

//2017
#if defined(MC_2017) || defined(DATA_2017)
      std::vector<std::string> trigNames; 
      trigNames.push_back("HLT_Soup");
      trigNames.push_back("HLT_AK8PFJet500_17");
      trigNames.push_back("HLT_PFJet500_17");
      trigNames.push_back("HLT_PFHT1050_17");
  #if defined(MC_2017) || !defined(DATA_2017B)
      trigNames.push_back("HLT_AK8PFJet400_TrimMass30_17");
      trigNames.push_back("HLT_AK8PFHT800_TrimMass50_17");
  #endif 
  #if defined(MC_2017) || defined(DATA_2017F)
      trigNames.push_back("HLT_AK8PFJet330_PFAK8BTagCSV_p17");
  #endif
#endif

//2018
#if defined(MC_2018) || defined(DATA_2018)
      std::vector<std::string> trigNames;
      trigNames.push_back("HLT_Soup");
      trigNames.push_back("HLT_AK8PFHT800_TrimMass50_18");
      trigNames.push_back("HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_np4");
      trigNames.push_back("HLT_AK8PFJet400_TrimMass30_18");
      trigNames.push_back("HLT_AK8PFJet500_18");
      trigNames.push_back("HLT_PFJet500_18");
      trigNames.push_back("HLT_PFHT1050_18");
#endif

      if (jets.size()>0) { 
        if (jets[0].m_lvec.M() > 40 && jets[0].m_lvec.M() < 300) {
          for(auto trigName : trigNames) {
            if (trigName == "HLT_Soup") {
              h_pt_msd_xbb_tot.at(trigName)->Fill(jets[0].m_lvec.Pt(),jets[0].m_lvec.M(),jets[0].m_PN_Xbb,evtW);
              h_pt_msd_xcc_tot.at(trigName)->Fill(jets[0].m_lvec.Pt(),jets[0].m_lvec.M(),jets[0].m_PN_Xcc,evtW);
            }
            if (trig_results.at(trigName)) {
              h_pt_msd_xbb_pass.at(trigName)->Fill(jets[0].m_lvec.Pt(),jets[0].m_lvec.M(),jets[0].m_PN_Xbb,evtW);
              h_pt_msd_xcc_pass.at(trigName)->Fill(jets[0].m_lvec.Pt(),jets[0].m_lvec.M(),jets[0].m_PN_Xcc,evtW);
            }
          }
        }
      }
      /////////////////////////////
      //find trigger eff for selection used in analysis
      /////////////////////////////
      std::vector<unsigned> idx_tmps;
      float ht(0);
      for(unsigned i = 0; i < jets.size();++i) {
        if (jets[i].m_lvec.M()>40) idx_tmps.push_back(i);
        ht += jets[i].m_lvec.Pt();
      }
      if (idx_tmps.size() > 1) {
        for(auto trigName : trigNames) {
          if (trigName == "HLT_Soup") {
            h_pt1_pt2_ht_tot.at(trigName)->Fill(jets[idx_tmps[0]].m_lvec.Pt(),jets[idx_tmps[1]].m_lvec.Pt(),ht,evtW);
          }
          if (trig_results.at(trigName)) {
            h_pt1_pt2_ht_pass.at(trigName)->Fill(jets[idx_tmps[0]].m_lvec.Pt(),jets[idx_tmps[1]].m_lvec.Pt(),ht,evtW);
          }
        }
      }
      
      /*
      //jet rho cut
      std::vector<unsigned> idx_tmps_1;
      //consider jets passing kinematic cuts
      for (auto i:idx_tmps) {
          if(jets[i].m_rho > -6 && jets[i].m_rho < -2.1) idx_tmps_1.push_back(i);
      }

      //jet n2b1 cut 
      std::vector<unsigned> idx_tmps_2;
      //consider jets passing kine and rho
      for (auto i:idx_tmps_1) {
        //std::cout << "\n" << jets[i].m_lvec.Pt() << " " << jets[i].m_rho << " " << Getn2b1Cut(jets[i].m_rho,jets[i].m_lvec.Pt());
        if(jets[i].m_n2b1 < Getn2b1Cut(jets[i].m_rho,jets[i].m_lvec.Pt())) idx_tmps_2.push_back(i);
        //if(true) idx_tmps_2.push_back(i);
      }

      //use ParticleNet
      ///////////////////////////////////////
      //using particle net tagger
      ///////////////////////////////////////
      float XccCut = 0.9743;//2016 post  FIXME add pre
      float XbbCut = 0.9735;//2016 post  FIXME add pre
#if defined(DATA_2017) || defined(MC_2017)
      XccCut = 0.9765; 
      XbbCut = 0.9714;
#endif
#if defined(DATA_2018) || defined(MC_2018)
      XccCut = 0.9777;
      XbbCut = 0.9734;
#endif
      //find jets with maximum BvL among jet passing kine, met, rho, and n2b1
      int idx_bb = -1;
      float max_Xbb = -10;
      bool filled = false;
      for(auto i:idx_tmps_2) {
        if(!filled) {
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

      if(idx_bb>=0) {
        if(idx_cc>=0) {
           if (jets[idx_bb].m_PN_Xbb>=XbbCut) {
             if(jets[idx_cc].m_PN_Xcc>=XccCut) {
               for(auto trigName : trigNames) {
                  if (trigName == "HLT_Soup") {
                    h_pt1_pt2_tot.at(trigName)->Fill(jets[idx_bb].m_lvec.Pt(),jets[idx_cc].m_lvec.Pt());
                  }
                  if (trig_results.at(trigName)) {
                    h_pt1_pt2_pass.at(trigName)->Fill(jets[idx_bb].m_lvec.Pt(),jets[idx_cc].m_lvec.Pt());
                  }
               }
             }
           }
        }
      }
      */

#if defined(DATA_2016PRE) || defined(DATA_2016) || defined(DATA_2017) || defined(DATA_2018)
    }
#endif
  }

//endif for !defined(POSTPROC)
#endif


} //end Process



void VbbHcc_triggerSel::Terminate(TList* mergedList, std::string outFileName) {
}
