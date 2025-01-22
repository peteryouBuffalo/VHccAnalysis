#ifndef Plots_cxx
#define Plots_cxx

#include "Plots.h"
//#include "Obj.cxx"
//#include "Reader.h"

void VHBoostedPlots::FillPdfScaleUnc(Reader* r, HObj& H, int iPdfStart, int iPdfStop, int nLHEscales, float w) {
      for (int i = 0 ; i < (iPdfStop - iPdfStart) ; ++i) {
        float pdfW = 1.0;
        int iPdf = i+iPdfStart;
        //PDF,SCALE
  #if defined(MC_2016) || defined(MC_2016PRE) || defined(MC_2017) || defined(MC_2018)
        if (iPdf < *(r->nLHEPdfWeight)) pdfW = (r->LHEPdfWeight)[iPdf];
  #endif
        h_HMass_LHEPdfs[i]->Fill(H.m_lvec.M(),w*pdfW); 
      }
      
  #if defined(MC_2016) || defined(MC_2016PRE) || defined(MC_2017) || defined(MC_2018)
      for (int i = 0 ; i < nLHEscales ; ++i) {
        //float scaleW = 1.;
        //PDF,SCALE
        float scaleW = (r->LHEScaleWeight)[i];
        h_HMass_LHEScales[i]->Fill(H.m_lvec.M(),w*scaleW);      
      }
  #endif
}


#endif
