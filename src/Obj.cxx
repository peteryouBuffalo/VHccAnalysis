#ifndef Obj_cxx
#define Obj_cxx

//The objects are wrapper so that the analysis is independent on variable name on the ntuple, only when the objects are created such dependences occur 

#include "TLorentzVector.h"
#include <math.h> 

//-------------------------------objs------------------------------
class LepObj { // Lepton

  public:
    
    // Constructor & Deconstructor
    LepObj(float pt, float eta, float scEta, float phi, float mass, unsigned idx, int charge, 
           float iso) : m_scEta(scEta), m_idx(idx), m_charge(charge), m_iso(iso) { 
             m_lvec.SetPtEtaPhiM(pt, eta, phi, mass) ; 
    } ;

    virtual ~LepObj() {} ;

    // Variables
    TLorentzVector m_lvec ;  //4-vector
    
    float m_scEta ;  // eta value
    unsigned m_idx ; // index
    int m_charge ;   // charge
    float m_iso ;    // isolation
    
} ;


class JetObj { // Jets
  
  public:
    
    // Constructor & Deconstructor
    JetObj(float pt, float eta, float phi, float mass, unsigned flav, float deepFlavB, float PUjetID) : m_flav(flav), m_deepFlavB(deepFlavB), m_puid(PUjetID)  {
              m_lvec.SetPtEtaPhiM(pt, eta, phi, mass) ;
    } ;

    virtual ~JetObj() {} ;

    // IsLepton - checks to make sure our jet isn't actually a lepton
    bool IsLepton(std::vector<LepObj>& leps, float dRcut=0.4) {
      float minDr = 1000 ;
      for (std::vector<LepObj>::iterator it = leps.begin() ; it != leps.end() ; ++it) {
        float dRtmp = m_lvec.DeltaR(it->m_lvec) ;
        if (dRtmp < minDr) minDr = dRtmp ;
      }
      return minDr <= dRcut ;
    }

    // SetSV - determines the best secondary vertex for the jet
    void SetSV(std::vector<TLorentzVector>& sv) {
      float maxPt = -1;
      m_svIdx = -1;
      m_mSV = -1;
      for (unsigned isv = 0 ; isv < sv.size() ; ++isv) {
        float dRtmp = m_lvec.DeltaR(sv[isv]);
        if (dRtmp <= 0.4 && sv[isv].Pt() > maxPt) {
          m_svIdx = isv; 
          m_mSV = sv[isv].M();
        }
      }
    } ;

    // Variables
    TLorentzVector m_lvec ; // 4-vector
    unsigned m_flav;       // jet flavor
    float m_deepFlavB;
    unsigned m_svIdx;      // SV index
    float m_mSV;            // SV mass
    float m_puid;           // PU ID

} ;

class JetObjBoosted: public JetObj {
  
  public:
  
    JetObjBoosted(float pt, float eta, float phi, float mass, unsigned flav, float DDCvB, float DDCvL, float DDBvL, float DT_ZHccvsQCD, float DT_ZbbvsQCD, float PN_Xcc, float PN_Xbb, float PN_pQCD, float PN_TopvsQCD, float PN_WvsQCD, float PN_ZvsQCD, float n2b1, float PUjetID):
    JetObj(pt, eta, phi, mass, flav, -1, PUjetID), 
    m_DDCvB(DDCvB), m_DDCvL(DDCvL), m_DDBvL(DDBvL), 
    m_DT_ZHccvsQCD(DT_ZHccvsQCD), m_DT_ZbbvsQCD(DT_ZbbvsQCD),
    m_PN_Xcc(PN_Xcc),
    m_PN_Xbb(PN_Xbb),
    m_PN_pQCD(PN_pQCD),
    m_PN_TopvsQCD(PN_TopvsQCD),
    m_PN_WvsQCD(PN_WvsQCD),
    m_PN_ZvsQCD(PN_ZvsQCD),
    m_n2b1(n2b1) {
      m_rho = -10;
      if(pt>0 && mass>0) m_rho = 2*log(mass/pt);
    };
    virtual ~JetObjBoosted() {} ;
    float m_DDCvB;       // DeepDoubCvsB value
    float m_DDCvL;       // DeepDoubCvsL value
    float m_DDBvL;       // DeepDoubBvsL value
    float m_DT_ZHccvsQCD;//FatJet_deepTagMD_ZHccvsQCD
    float m_DT_ZbbvsQCD;//FatJet_deepTagMD_ZbbvsQCD
    float m_PN_Xbb;//XbbVsQCD: FatJet_particleNetMD_Xbb/(FatJet_particleNetMD_Xbb+FatJet_particleNetMD_QCD)
    float m_PN_Xcc;//XccVsQCD: FatJet_particleNetMD_Xcc/(FatJet_particleNetMD_Xcc+FatJet_particleNetMD_QCD)
    float m_PN_pQCD;
    float m_PN_TopvsQCD;
    float m_PN_WvsQCD;
    float m_PN_ZvsQCD;
    float m_n2b1;
    float m_rho;
} ;


class ZObj { // Z Boson

  public:

    // Constructor & Deconstructor
    ZObj(std::vector<JetObj> jetlist) : m_jets(jetlist) {
      for (int idx = 0; idx < jetlist.size(); ++idx) {
        m_lvec += jetlist.at(idx).m_lvec;
        m_flav += jetlist.at(idx).m_flav;
      }
    }

    ZObj(JetObj jet) {
      m_jets.push_back(jet);
      m_lvec = jet.m_lvec;
      m_flav = jet.m_flav;
    }

  
    virtual ~ZObj() {} ;

    // Methods
    size_t nJets() {
      return m_jets.size();
    }

    float DeltaR() {
      return m_jets.at(0).m_lvec.DeltaR(m_jets.at(1).m_lvec);
    }
 
    float Dphi(float x, float y) { return (x-y); }
    float DphiC(double x) {
      if ((x <= TMath::Pi() && x >= 0) or (x<0 && x > -TMath::Pi())) return x;
      else if (x >= TMath::Pi()) return DphiC(x-2*TMath::Pi());
      else if (x < -TMath::Pi()) return DphiC(x+2*TMath::Pi());
    }
    
    float DPhi() {
      float dphi = Dphi(m_jets.at(0).m_lvec.Phi(), m_jets.at(1).m_lvec.Phi());
      return DphiC(dphi);
    }

    // Variables
    TLorentzVector m_lvec;      // 4-vector
    std::vector<JetObj> m_jets; // list of jets that compose Z boson
    int m_flav; //sum of flavor of jets that compose Z boson

} ;


class HObj { // Higgs boson

  public:

    // Constructro & Deconstructor
    HObj(std::vector<JetObj> jetlist) : m_jets(jetlist) {
      for (int idx = 0; idx < jetlist.size(); ++idx) {
        m_lvec += jetlist.at(idx).m_lvec;
        m_flav += jetlist.at(idx).m_flav;
      }
    }

    HObj(JetObj jet) {
      m_jets.push_back(jet);
      m_lvec = jet.m_lvec;
      m_flav = jet.m_flav;
    }

    virtual ~HObj() {} ;

    // Methods
    size_t nJets() {
      return m_jets.size();
    }

    float DeltaR() {
      return m_jets.at(0).m_lvec.DeltaR(m_jets.at(1).m_lvec);
    }

    float Dphi(float x, float y) { return (x-y); }
    float DphiC(double x) {
      if ((x <= TMath::Pi() && x >= 0) or (x<0 && x > -TMath::Pi())) return x;
      else if (x >= TMath::Pi()) return DphiC(x-2*TMath::Pi());
      else if (x < -TMath::Pi()) return DphiC(x+2*TMath::Pi());
    }

    float DPhi() {
      float dphi = Dphi(m_jets.at(0).m_lvec.Phi(), m_jets.at(1).m_lvec.Phi());
      return DphiC(dphi);
    }


    // Variables
    TLorentzVector m_lvec;      // 4-vector
    std::vector<JetObj> m_jets; // list of jets that compose Higgs boson
    int m_flav; //sum of flavor of jets that compose Higgs boson
} ;


#endif
