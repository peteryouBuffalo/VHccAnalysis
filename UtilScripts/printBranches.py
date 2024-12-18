import os,sys
import ROOT

#f = ROOT.TFile.Open("root:://cmsxrootd.fnal.gov///store/mc/RunIISummer16NanoAODv7/DY01234jets_13TeV-sherpa/NANOAODSIM/PUMoriond17_Nano02Apr2020_102X_mcRun2_asymptotic_v8-v1/110000/52841C5F-E537-824C-97E9-8683E8B768D4.root","READ")
#f = ROOT.TFile.Open("root:://cmsxrootd.fnal.gov///store/mc/RunIIFall17NanoAODv7/ZH_HToCC_ZToQQ_M125_13TeV_powheg_pythia8/NANOAODSIM/PU2017_12Apr2018_Nano02Apr2020_102X_mc2017_realistic_v8-v1/120000/899C4769-FE77-DA4D-A00D-463215D1F6FB.root","READ")
f = ROOT.TFile.Open("/eos/uscms/store/user/lpcphys/duong/NanoAOD_postProcessing/QCD_HT300to500_v9_MC_2016/FFBF1499-95FE-5842-A428-796F9D29EDCC_Skim.root")

tr = f.Get("Events")

tr.Print("Title")
