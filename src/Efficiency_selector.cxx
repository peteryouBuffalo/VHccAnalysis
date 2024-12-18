#define Efficiency_selector_cxx

#include <math.h>

#include "TList.h"
#include "TParameter.h"

#include "Efficiency_selector.h"
#include "Global.h"
//#include "Obj.cxx"

const int N_PTBINS = 3;
const double PTBINS[4] = {450,500,600,1000};
const float FLAVORBINS[4] = {0,4,5,6};

///////////////////////////////////////
//using particle net tagger
///////////////////////////////////////
//pQCD cut from AN2022_031_v3.pdf table 11, page 25
#if defined(DATA_2016) || defined(MC_2016)
//medium
//const float XccCut = 0.9743;//2016 post  FIXME add pre
//loose
const float XccCut = 0.9252;//2016 post
const float XbbCut = 0.9735;//2016 post  FIXME add pre
const float pQCDcut = 0.0681;
#endif

#if defined(DATA_2016PRE) || defined(MC_2016PRE)
//medium
//const float XccCut = 0.9751;
//loose WP
const float XccCut = 0.9252;
const float XbbCut = 0.9737;
const float pQCDcut = 0.0541;
#endif

#if defined(DATA_2017) || defined(MC_2017)
//medium
//const float  XccCut = 0.9765; 
//loose
const float  XccCut = 0.9347; 
const float  XbbCut = 0.9714;
const float pQCDcut = 0.0541;
#endif
#if defined(DATA_2018) || defined(MC_2018)
//medium
//const float XccCut = 0.9777;
//loose
const float XccCut = 0.9368;
const float XbbCut = 0.9734;
const float pQCDcut = 0.0741;
#endif


Efficiency_selector::~Efficiency_selector() {
  /*
  if(h_eff_xbb_bb) delete h_eff_xbb_bb;
  if(h_eff_xbb_cc) delete h_eff_xbb_cc;
  if(h_eff_xbb_light) delete h_eff_xbb_light;
  if(h_eff_xcc_bb) delete h_eff_xcc_bb;
  if(h_eff_xcc_cc) delete h_eff_xcc_cc;
  if(h_eff_xcc_light) delete h_eff_xcc_light;
  if(h_eff_bdj) delete h_eff_bdj;
  if(h_eff_cdj) delete h_eff_cdj;
  if(h_eff_ldj) delete h_eff_ldj;
  //if(h_evt_all) delete h_evt_all;
  */
} 

void Efficiency_selector::SlaveBegin(Reader* r) {
  h_evt_all = new TH1D("Nevt_all_VbbHcc_boosted_effSel","",4,-1.5,2.5) ; //This stores events before any filter or cut, bin 1: total negative weight events, bin 2: total positive weight events, bin 3: total event weighted by genWeight (= bin2 - bin1, if genWeight are always -1,1
  h_eff_xbb_bb = new BoostedJetEffPlots("xbb_bb");
  h_eff_xbb_cc = new BoostedJetEffPlots("xbb_cc");
  h_eff_xbb_light = new BoostedJetEffPlots("xbb_light");
  h_eff_xcc_bb = new BoostedJetEffPlots("xcc_bb");
  h_eff_xcc_cc = new BoostedJetEffPlots("xcc_cc");
  h_eff_xcc_light = new BoostedJetEffPlots("xcc_light");
  
  unsigned nBins = 9 ;
  float bins[10] = {20, 30, 50, 70, 100, 140, 200, 300, 600, 1000} ;
  h_eff_bdj = new EffPlots("bdj", nBins, bins) ;
  h_eff_cdj = new EffPlots("cdj", nBins, bins) ;
  h_eff_ldj = new EffPlots("ldj", nBins, bins) ;

  std::vector<TH1*> tmp = h_eff_xbb_bb->returnHisto() ;
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
  tmp = h_eff_bdj->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_eff_cdj->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);
  tmp = h_eff_ldj->returnHisto() ;
  for(size_t i=0;i<tmp.size();i++) r->GetOutputList()->Add(tmp[i]);

  r->GetOutputList()->Add(h_evt_all) ;
}

//////////////////////////////////
void Efficiency_selector::Fill_btagEffi(std::vector<JetObj> jets, std::string bTagWP, float w) { //bTagWP = L,M,T
  for(unsigned iJ = 0 ; iJ < jets.size() ; ++iJ) {
    if(jets[iJ].m_flav == 5) {
      h_eff_bdj->Fill(jets[iJ].m_lvec.Pt(),"deno",w) ;
      if (jets[iJ].m_deepFlavB > CUTS.Get<float>("jet_deepFlavB"+bTagWP+"_" + m_year)) h_eff_bdj->Fill(jets[iJ].m_lvec.Pt(),"num",w) ; 
    }
    if(jets[iJ].m_flav == 4) {
      h_eff_cdj->Fill(jets[iJ].m_lvec.Pt(),"deno",w) ;
      if (jets[iJ].m_deepFlavB > CUTS.Get<float>("jet_deepFlavB"+bTagWP+"_" + m_year)) h_eff_cdj->Fill(jets[iJ].m_lvec.Pt(),"num",w) ; 
    }
    if(jets[iJ].m_flav == 0) {
      h_eff_ldj->Fill(jets[iJ].m_lvec.Pt(),"deno",w) ;
      if (jets[iJ].m_deepFlavB > CUTS.Get<float>("jet_deepFlavB"+bTagWP+"_" + m_year)) h_eff_ldj->Fill(jets[iJ].m_lvec.Pt(),"num",w) ; 
    }
  }
}



void Efficiency_selector::Process(Reader* r) {
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
  
  //ak04 jets
  std::vector<JetObj> ak4Jets;
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
    
    JetObjBoosted jet(jetPt,(r->FatJet_eta)[i],(r->FatJet_phi)[i],(r->FatJet_mass)[i],jetFlav,
        (r->FatJet_btagDDCvB)[i],(r->FatJet_btagDDCvL)[i], (r->FatJet_btagDDBvL)[i], 
        (r->FatJet_deepTagMD_ZHccvsQCD)[i],(r->FatJet_deepTagMD_ZbbvsQCD)[i],
        Xcc,Xbb,(r->FatJet_particleNetMD_QCD)[i],
        (r->FatJet_particleNet_TvsQCD)[i],
        (r->FatJet_particleNet_WvsQCD)[i],
        (r->FatJet_particleNet_ZvsQCD)[i],
        (r->FatJet_n2b1)[i], -1) ;
    if(jet.IsLepton(eles_jetOverlap,0.8) || jet.IsLepton(muons_jetOverlap,0.8)) continue;
    if((r->FatJet_pt)[i] > CUTS.Get<float>("jet_pt_ak08") && fabs((r->FatJet_eta)[i]) < CUTS.Get<float>("jet_eta_ak08")) jets.push_back(jet) ;
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

  //tagging efficiency
  Fill_btagEffi(ak4Jets,"M",evtW) ; //Eff for medium WP

} //end Process



void Efficiency_selector::Terminate(TList* mergedList, std::string outFileName) {
  /*
  TList* aList = new TList() ;
  TParameter<double>* a = new TParameter<double>("lep_eta",CUTS.Get<float>("lep_eta")) ;
  aList->Add(a) ;
  a = new TParameter<double>("lep_pt",CUTS.Get<float>("lep_pt")) ;
  aList->Add(a) ;
  
  TFile* f = new TFile(outFileName.c_str(), "UPDATE") ;
  aList->Write("Efficiency_selectorCuts",TObject::kSingleKey) ;

  f->Close() ;
  */
}
