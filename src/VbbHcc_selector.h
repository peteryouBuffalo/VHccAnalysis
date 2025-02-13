#ifndef VbbHcc_selector_h
#define VbbHcc_selector_h

//Template for creating new selection

#include <bits/stdc++.h>
#include "Reader.h"
#include "Selector.h"
#include "Plots.h"

#include <random>

//The Selection does not have Begin, since we do not have anything to do at the begining (overall) at client
//The histograms, ..., are booked and added to output list at SlaveBegin
//We need to have terminate since we might want to do overall tasks related to this selection only. At termination, all inforamtion from slaves is added. Example task is cutflow for this selection, which need information from all slaves.
//SlaveTerminate and Terminate are here for reservation now. histograms are written back in SlaveTerminate of Processor class

class VbbHcc_selector : public Selector {
public:
  VbbHcc_selector() {};
  virtual ~VbbHcc_selector() ;
    // These methods are called at the corresponding stage of processing of TSelector
  virtual void SlaveBegin(Reader* r) ;
  virtual void Process(Reader* r) ;
  virtual void SlaveTerminate(Reader* r) {} ;
  virtual void Terminate(TList* mergedList, std::string outFileName) ;
  //index of daughters from H and Z
  std::vector<std::vector<int> > DauIdxs_ZH(Reader* r);
  int VZDecayMode(Reader* r);
  int VZDecayModeGroup(int decayMode);
  static bool sortbysecdesc(const std::pair<int,float> &a, const std::pair<int,float> &b);
  static bool sortbysecdesc1(const std::pair<std::vector<int>,float> &a, const std::pair<std::vector<int>,float> &b);
  std::vector<JetObj> NextraJet(std::vector<JetObjBoosted>& fatJets, std::vector<JetObj>& jets);
  
  //double CalculateJESunc(std::map<std::string, float> jetMap, JESUncPlots* plots, int var, bool isData=false);


private:

  std::mt19937 m_random_generator;
  //histograms
  
  TH1D* h_evt_all;
  TH1D* h_evt;
  TH1D* h_evtW_ZccHcc;
  TH1D* h_tagW_ZccHcc;
  TH1D* h_trigW_ZccHcc;
  TH1D* h_evtW_VHcc;
  TH1D* h_tagW_VHcc;
  TH1D* h_trigW_VHcc;
  TH1D* h_bStatus_noMother;
  TH1D* h_bStatus_hasMother;
  TH1D* h_nb;
  TH1D* h_nB;
  TH1D* h_nbB;
  //in the signal regions
  TH1D* h_nb_1;
  TH1D* h_nB_1;
  TH1D* h_nbB_1;
  TH1D* h_cutFlow_ZccHcc_PN_med;
  TH1D* h_cutFlow_VHcc_PN_med;
  TH1D* h_test_MH;
  TH1D* h_test_MZ;
  TH1D* h_NextraJet;
  TH1D* h_NextraJet_ZccHcc;
  TH1D* h_NextraJet_VHcc;
  TH1D* h_triggerCheck;
  TH1D* h_ljpt;
  TH1D* h_ljpt_gen;
  TH3D* h_pt_rho_n2b1;
  TH1D* h_jet_mass;
  TH1D* h_jet_pt;

  TH1D* h_VZtype;
  
  VHBoostedPlots* h_ZccHcc_PN_med; //using particle net medium WP 
  VHBoostedPlots* h_ZccHcc_PN_med_zmass_deltaPhi; //using particle net medium WP
  VHBoostedPlots* h_ZccHcc_PN_med_xccWeight; //using particle net medium WP
  VHBoostedPlots* h_ZccHcc_PN_med_qcdCR; //control regions used in QCD bkg estimation in SR using particle net medium WP
  VHBoostedPlots* h_ZccHcc_PN_med_topCR_pass; //top control regions passing SR tagging requirements using particle net medium WP
  VHBoostedPlots* h_ZccHcc_PN_med_VjetCR_pass; //Vjet control regions passing SR tagging requirements using particle net medium WP
  VHBoostedPlots* h_ZccHcc_PN_med_VZcc; //using particle net medium WP
  VHBoostedPlots* h_ZccHcc_PN_med_VZbb; //using particle net medium WP
  VHBoostedPlots* h_ZccHcc_PN_med_VZother; //using particle net medium WP
  VHBoostedPlots* h_ZccHcc_PN_med_qcdCR_VZcc; //using particle net med_qcdCRium WP
  VHBoostedPlots* h_ZccHcc_PN_med_qcdCR_VZbb; //using particle net med_qcdCRium WP
  VHBoostedPlots* h_ZccHcc_PN_med_qcdCR_VZother; //using particle net med_qcdCRium WP
  VHBoostedPlots* h_ZccHcc_PN_med_topCR_pass_VZcc; //using particle net med_topCR_passium WP
  VHBoostedPlots* h_ZccHcc_PN_med_topCR_pass_VZbb; //using particle net med_topCR_passium WP
  VHBoostedPlots* h_ZccHcc_PN_med_topCR_pass_VZother; //using particle net med_topCR_passium WP

  VHBoostedPlots* h_VHcc_PN_med; //using particle net medium WP
  VHBoostedPlots* h_VHcc_PN_med_zmass_deltaPhi; //using particle net medium WP
  VHBoostedPlots* h_VHcc_PN_med_qcdCR; //control regions used in QCD bkg estimation in SR using particle net medium WP
  VHBoostedPlots* h_VHcc_PN_med_topCR_pass; //top control regions passing SR tagging requirements using particle net medium WP
  VHBoostedPlots* h_VHcc_PN_med_VjetCR_pass; //top control regions passing SR tagging requirements using particle net medium WP
  VHBoostedPlots* h_VHcc_PN_med_VZcc; //using particle net medium WP
  VHBoostedPlots* h_VHcc_PN_med_VZbb; //using particle net medium WP
  VHBoostedPlots* h_VHcc_PN_med_VZother; //using particle net medium WP
  VHBoostedPlots* h_VHcc_PN_med_qcdCR_VZcc; //using particle net qcdCRium WP
  VHBoostedPlots* h_VHcc_PN_med_qcdCR_VZbb; //using particle net qcdCRium WP
  VHBoostedPlots* h_VHcc_PN_med_qcdCR_VZother; //using particle net qcdCRium WP
  VHBoostedPlots* h_VHcc_PN_med_topCR_pass_VZcc; //using particle net topCR_passium WP
  VHBoostedPlots* h_VHcc_PN_med_topCR_pass_VZbb; //using particle net topCR_passium WP
  VHBoostedPlots* h_VHcc_PN_med_topCR_pass_VZother; //using particle net topCR_passium WP
  

  JESUncPlots* h_jesUnc;

} ;

#endif
