import os,sys
import ROOT
peds = ['B','C','D','E','F']
print('New    Old')

for y in ['16','17']:
  print("y = ", y)
  print("==========================")
  for ped in peds:
    f1_n = '/uscms_data/d3/peteryou/boosted_new/CMSSW_14_0_6/src/VHccAnalysis/condor_results/2025Jan_new/NONE/JetHT_DATA_20' + y +ped+'.root'
    f2_n = '/uscms/home/duong/Scratch/Output_VH/NONE_fromPeter2024Dec_2025Jan/JetHT_DATA_20'+y+ped+'.root'
    f1 = ROOT.TFile.Open(f1_n,'READ')
    f2 = ROOT.TFile.Open(f2_n,'READ')
    h1 = f1.Get('Nevt_VbbHcc_boosted')
    h2 = f2.Get('Nevt_VbbHcc_boosted')
    print(ped, ' ', h1.GetBinContent(1), ' ', h2.GetBinContent(1))

