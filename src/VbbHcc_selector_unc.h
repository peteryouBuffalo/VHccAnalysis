#ifndef VbbHcc_selector_unc_h
#define VbbHcc_selector_unc_h

//Template for creating new selection

#include <bits/stdc++.h>
#include "Reader.h"
#include "Selector.h"
#include "Plots.cxx"

//The Selection does not have Begin, since we do not have anything to do at the begining (overall) at client
//The histograms, ..., are booked and added to output list at SlaveBegin
//We need to have terminate since we might want to do overall tasks related to this selection only. At termination, all inforamtion from slaves is added. Example task is cutflow for this selection, which need information from all slaves.
//SlaveTerminate and Terminate are here for reservation now. histograms are written back in SlaveTerminate of Processor class

class VbbHcc_selector_unc : public Selector {
public:
  VbbHcc_selector_unc() {};
  virtual ~VbbHcc_selector_unc() ;
    // These methods are called at the corresponding stage of processing of TSelector
  virtual void SlaveBegin(Reader* r) ;
  virtual void Process(Reader* r) ;
  virtual void SlaveTerminate(Reader* r) {} ;
  virtual void Terminate(TList* mergedList, std::string outFileName) ;
  //index of daughters from H and Z
  std::vector<std::vector<int> > DauIdxs_ZH(Reader* r);
  static bool sortbysecdesc(const std::pair<int,float> &a, const std::pair<int,float> &b);
  static bool sortbysecdesc1(const std::pair<std::vector<int>,float> &a, const std::pair<std::vector<int>,float> &b);
  int NextraJet(std::vector<JetObjBoosted>& fatJets, std::vector<JetObj>& jets);


private:
  //histograms
  
  TH1D* h_evt_all;
  TH1D* h_evt;
  TH1D* h_bStatus_noMother;
  TH1D* h_bStatus_hasMother;
  TH1D* h_nb;
  TH1D* h_nB;
  TH1D* h_nbB;
  //in the signal regions
  TH1D* h_nb_1;
  TH1D* h_nB_1;
  TH1D* h_nbB_1;
  TH1D* h_cutFlow;
  TH1D* h_cutFlow_hcc;
  TH1D* h_cutFlow_VbbHcc_select1;
  TH1D* h_cutFlow_VbbHcc_select2;
  TH1D* h_cutFlow_VbbHcc_select3;
  TH1D* h_cutFlow_VbbHcc_select4;
  TH1D* h_cutFlow_VbbHcc_PN_med;
  TH1D* h_test_MH;
  TH1D* h_test_MZ;
  TH1D* h_NextraJet;
  TH1D* h_triggerCheck;
  TH1D* h_ljpt;
  TH1D* h_ljpt_gen;
  TH3D* h_pt_rho_n2b1;
  TH1D* h_jet_mass;
  VHBoostedPlots* h_VbbHcc;
  VHBoostedPlots* h_VbbHcc_twojets;
  VHBoostedPlots* h_VbbHcc_select1;
  VHBoostedPlots* h_VbbHcc_select2;
  VHBoostedPlots* h_VbbHcc_select3;
  VHBoostedPlots* h_VbbHcc_select4;
  VHBoostedPlots* h_VbbHcc_PN_med; //using particle net medium WP
  VHBoostedPlots* h_VbbHcc_PN_med_xccWeight; //using particle net medium WP
  VHBoostedPlots* h_VbbHcc_PN_med_qcdCR; //control regions used in QCD bkg estimation in SR using particle net medium WP
  VHBoostedPlots* h_VbbHcc_PN_med_topCR_pass; //top control regions passing SR tagging requirements using particle net medium WP
  VHBoostedPlots* h_VbbHcc_PN_med_topCR_fail; //top control regions fail SR tagging requirements using particle net medium WP
  VHBoostedPlots* h_VbbHcc_PN_med_VR; //validation region using particle net medium WP
  VHBoostedPlots* h_VbbHcc_PN_med_qcdCR1; //QCD CR1 region used for validation using particle net medium WP
  VHBoostedPlots* h_VbbHcc_PN_med_topCR1_pass; //top CR1 to be used with VR (passing tagging requirements for VR) using particle net medium WP
  VHBoostedPlots* h_VbbHcc_PN_med_topCR1_fail; //top CR1 to be used with VR (fail tagging requirements for VR) using particle net medium WP
  VHBoostedPlots* h_VbbHcc_qcd;
  VHBoostedPlots* h_VbbHcc_qcd_1;
  VHBoostedPlots* h_VbbHcc_qcd_2;
  VHBoostedPlots* h_VbbHcc_qcd_3;
  HBoostedPlots* h_Hcc;
  BoostedJetEffPlots* h_eff_xbb_bb;
  BoostedJetEffPlots* h_eff_xbb_cc;
  BoostedJetEffPlots* h_eff_xbb_light;
  BoostedJetEffPlots* h_eff_xcc_bb;
  BoostedJetEffPlots* h_eff_xcc_cc;
  BoostedJetEffPlots* h_eff_xcc_light;
} ;

#endif
