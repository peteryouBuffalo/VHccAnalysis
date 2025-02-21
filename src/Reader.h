//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Feb 14 16:02:21 2020 by ROOT version 6.14/09
// from TTree Events/Events
// found on file: 8F93A522-A364-BE4A-8A5D-26591CD081F1.root
//////////////////////////////////////////////////////////

#ifndef Reader_h
#define Reader_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
//#include "SafeTTreeReaderValue.cxx"

// Headers needed by this particular selector


class Reader : public TSelector {
public :
   TTreeReader     fReader;  //!the tree reader
   TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain
   
   // Readers to access the data (delete the ones you do not need).
#if defined(DATA_2016PRE) || defined(DATA_2016) || defined(DATA_2017) || defined(DATA_2018)
   TTreeReaderValue<UInt_t> run = {fReader, "run"};
   TTreeReaderValue<UInt_t> luminosityBlock = {fReader, "luminosityBlock"};
   TTreeReaderValue<ULong64_t> event = {fReader, "event"};
#endif
   TTreeReaderValue<Float_t> fixedGridRhoFastjetAll = {fReader, "fixedGridRhoFastjetAll" };

#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017)
   TTreeReaderValue<Float_t> L1PreFiringWeight_Dn = {fReader, "L1PreFiringWeight_Dn"};
   TTreeReaderValue<Float_t> L1PreFiringWeight_Nom = {fReader, "L1PreFiringWeight_Nom"};
   TTreeReaderValue<Float_t> L1PreFiringWeight_Up = {fReader, "L1PreFiringWeight_Up"};
#endif
///////////////////////////////////////////////
//Trigger
///////////////////////////////////////////////
#if defined(MC_2016PRE) || defined(MC_2016) || defined(DATA_2016PRE) || defined(DATA_2016)
   TTreeReaderValue<Bool_t> HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p20 = {fReader, "HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p20"};
   TTreeReaderValue<Bool_t> HLT_AK8PFJet360_TrimMass30 = {fReader, "HLT_AK8PFJet360_TrimMass30"};
   TTreeReaderValue<Bool_t> HLT_AK8PFJet450 = {fReader, "HLT_AK8PFJet450"};
   TTreeReaderValue<Bool_t> HLT_PFJet450 = {fReader, "HLT_PFJet450"};
   TTreeReaderValue<Bool_t> HLT_AK8PFHT700_TrimR0p1PT0p03Mass50 = {fReader, "HLT_AK8PFHT700_TrimR0p1PT0p03Mass50"};
   TTreeReaderValue<Bool_t> HLT_PFHT900 = {fReader, "HLT_PFHT900"};
  #if !defined(POSTPROC)
   TTreeReaderValue<Bool_t> HLT_IsoMu24 = {fReader, "HLT_IsoMu24"};
   TTreeReaderValue<Bool_t> HLT_IsoTkMu24 = {fReader, "HLT_IsoTkMu24"};
   TTreeReaderValue<Bool_t> HLT_Mu50 = {fReader, "HLT_Mu50"};
   TTreeReaderValue<Bool_t> HLT_TkMu50 = {fReader, "HLT_TkMu50"};
  #endif
  #if !defined(DATA_2016H)
   TTreeReaderValue<Bool_t> HLT_PFHT800 = {fReader, "HLT_PFHT800"};
  #endif
#endif
#if defined(MC_2017) || defined(DATA_2017)
   TTreeReaderValue<Bool_t> HLT_AK8PFJet500 = {fReader, "HLT_AK8PFJet500"};
   TTreeReaderValue<Bool_t> HLT_PFJet500 = {fReader, "HLT_PFJet500"};
   TTreeReaderValue<Bool_t> HLT_PFHT1050 = {fReader, "HLT_PFHT1050"};
  #if !defined(POSTPROC)
   TTreeReaderValue<Bool_t> HLT_Mu27 = {fReader, "HLT_Mu27"};
   TTreeReaderValue<Bool_t> HLT_IsoMu27 = {fReader, "HLT_IsoMu27"};
   TTreeReaderValue<Bool_t> HLT_Mu50 = {fReader, "HLT_Mu50"};
//   TTreeReaderValue<Bool_t> HLT_OldMu100 = {fReader, "HLT_OldMu100"};
//   TTreeReaderValue<Bool_t> HLT_TkMu100 = {fReader, "HLT_TkMu100"};
  #endif
  #if defined(MC_2017) || !defined(DATA_2017B)
   TTreeReaderValue<Bool_t> HLT_AK8PFJet400_TrimMass30 = {fReader, "HLT_AK8PFJet400_TrimMass30"};
   TTreeReaderValue<Bool_t> HLT_AK8PFHT800_TrimMass50 = {fReader, "HLT_AK8PFHT800_TrimMass50"};
  #endif
  #if defined(MC_2017) || defined(DATA_2017F)
   TTreeReaderValue<Bool_t> HLT_AK8PFJet330_PFAK8BTagCSV_p17 = {fReader, "HLT_AK8PFJet330_PFAK8BTagCSV_p17"};
  #endif
#endif
#if defined(MC_2018) || defined(DATA_2018)
   TTreeReaderValue<Bool_t> HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_np4 = {fReader, "HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_np4"};
   TTreeReaderValue<Bool_t> HLT_AK8PFJet400_TrimMass30 = {fReader, "HLT_AK8PFJet400_TrimMass30"};
   TTreeReaderValue<Bool_t> HLT_AK8PFJet500 = {fReader, "HLT_AK8PFJet500"};
   TTreeReaderValue<Bool_t> HLT_PFJet500 = {fReader, "HLT_PFJet500"};
   TTreeReaderValue<Bool_t> HLT_AK8PFHT800_TrimMass50 = {fReader, "HLT_AK8PFHT800_TrimMass50"};
   TTreeReaderValue<Bool_t> HLT_PFHT1050 = {fReader, "HLT_PFHT1050"};
  #if !defined(POSTPROC)
   TTreeReaderValue<Bool_t> HLT_IsoMu24 = {fReader, "HLT_IsoMu24"};
   TTreeReaderValue<Bool_t> HLT_Mu50 = {fReader, "HLT_Mu50"};
   TTreeReaderValue<Bool_t> HLT_OldMu100 = {fReader, "HLT_OldMu100"};
   TTreeReaderValue<Bool_t> HLT_TkMu100 = {fReader, "HLT_TkMu100"};
  #endif
#endif
   //Jet
   TTreeReaderValue<UInt_t> nJet = {fReader, "nJet"};
   TTreeReaderArray<Float_t> Jet_pt = {fReader, "Jet_pt"};
   TTreeReaderArray<Float_t> Jet_eta = {fReader, "Jet_eta"};
   TTreeReaderArray<Float_t> Jet_phi = {fReader, "Jet_phi"};
   TTreeReaderArray<Float_t> Jet_mass = {fReader, "Jet_mass"};
   TTreeReaderArray<Int_t> Jet_jetId = {fReader, "Jet_jetId"};
   TTreeReaderArray<Float_t> Jet_btagDeepB = {fReader, "Jet_btagDeepB"};
   TTreeReaderArray<Float_t> Jet_btagDeepFlavB = {fReader, "Jet_btagDeepFlavB"};
   TTreeReaderArray<Float_t> Jet_puIdDisc = {fReader, "Jet_puIdDisc"};

   //SubJet
   TTreeReaderValue<UInt_t> nSubJet = {fReader, "nSubJet"};
   TTreeReaderArray<Float_t> SubJet_eta = {fReader, "SubJet_eta"};
   TTreeReaderArray<Float_t> SubJet_mass = {fReader, "SubJet_mass"};
   TTreeReaderArray<Float_t> SubJet_phi = {fReader, "SubJet_phi"};
   TTreeReaderArray<Float_t> SubJet_pt = {fReader, "SubJet_pt"};
   TTreeReaderArray<Float_t> SubJet_rawFactor = {fReader, "SubJet_rawFactor"};
  
   //FatJet
   TTreeReaderValue<UInt_t> nFatJet = {fReader, "nFatJet"};
   TTreeReaderArray<Float_t> FatJet_pt = {fReader, "FatJet_pt"};
   TTreeReaderArray<Float_t> FatJet_eta = {fReader, "FatJet_eta"};
   TTreeReaderArray<Float_t> FatJet_phi = {fReader, "FatJet_phi"};
   TTreeReaderArray<Float_t> FatJet_area = {fReader, "FatJet_area"};
   TTreeReaderArray<Float_t> FatJet_mass = {fReader, "FatJet_mass"};
   TTreeReaderArray<Float_t> FatJet_n2b1 = {fReader, "FatJet_n2b1"};
   TTreeReaderArray<Float_t> FatJet_msoftdrop = {fReader, "FatJet_msoftdrop"};
   TTreeReaderArray<Int_t> FatJet_subJetIdx1 = {fReader, "FatJet_subJetIdx1"};
   TTreeReaderArray<Int_t> FatJet_subJetIdx2 = {fReader, "FatJet_subJetIdx2"};
#if defined(NANOAODV7)
   TTreeReaderArray<Float_t> FatJet_btagDDCvB = {fReader, "FatJet_btagDDCvB"};
   TTreeReaderArray<Float_t> FatJet_btagDDCvL = {fReader, "FatJet_btagDDCvL"};
   TTreeReaderArray<Float_t> FatJet_btagDDBvL = {fReader, "FatJet_btagDDBvL"};
#endif
#if defined(NANOAODV9)
   TTreeReaderArray<Float_t> FatJet_btagDDCvB = {fReader, "FatJet_btagDDCvBV2"};
   TTreeReaderArray<Float_t> FatJet_btagDDCvL = {fReader, "FatJet_btagDDCvLV2"};
   TTreeReaderArray<Float_t> FatJet_btagDDBvL = {fReader, "FatJet_btagDDBvLV2"};
   TTreeReaderArray<Float_t> FatJet_particleNetMD_QCD = {fReader, "FatJet_particleNetMD_QCD"};
   TTreeReaderArray<Float_t> FatJet_particleNetMD_Xbb = {fReader, "FatJet_particleNetMD_Xbb"};
   TTreeReaderArray<Float_t> FatJet_particleNetMD_Xcc = {fReader, "FatJet_particleNetMD_Xcc"};
   TTreeReaderArray<Float_t> FatJet_particleNetMD_Xqq = {fReader, "FatJet_particleNetMD_Xqq"};
   TTreeReaderArray<Float_t> FatJet_particleNet_TvsQCD = {fReader, "FatJet_particleNet_TvsQCD"};
   TTreeReaderArray<Float_t> FatJet_particleNet_WvsQCD = {fReader, "FatJet_particleNet_WvsQCD"};
   TTreeReaderArray<Float_t> FatJet_particleNet_ZvsQCD = {fReader, "FatJet_particleNet_ZvsQCD"};
#endif
   TTreeReaderArray<Float_t> FatJet_deepTagMD_ZHccvsQCD = {fReader, "FatJet_deepTagMD_ZHccvsQCD"};
   TTreeReaderArray<Float_t> FatJet_deepTagMD_ZbbvsQCD = {fReader, "FatJet_deepTagMD_ZbbvsQCD"};
   TTreeReaderArray<Float_t> FatJet_rawFactor = {fReader, "FatJet_rawFactor"};

#if defined(POSTPROC) && (defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018))
  TTreeReaderArray<Float_t> FatJet_pt_raw = {fReader, "FatJet_pt_raw"};
  TTreeReaderArray<Float_t> FatJet_pt_nom = {fReader, "FatJet_pt_nom"};
  TTreeReaderArray<Float_t> FatJet_mass_raw = {fReader, "FatJet_mass_raw"};
  TTreeReaderArray<Float_t> FatJet_mass_nom = {fReader, "FatJet_mass_nom"};
  TTreeReaderArray<Float_t> FatJet_corr_JEC = {fReader, "FatJet_corr_JEC"};
  TTreeReaderArray<Float_t> FatJet_corr_JER = {fReader, "FatJet_corr_JER"};
  TTreeReaderArray<Float_t> FatJet_corr_JMS = {fReader, "FatJet_corr_JMS"};
  TTreeReaderArray<Float_t> FatJet_corr_JMR = {fReader, "FatJet_corr_JMR"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_raw = {fReader, "FatJet_msoftdrop_raw"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_nom = {fReader, "FatJet_msoftdrop_nom"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_corr_JMR = {fReader, "FatJet_msoftdrop_corr_JMR"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_corr_JMS = {fReader, "FatJet_msoftdrop_corr_JMS"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_corr_PUPPI = {fReader, "FatJet_msoftdrop_corr_PUPPI"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_tau21DDT_nom = {fReader, "FatJet_msoftdrop_tau21DDT_nom"};
  TTreeReaderArray<Float_t> FatJet_pt_jesTotalUp = {fReader, "FatJet_pt_jesTotalUp"};
  TTreeReaderArray<Float_t> FatJet_pt_jesTotalDown = {fReader, "FatJet_pt_jesTotalDown"};
  TTreeReaderArray<Float_t> FatJet_pt_jer0Up = {fReader, "FatJet_pt_jer0Up"};
  TTreeReaderArray<Float_t> FatJet_pt_jer0Down = {fReader, "FatJet_pt_jer0Down"};
  TTreeReaderArray<Float_t> FatJet_pt_jer1Up = {fReader, "FatJet_pt_jer1Up"};
  TTreeReaderArray<Float_t> FatJet_pt_jer1Down = {fReader, "FatJet_pt_jer1Down"};
  TTreeReaderArray<Float_t> FatJet_pt_jer2Up = {fReader, "FatJet_pt_jer2Up"};
  TTreeReaderArray<Float_t> FatJet_pt_jer2Down = {fReader, "FatJet_pt_jer2Down"};
  TTreeReaderArray<Float_t> FatJet_pt_jer3Up = {fReader, "FatJet_pt_jer3Up"};
  TTreeReaderArray<Float_t> FatJet_pt_jer3Down = {fReader, "FatJet_pt_jer3Down"};
  TTreeReaderArray<Float_t> FatJet_pt_jer4Up = {fReader, "FatJet_pt_jer4Up"};
  TTreeReaderArray<Float_t> FatJet_pt_jer4Down = {fReader, "FatJet_pt_jer4Down"};
  TTreeReaderArray<Float_t> FatJet_mass_jesTotalUp = {fReader, "FatJet_mass_jesTotalUp"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_jesTotalUp = {fReader, "FatJet_msoftdrop_jesTotalUp"};
  TTreeReaderArray<Float_t> FatJet_mass_jesTotalDown = {fReader, "FatJet_mass_jesTotalDown"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_jesTotalDown = {fReader, "FatJet_msoftdrop_jesTotalDown"};
  TTreeReaderArray<Float_t> FatJet_mass_jer0Up = {fReader, "FatJet_mass_jer0Up"};
  TTreeReaderArray<Float_t> FatJet_mass_jer1Up = {fReader, "FatJet_mass_jer1Up"};
  TTreeReaderArray<Float_t> FatJet_mass_jer2Up = {fReader, "FatJet_mass_jer2Up"};
  TTreeReaderArray<Float_t> FatJet_mass_jer3Up = {fReader, "FatJet_mass_jer3Up"};
  TTreeReaderArray<Float_t> FatJet_mass_jer4Up = {fReader, "FatJet_mass_jer4Up"};
  TTreeReaderArray<Float_t> FatJet_mass_jer5Up = {fReader, "FatJet_mass_jer5Up"};
  TTreeReaderArray<Float_t> FatJet_mass_jmrUp = {fReader, "FatJet_mass_jmrUp"};
  TTreeReaderArray<Float_t> FatJet_mass_jmsUp = {fReader, "FatJet_mass_jmsUp"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_jer0Up = {fReader, "FatJet_msoftdrop_jer0Up"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_tau21DDT_jer0Up = {fReader, "FatJet_msoftdrop_tau21DDT_jer0Up"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_jer1Up = {fReader, "FatJet_msoftdrop_jer1Up"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_tau21DDT_jer1Up = {fReader, "FatJet_msoftdrop_tau21DDT_jer1Up"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_jer2Up = {fReader, "FatJet_msoftdrop_jer2Up"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_tau21DDT_jer2Up = {fReader, "FatJet_msoftdrop_tau21DDT_jer2Up"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_jer3Up = {fReader, "FatJet_msoftdrop_jer3Up"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_tau21DDT_jer3Up = {fReader, "FatJet_msoftdrop_tau21DDT_jer3Up"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_jer4Up = {fReader, "FatJet_msoftdrop_jer4Up"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_tau21DDT_jer4Up = {fReader, "FatJet_msoftdrop_tau21DDT_jer4Up"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_jer5Up = {fReader, "FatJet_msoftdrop_jer5Up"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_tau21DDT_jer5Up = {fReader, "FatJet_msoftdrop_tau21DDT_jer5Up"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_jmrUp = {fReader, "FatJet_msoftdrop_jmrUp"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_jmsUp = {fReader, "FatJet_msoftdrop_jmsUp"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_tau21DDT_jmrUp = {fReader, "FatJet_msoftdrop_tau21DDT_jmrUp"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_tau21DDT_jmsUp = {fReader, "FatJet_msoftdrop_tau21DDT_jmsUp"};
  TTreeReaderArray<Float_t> FatJet_mass_jer0Down = {fReader, "FatJet_mass_jer0Down"};
  TTreeReaderArray<Float_t> FatJet_mass_jer1Down = {fReader, "FatJet_mass_jer1Down"};
  TTreeReaderArray<Float_t> FatJet_mass_jer2Down = {fReader, "FatJet_mass_jer2Down"};
  TTreeReaderArray<Float_t> FatJet_mass_jer3Down = {fReader, "FatJet_mass_jer3Down"};
  TTreeReaderArray<Float_t> FatJet_mass_jer4Down = {fReader, "FatJet_mass_jer4Down"};
  TTreeReaderArray<Float_t> FatJet_mass_jer5Down = {fReader, "FatJet_mass_jer5Down"};
  TTreeReaderArray<Float_t> FatJet_mass_jmrDown = {fReader, "FatJet_mass_jmrDown"};
  TTreeReaderArray<Float_t> FatJet_mass_jmsDown = {fReader, "FatJet_mass_jmsDown"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_jer0Down = {fReader, "FatJet_msoftdrop_jer0Down"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_tau21DDT_jer0Down = {fReader, "FatJet_msoftdrop_tau21DDT_jer0Down"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_jer1Down = {fReader, "FatJet_msoftdrop_jer1Down"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_tau21DDT_jer1Down = {fReader, "FatJet_msoftdrop_tau21DDT_jer1Down"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_jer2Down = {fReader, "FatJet_msoftdrop_jer2Down"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_tau21DDT_jer2Down = {fReader, "FatJet_msoftdrop_tau21DDT_jer2Down"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_jer3Down = {fReader, "FatJet_msoftdrop_jer3Down"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_tau21DDT_jer3Down = {fReader, "FatJet_msoftdrop_tau21DDT_jer3Down"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_jer4Down = {fReader, "FatJet_msoftdrop_jer4Down"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_tau21DDT_jer4Down = {fReader, "FatJet_msoftdrop_tau21DDT_jer4Down"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_jer5Down = {fReader, "FatJet_msoftdrop_jer5Down"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_tau21DDT_jer5Down = {fReader, "FatJet_msoftdrop_tau21DDT_jer5Down"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_jmrDown = {fReader, "FatJet_msoftdrop_jmrDown"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_jmsDown = {fReader, "FatJet_msoftdrop_jmsDown"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_tau21DDT_jmrDown = {fReader, "FatJet_msoftdrop_tau21DDT_jmrDown"};
  TTreeReaderArray<Float_t> FatJet_msoftdrop_tau21DDT_jmsDown = {fReader, "FatJet_msoftdrop_tau21DDT_jmsDown"};
#endif

#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018)
  TTreeReaderValue<Float_t> Pileup_nTrueInt = {fReader, "Pileup_nTrueInt"};
  TTreeReaderValue<Float_t> Pileup_pudensity = {fReader, "Pileup_pudensity"};
  TTreeReaderValue<Float_t> genWeight = {fReader, "genWeight"};
  //PDF,SCALE
#if !defined(MC_VV_LO)
  TTreeReaderValue<UInt_t> nLHEPdfWeight = {fReader, "nLHEPdfWeight"};
  TTreeReaderArray<Float_t> LHEPdfWeight = {fReader, "LHEPdfWeight"};
  TTreeReaderValue<UInt_t> nLHEScaleWeight = {fReader, "nLHEScaleWeight"};
  TTreeReaderArray<Float_t> LHEScaleWeight = {fReader, "LHEScaleWeight"};
#endif
  TTreeReaderArray<Int_t> Jet_hadronFlavour = {fReader, "Jet_hadronFlavour"};
  TTreeReaderArray<Int_t> FatJet_hadronFlavour = {fReader, "FatJet_hadronFlavour"};
  TTreeReaderValue<UInt_t> nGenPart = {fReader, "nGenPart"};
  TTreeReaderArray<Float_t> GenPart_eta = {fReader, "GenPart_eta"};
  TTreeReaderArray<Float_t> GenPart_mass = {fReader, "GenPart_mass"};
  TTreeReaderArray<Float_t> GenPart_phi = {fReader, "GenPart_phi"};
  TTreeReaderArray<Float_t> GenPart_pt = {fReader, "GenPart_pt"};
  TTreeReaderArray<Int_t> GenPart_genPartIdxMother = {fReader, "GenPart_genPartIdxMother"};
  TTreeReaderArray<Int_t> GenPart_pdgId = {fReader, "GenPart_pdgId"};
  TTreeReaderValue<UInt_t> nGenJetAK8 = {fReader, "nGenJetAK8"};
  TTreeReaderArray<Float_t> GenJetAK8_eta = {fReader, "GenJetAK8_eta"};
  TTreeReaderArray<Float_t> GenJetAK8_mass = {fReader, "GenJetAK8_mass"};
  TTreeReaderArray<Float_t> GenJetAK8_phi = {fReader, "GenJetAK8_phi"};
  TTreeReaderArray<Float_t> GenJetAK8_pt = {fReader, "GenJetAK8_pt"};
  TTreeReaderArray<UChar_t> GenJetAK8_hadronFlavour = {fReader, "GenJetAK8_hadronFlavour"};
  TTreeReaderArray<Int_t> GenPart_status = {fReader, "GenPart_status"};
  //TTreeReaderArray<Int_t> GenPart_statusFlags = {fReader, "GenPart_statusFlags"};
  //TTreeReaderValue<UInt_t> nGenJet = {fReader, "nGenJet"};
  //TTreeReaderArray<Float_t> GenJet_eta = {fReader, "GenJet_eta"};
  //TTreeReaderArray<Float_t> GenJet_pt = {fReader, "GenJet_pt"};

  TTreeReaderValue<UInt_t> nGenJet = {fReader, "nGenJet"};
  TTreeReaderArray<Float_t> GenJet_eta = {fReader, "GenJet_eta"};
  TTreeReaderArray<Float_t> GenJet_pt = {fReader, "GenJet_pt"};
  TTreeReaderArray<Float_t> GenJet_mass = {fReader, "GenJet_mass"};
  TTreeReaderArray<Float_t> GenJet_phi = {fReader, "GenJet_phi"};
  TTreeReaderArray<UChar_t> GenJet_hadronFlavour = {fReader, "GenJet_hadronFlavour"};
#endif

   // Electron
   TTreeReaderValue<UInt_t> nElectron = {fReader, "nElectron"};
   TTreeReaderArray<Float_t> Electron_pt = {fReader, "Electron_pt"};
   TTreeReaderArray<Float_t> Electron_eta = {fReader, "Electron_eta"};
   TTreeReaderArray<Float_t> Electron_phi = {fReader, "Electron_phi"};
   TTreeReaderArray<Float_t> Electron_mass = {fReader, "Electron_mass"};
   TTreeReaderArray<Int_t> Electron_charge = {fReader, "Electron_charge"};
   TTreeReaderArray<Float_t> Electron_deltaEtaSC = {fReader, "Electron_deltaEtaSC"};
   TTreeReaderArray<Int_t> Electron_cutBased = {fReader, "Electron_cutBased"};
   TTreeReaderArray<Float_t> Electron_dz = {fReader, "Electron_dz"};
   TTreeReaderArray<Float_t> Electron_dxy = {fReader, "Electron_dxy"};

   // Muon
   TTreeReaderValue<UInt_t> nMuon = {fReader, "nMuon"};
   TTreeReaderArray<Float_t> Muon_pt = {fReader, "Muon_pt"};
   TTreeReaderArray<Float_t> Muon_eta = {fReader, "Muon_eta"};
   TTreeReaderArray<Float_t> Muon_phi = {fReader, "Muon_phi"};
   TTreeReaderArray<Float_t> Muon_mass = {fReader, "Muon_mass"};
   TTreeReaderArray<Int_t> Muon_charge = {fReader, "Muon_charge"};
   TTreeReaderArray<Float_t> Muon_pfRelIso04_all = {fReader, "Muon_pfRelIso04_all"};
   TTreeReaderArray<Bool_t> Muon_mediumId = {fReader, "Muon_mediumId"};
#if !defined(POSTPROC)
   TTreeReaderArray<Bool_t> Muon_looseId = {fReader, "Muon_looseId"};
#endif

   //Tau
   TTreeReaderValue<UInt_t> nTau = {fReader, "nTau"};
   TTreeReaderArray<Float_t> Tau_eta = {fReader, "Tau_eta"};
   TTreeReaderArray<Float_t> Tau_pt = {fReader, "Tau_pt"};
   TTreeReaderArray<Float_t> Tau_phi = {fReader, "Tau_phi"};
   TTreeReaderArray<Float_t> Tau_mass = {fReader, "Tau_mass"};
   TTreeReaderArray<Int_t> Tau_charge = {fReader, "Tau_charge"};
   TTreeReaderArray<Float_t> Tau_chargedIso = {fReader, "Tau_chargedIso"};
   TTreeReaderArray<Int_t> Tau_decayMode = {fReader, "Tau_decayMode"};
   TTreeReaderArray<UChar_t> Tau_idDeepTau2017v2p1VSe = {fReader, "Tau_idDeepTau2017v2p1VSe"};
   TTreeReaderArray<UChar_t> Tau_idDeepTau2017v2p1VSjet = {fReader, "Tau_idDeepTau2017v2p1VSjet"};
   TTreeReaderArray<UChar_t> Tau_idDeepTau2017v2p1VSmu = {fReader, "Tau_idDeepTau2017v2p1VSmu"};

   //MET
   TTreeReaderValue<Float_t> MET_pt = {fReader, "MET_pt"};
   TTreeReaderValue<Float_t> MET_phi = {fReader, "MET_phi"};

   //bool hasBranch;

   Reader(TTree * /*tree*/ =0) {}

   virtual ~Reader() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();

   ClassDef(Reader,0);

};

#endif

/*
#ifdef Reader_cxx
void Reader::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the reader is initialized.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   fReader.SetTree(tree);
}

Bool_t Reader::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}


#endif // #ifdef Reader_cxx
*/
