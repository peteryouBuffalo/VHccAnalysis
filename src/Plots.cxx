#ifndef Plots_cxx
#define Plots_cxx

#include "Obj.cxx"
#include "TString.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TMath.h"
#include "TLorentzVector.h"
#include <TTreeReaderArray.h>

// These constants define the number of bins
// & pT ranges we want for different variables.
const unsigned NBIN_PT_JET = 2000;
const float X_PT_JET[2] = {0, 2000};
const unsigned NBIN_PT_Z = 2000;
const float X_PT_Z[2] = {0, 2000};
const unsigned NBIN_ETA = 500;
const float X_ETA[2] = {-2.5, 2.5};
const unsigned NBIN_PHI = 480;
const float X_PHI[2] = {-4.0, 4.0};

const float NBIN_M_J = 300;
const float X_M_J[2] = {0, 300};
const float NBIN_M_H = 300;
const float X_M_H[2] = {0, 300};
const float NBIN_M_Z = 300;
const float X_M_Z[2] = {0, 300};

const float NBIN_SF = 200;
const float X_SF[2] = {0, 2};
const float NBIN_UNC = 50;
const float X_UNC[2] = {0, 0.5};


// Main Plots class
class VHPlots
{

  public:
    // Constructor
    VHPlots(TString name) : m_name(name) {
 
      h_pt_jet = new TH1D(name + "_pt_jet", "", NBIN_PT_JET, X_PT_JET[0], X_PT_JET[1]);
      h_eta_jet = new TH1D(name + "_eta_jet", "", NBIN_ETA, X_ETA[0], X_ETA[1]);
      h_phi_jet = new TH1D(name + "_phi_jet", "", NBIN_PHI, X_PHI[0], X_PHI[1]);
      h_mSV_jet = new TH1D(name + "_mSV_jet", "", 100, 0, 10);
      h_Njet = new TH1D(name + "_Njet", "", 15, 0, 15);   
      
      h_HPt = new TH1D(name + "_HPt", "", NBIN_PT_JET, X_PT_JET[0], X_PT_JET[1]);
      h_HEta = new TH1D(name + "_HEta", "", NBIN_ETA, X_ETA[0], X_ETA[1]);
      h_HMass = new TH1D(name + "_HMass", "", NBIN_M_H, X_M_H[0], X_M_H[1]);
      h_ZPt = new TH1D(name + "_ZPt", "", NBIN_PT_JET, X_PT_JET[0], X_PT_JET[1]);
      h_ZEta = new TH1D(name + "_ZEta", "", NBIN_ETA, X_ETA[0], X_ETA[1]);
      h_ZMass = new TH1D(name + "_ZMass", "", NBIN_M_Z, X_M_Z[0], X_M_Z[1]);

      h_dR_H = new TH1D(name + "_dR_H", "", 100, 0, 10);
      h_dR_Z = new TH1D(name + "_dR_Z", "", 100, 0, 10);
      h_dPhi_H = new TH1D(name + "_dPhi_H", "", 60, -TMath::Pi(), TMath::Pi());
      h_dPhi_Z = new TH1D(name + "_dPhi_Z", "", 60, -TMath::Pi(), TMath::Pi());

      h_pt_jet->Sumw2();
      h_eta_jet->Sumw2();
      h_phi_jet->Sumw2();
      h_mSV_jet->Sumw2();
      h_Njet->Sumw2();
      h_HMass->Sumw2();
      h_ZMass->Sumw2();
      h_dR_H->Sumw2();
      h_dR_Z->Sumw2();
      h_dPhi_H->Sumw2();
      h_dPhi_Z->Sumw2();
    } ;

    // Fill the general histograms.
    void Fill(HObj& H, ZObj& Z, float w=1) {

      h_HMass->Fill(H.m_lvec.M(), w);
      h_HPt->Fill(H.m_lvec.Pt(), w);
      h_HEta->Fill(H.m_lvec.Eta(), w);
      h_ZMass->Fill(Z.m_lvec.M(), w);
      h_ZPt->Fill(Z.m_lvec.Pt(), w);
      h_ZEta->Fill(Z.m_lvec.Eta(), w);
      
      if (H.nJets() >= 2) {
        h_dR_H->Fill(H.DeltaR(), w);
        h_dPhi_H->Fill(H.DPhi(), w);
      }
      if (Z.nJets() >= 2) {
        h_dR_Z->Fill(Z.DeltaR(), w);
        h_dPhi_Z->Fill(Z.DPhi(), w);
      }
    };

    // Fill stuff about jets.
    void FillJets(std::vector<JetObj>& jets, float w=1) {
      for (auto it : jets) {
        h_pt_jet->Fill(it.m_lvec.Pt(), w);
        h_eta_jet->Fill(it.m_lvec.Eta(), w);
        h_phi_jet->Fill(it.m_lvec.Phi(), w);
        h_mSV_jet->Fill(it.m_mSV, w);
      }
    }

    void FillNjet(size_t nJet, float w=1) {
      h_Njet->Fill(nJet, w);
    }

    // Return a list of all the histograms.
    std::vector<TH1*> returnHisto() {
      std::vector<TH1*> histolist;
 
      // Jet-Related Plots
      histolist.push_back(h_pt_jet);
      histolist.push_back(h_eta_jet);
      histolist.push_back(h_phi_jet);
      histolist.push_back(h_mSV_jet);
      histolist.push_back(h_Njet);
     
      // V and H plots
      histolist.push_back(h_HMass);
      histolist.push_back(h_HPt);
      histolist.push_back(h_HEta);
      histolist.push_back(h_ZMass);
      histolist.push_back(h_ZPt);
      histolist.push_back(h_ZEta);

      // VH-Related Plots
      histolist.push_back(h_dR_H);
      histolist.push_back(h_dR_Z);
      histolist.push_back(h_dPhi_H);
      histolist.push_back(h_dPhi_Z);

      return histolist;
    }

  protected:
    // Variables
    TString m_name;
    
    //Higgs Plots
    TH1D* h_HPt;    
    TH1D* h_HEta;    
    TH1D* h_HMass;    
    TH1D* h_dR_H;
    TH1D* h_dPhi_H;

    //Z Plots
    TH1D* h_ZPt;
    TH1D* h_ZEta;
    TH1D* h_ZMass;
    TH1D* h_dR_Z;
    TH1D* h_dPhi_Z;
    
    //VH Plots
    TH1D* h_pt_jet;
    TH1D* h_eta_jet;
    TH1D* h_phi_jet;
    TH1D* h_mSV_jet;
    TH1D* h_Njet;
} ;

// Main Plots class
class VHBoostedPlots
{

  public:
    // Constructor
    VHBoostedPlots(TString name) : m_name(name) {
 
      h_pt_jet = new TH1D(name + "_pt_jet", "", NBIN_PT_JET, X_PT_JET[0], X_PT_JET[1]);
      h_eta_jet = new TH1D(name + "_eta_jet", "", NBIN_ETA, X_ETA[0], X_ETA[1]);
      h_phi_jet = new TH1D(name + "_phi_jet", "", NBIN_PHI, X_PHI[0], X_PHI[1]);
      h_m_jet = new TH1D(name + "_m_jet", "", NBIN_M_J, X_M_J[0], X_M_J[1]);
      h_flav_jet = new TH1D(name + "_flav_jet", "", 10, 0, 10);
      
      h_pt_jet0 = new TH1D(name + "_pt_jet0", "", NBIN_PT_JET, X_PT_JET[0], X_PT_JET[1]);
      h_eta_jet0 = new TH1D(name + "_eta_jet0", "", NBIN_ETA, X_ETA[0], X_ETA[1]);
      h_phi_jet0 = new TH1D(name + "_phi_jet0", "", NBIN_PHI, X_PHI[0], X_PHI[1]);
      h_m_jet0 = new TH1D(name + "_m_jet0", "", NBIN_M_J, X_M_J[0], X_M_J[1]);
      h_flav_jet0 = new TH1D(name + "_flav_jet0", "", 10, 0, 10);
      
      h_pt_jet1 = new TH1D(name + "_pt_jet1", "", NBIN_PT_JET, X_PT_JET[0], X_PT_JET[1]);
      h_eta_jet1 = new TH1D(name + "_eta_jet1", "", NBIN_ETA, X_ETA[0], X_ETA[1]);
      h_phi_jet1 = new TH1D(name + "_phi_jet1", "", NBIN_PHI, X_PHI[0], X_PHI[1]);
      h_m_jet1 = new TH1D(name + "_m_jet1", "", NBIN_M_J, X_M_J[0], X_M_J[1]);
      h_flav_jet1 = new TH1D(name + "_flav_jet1", "", 10, 0, 10);

      h_bbTagDis_beforeCut = new TH1D(name + "_bbTagDis_beforeCut", "", 1000, 0, 1);
      h_ccTagDis_beforeCut = new TH1D(name + "_ccTagDis_beforeCut", "", 1000, 0, 1);
      h_pQCD_beforeCut = new TH1D(name + "_pQCD_beforeCut", "", 1000, 0, 1);
      h_bbTagDis = new TH1D(name + "_bbTagDis", "", 1000, 0, 1);
      h_ccTagDis = new TH1D(name + "_ccTagDis", "", 1000, 0, 1);
      h_NextraJet_beforeCut = new TH1D(name+"_NextraJet_beforeCut","",10,0,10);
      h_nBjet_passNextraJetCut = new TH1D(name+"_nBjet_passNextraJetCut","",10,0,10);
      h_nBjet_failNextraJetCut = new TH1D(name+"_nBjet_failNextraJetCut","",10,0,10);
      h_nBjetExtraJet_passNextraJetCut = new TH1D(name+"_nBjetExtraJet_passNextraJetCut","",10,0,10);
      h_nBjetExtraJet_failNextraJetCut = new TH1D(name+"_nBjetExtraJet_failNextraJetCut","",10,0,10);
      h_MET_beforeCut = new TH1D(name+"_MET_beforeCut","",1000,0,1000);
      h_MET_beforeCut_1 = new TH1D(name+"_MET_beforeCut_1","",1000,0,1000);
      h_MET = new TH1D(name+"_MET","",1000,0,1000);
      h_ccPN_TopvsQCD= new TH1D(name + "_ccPN_TopvsQCD", "", 1000, 0, 1);
      h_ccPN_WvsQCD= new TH1D(name + "_ccPN_WvsQCD", "", 1000, 0, 1);
      h_ccPN_ZvsQCD= new TH1D(name + "_ccPN_ZvsQCD", "", 1000, 0, 1);
      h_bbPN_TopvsQCD= new TH1D(name + "_bbPN_TopvsQCD", "", 1000, 0, 1);
      h_bbPN_WvsQCD= new TH1D(name + "_bbPN_WvsQCD", "", 1000, 0, 1);
      h_bbPN_ZvsQCD= new TH1D(name + "_bbPN_ZvsQCD", "", 1000, 0, 1);
      h_HPt = new TH1D(name + "_HPt", "", NBIN_PT_JET, X_PT_JET[0], X_PT_JET[1]);
      h_HEta = new TH1D(name + "_HEta", "", NBIN_ETA, X_ETA[0], X_ETA[1]);
      h_HMass = new TH1D(name + "_HMass", "", NBIN_M_H, X_M_H[0], X_M_H[1]);
      h_HFlav = new TH1D(name + "_HFlav", "", 10, 0, 10);
      h_ZPt = new TH1D(name + "_ZPt", "", NBIN_PT_JET, X_PT_JET[0], X_PT_JET[1]);
      h_ZEta = new TH1D(name + "_ZEta", "", NBIN_ETA, X_ETA[0], X_ETA[1]);
      h_ZMass = new TH1D(name + "_ZMass", "", NBIN_M_Z, X_M_Z[0], X_M_Z[1]);
      h_ZFlav = new TH1D(name + "_ZFlav", "", 10, 0, 10);
      h_HMassZMass = new TH2D(name + "_HMassZMass", "", NBIN_M_H, X_M_H[0], X_M_H[1], NBIN_M_Z, X_M_Z[0], X_M_Z[1]);
      h_DEta = new TH1D(name + "_DEtaZH", "", 1000, -5, 5);
      h_DPhi = new TH1D(name + "_DPhiZH", "", 314, 0, TMath::Pi());
      h_DR = new TH1D(name + "_DRZH", "", 500, 0, 5);
      
      h_DPhiJetMet_passNextraJetCut = new TH1D(name + "_DPhiJetMet_passNextraJetCut", "", 314, 0, TMath::Pi());
      h_DPhiJetMet_failNextraJetCut = new TH1D(name + "_DPhiJetMet_failNextraJetCut", "", 314, 0, TMath::Pi());

      h_pt_jet->Sumw2();
      h_eta_jet->Sumw2();
      h_phi_jet->Sumw2();
      h_m_jet->Sumw2();
      h_flav_jet->Sumw2();
      h_pt_jet0->Sumw2();
      h_eta_jet0->Sumw2();
      h_phi_jet0->Sumw2();
      h_m_jet0->Sumw2();
      h_flav_jet0->Sumw2();
      h_pt_jet1->Sumw2();
      h_eta_jet1->Sumw2();
      h_phi_jet1->Sumw2();
      h_m_jet1->Sumw2();
      h_flav_jet1->Sumw2();
      h_HPt->Sumw2();
      h_HEta->Sumw2();
      h_HMass->Sumw2();
      h_HFlav->Sumw2();
      h_ZPt->Sumw2();
      h_ZEta->Sumw2();
      h_ZMass->Sumw2();
      h_ZFlav->Sumw2();
      h_HMassZMass->Sumw2();
      h_DEta->Sumw2();
      h_DPhi->Sumw2();
      h_DPhiJetMet_passNextraJetCut->Sumw2(); 
      h_DPhiJetMet_failNextraJetCut->Sumw2();
      h_DR->Sumw2();
      h_bbTagDis_beforeCut->Sumw2();
      h_bbTagDis->Sumw2();
      h_ccTagDis_beforeCut->Sumw2();
      h_ccTagDis->Sumw2();
      h_pQCD_beforeCut->Sumw2();
      h_NextraJet_beforeCut->Sumw2();
      h_nBjet_passNextraJetCut->Sumw2(); 
      h_nBjet_failNextraJetCut->Sumw2();
      h_nBjetExtraJet_passNextraJetCut->Sumw2();
      h_nBjetExtraJet_failNextraJetCut->Sumw2();
      h_MET_beforeCut->Sumw2();
      h_MET_beforeCut_1->Sumw2();
      h_MET->Sumw2();
      h_ccPN_TopvsQCD->Sumw2();
      h_ccPN_WvsQCD->Sumw2();
      h_ccPN_ZvsQCD->Sumw2();
      h_bbPN_TopvsQCD->Sumw2();
      h_bbPN_WvsQCD->Sumw2();
      h_bbPN_ZvsQCD->Sumw2();
    } ;

    // Fill the general histograms.
    void Fill(HObj& H, ZObj& Z, float w=1) {

      h_HMass->Fill(H.m_lvec.M(), w);
      h_HPt->Fill(H.m_lvec.Pt(), w);
      h_HEta->Fill(H.m_lvec.Eta(), w);
      h_HFlav->Fill(H.m_flav, w);
      h_ZMass->Fill(Z.m_lvec.M(), w);
      h_ZPt->Fill(Z.m_lvec.Pt(), w);
      h_ZEta->Fill(Z.m_lvec.Eta(), w);
      h_ZFlav->Fill(Z.m_flav, w);
      h_HMassZMass->Fill(H.m_lvec.M(),Z.m_lvec.M(), w);
      h_DR->Fill(H.m_lvec.DeltaR(Z.m_lvec), w);
      h_DPhi->Fill(fabs(H.m_lvec.DeltaPhi(Z.m_lvec)), w);
      h_DEta->Fill(H.m_lvec.Eta()-Z.m_lvec.Eta(), w);
    };

    // Fill stuff about jets.
    void FillJets(std::vector<JetObjBoosted>& jets, float w=1) {
      for (auto it : jets) {
        h_pt_jet->Fill(it.m_lvec.Pt(), w);
        h_eta_jet->Fill(it.m_lvec.Eta(), w);
        h_phi_jet->Fill(it.m_lvec.Phi(), w);
        h_m_jet->Fill(it.m_lvec.M(), w);
        h_flav_jet->Fill(it.m_flav, w);
      }
      //sorting jets according to pT
      int idx_0(-1);
      int idx_1(-1);
      if (jets.size()>0) idx_0=0;
      if (jets.size()>1) {
        if(jets[0].m_lvec.Pt()>=jets[1].m_lvec.Pt()) {idx_0=0;idx_1=1;}
        else{idx_0=1;idx_1=0;};
      }
      if (idx_0>=0) {
        h_pt_jet0->Fill(jets[idx_0].m_lvec.Pt(), w);
        h_eta_jet0->Fill(jets[idx_0].m_lvec.Eta(), w);
        h_phi_jet0->Fill(jets[idx_0].m_lvec.Phi(), w); 
        h_m_jet0->Fill(jets[idx_0].m_lvec.M(), w); 
        h_flav_jet0->Fill(jets[idx_0].m_flav, w); 
      }
      if (idx_1>=0) {
        h_pt_jet1->Fill(jets[idx_1].m_lvec.Pt(), w);
        h_eta_jet1->Fill(jets[idx_1].m_lvec.Eta(), w);
        h_phi_jet1->Fill(jets[idx_1].m_lvec.Phi(), w); 
        h_m_jet1->Fill(jets[idx_1].m_lvec.M(), w); 
        h_flav_jet1->Fill(jets[idx_1].m_flav, w); 
      }
    }

    // Return a list of all the histograms.
    std::vector<TH1*> returnHisto() {
      std::vector<TH1*> histolist;
 
      // Jet-Related Plots
      histolist.push_back(h_pt_jet);
      histolist.push_back(h_eta_jet);
      histolist.push_back(h_phi_jet);
      histolist.push_back(h_m_jet);
      histolist.push_back(h_flav_jet);
      histolist.push_back(h_pt_jet0);
      histolist.push_back(h_eta_jet0);
      histolist.push_back(h_phi_jet0);
      histolist.push_back(h_m_jet0);
      histolist.push_back(h_flav_jet0);
      histolist.push_back(h_pt_jet1);
      histolist.push_back(h_eta_jet1);
      histolist.push_back(h_phi_jet1);
      histolist.push_back(h_m_jet1);
      histolist.push_back(h_flav_jet1);
      histolist.push_back(h_bbTagDis_beforeCut);
      histolist.push_back(h_bbTagDis);
      histolist.push_back(h_ccTagDis_beforeCut);
      histolist.push_back(h_ccTagDis);
      histolist.push_back(h_pQCD_beforeCut);
      histolist.push_back(h_NextraJet_beforeCut);
      histolist.push_back(h_nBjet_passNextraJetCut);
      histolist.push_back(h_nBjet_failNextraJetCut);
      histolist.push_back(h_nBjetExtraJet_passNextraJetCut);
      histolist.push_back(h_nBjetExtraJet_failNextraJetCut);
      histolist.push_back(h_MET_beforeCut);
      histolist.push_back(h_MET_beforeCut_1);
      histolist.push_back(h_MET);
      histolist.push_back(h_ccPN_TopvsQCD);
      histolist.push_back(h_ccPN_WvsQCD);
      histolist.push_back(h_ccPN_ZvsQCD);
      histolist.push_back(h_bbPN_TopvsQCD);
      histolist.push_back(h_bbPN_WvsQCD);
      histolist.push_back(h_bbPN_ZvsQCD);
     
      // V and H plots
      histolist.push_back(h_HMass);
      histolist.push_back(h_HPt);
      histolist.push_back(h_HEta);
      histolist.push_back(h_HFlav);
      histolist.push_back(h_ZMass);
      histolist.push_back(h_ZPt);
      histolist.push_back(h_ZEta);
      histolist.push_back(h_ZFlav);
      histolist.push_back(h_HMassZMass);
      histolist.push_back(h_DEta);
      histolist.push_back(h_DPhi);
      histolist.push_back(h_DR);
      histolist.push_back(h_DPhiJetMet_passNextraJetCut); 
      histolist.push_back(h_DPhiJetMet_failNextraJetCut);
      return histolist;
    }
    
    TH1D* h_bbTagDis_beforeCut;
    TH1D* h_ccTagDis_beforeCut;
    TH1D* h_pQCD_beforeCut;
    TH1D* h_bbTagDis;
    TH1D* h_ccTagDis;
    TH1D* h_NextraJet_beforeCut;
    TH1D* h_nBjet_passNextraJetCut;
    TH1D* h_nBjet_failNextraJetCut;
    TH1D* h_nBjetExtraJet_passNextraJetCut;
    TH1D* h_nBjetExtraJet_failNextraJetCut;
    TH1D* h_MET_beforeCut;
    TH1D* h_MET_beforeCut_1;
    TH1D* h_MET;
    TH1D* h_ccPN_TopvsQCD;
    TH1D* h_ccPN_WvsQCD;
    TH1D* h_ccPN_ZvsQCD;
    TH1D* h_bbPN_TopvsQCD;
    TH1D* h_bbPN_WvsQCD;
    TH1D* h_bbPN_ZvsQCD;
    TH1D* h_DPhiJetMet_passNextraJetCut;
    TH1D* h_DPhiJetMet_failNextraJetCut ;

  protected:
    // Variables
    TString m_name;
    
    //Higgs Plots
    TH1D* h_HPt;    
    TH1D* h_HEta;    
    TH1D* h_HMass;    
    TH1D* h_HFlav;    

    //Z Plots
    TH1D* h_ZPt;
    TH1D* h_ZEta;
    TH1D* h_ZMass;
    TH1D* h_ZFlav;
    
    TH2D* h_HMassZMass;
    TH1D* h_DEta;
    TH1D* h_DPhi;
    TH1D* h_DR;
    
    //VHBoosted Plots
    TH1D* h_pt_jet;
    TH1D* h_eta_jet;
    TH1D* h_phi_jet;
    TH1D* h_m_jet;
    TH1D* h_flav_jet;
    TH1D* h_pt_jet0;
    TH1D* h_eta_jet0;
    TH1D* h_phi_jet0;
    TH1D* h_m_jet0;
    TH1D* h_flav_jet0;
    TH1D* h_pt_jet1;
    TH1D* h_eta_jet1;
    TH1D* h_phi_jet1;
    TH1D* h_m_jet1;
    TH1D* h_flav_jet1;
} ;

class HBoostedPlots
{

  public:
    // Constructor
    HBoostedPlots(TString name) : m_name(name) {
      h_pt_jet = new TH1D(name + "_pt_jet", "", NBIN_PT_JET, X_PT_JET[0], X_PT_JET[1]);
      h_eta_jet = new TH1D(name + "_eta_jet", "", NBIN_ETA, X_ETA[0], X_ETA[1]);
      h_phi_jet = new TH1D(name + "_phi_jet", "", NBIN_PHI, X_PHI[0], X_PHI[1]);
      h_m_jet = new TH1D(name + "_m_jet", "", NBIN_M_J, X_M_J[0], X_M_J[1]);
      
      h_pt_jet->Sumw2();
      h_eta_jet->Sumw2();
      h_phi_jet->Sumw2();
      h_m_jet->Sumw2();
    } ;

    // Fill stuff about jets.
    void FillJet(JetObjBoosted& jet, float w=1) {
      h_pt_jet->Fill(jet.m_lvec.Pt(), w);
      h_eta_jet->Fill(jet.m_lvec.Eta(), w);
      h_phi_jet->Fill(jet.m_lvec.Phi(), w);
      h_m_jet->Fill(jet.m_lvec.M(), w);
    }

    // Return a list of all the histograms.
    std::vector<TH1*> returnHisto() {
      std::vector<TH1*> histolist;
 
      // Jet-Related Plots
      histolist.push_back(h_pt_jet);
      histolist.push_back(h_eta_jet);
      histolist.push_back(h_phi_jet);
      histolist.push_back(h_m_jet);

      return histolist;
    }

  protected:
    // Variables
    TString m_name;
    
    //HBoosted Plots
    TH1D* h_pt_jet;
    TH1D* h_eta_jet;
    TH1D* h_phi_jet;
    TH1D* h_m_jet;
} ;

// Main Plots class
class BoostedJetEffPlots 
{

  public:
    // Constructor
    BoostedJetEffPlots(TString name) : m_name(name) {
      h_pt_eta_m_pass = new TH3D(name + "_pt_eta_mass_pass", "", 100,200,1200,8,0,2.5,30,40,340); 
      h_pt_eta_m_tot = new TH3D(name + "_pt_eta_mass_tot", "", 100,200,1200,8,0,2.5,30,40,340);
      h_pt_eta_m_pass->Sumw2();
      h_pt_eta_m_tot->Sumw2();
    } ;

    // Fill stuff about jets.
    void Fill(JetObjBoosted& jet, bool doPass, float w=1) {
      h_pt_eta_m_tot->Fill(jet.m_lvec.Pt(), jet.m_lvec.Eta(), jet.m_lvec.M(), w);
      if (doPass) h_pt_eta_m_pass->Fill(jet.m_lvec.Pt(), jet.m_lvec.Eta(), jet.m_lvec.M(), w);
    }

    // Return a list of all the histograms.
    std::vector<TH1*> returnHisto() {
      std::vector<TH1*> histolist;
 
      // Jet-Related Plots
      histolist.push_back(h_pt_eta_m_pass);
      histolist.push_back(h_pt_eta_m_tot);

      return histolist;
    }

  protected:
    // Variables
    TString m_name;
    
    TH3D* h_pt_eta_m_pass;
    TH3D* h_pt_eta_m_tot;
} ;

class EffPlots
{
  public:
    EffPlots(TString name, int nBins, float bins[]) : m_name(name) {
      h_pt_deno = new TH1D(name + "_pt_deno","", nBins, bins) ; 
      h_pt_num = new TH1D(name + "_pt_num","", nBins, bins) ;
      h_pt_deno->Sumw2() ;
      h_pt_num->Sumw2() ;
    } ;
    void Fill(float val, TString where, float w=1) {
      if (where == "deno") h_pt_deno->Fill(val, w) ;
      if (where == "num") h_pt_num->Fill(val, w) ;
    } ;
    std::vector<TH1*> returnHisto() {
      std::vector<TH1*> histolist ;
      histolist.push_back(h_pt_deno) ;
      histolist.push_back(h_pt_num) ;
      return histolist ;
    } ;
  private:
    TString m_name ;
    TH1D* h_pt_deno ;
    TH1D* h_pt_num ;
} ;


// Plots for JES & unc
class JESUncPlots
{
  public:
    
    // Constructor
    JESUncPlots(TString name) : m_name(name) {

      // Initialize pt plots
      h_pt_raw = new TH1D(m_name + "_pt_raw", "", NBIN_PT_JET, X_PT_JET[0], X_PT_JET[1]);
      h_pt_nominal = new TH1D(m_name + "_pt_nominal", "", NBIN_PT_JET, X_PT_JET[0], X_PT_JET[1]);
      h_pt_calc = new TH1D(m_name + "_pt_calc", "", NBIN_PT_JET, X_PT_JET[0], X_PT_JET[1]);
      h_pt_ratio = new TH1D(m_name + "_pt_ratio", "", 200, 0, 2);
      
      h_pt_raw->Sumw2();
      h_pt_nominal->Sumw2();
      h_pt_calc->Sumw2();
      h_pt_ratio->Sumw2();
      
      // Initialize JES plots
      h_L1FastJet = new TH1D(m_name + "_L1FastJet", "", NBIN_SF, X_SF[0], X_SF[1]);
      h_L2Relative = new TH1D(m_name + "_L2Relative", "", NBIN_SF, X_SF[0], X_SF[1]);
      h_L3Absolute = new TH1D(m_name + "_L3Absolute", "", NBIN_SF, X_SF[0], X_SF[1]);
      h_L2L3Residual = new TH1D(m_name + "_L2L3Residual", "", NBIN_SF, X_SF[0], X_SF[1]);

      h_L1FastJet->Sumw2();
      h_L2Relative->Sumw2();
      h_L3Absolute->Sumw2();
      h_L2L3Residual->Sumw2();

      // Initialize unc plots
      h_SubTotalAbsolute = new TH1D(m_name + "_SubTotalAbsolute", "", NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_SubTotalMC = new TH1D(m_name + "_SubTotalMC", "", NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_SubTotalPileup = new TH1D(m_name + "_SubTotalPileup", "", NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_SubTotalPt = new TH1D(m_name + "_SubTotalPt", "", NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_SubTotalRelative = new TH1D(m_name + "_SubTotalRelative", "", NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_SubTotalScale = new TH1D(m_name + "_SubTotalScale", "", NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_TotalUnc = new TH1D(m_name + "_TotalUnc", "", NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_FlavorQCD = new TH1D(m_name + "_FlavorQCD", "", NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_TimePtEta = new TH1D(m_name + "_TimePtEta", "", NBIN_UNC, X_UNC[0], X_UNC[1]);

      h_SubTotalAbsolute_stored = new TH1D(m_name + "_SubTotalAbsolute_stored", "", NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_SubTotalMC_stored = new TH1D(m_name + "_SubTotalMC_stored", "", NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_SubTotalPileup_stored = new TH1D(m_name + "_SubTotalPileup_stored", "", NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_SubTotalPt_stored = new TH1D(m_name + "_SubTotalPt_stored", "", NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_SubTotalRelative_stored = new TH1D(m_name + "_SubTotalRelative_stored", "", NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_SubTotalScale_stored = new TH1D(m_name + "_SubTotalScale_stored", "", NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_TotalUnc_stored = new TH1D(m_name + "_TotalUnc_stored", "", NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_FlavorQCD_stored = new TH1D(m_name + "_FlavorQCD_stored", "", NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_TimePtEta_stored = new TH1D(m_name + "_TimePtEta_stored", "", NBIN_UNC, X_UNC[0], X_UNC[1]);

      h_sJER = new TH1D(m_name + "_sJER", "", NBIN_SF, X_SF[0], X_SF[1]);
      h_cJER = new TH1D(m_name + "_cJER", "", NBIN_SF, X_SF[0], X_SF[1]);
      h_jerUnc = new TH1D(m_name +"_jerUnc", "", NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_ptRes = new TH1D(m_name +"_ptRes", "", NBIN_SF, X_SF[0], X_SF[1]);
      
      h_sJER->Sumw2();
      h_cJER->Sumw2();
      h_jerUnc->Sumw2();
      h_ptRes->Sumw2();
      
      h_SubTotalAbsolute->Sumw2();
      h_SubTotalMC->Sumw2();
      h_SubTotalPileup->Sumw2();
      h_SubTotalPt->Sumw2();
      h_SubTotalRelative->Sumw2();
      h_SubTotalScale->Sumw2();
      h_TotalUnc->Sumw2();
      h_FlavorQCD->Sumw2();
      h_TimePtEta->Sumw2();

      h_SubTotalAbsolute_stored->Sumw2();
      h_SubTotalMC_stored->Sumw2();
      h_SubTotalPileup_stored->Sumw2();
      h_SubTotalPt_stored->Sumw2();
      h_SubTotalRelative_stored->Sumw2();
      h_SubTotalScale_stored->Sumw2();
      h_TotalUnc_stored->Sumw2();
      h_FlavorQCD_stored->Sumw2();
      h_TimePtEta_stored->Sumw2();

      h_pt_v_Absolute = new TH2D(m_name + "_pt_v_Absolute", "", NBIN_PT_JET, X_PT_JET[0], X_PT_JET[1],
                                 NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_pt_v_MC = new TH2D(m_name + "_pt_v_MC", "", NBIN_PT_JET, X_PT_JET[0], X_PT_JET[1],
			   NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_pt_v_Pileup = new TH2D(m_name + "_pt_v_Pileup", "", NBIN_PT_JET, X_PT_JET[0], X_PT_JET[1],
			       NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_pt_v_Pt = new TH2D(m_name + "_pt_v_Pt", "", NBIN_PT_JET, X_PT_JET[0], X_PT_JET[1],
			   NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_pt_v_Relative = new TH2D(m_name + "_pt_v_Relative", "", NBIN_PT_JET, X_PT_JET[0], X_PT_JET[1],
				 NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_pt_v_Scale = new TH2D(m_name + "_pt_v_Scale", "", NBIN_PT_JET, X_PT_JET[0], X_PT_JET[1],
			      NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_pt_v_Total = new TH2D(m_name + "_pt_v_Total", "", NBIN_PT_JET, X_PT_JET[0], X_PT_JET[1],
			      NBIN_UNC, X_UNC[0], X_UNC[1]);

      h_pt_v_Absolute->Sumw2();
      h_pt_v_MC->Sumw2();
      h_pt_v_Pileup->Sumw2();
      h_pt_v_Pt->Sumw2();
      h_pt_v_Relative->Sumw2();
      h_pt_v_Scale->Sumw2();
      h_pt_v_Total->Sumw2();

      h_eta_v_Absolute = new TH2D(m_name + "_eta_v_Absolute", "", NBIN_ETA, X_ETA[0], X_ETA[1],
                         NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_eta_v_MC = new TH2D(m_name + "_eta_v_MC", "", NBIN_ETA, X_ETA[0], X_ETA[1],
                   NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_eta_v_Pileup = new TH2D(m_name + "_eta_v_Pileup", "", NBIN_ETA, X_ETA[0], X_ETA[1],
                       NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_eta_v_Pt = new TH2D(m_name + "_eta_v_Pt", "", NBIN_ETA, X_ETA[0], X_ETA[1],
                   NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_eta_v_Relative = new TH2D(m_name + "_eta_v_Relative", "", NBIN_ETA, X_ETA[0], X_ETA[1],
                         NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_eta_v_Scale = new TH2D(m_name + "_eta_v_Scale", "", NBIN_ETA, X_ETA[0], X_ETA[1],
                      NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_eta_v_Total = new TH2D(m_name + "_eta_v_Total", "", NBIN_ETA, X_ETA[0], X_ETA[1],
                      NBIN_UNC, X_UNC[0], X_UNC[1]);

      h_eta_v_Absolute->Sumw2();
      h_eta_v_MC->Sumw2();
      h_eta_v_Pileup->Sumw2();
      h_eta_v_Pt->Sumw2();
      h_eta_v_Relative->Sumw2();
      h_eta_v_Scale->Sumw2();
      h_eta_v_Total->Sumw2();

      // Other useful histos
      h_rawFactor = new TH1D(m_name + "_rawFactor", "", NBIN_UNC, X_UNC[0], X_UNC[1]);
      h_1mRawFactor = new TH1D(m_name + "_1mRawFactor", "", NBIN_UNC, X_UNC[0], X_UNC[1]);
      
      h_rawFactor->Sumw2();
      h_1mRawFactor->Sumw2();

    };

    // Fill the pt values
    void FillPt(TString pt_name, float pt, float w = 1.) {

      //std::cout << "pt_name = " << pt_name << std::endl;
      if (pt_name == "raw") h_pt_raw->Fill(pt, w);
      else if (pt_name == "nominal") h_pt_nominal->Fill(pt, w);
      else if (pt_name == "calc")
      {
	//std::cout << "calc!" << std::endl;
	h_pt_calc->Fill(pt, w);
      }
      else if (pt_name =="ratio") h_pt_ratio->Fill(pt, w);
      
    }//end-fill-pt
  

    // Fill the JES values
    void FillJES(TString jes_name, float jes, float w = 1.) {

      if (jes_name == "L1") h_L1FastJet->Fill(jes, w);
      else if (jes_name == "L2") h_L2Relative->Fill(jes, w);
      else if (jes_name == "L3") h_L3Absolute->Fill(jes, w);
      else if (jes_name == "L2L3") h_L2L3Residual->Fill(jes, w);
      
    }//end-fill-jes

    // Fill the JER values
    void FillJER(TString jer_name, float jer, float w = 1.) {
      if (jer_name == "sJER") h_sJER->Fill(jer, w);
      else if (jer_name == "cJER") h_cJER->Fill(jer, w);
      else if (jer_name == "jerUnc") h_jerUnc->Fill(jer, w);
      else if (jer_name == "ptRes") h_ptRes->Fill(jer, w);
    }//end-fill-jer

    // Fill the unc values
    void FillUnc(TString unc_name, float unc, float stored, float w = 1.) {

      if (unc_name == "SubTotalAbsolute") h_SubTotalAbsolute->Fill(unc, w);
      else if (unc_name == "SubTotalMC") h_SubTotalMC->Fill(unc, w);
      else if (unc_name == "SubTotalPileUp") h_SubTotalPileup->Fill(unc, w);
      else if (unc_name == "SubTotalPt") h_SubTotalPt->Fill(unc, w);
      else if (unc_name == "SubTotalRelative") h_SubTotalRelative->Fill(unc, w);
      else if (unc_name == "SubTotalScale") h_SubTotalScale->Fill(unc, w);
      else if (unc_name == "Total") h_TotalUnc->Fill(unc, w);
      else if (unc_name == "RawFactor") h_rawFactor->Fill(unc, w);
      else if (unc_name == "1mRawFactor") h_1mRawFactor->Fill(unc, w);
      else if (unc_name == "FlavorQCD") h_FlavorQCD->Fill(unc, w);
      else if (unc_name == "TimePtEta") h_TimePtEta->Fill(unc, w);

      if (unc_name == "SubTotalAbsolute") h_SubTotalAbsolute_stored->Fill(stored, w);
      else if (unc_name == "SubTotalMC") h_SubTotalMC_stored->Fill(stored, w);
      else if (unc_name == "SubTotalPileUp") h_SubTotalPileup_stored->Fill(stored, w);
      else if (unc_name == "SubTotalPt") h_SubTotalPt_stored->Fill(stored, w);
      else if (unc_name == "SubTotalRelative") h_SubTotalRelative_stored->Fill(stored, w);
      else if (unc_name == "SubTotalScale") h_SubTotalScale_stored->Fill(stored, w);
      else if (unc_name == "Total") h_TotalUnc_stored->Fill(stored, w);
      //else if (unc_name == "RawFactor") h_rawFactor_stored->Fill(stored, w);
      //else if (unc_name == "1mRawFactor") h_1mRawFactor_stored->Fill(stored, w);
      else if (unc_name == "FlavorQCD") h_FlavorQCD_stored->Fill(stored, w);
      else if (unc_name == "TimePtEta") h_TimePtEta_stored->Fill(stored, w);
      
    }//end-fill-unc

    // Fill the unc vs pt
    void FillUncVsPt(TString unc_name, float unc, float pt, float w = 1.) {

      if (unc_name == "Absolute") h_pt_v_Absolute->Fill(unc, pt, w);
      else if (unc_name == "MC") h_pt_v_MC->Fill(unc, pt, w);
      else if (unc_name == "Pileup") h_pt_v_Pileup->Fill(unc, pt, w);
      else if (unc_name == "Pt") h_pt_v_Pt->Fill(unc, pt, w);
      else if (unc_name == "Relative") h_pt_v_Relative->Fill(unc, pt, w);
      else if (unc_name == "Scale") h_pt_v_Scale->Fill(unc, pt, w);
      else if (unc_name == "Total") h_pt_v_Total->Fill(unc, pt, w);
      
    }//end-unc-vs-pt

  
    // Fill the unc vs eta
    void FillUncVsEta(TString unc_name, float unc, float eta, float w = 1.) {

      if (unc_name == "Absolute") h_eta_v_Absolute->Fill(unc, eta, w);
      else if (unc_name == "MC") h_eta_v_MC->Fill(unc, eta, w);
      else if (unc_name == "Pileup") h_eta_v_Pileup->Fill(unc, eta, w);
      else if (unc_name == "Pt") h_eta_v_Pt->Fill(unc, eta, w);
      else if (unc_name == "Relative") h_eta_v_Relative->Fill(unc, eta, w);
      else if (unc_name == "Scale") h_eta_v_Scale->Fill(unc, eta, w);
      else if (unc_name == "Total") h_eta_v_Total->Fill(unc, eta, w);
   
    }//end-unc-vs-eta

  
    // Return histo list
    std::vector<TH1*> returnHisto() {

      std::vector<TH1*> histolist;

      // pt-Related Plots
      histolist.push_back(h_pt_raw);
      histolist.push_back(h_pt_nominal);
      histolist.push_back(h_pt_calc);
      histolist.push_back(h_pt_ratio);
      
      // JES-Related Plots          
      histolist.push_back(h_L1FastJet);
      histolist.push_back(h_L2Relative);
      histolist.push_back(h_L3Absolute);
      histolist.push_back(h_L2L3Residual);

      // Unc-Related Plots
      histolist.push_back(h_SubTotalAbsolute);
      histolist.push_back(h_SubTotalMC);
      histolist.push_back(h_SubTotalPileup);
      histolist.push_back(h_SubTotalPt);
      histolist.push_back(h_SubTotalRelative);
      histolist.push_back(h_SubTotalScale);
      histolist.push_back(h_TotalUnc);
      histolist.push_back(h_FlavorQCD);
      histolist.push_back(h_TimePtEta);

      histolist.push_back(h_SubTotalAbsolute_stored);
      histolist.push_back(h_SubTotalMC_stored);
      histolist.push_back(h_SubTotalPileup_stored);
      histolist.push_back(h_SubTotalPt_stored);
      histolist.push_back(h_SubTotalRelative_stored);
      histolist.push_back(h_SubTotalScale_stored);
      histolist.push_back(h_TotalUnc_stored);
      histolist.push_back(h_FlavorQCD_stored);
      histolist.push_back(h_TimePtEta_stored);

      histolist.push_back(h_ptRes);
      histolist.push_back(h_sJER);
      histolist.push_back(h_cJER);
      histolist.push_back(h_jerUnc);
      
      histolist.push_back(h_pt_v_Absolute);
      histolist.push_back(h_pt_v_MC);
      histolist.push_back(h_pt_v_Pileup);
      histolist.push_back(h_pt_v_Pt);
      histolist.push_back(h_pt_v_Relative);
      histolist.push_back(h_pt_v_Scale);
      histolist.push_back(h_pt_v_Total);

      histolist.push_back(h_eta_v_Absolute);
      histolist.push_back(h_eta_v_MC);
      histolist.push_back(h_eta_v_Pileup);
      histolist.push_back(h_eta_v_Pt);
      histolist.push_back(h_eta_v_Relative);
      histolist.push_back(h_eta_v_Scale);
      histolist.push_back(h_eta_v_Total);
      
      // Other Plots
      histolist.push_back(h_rawFactor);
      histolist.push_back(h_1mRawFactor);
      
      return histolist;
    }

  protected:
    // Variables
    TString m_name;

    // pt Plots
    TH1D* h_pt_raw;
    TH1D* h_pt_nominal;
    TH1D* h_pt_calc;
    TH1D* h_pt_ratio;

    // JES Plots
    TH1D* h_L1FastJet;
    TH1D* h_L2Relative;
    TH1D* h_L3Absolute;
    TH1D* h_L2L3Residual;

    // Unc Plots
    TH1D* h_SubTotalAbsolute;
    TH1D* h_SubTotalMC;
    TH1D* h_SubTotalPileup;
    TH1D* h_SubTotalPt;
    TH1D* h_SubTotalRelative;
    TH1D* h_SubTotalScale;
    TH1D* h_TotalUnc;
    TH1D* h_FlavorQCD;
    TH1D* h_TimePtEta;

    TH1D* h_SubTotalAbsolute_stored;
    TH1D* h_SubTotalMC_stored;
    TH1D* h_SubTotalPileup_stored;
    TH1D* h_SubTotalPt_stored;
    TH1D* h_SubTotalRelative_stored;
    TH1D* h_SubTotalScale_stored;
    TH1D* h_TotalUnc_stored;
    TH1D* h_FlavorQCD_stored;
    TH1D* h_TimePtEta_stored;

    TH1D* h_sJER;
    TH1D* h_cJER;
    TH1D* h_jerUnc;
    TH1D* h_ptRes;
  
    TH2D* h_pt_v_Absolute;
    TH2D* h_pt_v_MC;
    TH2D* h_pt_v_Pileup;
    TH2D* h_pt_v_Pt;
    TH2D* h_pt_v_Relative;
    TH2D* h_pt_v_Scale;
    TH2D* h_pt_v_Total;

    TH2D* h_eta_v_Absolute;
    TH2D* h_eta_v_MC;
    TH2D* h_eta_v_Pileup;
    TH2D* h_eta_v_Pt;
    TH2D* h_eta_v_Relative;
    TH2D* h_eta_v_Scale;
    TH2D* h_eta_v_Total;

    // Other
    TH1D* h_rawFactor;
    TH1D* h_1mRawFactor;
};

#endif
