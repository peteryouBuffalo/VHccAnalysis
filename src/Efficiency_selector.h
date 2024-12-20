#ifndef Efficiency_selector_h
#define Efficiency_selector_h

//Template for creating new selection

#include "Reader.h"
#include "Selector.h"
#include "Plots.h"

#include "Obj.cxx"

//The Selection does not have Begin, since we do not have anything to do at the begining (overall) at client
//The histograms, ..., are booked and added to output list at SlaveBegin
//We need to have terminate since we might want to do overall tasks related to this selection only. At termination, all inforamtion from slaves is added. Example task is cutflow for this selection, which need information from all slaves.
//SlaveTerminate and Terminate are here for reservation now. histograms are written back in SlaveTerminate of Processor class


class Efficiency_selector : public Selector {
public:
  Efficiency_selector() {};
  virtual ~Efficiency_selector() ;
    // These methods are called at the corresponding stage of processing of TSelector
  virtual void SlaveBegin(Reader* r) ;
  virtual void Process(Reader* r) ;
  virtual void SlaveTerminate(Reader* r) {} ;
  virtual void Terminate(TList* mergedList, std::string outFileName) ;
  virtual void Fill_btagEffi(std::vector<JetObj> jets, std::string bTagWP, float w) ;
private:
  //histograms
  TH1D* h_evt_all;
  BoostedJetEffPlots* h_eff_xbb_bb;
  BoostedJetEffPlots* h_eff_xbb_cc;
  BoostedJetEffPlots* h_eff_xbb_light;
  BoostedJetEffPlots* h_eff_xcc_bb;
  BoostedJetEffPlots* h_eff_xcc_cc;
  BoostedJetEffPlots* h_eff_xcc_light;
  EffPlots* h_eff_bdj ;
  EffPlots* h_eff_cdj ;
  EffPlots* h_eff_ldj ;
 


} ;

#endif
