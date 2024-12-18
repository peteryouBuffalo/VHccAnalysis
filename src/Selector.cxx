#include "Selector.h"
#include "Global.h"
#include "math.h"
#include <random>

#include "correction.h"

void Selector::Process(Reader* r) { 
} 

void Selector::SetRandom() {
  m_rand = new TRandom3() ;
}

void Selector::SetLumiMaskFilter(std::string fName_lumiMaskFilter) {
  m_lumiFilter.Set(fName_lumiMaskFilter) ;
}

void Selector::SetBtagCalib(std::string corFileName, std::string effFileName, std::string btagUncType) {
  //m_btagCal = BTagCalibration(taggerName, csvFileName) ;
  //m_btagReader = BTagCalibrationReader(BTagEntry::OP_MEDIUM,  // operating point
  //                                     "central",            //central sys type
  //                                     {"up","down"});       //other sys type
  
  //m_btagReader.load(m_btagCal,     // calibration instance
  //          BTagEntry::FLAV_B,    // btag flavour
  //          "comb") ;             // measurement type
  //m_btagReader.load(m_btagCal,  
  //          BTagEntry::FLAV_C,    
  //          "comb") ;            
  //m_btagReader.load(m_btagCal, 
  //          BTagEntry::FLAV_UDSG,
  //          "incl") ;           
  //m_btag_corFilename = corFileName;
  m_btagUncType = btagUncType;
  m_btagEffFile = new TFile(effFileName.c_str(),"READ") ;
  m_corrPtrBtag = correction::CorrectionSet::from_file(corFileName);
}

void Selector::SetJECcorr(std::string jsonFileName){
  m_jec_corFilename = jsonFileName;
  m_corrPtrJEC = correction::CorrectionSet::from_file(jsonFileName);
}

void Selector::SetMSDcorr(std::string jsonFileName){
  m_msd_corFilename = jsonFileName;
  m_corrPtrMSD = correction::CorrectionSet::from_file(jsonFileName);
}

void Selector::SetEleEffCorr(std::vector<std::string> fName_trig,std::string fName_recSF, std::string fName_IDSF, std::vector<float> w_trig, std::string eleUncType) {
  std::string trigN("EGamma_SF2D");
  TFile* fRec = new TFile(fName_recSF.c_str(),"READ") ;
  TFile* fID = new TFile(fName_IDSF.c_str(),"READ") ;
  m_hSF_eleRec = (TH2F*)fRec->Get("EGamma_SF2D") ;
  m_hSF_eleID = (TH2F*)fID->Get("EGamma_SF2D") ;
  for(std::string fN : fName_trig) {
    TFile* f = new TFile(fN.c_str(),"READ");
    m_hSF_eleTrig.push_back((TH2F*)f->Get(trigN.c_str()));
    m_hSF_eleTrig.back()->SetDirectory(0);
  }

  for(float w : w_trig) m_eleTrig_w.push_back(w) ;
  m_eleUncType = eleUncType;
}

void Selector::Setn2b1Cut(std::string fName_n2b1_cut) {
  TFile* fCut = new TFile(fName_n2b1_cut.c_str(),"READ");
  m_hn2b1 = (TH2D*)fCut->Get("Cut");
  m_hn2b1->SetDirectory(0);
}

float Selector::Getn2b1Cut(float rho_in, float pt_in) {
  
  double pt = pt_in;
  double rho = rho_in;
  if (pt > 1000) pt = 999;
  if (rho > -1.5) rho = -1.51;
  if (rho < -6) rho = -5.99;
  int iB = m_hn2b1->FindFixBin(rho,pt);
  return m_hn2b1->GetBinContent(iB);

}

//multiple inputs to deal with different SFs for different run periods 
void Selector::SetMuonEffCorr(std::vector<std::string> fName_trig, std::vector<std::string> fName_ID, std::vector<std::string> fName_iso, std::vector<float> w_trig, std::vector<float> w_ID, std::vector<float> w_iso, std::string muonUncType) {
  std::string trigN("IsoMu24_OR_IsoTkMu24_PtEtaBins/abseta_pt_ratio");
  //std::string idN("NUM_MediumID_DEN_genTracks_eta_pt_syst");
  //std::string isoN("NUM_TightRelIso_DEN_MediumID_eta_pt_syst");//FIXME tight iso?
  std::string idN("NUM_MediumID_DEN_genTracks_eta_pt");//use this to get total unc = stat + syst
  std::string isoN("NUM_TightRelIso_DEN_MediumID_eta_pt");//FIXME tight iso?
  std::string isoN1("NUM_TightRelIso_DEN_MediumID_eta_pt");//FIXME: only stat available for GH for 2016 legacy
#if defined(MC_2017)
  trigN = "IsoMu27_PtEtaBins/abseta_pt_ratio";
  //idN = "NUM_MediumID_DEN_genTracks_pt_abseta_syst";
  //isoN = "NUM_TightRelIso_DEN_MediumID_pt_abseta_syst";
  idN = "NUM_MediumID_DEN_genTracks_pt_abseta"; //use this to get total unc = stat + syst
  isoN = "NUM_TightRelIso_DEN_MediumID_pt_abseta";
#endif
#if defined(MC_2018)
  trigN = "IsoMu24_PtEtaBins/abseta_pt_ratio";
  //idN = "NUM_MediumID_DEN_TrackerMuons_pt_abseta_syst";
  //isoN = "NUM_TightRelIso_DEN_MediumID_pt_abseta_syst";
  idN = "NUM_MediumID_DEN_TrackerMuons_pt_abseta";
  isoN = "NUM_TightRelIso_DEN_MediumID_pt_abseta";
#endif
  for(std::string fN : fName_trig) {
    TFile* f = new TFile(fN.c_str(),"READ");
    m_hSF_muonTrig.push_back((TH2F*)f->Get(trigN.c_str()));
    m_hSF_muonTrig.back()->SetDirectory(0);
  }
  for(std::string fN : fName_ID) {
    TFile* f = new TFile(fN.c_str(),"READ");
    m_hSF_muonID.push_back((TH2F*)f->Get(idN.c_str()));
    m_hSF_muonID.back()->SetDirectory(0);
  }
  for(std::string fN : fName_iso) {
    TFile* f = new TFile(fN.c_str(),"READ");
    if (fN.find("RunGH") != std::string::npos) {
      m_hSF_muonIso.push_back((TH2F*)f->Get(isoN1.c_str()));
    }
    else {
      m_hSF_muonIso.push_back((TH2F*)f->Get(isoN.c_str()));
    }
    m_hSF_muonIso.back()->SetDirectory(0);
  }
  for(float w : w_trig) m_muonTrig_w.push_back(w) ;
  for(float w : w_iso) m_muonIso_w.push_back(w) ;
  for(float w : w_ID) m_muonID_w.push_back(w) ;

  m_muonUncType = muonUncType;
}

void Selector::SetPileupSF(std::string fName_puSF) {
  TFile* f = new TFile(fName_puSF.c_str(),"READ") ;
  m_hSF_pu = (TH1D*)f->Get("pileup_ratio") ;
  m_hSF_pu->SetDirectory(0);
}

void Selector::SetXbbXccEff(std::string fName_xbb_xcc_eff) {
  TFile* f = new TFile(fName_xbb_xcc_eff.c_str(),"READ") ;
  std::string y = "16"; 
  if (m_year == "2016PRE") y = "16_preVFP";
  if (m_year == "2017") y = "17";
  if (m_year == "2018") y = "18";
  std::string hN = "xcc_bb_pt_mass_sf_" + y;
  std::vector<TH2D*> hTmps;
  hTmps.push_back((TH2D*)f->Get(hN.c_str())) ;
  hN = "xcc_cc_pt_mass_sf_" + y;
  hTmps.push_back((TH2D*)f->Get(hN.c_str())) ;
  hN = "xcc_light_pt_mass_sf_" + y;
  hTmps.push_back((TH2D*)f->Get(hN.c_str())) ;
  m_hEff_xbb_xcc["xcc"] = hTmps;
  for(auto h:m_hEff_xbb_xcc["xcc"]) h->SetDirectory(0);
}

void Selector::SetXbbXccEff1D(std::string fName_xbb_xcc_eff) {
  TFile* f = new TFile(fName_xbb_xcc_eff.c_str(),"READ") ;
  std::string y = "16"; 
  if (m_year == "2016PRE") y = "16_preVFP";
  if (m_year == "2017") y = "17";
  if (m_year == "2018") y = "18";
  std::string hN = "xcc_bb_pt_eff_" + y;
  std::vector<TH1D*> hTmps;
  hTmps.push_back((TH1D*)f->Get(hN.c_str())) ;
  hN = "xcc_cc_pt_eff_" + y;
  hTmps.push_back((TH1D*)f->Get(hN.c_str())) ;
  hN = "xcc_light_pt_eff_" + y;
  hTmps.push_back((TH1D*)f->Get(hN.c_str())) ;
  m_hEff1D_xbb_xcc["xcc"] = hTmps;
  for(auto h:m_hEff1D_xbb_xcc["xcc"]) h->SetDirectory(0);
}


float Selector::Get_xccbb_weight(std::vector<JetObjBoosted>& fatJets, int idx_excludedJet, float XbbCut, float XccCut, std::string type="xcc") {
  float w = 1;
  for(unsigned iJ = 0; iJ < fatJets.size();++iJ) {
    float m = fatJets[iJ].m_lvec.M();
    float pt = fatJets[iJ].m_lvec.Pt(); 
    int iB = m_hEff_xbb_xcc[type][0]->FindFixBin(m,pt); //note m = x-axis, pt = y-axis
    if (iJ != idx_excludedJet) {
      int iFlav = 0;
      if (fatJets[iJ].m_flav == 4) iFlav = 1;
      if (fatJets[iJ].m_flav != 4 && fatJets[iJ].m_flav != 5) iFlav = 2;
      //std::cout << "\n" << iFlav << " " << m << " " << pt << " " << m_hEff_xbb_xcc[type][iFlav]->GetBinContent(iB);
       
      auto jetTmp = std::make_pair(fatJets[iJ].m_lvec.Pt(),false);
      float sfTmp = 1;
      /*
      if(type == "xcc") {
        if(fatJets[iJ].m_PN_Xcc >= XccCut) jetTmp.second = true;
        sfTmp = CalTagWeightBoosted_1jet(jetTmp,fatJets[iJ].m_flav,type,m_hfTagUncType);
      }
      if(type == "xbb") {
        if(fatJets[iJ].m_PN_Xbb >= XbbCut) jetTmp.second = true;
        sfTmp = CalTagWeightBoosted_1jet(jetTmp,fatJets[iJ].m_flav,type,m_hfTagUncType);
      }*/
      w *= 1-sfTmp*m_hEff_xbb_xcc[type][iFlav]->GetBinContent(iB);
    }
  }
  //std::cout << "\n w: " << 1-w << " " << fatJets.size();
  return 1-w;
}

void Selector::SetTriggerSF(std::string fName_triggerSF) {
  TFile* f = new TFile(fName_triggerSF.c_str(),"READ") ;
  std::string y = "16"; //FIXME add 2016_preVFP and add xbb
  if (m_year == "2016PRE") y = "16_preVFP";
  if (m_year == "2017") y = "17";
  if (m_year == "2018") y = "18";
  std::string hN = "trig_SF_HLT_Soup_data_" + y;
  m_hTrig_SF = (TGraphAsymmErrors*)f->Get(hN.c_str());
  hN = "SF_pt1_pt2_HLT_Soup_" + y;
  m_hTrig_SF_2D = (TH2D*)f->Get(hN.c_str());
}

float Selector::GetTrigSF(float jetPt) {
  unsigned iB = m_hTrig_SF->GetXaxis()->FindFixBin(jetPt);
  //std::cout << "\n " << m_hTrig_SF->GetName() << " " << jetPt << " " << iB << " " << m_hTrig_SF->GetPointY(iB);
  return m_hTrig_SF->GetPointY(iB);
}

float Selector::GetTrigSF(float jetPt1, float jetPt2) { //pt1, pt2 = leading, subleading
  unsigned iPt1_y = m_hTrig_SF_2D->GetYaxis()->FindFixBin(jetPt1);
  unsigned iPt2_x = m_hTrig_SF_2D->GetXaxis()->FindFixBin(jetPt2);
  //std::cout << "\n " << m_hTrig_SF_2D->GetName() << " " << jetPt2 << " " << iPt2_x << " " << jetPt1 << " " << iPt1_y << " " << m_hTrig_SF_2D->GetBinContent(iPt2_x,iPt1_y);
  return m_hTrig_SF_2D->GetBinContent(iPt2_x,iPt1_y);
}


float Selector::PileupSF(int nTrueInt) {
  int iBin = m_hSF_pu->FindFixBin(nTrueInt) ;
  return m_hSF_pu->GetBinContent(iBin); 
}

void Selector::SetPUjetidCalib(std::string fName_PUjetID_SF,std::string fName_PUjetID_eff,std::string jetPUidUncType){
  //get files
  PUjetID_SF = new TFile(fName_PUjetID_SF.c_str(),"READ") ;
  PUjetID_eff = new TFile(fName_PUjetID_eff.c_str(),"READ") ;
  m_jetPUidUncType = jetPUidUncType ;
}

float Selector::PUjetWeight(std::vector<JetObj>& jets){
  //get SF
  std::string effSF = "h2_eff_sf"+m_year+"_M";
  TH2D* heffSF = (TH2D*)PUjetID_SF->Get(effSF.c_str()) ;

  //h2_eff_sf2016_M_Systuncty
  std::string effSFunc = "h2_eff_sf"+m_year+"_M_Systuncty";
  TH2D* heffSFunc = (TH2D*)PUjetID_SF->Get(effSFunc.c_str()) ;
  
  //get eff
  std::string effMC = "h2_eff_mc"+m_year+"_M";
  TH2D* heffMC = (TH2D*)PUjetID_eff->Get(effMC.c_str()) ;

  float pMC(1.0);
  float pData(1.0);
  float weight(1.) ;
  for (std::vector<JetObj>::iterator jetIt = jets.begin() ; jetIt != jets.end() ; ++jetIt) {
  float jetPt = (jetIt->m_lvec).Pt() ;
  float jetEta = (jetIt->m_lvec).Eta() ;
  float PUjet = jetIt->m_puid;
  if (fabs(jetEta) > CUTS.Get<float>("jet_eta") || jetPt < CUTS.Get<float>("jet_pt"))continue;

  int iBin = heffSF->FindFixBin(jetPt,jetEta) ; 
  float sf = heffSF->GetBinContent(iBin); 
  float sf_unc = heffSFunc->GetBinContent(iBin);
  if (m_jetPUidUncType == "up") sf = sf + sf_unc;
  if (m_jetPUidUncType == "down") sf = sf - sf_unc;
  if (sf < 0.5) sf = 0.5;
  if (sf > 1.5) sf = 1.5;

  int bin = heffMC->FindFixBin(jetPt,jetEta) ; 
  float eff = heffMC->GetBinContent(bin); 

   if (jetPt < 50){
      if (PUjet > 0.61){
      pData = pData*sf*eff ;
      pMC = pMC*eff ;
    }
    else {
      pData = pData*(1-sf*eff) ;
      pMC = pMC*(1-eff) ;
     }
   }//end jet pt > 50 GeV
  }// end jet loop
  if (pMC > 0) weight = pData/pMC ;
   return weight ;
}

float Selector::CalBtagWeight(std::vector<JetObj>& jets, std::string jet_main_btagWP, std::string uncType) {
  //get calibration file
  std::string bN = "b_pt_eff_"+m_year;
  std::string cN = "c_pt_eff_"+m_year;
  std::string lN = "l_pt_eff_"+m_year;
  if (jet_main_btagWP.find("deepFlavB") != std::string::npos) {
    bN = "bdj_pt_eff_"+m_year;
    cN = "cdj_pt_eff_"+m_year;
    lN = "ldj_pt_eff_"+m_year;
  }
  TH1D* hEff_b = (TH1D*)m_btagEffFile->Get(bN.c_str());
  TH1D* hEff_c = (TH1D*)m_btagEffFile->Get(cN.c_str());
  TH1D* hEff_l = (TH1D*)m_btagEffFile->Get(lN.c_str());
  //auto correctionSet = correction::CorrectionSet::from_file(m_btag_corFilename);
  float pMC(1.);
  float pData(1.);
  for (std::vector<JetObj>::iterator jetIt = jets.begin() ; jetIt != jets.end() ; ++jetIt) {
    float jetPt = (jetIt->m_lvec).Pt() ;
    float absEta = fabs((jetIt->m_lvec).Eta());
    int iBin = hEff_b->FindFixBin(jetPt) ; //return overflow bin if jetPt > max pt range
    int flav = jetIt->m_flav ;
    std::string uncTypeInput = "central";
    float eff = hEff_l->GetBinContent(iBin); //jet with pt > max pt range of efficinecy histogram will get the eff of overflow bins
    if (eff <= 0) std::cout << "\n Warning: Efficiency <=0, " << eff ; //we do not want eff = 0 
    BTagEntry::JetFlavor flavCode(BTagEntry::FLAV_UDSG) ;
    if (flav == 5) {
      eff = hEff_b->GetBinContent(iBin);
      flavCode = BTagEntry::FLAV_B;
    }
    if (flav == 4) {
      eff = hEff_c->GetBinContent(iBin);
      flavCode = BTagEntry::FLAV_C;
    }

    if (uncType == "up" || uncType == "down") uncTypeInput = uncType; //all bc and light are up together
    if (uncType == "light_up" && flav != 4 && flav != 5) uncTypeInput = "up";
    if (uncType == "light_down" && flav != 4 && flav != 5) uncTypeInput = "down";
    if (uncType == "bc_up" && (flav == 4 || flav == 5)) uncTypeInput = "up";
    if (uncType == "bc_down" && (flav == 4 || flav == 5)) uncTypeInput = "down";
    
    std::string taggerName_sfSet = "";
    if (jet_main_btagWP.find("deepFlavB") != std::string::npos) taggerName_sfSet = "deepJet";
    std::string wp(1,jet_main_btagWP.back());
    if (flav == 5 || flav == 4) taggerName_sfSet = taggerName_sfSet + "_comb";
    else taggerName_sfSet = taggerName_sfSet + "_incl";

    auto btagInfo = m_corrPtrBtag->at(taggerName_sfSet);
    float sf = 1.0;
    try {
      if (absEta < 0.0001) absEta = 0.0001;
      if (absEta > 2.4999) absEta = 2.4999;
      sf = btagInfo->evaluate({m_btagUncType,wp,flav,absEta,jetPt});
    }
    catch (std::exception& e) {
      std::cout << "\n btagInfo fail evaluate: " << m_btagUncType << " " << wp << " " << flav << " " << absEta << " " << jetPt;
    }

    /*
    float sf = m_btagReader.eval_auto_bounds(
                 uncTypeInput, 
                 flavCode, 
                 fabs((jetIt->m_lvec).Eta()), // absolute value of eta
                 jetPt
    );
    */
    //pass b-tagging requirement
    if (jetIt->m_deepFlavB > CUTS.Get<float>("jet_"+jet_main_btagWP+"_" + m_year)) {
      pData = pData*sf*eff ;
      pMC = pMC*eff ;
    }
    else {
      pData = pData*(1-sf*eff) ;
      pMC = pMC*(1-eff) ;
    }
  } //end loop over jet 
  float sf(1.) ;
  if (pMC > 0) sf = pData/pMC ;
  return sf ;
}

float Selector::CalTagWeightBoosted_1jet(std::pair<float,bool> jet, int jet_flav, std::string tagType, std::string uncType) {
   //Table 18 and 22
   std::vector<std::vector<float>> sf_bb_all = {{1.029,1.070,1.077},{1.0,1.0,1.0},{1.006,1.051,0.991},{0.966,1.033,1.010}}; //FIXME dummy 2016pre
   std::vector<std::vector<float>> sf_bb_eru_all = {{0.051,0.066,0.067},{0,0,0},{0.052,0.056,0.038},{0.056,0.030,0.030}};
   std::vector<std::vector<float>> sf_bb_erd_all = {{0.045,0.062,0.059},{0,0,0},{0.052,0.055,0.043},{0.057,0.025,0.035}};
   //medium
   //std::vector<std::vector<float>> sf_cc_all = {{1.006,1.150,0.991},{1.252,0.937,1.243},{1.439,1.231,1.142},{1.061,0.999,1.011}}; //post = 2016, pre = 2016PRE, 2017, 2018
   //std::vector<std::vector<float>> sf_cc_eru_all = {{0.178,0.171,0.184},{0.184,0,108,0.241},{0.396,0.295,0.182},{0.163,0.118,0.114}};
   //std::vector<std::vector<float>> sf_cc_erd_all = {{0.135,0.180,0.162},{0.176,0.098,0.259},{0.402,0.288,0.173},{0.131,0.107,0.094}};
   //loose
   std::vector<std::vector<float>> sf_cc_all = {{1.177,1.055,1.165},{1.169,0.874,1.2},{1.270,1.308,1.095},{0.97,0.963,0.912}}; //post = 2016, pre = 2016PRE, 2017, 2018
   std::vector<std::vector<float>> sf_cc_eru_all = {{0.166,0.189,0.262},{0.229,0.122,0.240},{0.294,0.334,0.194},{0.154,0.092,0.073}};
   std::vector<std::vector<float>> sf_cc_erd_all = {{0.184,0.176,0.292},{0.217,0.099,0.233},{0.281,0.327,0.175},{0.111,0.086,0.060}};
   //std::vector<std::vector<float>> sf_cc_all = {{1.005,1.130,0.982},{1.464,1.198,1.203},{1.098,1.003,1.031}};
   //std::vector<std::vector<float>> sf_cc_eru_all = {{0.182,0.185,0.181},{0.426,0.268,0.230},{0.234,0.131,0.126}};
   //std::vector<std::vector<float>> sf_cc_erd_all = {{0.157,0.196,0.148},{0.422,0.262,0.227},{0.188,0.119,0.107}};
   int yearBin(0);
   if(m_year == "2016PRE") yearBin = 1;
   if(m_year == "2017") yearBin = 2;
   if(m_year == "2018") yearBin = 3;
   float sf(1.0);
   int ptbin(0);
   if(jet.first >= 500 && jet.first < 600) ptbin = 1;
   if(jet.first >= 600) ptbin = 2;
   float eff(0.5); //FIXME
   float sf_tmp(1.0);
   if (tagType == "xbb") {
     eff = 0.5; //FIXME
     sf_tmp = sf_bb_all[yearBin][ptbin];
     if(uncType=="bbup") sf_tmp += sf_bb_eru_all[yearBin][ptbin];
     if(uncType=="bbdown") sf_tmp -= sf_bb_erd_all[yearBin][ptbin];
   }
   
   if (tagType == "xcc") {
     int iB = m_hEff1D_xbb_xcc[tagType][0]->FindFixBin(jet.first);
     int iFlav = 0;
     if (jet_flav == 4) iFlav = 1;
     if (jet_flav != 4 && jet_flav != 5) iFlav = 2;
     eff = m_hEff1D_xbb_xcc[tagType][iFlav]->GetBinContent(iB);
     sf_tmp = sf_cc_all[yearBin][ptbin];
     if(uncType=="ccup") sf_tmp += sf_cc_eru_all[yearBin][ptbin];
     if(uncType=="ccdown") sf_tmp -= sf_cc_erd_all[yearBin][ptbin];
   }

   if(jet.second) sf = sf_tmp;
   else sf = (1-eff*sf_tmp)/(1-eff);
    
   //std::cout << "\n" << uncType << " " << sf_bb_tmp << " " << sf_cc_tmp;

   return sf;

}


float Selector::CalBtagWeightBoosted(std::pair<float,bool> jet_bb, std::pair<float,bool> jet_cc, std::string uncType) {
   //Table 18 and 22
   std::vector<std::vector<float>> sf_bb_all = {{1.029,1.070,1.077},{1.006,1.051,0.991},{0.966,1.033,1.010}};
   std::vector<std::vector<float>> sf_cc_all = {{1.005,1.130,0.982},{1.464,1.198,1.203},{1.098,1.003,1.031}};
   std::vector<std::vector<float>> sf_bb_eru_all = {{0.051,0.066,0.067},{0.052,0.056,0.038},{0.056,0.030,0.030}};
   std::vector<std::vector<float>> sf_bb_erd_all = {{0.045,0.062,0.059},{0.052,0.055,0.043},{0.057,0.025,0.035}};
   std::vector<std::vector<float>> sf_cc_eru_all = {{0.182,0.185,0.181},{0.426,0.268,0.230},{0.234,0.131,0.126}};
   std::vector<std::vector<float>> sf_cc_erd_all = {{0.157,0.196,0.148},{0.422,0.262,0.227},{0.188,0.119,0.107}};
   //FIXME add 2016PRE
   int yearBin(0);
   if(m_year == "2017") yearBin = 1;
   if(m_year == "2018") yearBin = 2;
   float sf_bb(1.0);
   float sf_cc(1.0);
   int ptbin(0);
   if(jet_bb.first >= 500 && jet_bb.first < 600) ptbin = 1;
   if(jet_bb.first >= 600) ptbin = 2;
   float eff_bb(0.5); //FIXME
   float sf_bb_tmp = sf_bb_all[yearBin][ptbin];
   if(uncType=="bbup") sf_bb_tmp += sf_bb_eru_all[yearBin][ptbin];
   if(uncType=="bbdown") sf_bb_tmp -= sf_bb_erd_all[yearBin][ptbin];
   if(jet_bb.second) sf_bb = sf_bb_tmp;
   else sf_bb = (1-eff_bb*sf_bb_tmp)/(1-eff_bb);
   
   if(jet_cc.first >= 500 && jet_cc.first < 600) ptbin = 1;
   if(jet_cc.first >= 600) ptbin = 2;
   float eff_cc(0.5); //FIXME
   float sf_cc_tmp = sf_cc_all[yearBin][ptbin];
   if(uncType=="ccup") sf_cc_tmp += sf_cc_eru_all[yearBin][ptbin];
   if(uncType=="ccdown") sf_cc_tmp -= sf_cc_erd_all[yearBin][ptbin];
   if(jet_cc.second) sf_cc = sf_cc_tmp;
   else sf_cc = (1-eff_cc*sf_cc_tmp)/(1-eff_cc);
   
   //std::cout << "\n" << uncType << " " << sf_bb_tmp << " " << sf_cc_tmp;

   return sf_bb*sf_cc;

}

float Selector::CalCtagWeightBoosted(std::pair<JetObjBoosted,bool> jet_1, std::pair<JetObjBoosted,bool> jet_2, std::string uncType) {
   auto jet = std::make_pair(jet_1.first.m_lvec.Pt(), jet_1.second);
   float sf1 = CalTagWeightBoosted_1jet(jet, jet_1.first.m_flav, "xcc", uncType);
   jet = std::make_pair(jet_2.first.m_lvec.Pt(), jet_2.second);
   float sf2 = CalTagWeightBoosted_1jet(jet, jet_2.first.m_flav, "xcc", uncType);
   return sf1*sf2;
   /*
   //Table 18 and 22
   //std::vector<std::vector<float>> sf_cc_all = {{1.005,1.130,0.982},{1.464,1.198,1.203},{1.098,1.003,1.031}};
   //std::vector<std::vector<float>> sf_cc_eru_all = {{0.182,0.185,0.181},{0.426,0.268,0.230},{0.234,0.131,0.126}};
   //std::vector<std::vector<float>> sf_cc_erd_all = {{0.157,0.196,0.148},{0.422,0.262,0.227},{0.188,0.119,0.107}};
   //medium
   //std::vector<std::vector<float>> sf_cc_all = {{1.006,1.150,0.991},{1.252,0.937,1.243},{1.439,1.231,1.142},{1.061,0.999,1.011}}; //post = 2016, pre = 2016PRE, 2017, 2018
   //std::vector<std::vector<float>> sf_cc_eru_all = {{0.178,0.171,0.184},{0.184,0,108,0.241},{0.396,0.295,0.182},{0.163,0.118,0.114}};
   //std::vector<std::vector<float>> sf_cc_erd_all = {{0.135,0.180,0.162},{0.176,0.098,0.259},{0.402,0.288,0.173},{0.131,0.107,0.094}};
   //loose
   std::vector<std::vector<float>> sf_cc_all = {{1.177,1.055,1.165},{1.169,0.874,1.2},{1.270,1.308,1.095},{0.97,0.963,0.912}}; //post = 2016, pre = 2016PRE, 2017, 2018
   std::vector<std::vector<float>> sf_cc_eru_all = {{0.166,0.189,0.262},{0.229,0.122,0.240},{0.294,0.334,0.194},{0.154,0.092,0.073}};
   std::vector<std::vector<float>> sf_cc_erd_all = {{0.184,0.176,0.292},{0.217,0.099,0.233},{0.281,0.327,0.175},{0.111,0.086,0.060}};
   //std::vector<std::vector<float>> sf_cc_all = {{0.9,0.95,0.85},{1.05,0.9,1.1},{1.0,1.0,1.0},{0.95,0.95,0.95}}; //post = 2016, pre = 2016PRE, 2017, 2018
   //std::vector<std::vector<float>> sf_cc_eru_all = {{0.178,0.171,0.184},{0.184,0,108,0.241},{0.396,0.295,0.182},{0.163,0.118,0.114}};
   //std::vector<std::vector<float>> sf_cc_erd_all = {{0.135,0.180,0.162},{0.176,0.098,0.259},{0.402,0.288,0.173},{0.131,0.107,0.094}};
   int yearBin(0);
   if(m_year == "2016PRE") yearBin = 1;
   if(m_year == "2017") yearBin = 2;
   if(m_year == "2018") yearBin = 3;
   float sf_cc_1(1.0);
   float sf_cc_2(1.0);
   float jet_1_pt = jet_1.first.m_lvec.Pt();
   //bb = 0, cc = 1, light = 2
   int iFlav = 0;
   if (jet_1.first.m_flav == 4) iFlav = 1;
   if (jet_1.first.m_flav != 4 && jet_1.first.m_flav != 5) iFlav = 2;
   int ptbin(0);
   if(jet_1_pt >= 500 && jet_1_pt < 600) ptbin = 1;
   if(jet_1_pt >= 600) ptbin = 2;
   
   int iB = m_hEff1D_xbb_xcc[tagType][0]->FindFixBin(jet_1_pt); 
   float eff_cc = m_hEff1D_xbb_xcc[tagType][iFlav]->GetBinContent(iB);
   float sf_cc_tmp = sf_cc_all[yearBin][ptbin];
   if(uncType=="ccup") sf_cc_tmp += sf_cc_eru_all[yearBin][ptbin];
   if(uncType=="ccdown") sf_cc_tmp -= sf_cc_erd_all[yearBin][ptbin];
   if(jet_1.second) sf_cc_1 = sf_cc_tmp;
   else sf_cc_1 = (1-eff_cc*sf_cc_tmp)/(1-eff_cc);
   
  
  if (!byPassJet2) {
    float jet_2_pt = jet_2.first.m_lvec.Pt();
    iB = m_hEff1D_xbb_xcc[tagType][0]->FindFixBin(jet_2_pt); 
    iFlav = 0;
    if (jet_2.first.m_flav == 4) iFlav = 1;
    if (jet_2.first.m_flav != 4 && jet_2.first.m_flav != 5) iFlav = 2;
    ptbin = 0;
    if(jet_2_pt >= 500 && jet_2_pt < 600) ptbin = 1;
    if(jet_2_pt >= 600) ptbin = 2;

    eff_cc = m_hEff1D_xbb_xcc[tagType][iFlav]->GetBinContent(iB);
    sf_cc_tmp = sf_cc_all[yearBin][ptbin];
    if(uncType=="ccup") sf_cc_tmp += sf_cc_eru_all[yearBin][ptbin];
    if(uncType=="ccdown") sf_cc_tmp -= sf_cc_erd_all[yearBin][ptbin];
    if(jet_2.second) sf_cc_2 = sf_cc_tmp;
    else sf_cc_2 = (1-eff_cc*sf_cc_tmp)/(1-eff_cc);
  }
   
   return sf_cc_1*sf_cc_2;
   */
}


// Get the JES scale factor for a given jet. The information is given in a map, the variable
// refers to if we're looking to apply this directly to pT or mass. The jetmetSystType will
// indicate if we're looking at uncertainty related to the mass or pt and having "var" allows
// us to know if we want the uncertainty applied to this factor. (isData is self-explanatory).
float Selector::CalculateJES(std::map<std::string, float> jet_info, std::string var,
			     std::string systType, bool isData, JESUncPlots* plots)
{
  //auto correctionSet = correction::CorrectionSet::from_file(m_jec_corFilename);

  // Pull the values from the map (for convenience).
  if (jet_info.find("pt_raw") == jet_info.end())
  { std::cout << "'pt_raw' not provided.\n"; return 1.0f; }
  float pt_raw = jet_info["pt_raw"];

  if (jet_info.find("eta") == jet_info.end())
  { std::cout << "'eta' not provided.\n"; return 1.0f; }
  float eta = jet_info["eta"];

  if (jet_info.find("area") == jet_info.end())
  { std::cout << "'area' not provied.\n"; return 1.0f; }
  float area = jet_info["area"];

  if (jet_info.find("rho") == jet_info.end())
  { std::cout << "'rho' not provided.\n"; return 1.0f; }
  float rho = jet_info["rho"];

  if (plots != NULL) plots->FillPt("raw", pt_raw);
  
  float pt_calc = pt_raw;
  float sf = 1.0f;
  
  std::map<std::string, std::string> corr_map;
  corr_map["L1"] = "L1FastJet";
  corr_map["L2"] = "L2Relative";
  corr_map["L3"] = "L3Absolute";
  corr_map["L2L3"] = "L2L3Residual";
  
  std::string year = "Summer19UL16APV_V7";
#if defined(MC_2016) || defined(DATA_2016)
  year = "Summer19UL16_V7";
#elif defined(MC_2017) || defined(DATA_2017)
  year = "Summer19UL17_V5";
#elif defined(MC_2018) || defined(DATA_2018)
  year = "Summer19UL18_V5";
#endif

  for (auto my_pair : corr_map)
  {
    std::string mc_data_str = (isData) ? "DATA" : "MC";
    std::string full_source = year + "_" + mc_data_str + "_" + my_pair.second + "_AK8PFPuppi";

    try {
      auto jes_info = m_corrPtrJEC->at(full_source);
      float sf_tmp = 1.0f;
      if (my_pair.first == "L1") sf_tmp = jes_info->evaluate({area, rho, eta, pt_raw});
      else sf_tmp = jes_info->evaluate({eta, pt_raw});

      pt_calc *= sf_tmp;
      sf *= sf_tmp;

      if (plots != NULL) plots->FillJES(my_pair.first, sf_tmp);
    }
    catch(const std::out_of_range& oor) {
      std::cout << "ERROR: trouble getting source = '" << full_source << "'" << std::endl;
    }
  }

  // To get the total uncertainty, we need to calculate the sub total unc                     
  // for the following categories:                                                            
  // 1. SubTotalPileUp = sum(PileUpPt[*] + PileUpDatMC)                                       
  // 2. SubTotalRelative = sum(Relative[*])                                                   
  // 3. SubTotalAbsolute = SubTotalPt + SubTotalScale                                         
  // 4. FlavorQCD = self                                                                      
  // 5. TimePtEta = self                                                                      

  // To calculate these, we need to also calculate                                            
  // A) SubTotalPt = Fragmentation + SinglePion[*]                                            
  // B) SubTotalScale = AbsoluteStat + AbsoluteMPFBias, AbsoluteScale

  // The following map will hold the categories of uncertainty and then show their title.     
  std::map<std::string, std::vector<std::string>> unc_map;
  unc_map["SubTotalPt"] = { "Fragmentation", "SinglePionECAL", "SinglePionHCAL" };
  unc_map["SubTotalScale"] = { "AbsoluteStat", "AbsoluteMPFBias", "AbsoluteScale" };

  unc_map["SubTotalPileUp"] = { "PileUpPtBB", "PileUpPtEC1", "PileUpPtEC2", "PileUpPtHF", "Pil\
eUpPtRef", "PileUpDataMC" };
  unc_map["SubTotalRelative"] = { "RelativeBal", "RelativeFSR", "RelativeJEREC1", "RelativeJER\
EC2", "RelativeJERHF",
    "RelativePtBB", "RelativePtEC1", "RelativePtEC2", "RelativePtHF", "RelativeSample", "Rela\
tiveStatEC",
    "RelativeStatFSR", "RelativeStatHF" };
  unc_map["SubTotalAbsolute"] = { "SubTotalPt", "SubTotalScale" };
  unc_map["FlavorQCD"] = { "FlavorQCD" };
  unc_map["TimePtEta"] = { "TimePtEta" };
  unc_map["Total"] = { "SubTotalPileUp", "SubTotalRelative", "SubTotalAbsolute", "FlavorQCD", \
"TimePtEta" };

  // Now, calculate the actual uncertainties.                                                 
  std::map<std::string, float> sub_tot_map;
  sub_tot_map["SubTotalPt"] = 0.0f;
  sub_tot_map["SubTotalScale"] = 0.0f;
  sub_tot_map["SubTotalPileUp"] = 0.0f;
  sub_tot_map["SubTotalRelative"] = 0.0f;
  sub_tot_map["SubTotalAbsolute"] = 0.0f;
  sub_tot_map["FlavorQCD"] = 0.0f;
  sub_tot_map["TimePtEta"] = 0.0f;

  for (auto my_pair : sub_tot_map)
  {
    // For now, handle this one separately because it's                                       
    // a sum of the some of the other uncs.                                                   
    if (my_pair.first == "SubTotalAbsolute") continue;

    // Get the list of sub-unc that need to be added.                                         
    std::vector<std::string> names = unc_map[my_pair.first];
    float uncSub = 0.0f;

    for (size_t i = 0; i < names.size(); ++i)
    {
      std::string fullSource = year + "_MC_" + names[i] + "_AK8PFPuppi";
      //std::cout << "unc : " << fullSource << std::endl;                                     
      auto jes_unc = m_corrPtrJEC->at(fullSource);
      double unc = jes_unc->evaluate({eta,pt_raw});
      uncSub += pow(unc, 2);
    }

    std::string full_source = year + "_MC_" + my_pair.first + "_AK8PFPuppi";
    auto jes_stored = m_corrPtrJEC->at(full_source);
    float stored = jes_stored->evaluate({eta, pt_raw});

    sub_tot_map[my_pair.first] = sqrt(uncSub);
    if (plots != NULL) plots->FillUnc(my_pair.first, sqrt(uncSub), stored);
  }

  // Now that we've calculate all these ones, let's calculate Absolute & Total
  float uncAbs = pow(sub_tot_map["SubTotalPt"],2) + pow(sub_tot_map["SubTotalScale"],2);
  uncAbs = sqrt(uncAbs);
  sub_tot_map["SubTotalAbsolute"] = uncAbs;

  auto jes_stored = m_corrPtrJEC->at(year + "_MC_SubTotalAbsolute_AK8PFPuppi");
  float stored = jes_stored->evaluate({eta,pt_raw});
  if (plots != NULL) plots->FillUnc("SubTotalAbsolute", uncAbs, stored);

  float uncTot = 0.0f;
  std::vector<std::string> names = unc_map["Total"];
  for (size_t i = 0; i < names.size(); ++i)
  {
    uncTot += pow(sub_tot_map[names[i]], 2);
  }
  uncTot = sqrt(uncTot);

  jes_stored = m_corrPtrJEC->at(year + "_MC_Total_AK8PFPuppi");
  stored = jes_stored->evaluate({eta,pt_raw});
  if (plots != NULL) plots->FillUnc("Total", uncTot, stored);
  
  // Now, determine if we wanna add the uncertainty and then return it.
  float proper_scale = sf;

  //std::cout << "systType = " << systType << std::endl;
  if (
      (systType == "jesu" && var == "pt") ||
      (systType == "jmsu" && var == "mass")
  )
  {
    proper_scale += uncTot;
  }
  else if (
      (systType == "jesd" && var == "pt") ||
      (systType == "jmsd" && var == "mass")
  )
  {
    proper_scale -= uncTot;
  }

  return proper_scale;
}


// Calculate the JER corrections for a given jet.
float Selector::CalculateJER(std::map<std::string, float> jet_info, std::vector<JetObjBoosted> genjets, std::string var, std::string systType)
{

  //std::cout << "systType(JER) = " << systType << std::endl;
  std::string syst = "nom";
  if (systType == "jeru") syst = "up";
  else if (systType == "jerd") syst = "down";

  std::string year = "Summer20UL16APV_JRV3";
#if defined(MC_2016)
  year = "Summer20UL16_JRV3";
#elif defined(MC_2017)
  year = "Summer19UL17_JRV2";
#elif defined(MC_2018)
  year = "Summer19UL18_JRV2";
#endif

  if (jet_info.find("eta") == jet_info.end())
  { std::cout << "'eta' not provided.\n"; return 1.0f; }
  float eta = jet_info["eta"];

  if (jet_info.find("pt") == jet_info.end())
  { std::cout << "'pt' not provided.\n"; return 1.0f; }
  float pt = jet_info["pt"];

  if (jet_info.find("rho") == jet_info.end())
  { std::cout << "'rho' not provied.\n"; return 1.0f; }
  float rho = jet_info["rho"];

  if (jet_info.find("mass") == jet_info.end())
  { std::cout << "'mass' not provided.\n"; return 1.0f; }
  float mass = jet_info["mass"];

  if (jet_info.find("phi") == jet_info.end())
  { std::cout << "'phi' not provided.\n"; return 1.0f; }
  float phi = jet_info["phi"];
  
  //auto correctionSet = correction::CorrectionSet::from_file(m_jec_corFilename);

  auto jer_set = m_corrPtrJEC->at(year + "_MC_ScaleFactor_AK8PFPuppi");
  float sJER = jer_set->evaluate({eta, "nom"});
  float sJER_up = jer_set->evaluate({eta, "up"});
  float sJER_dn = jer_set->evaluate({eta, "down"});

  auto unc_set = m_corrPtrJEC->at(year + "_MC_PtResolution_AK8PFPuppi");
  float res = unc_set->evaluate({eta, pt, rho});

  // See if we have an AK8 gen jet that matches our jet criteria.
  float Rcone = 0.8; float RconeCut = Rcone/2;
  bool foundGenJet = false;

  float cJER = 1.0f;
  float pt_gen = 1.0f;
  float dPtCut = 3 * res * pt;

  TLorentzVector *vec = new TLorentzVector();
  vec->SetPtEtaPhiM(pt, eta, phi, mass);

  for (auto gjet : genjets)
  {
    float deltaR = vec->DeltaR(gjet.m_lvec);
    bool matchesR = (deltaR < RconeCut);

    float dPt = abs(pt - gjet.m_lvec.Pt());
    bool passPtCut = (dPt < dPtCut);

    if (matchesR && passPtCut)
    {
      pt_gen = gjet.m_lvec.Pt();
      foundGenJet = true;
      break;
    }
    
  }

  // Choose the proper option based on whether or not we found a gen jet match.
  if (foundGenJet) // METHOD #1 - Scaling Method
  {
    float cJER_nom = 1 + (sJER - 1) * (pt - pt_gen) / pt;
    float cJER_up = 1 + (sJER_up - 1) * (pt - pt_gen) / pt;
    float cJER_dn = 1 + (sJER_dn - 1) * (pt - pt_gen) / pt;

    if (systType == "jeru" && var == "pt") cJER = cJER_up;
    else if (systType == "jerd" && var == "pt") cJER = cJER_dn;
    else if (systType == "jmru" && var == "mass") cJER = cJER_up;
    else if (systType == "jmrd" && var == "mass") cJER = cJER_dn;
    else cJER = cJER_nom;

  }
  else
  {
    std::default_random_engine generator;
    std::normal_distribution<float> normal(0, res);

    float rand = normal(generator);
    float cJER_nom = 1 + rand * sqrt(std::max(sJER*sJER - 1, 0.0f));
    float cJER_up = 1 + rand * sqrt(std::max(sJER_up*sJER_up - 1, 0.0f));
    float cJER_dn = 1 + rand * sqrt(std::max(sJER_dn*sJER_dn - 1, 0.0f));

    if (systType == "jeru" && var == "pt") cJER = cJER_up;
    else if (systType == "jerd" && var == "pt") cJER = cJER_dn;
    else if (systType == "jmru" && var == "mass") cJER = cJER_up;
    else if (systType == "jmrd" && var == "mass") cJER = cJER_dn;
    else cJER = cJER_nom;
  }

  return cJER;
}

    
//Get scale factors from a list of calibration histograms h (each histo corresponds to a run periods, for example muon in 2016 has scale factors for B->F and G->H sets. w are weights for each sets 
std::vector<float> Selector::GetSF_2DHist(float x, float y, std::vector<TH2F*> h, std::vector<float> w) {
  std::vector<float> o{1,0,0}; //value, absolute error, relative error
  unsigned nX = h[0]->GetNbinsX() ;
  unsigned nY = h[0]->GetNbinsY() ;
  unsigned iX = h[0]->GetXaxis()->FindFixBin(x) ;
  unsigned iY = h[0]->GetYaxis()->FindFixBin(y) ;
  if (iX == 0 || iY == 0 || iX > nX || iY > nY) { //underflow and overflow bins
    return o ;
  }
  float sf(0) ;
  float e_sf(0) ;
  for (unsigned i = 0 ; i < h.size() ; ++i) {
    sf += w[i]*h[i]->GetBinContent(iX,iY);
    e_sf += w[i]*w[i]*h[i]->GetBinError(iX,iY)*h[i]->GetBinError(iX,iY);
  }
  e_sf = sqrt(e_sf) ;
  o[0] = sf ;
  o[1] = e_sf ;
  if(sf!=0) o[2] = e_sf/sf; //relative error
  else o[2] = 0.;

  return o ;
}

float Selector::CalEleSF(LepObj e1, LepObj e2) {
  std::vector<float> w{1.0};
  float sf = 1;
  float err = 0; //relative error treated as uncorrelated = (dy/y)^2 = sum[(dxi/xi)^2]
  float errRec = 0.0;
  float errID = 0.0;
  //reconstruction
  std::vector<TH2F*> h{m_hSF_eleRec};
  //first lepton
  std::vector<float> sfs = GetSF_2DHist(e1.m_lvec.Eta(),e1.m_lvec.Pt(),h,w) ;
  sf = sfs[0]; 
  err += sfs[2]*sfs[2]; //2=relative error
  errRec += sfs[2]*sfs[2]; //2=relative error
  //second lepton
  sfs = GetSF_2DHist(e2.m_lvec.Eta(),e2.m_lvec.Pt(),h,w);
  sf *= sfs[0] ;
  err += sfs[2]*sfs[2]; //2=relative error
  errRec += sfs[2]*sfs[2]; //2=relative error
  
  //ID
  h[0] = m_hSF_eleID;
  //first lepton
  sfs = GetSF_2DHist(e1.m_lvec.Eta(),e1.m_lvec.Pt(),h,w);
  sf *= sfs[0] ;
  err += sfs[2]*sfs[2];
  errID += sfs[2]*sfs[2];
  //second lepton
  sfs = GetSF_2DHist(e2.m_lvec.Eta(),e2.m_lvec.Pt(),h,w);
  sf *= sfs[0] ; 
  err += sfs[2]*sfs[2];
  errID += sfs[2]*sfs[2];

  err = sqrt(err);
  errRec = sqrt(errRec);
  errID = sqrt(errID);

  if (m_eleUncType == "up") sf = sf*(1+err);
  if (m_eleUncType == "down") sf = sf*(1-err);
  if (m_eleUncType == "idup") sf = sf*(1+errID);
  if (m_eleUncType == "iddown") sf = sf*(1-errID);
  if (m_eleUncType == "recup") sf = sf*(1+errRec);
  if (m_eleUncType == "recdown") sf = sf*(1-errRec);

  return sf; 
}

float Selector::CalSingleEleSF(LepObj e1) {
  std::vector<float> w{1.0};
  float sf = 1;
  float err = 0; //relative error treated as uncorrelated = (dy/y)^2 = sum[(dxi/xi)^2]
  float errRec = 0.0;
  float errID = 0.0;
  //reconstruction
  std::vector<TH2F*> h{m_hSF_eleRec};
  //first lepton
  std::vector<float> sfs = GetSF_2DHist(e1.m_lvec.Eta(),e1.m_lvec.Pt(),h,w) ;
  sf = sfs[0]; 
  err += sfs[2]*sfs[2]; //2=relative error
  errRec += sfs[2]*sfs[2]; //2=relative error
  
  //ID
  h[0] = m_hSF_eleID;
  //first lepton
  sfs = GetSF_2DHist(e1.m_lvec.Eta(),e1.m_lvec.Pt(),h,w);
  sf *= sfs[0] ;
  err += sfs[2]*sfs[2];
  errID += sfs[2]*sfs[2];

  err = sqrt(err);
  errRec = sqrt(errRec);
  errID = sqrt(errID);
  if (m_eleUncType == "up") sf = sf*(1+err);
  if (m_eleUncType == "down") sf = sf*(1-err);
  if (m_eleUncType == "recup") sf = sf*(1+errRec);
  if (m_eleUncType == "recdown") sf = sf*(1-errRec);
  if (m_eleUncType == "idup") sf = sf*(1+errID);
  if (m_eleUncType == "iddown") sf = sf*(1-errID);

  return sf; 
}

float Selector::CalMuonSF_id_iso(LepObj e1, LepObj e2) {
  float sf(1.0);
  float err(0.0);
  float errID(0.0);
  float errIso(0.0);
#if defined(MC_2016PRE) || defined(MC_2016)
  //////////////
  //ID
  //////////////
  //first lepton
  std::vector<float> sfs = GetSF_2DHist(e1.m_lvec.Eta(),e1.m_lvec.Pt(), m_hSF_muonID, m_muonID_w);
  sf *= sfs[0];
  err += sfs[2]*sfs[2]; //relative error
  errID += sfs[2]*sfs[2]; //relative error
  //second lepton
  sfs = GetSF_2DHist(e2.m_lvec.Eta(),e2.m_lvec.Pt(), m_hSF_muonID, m_muonID_w);
  sf *= sfs[0];
  err += sfs[2]*sfs[2];
  errID += sfs[2]*sfs[2];
  
  /////////////////
  //Iso
  /////////////////
  //first muon
  sfs = GetSF_2DHist(e1.m_lvec.Eta(),e1.m_lvec.Pt(), m_hSF_muonIso, m_muonIso_w);
  sf *= sfs[0];
  err += sfs[2]*sfs[2];
  errIso += sfs[2]*sfs[2];
  //second lepton
  sfs = GetSF_2DHist(e2.m_lvec.Eta(),e2.m_lvec.Pt(), m_hSF_muonIso, m_muonIso_w);
  sf *= sfs[0];
  err += sfs[2]*sfs[2];
  errIso += sfs[2]*sfs[2];
#endif
#if defined(MC_2017) || defined(MC_2018)
  ///////////
  //ID
  ///////////
  //first muon
  std::vector<float> sfs = GetSF_2DHist(e1.m_lvec.Pt(),fabs(e1.m_lvec.Eta()), m_hSF_muonID, m_muonID_w);
  sf *= sfs[0];
  err += sfs[2]*sfs[2];
  errID += sfs[2]*sfs[2];
  //second muon
  sfs = GetSF_2DHist(e2.m_lvec.Pt(),fabs(e2.m_lvec.Eta()), m_hSF_muonID, m_muonID_w);
  sf *= sfs[0];
  err += sfs[2]*sfs[2];
  errID += sfs[2]*sfs[2];
  
  ///////////
  //Iso
  ///////////
  //first muon
  sfs = GetSF_2DHist(e1.m_lvec.Pt(),fabs(e1.m_lvec.Eta()), m_hSF_muonIso, m_muonIso_w);
  sf *= sfs[0];
  err += sfs[2]*sfs[2];
  errIso += sfs[2]*sfs[2];
  //second muon
  sfs = GetSF_2DHist(e2.m_lvec.Pt(),fabs(e2.m_lvec.Eta()), m_hSF_muonIso, m_muonIso_w);
  sf *= sfs[0];
  err += sfs[2]*sfs[2];
  errIso += sfs[2]*sfs[2];
#endif
  err = sqrt(err);
  errID = sqrt(errID);
  errIso = sqrt(errIso);
  if (m_muonUncType == "up") sf = sf*(1+err);
  if (m_muonUncType == "down") sf = sf*(1-err);
  if (m_muonUncType == "idup") sf = sf*(1+errID);
  if (m_muonUncType == "iddown") sf = sf*(1-errID);
  if (m_muonUncType == "isoup") sf = sf*(1+errIso);
  if (m_muonUncType == "isodown") sf = sf*(1-errIso);

  return sf ;
}

float Selector::CalSingleMuonSF_id_iso(LepObj e1) {
  float sf(1.0);
  float err(0.0);
  float errID(0.0);
  float errIso(0.0);
#if defined(MC_2016PRE) || defined(MC_2016)
  //////////////
  //ID
  //////////////
  //first lepton
  std::vector<float> sfs = GetSF_2DHist(e1.m_lvec.Eta(),e1.m_lvec.Pt(), m_hSF_muonID, m_muonID_w);
  sf *= sfs[0];
  err += sfs[2]*sfs[2]; //relative error
  errID += sfs[2]*sfs[2]; //relative error
  
  /////////////////
  //Iso
  /////////////////
  //first muon
  sfs = GetSF_2DHist(e1.m_lvec.Eta(),e1.m_lvec.Pt(), m_hSF_muonIso, m_muonIso_w);
  sf *= sfs[0];
  err += sfs[2]*sfs[2];
  errIso += sfs[2]*sfs[2];
#endif
#if defined(MC_2017) || defined(MC_2018)
  ///////////
  //ID
  ///////////
  //first muon
  std::vector<float> sfs = GetSF_2DHist(e1.m_lvec.Pt(),fabs(e1.m_lvec.Eta()), m_hSF_muonID, m_muonID_w);
  sf *= sfs[0];
  err += sfs[2]*sfs[2];
  errID += sfs[2]*sfs[2];
  
  ///////////
  //Iso
  ///////////
  //first muon
  sfs = GetSF_2DHist(e1.m_lvec.Pt(),fabs(e1.m_lvec.Eta()), m_hSF_muonIso, m_muonIso_w);
  sf *= sfs[0];
  err += sfs[2]*sfs[2];
  errIso += sfs[2]*sfs[2];
#endif
  err = sqrt(err);
  errID = sqrt(errID);
  errIso = sqrt(errIso);
  if (m_muonUncType == "up") sf = sf*(1+err);
  if (m_muonUncType == "down") sf = sf*(1-err);
  if (m_muonUncType == "idup") sf = sf*(1+errID);
  if (m_muonUncType == "iddown") sf = sf*(1-errID);
  if (m_muonUncType == "isoup") sf = sf*(1+errIso);
  if (m_muonUncType == "isodown") sf = sf*(1-errIso);

  return sf ;
}
/*
TLorentzVector Selector::GetTrigObj(Reader* r, unsigned id, unsigned bits, float ptThresh) { 

  int id_trigObj = -1 ;
  float maxPt = ptThresh ;

  for (unsigned i = 0 ; i < *(r->nTrigObj) ; ++i) {
    if ((abs(r->TrigObj_id[i]) == id) && ((r->TrigObj_filterBits)[i] & bits)) { //has correct id, bits
      //std::cout << "\n" << (r->TrigObj_id)[i] << " " << (r->TrigObj_filterBits)[i] << "  " << bits ;
      if ((r->TrigObj_pt)[i] > maxPt) { //choose maximum pt trigger object > threshold
        id_trigObj = i ;
        maxPt = (r->TrigObj_pt)[i] ;
      }
    }
  }
  
  TLorentzVector tl_out(0,0,0,0) ;
  float mass(0.1) ;
  if (id == 11) mass = 0.0005 ;
  if (id_trigObj>=0) tl_out.SetPtEtaPhiM((r->TrigObj_pt)[id_trigObj],(r->TrigObj_eta)[id_trigObj],(r->TrigObj_phi)[id_trigObj],mass) ;
  return tl_out ;
}
*/

float Selector::CalTrigSF(int id, LepObj lep1, LepObj lep2, TLorentzVector trigObj, TH1D* h_dR1_trig, TH1D* h_dR2_trig, TH1D* h_pt1_trig, TH1D* h_pt2_trig) {
  
  float trigSF = 1.0 ;
  if (trigObj.Pt() < 0.01) return trigSF ; //empty trigger object
  float dR1 = lep1.m_lvec.DeltaR(trigObj) ;
  float dR2 = lep2.m_lvec.DeltaR(trigObj) ;
  h_dR1_trig->Fill(dR1) ;
  h_dR2_trig->Fill(dR2) ;
  if ((dR1 < dR2) && (dR1 < 0.2)) {
    h_pt1_trig->Fill(lep1.m_lvec.Pt()) ;
    if (id == 13) {
      std::vector<float> sfTmp = GetSF_2DHist(fabs(lep1.m_lvec.Eta()),lep1.m_lvec.Pt(), m_hSF_muonTrig, m_muonTrig_w) ;
      trigSF = sfTmp[0];
      if (m_muonUncType == "trigup") trigSF = sfTmp[0]*(1+sfTmp[2]);
      if (m_muonUncType == "trigdown") trigSF = sfTmp[0]*(1-sfTmp[2]);
    }
    if (id == 11) {
      //SC eta
      std::vector<float> sfTmp = GetSF_2DHist(lep1.m_scEta,lep1.m_lvec.Pt(), m_hSF_eleTrig, m_eleTrig_w);
      trigSF = sfTmp[0];
      if (m_eleUncType == "trigup") trigSF = sfTmp[0]*(1+sfTmp[2]);
      if (m_eleUncType == "trigdown") trigSF = sfTmp[0]*(1-sfTmp[2]);
    }
  }    
  if ((dR2 < dR1) && (dR2 < 0.2)) {
    h_pt2_trig->Fill(lep2.m_lvec.Pt()) ;
    if (id == 13) {
      std::vector<float> sfTmp = GetSF_2DHist(fabs(lep2.m_lvec.Eta()),lep2.m_lvec.Pt(), m_hSF_muonTrig, m_muonTrig_w) ;
      trigSF = sfTmp[0];
      if (m_muonUncType == "trigup") trigSF = sfTmp[0]*(1+sfTmp[2]);
      if (m_muonUncType == "trigdown") trigSF = sfTmp[0]*(1-sfTmp[2]);
    }
    if (id == 11) {
      std::vector<float> sfTmp = GetSF_2DHist(lep2.m_scEta,lep2.m_lvec.Pt(), m_hSF_eleTrig, m_eleTrig_w) ;
      trigSF = sfTmp[0];
      if (m_eleUncType == "trigup") trigSF = sfTmp[0]*(1+sfTmp[2]);
      if (m_eleUncType == "trigdown") trigSF = sfTmp[0]*(1-sfTmp[2]);
    }
  }
  
  return trigSF ;
}

float Selector::CalTrigSF_singleLepton(int id, LepObj lep1, TLorentzVector trigObj) {
  
  float trigSF = 1.0 ;
  if (trigObj.Pt() < 0.01) return trigSF ; //empty trigger object
  float dR1 = lep1.m_lvec.DeltaR(trigObj) ;
  if ((dR1 < 0.2)) {
    if (id == 13) {
      std::vector<float> sfTmp = GetSF_2DHist(fabs(lep1.m_lvec.Eta()),lep1.m_lvec.Pt(), m_hSF_muonTrig, m_muonTrig_w) ;
      trigSF = sfTmp[0];
      if (m_muonUncType == "trigup") trigSF = sfTmp[0]*(1+sfTmp[2]);
      if (m_muonUncType == "trigdown") trigSF = sfTmp[0]*(1-sfTmp[2]);
    }
    if (id == 11) {
      //SC eta
      std::vector<float> sfTmp = GetSF_2DHist(lep1.m_scEta,lep1.m_lvec.Pt(), m_hSF_eleTrig, m_eleTrig_w);
      trigSF = sfTmp[0];
      if (m_eleUncType == "trigup") trigSF = sfTmp[0]*(1+sfTmp[2]);
      if (m_eleUncType == "trigdown") trigSF = sfTmp[0]*(1-sfTmp[2]);
    }
  }    
  
  return trigSF ;
}

/*
#if defined(MC_2016) || defined(MC_2017) || defined(MC_2018)
unsigned Selector::MatchGenLep(Reader* r, LepObj lep, int pdgId) {
  float dRmin(1000) ;
  int indO(-1) ;
  for (unsigned i = 0 ; i < *(r->nGenDressedLepton) ; ++i) {
    if (pdgId == fabs((r->GenDressedLepton_pdgId)[i])) {
      TLorentzVector gLep_lvec ;
      gLep_lvec.SetPtEtaPhiM((r->GenDressedLepton_pt)[i], (r->GenDressedLepton_eta)[i], (r->GenDressedLepton_phi)[i], (r->GenDressedLepton_mass)[i]) ;
      float dRtmp = lep.m_lvec.DeltaR(gLep_lvec) ;
      if (dRtmp < dRmin) {
        dRmin = dRmin ;
        indO = i ;
      }
    }
  }
  return indO ;
}
#endif

float Selector::MuonRcSF(Reader* r, LepObj lep, int pdgId) {
  float sf(1.) ;
  sf = m_rc.kScaleDT(lep.m_charge, lep.m_lvec.Pt(), lep.m_lvec.Eta(), lep.m_lvec.Phi());
#if defined(MC_2016) || defined(MC_2017) || defined(MC_2018)
  sf = 1 ;
  //int gLepInd = MatchGenLep(r, lep, 13) ;
  int gLepInd = r->Muon_genPartIdx[lep.m_idx] ;
  if(gLepInd >= 0) {
    sf = m_rc.kSpreadMC(lep.m_charge, lep.m_lvec.Pt(), lep.m_lvec.Eta(), lep.m_lvec.Phi(),(r->GenPart_pt[gLepInd]));
  }
  else {
    sf = m_rc.kSmearMC(lep.m_charge, lep.m_lvec.Pt(), lep.m_lvec.Eta(), lep.m_lvec.Phi(), (r->Muon_nTrackerLayers)[lep.m_idx], m_rand->Rndm());
  }
#endif
  return sf ;
}
*/

