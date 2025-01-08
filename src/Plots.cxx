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
        if (iPdf < *(r->nLHEPdfWeight)) pdfW = (r->LHEPdfWeight)[iPdf];
        h_HMass_LHEPdfs[i]->Fill(H.m_lvec.M(),w*pdfW); 
      }
      for (int i = 0 ; i < nLHEscales ; ++i) {
        //float scaleW = 1.;
        //PDF,SCALE
        float scaleW = (r->LHEScaleWeight)[i];
        h_HMass_LHEScales[i]->Fill(H.m_lvec.M(),w*scaleW);      
      }
}


#endif
