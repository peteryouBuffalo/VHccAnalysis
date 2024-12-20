#ifndef VbbHcc_triggerSel_h
#define VbbHcc_triggerSel_h

//Template for creating new selection

#include <bits/stdc++.h>
#include "Reader.h"
#include "Selector.h"
#include "Plots.h"

//The Selection does not have Begin, since we do not have anything to do at the begining (overall) at client
//The histograms, ..., are booked and added to output list at SlaveBegin
//We need to have terminate since we might want to do overall tasks related to this selection only. At termination, all inforamtion from slaves is added. Example task is cutflow for this selection, which need information from all slaves.
//SlaveTerminate and Terminate are here for reservation now. histograms are written back in SlaveTerminate of Processor class

class VbbHcc_triggerSel : public Selector {
public:
  VbbHcc_triggerSel() {};
  virtual ~VbbHcc_triggerSel() ;
    // These methods are called at the corresponding stage of processing of TSelector
  virtual void SlaveBegin(Reader* r) ;
  virtual void Process(Reader* r) ;
  virtual void SlaveTerminate(Reader* r) {} ;
  virtual void Terminate(TList* mergedList, std::string outFileName) ;
  //index of daughters from H and Z
  std::vector<std::vector<int> > DauIdxs_ZH(Reader* r);
  static bool sortbysecdesc(const std::pair<int,float> &a, const std::pair<int,float> &b);
  static bool sortbysecdesc1(const std::pair<std::vector<int>,float> &a, const std::pair<std::vector<int>,float> &b);
  int NextraJet(std::vector<JetObjBoosted>& fatJet, std::vector<JetObj>& jets);


private:
  
  //histograms
  TH1D* h_evt_all;
  TH1D* h_evt;
  std::map<std::string,TH3D*> h_pt_msd_xbb_pass;
  std::map<std::string,TH3D*> h_pt_msd_xbb_tot;
  std::map<std::string,TH3D*> h_pt_msd_xcc_pass;
  std::map<std::string,TH3D*> h_pt_msd_xcc_tot;
  std::map<std::string,TH3D*> h_pt1_pt2_ht_tot;
  std::map<std::string,TH3D*> h_pt1_pt2_ht_pass;

} ;

#endif
