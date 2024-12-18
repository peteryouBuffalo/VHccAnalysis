import ROOT
ROOT.gROOT.SetBatch(True)

fileLists = {}
fileLists['ZH'] = {}
fileLists['ZH']['2016'] = '../FileLists/ZH_HToCC_ZToQQ_MC_2016.txt'
trig_names = {}
trig_names['2016'] = ['HLT_QuadJet45_DoubleBTagCSV_p087']
fOut = ROOT.TFile('trig_eff.root','RECREATE')
for sam in ['ZH']:
  for y in ['2016']:
    ch = ROOT.TChain("Events")
    lines = open(fileLists[sam][y]).readlines()
    for fN in lines:
      ch.Add(fN)
    for trig_name in trig_names[y]:
      hN = trig_name+'_'+sam+'_'+y
      htmp = ROOT.TH1D(hN,'',2,0,2)
      ch.Draw(trig_name+">>"+hN)
      fOut.cd()
      htmp.Write()
    print(sam + ", " + y + ": total events: "),
    print(ch.GetEntries())
