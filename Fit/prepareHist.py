#this script makes preparation to perform limit setting for VbbHcc
#1. prepare data card
#2. prepare input file

import ROOT
import sys,os
import copy
import math
#sys.path.append('/uscms_data/d3/duong/CMSSW/CMSSW_7_6_5/src/ZplusC/python/')
sys.path.append('/uscms_data/d3/duong/CMSSW/CMSSW_7_6_5/src/ZplusC/python3/myutils/')
import configparser
import util_funcs as utl_func
import myutils as utl
import random

def makeline(words):
  tmp = '{0:30}'.format(words[0]) 
  for i in range(1,len(words)):
    tmp = tmp + '{0:20}'.format(words[i])
  return tmp

def getHist(pN,samName,fH,lS): #samName = ['Electron'],['DY_0J','DY_1J','DY_2J']...
  hOut = {}
  num = random.random()
  for y in years:
  #for y in ['17']:
    num = random.random()
    cName = pN + "_" + str(num)
    hOut[y] = fH[samName[0]][y][0].Get(pN).Clone(cName) #first sample, first file
    if samName[0] not in ['JetHT']:
      hOut[y].Scale(lS[samName[0]][y][0])

    for iS in range(len(samName)):
      for fi in range(len(fH[samName[iS]][y])):
        if iS == 0 and fi == 0: continue #first sample and first file is already included above 
        num = random.random()
        h = fH[samName[iS]][y][fi].Get(pN).Clone("tmp_"+str(num))
        if samName[iS] not in ['JetHT']:
          h.Scale(lS[samName[iS]][y][fi])
        hOut[y].Add(h)
      
  return hOut

#not use currently
def getHistIntegral(h,v1=-1,v2=-1):
  tmp = [0,0]
  bBin = 0 #include underflow bin
  eBin = h.GetNbinsX()+1 #include overflow bin
  if v1 != -1: bBin = h.GetXaxis().FindFixBin(v1)
  if v2 != -1: eBin = h.GetXaxis().FindFixBin(v2)
  for iB in range(v1,v2+1):
    tmp[0] += h.GetBinContent(iB)
    tmp[1] += h.GetBinError(iB)*h.GetBinError(iB)
  tmp[1] = math.sqrt(tmp[1])
  return tmp

#make a new histogram with new x-axis range, but not change the binwidth, for example creating h_mH_out in 40-200 GeV from h_mH_in 0-300 GeV 
def Rebinning(histIn, xRange):
    nBin = int((xRange[1] - xRange[0])/histIn.GetBinWidth(1)) 
    #print '>>>nBin: ', nBin
    num = random.random()
    hist = ROOT.TH1D(histIn.GetName() + '_'+str(num), '', nBin, xRange[0], xRange[1])
    for i in range(1,hist.GetNbinsX() + 1):
      #get bin number
      iBin = histIn.FindFixBin(hist.GetBinLowEdge(i))
      hist.SetBinContent(i, histIn.GetBinContent(iBin))
      hist.SetBinError(i, histIn.GetBinError(iBin))
    
    #get overflow
    ofBin = histIn.FindFixBin(xRange[1])
    nOf = 0
    nErrOf = 0
    for i in range(ofBin, histIn.GetNbinsX() + 2):
      nOf += histIn.GetBinContent(i)
      nErrOf += histIn.GetBinError(i)*histIn.GetBinError(i)
    nErrOf = math.sqrt(nErrOf)
    hist.SetBinContent(hist.GetNbinsX() + 1, nOf)
    hist.SetBinError(hist.GetNbinsX() + 1, nErrOf)

    #get underflow
    ufBin = histIn.FindFixBin(xRange[0]-histIn.GetBinWidth(1))
    nUf = 0
    nErrUf = 0
    for i in range(0, ufBin + 1):
      nUf += histIn.GetBinContent(i)
      nErrUf += histIn.GetBinError(i)*histIn.GetBinError(i)
    nErrUf = math.sqrt(nErrUf)
    hist.SetBinContent(0, nUf)
    hist.SetBinError(0, nErrUf)
    
    return hist


def getHisto(fIn, name, scale_factor=1,xRange=[-1,-1], xDiv=[-1,-1]):
  histIn = fIn.Get(name)
  #make a copy of histogram with the same bin width but with different range
  #print '>>>>>>>Hist name: ', name
  hist = None
  if xRange[0] != -1 and xRange[1] != -1:
    hist = Rebinning(histIn,xRange)
    hist.Scale(scale_factor)
  else:
    num = random.random()
    hist = histIn.Clone(name + '_' + str(num))
    hist.Scale(scale_factor)
  
  hist1 = None
  if xDiv[0] != -1 and xDiv[1] != -1:
    hist1 = utl_func.customBin(hist,xDiv)
    return hist1

  return hist


  
##############################################
# Main program
##############################################

years = ['16_preVFP','16','17','18']

#need to run both ZccHcc and VHcc to have all templates for fitting
chan = 'ZccHcc' #VHcc

VVtype = 'NLO_VV' #LO_VV

regions = {'CR_qcd':chan + '_boosted_PN_med_qcdCR','SR':chan+'_boosted_PN_med','topCR_pass':chan+'_boosted_PN_med_topCR_pass'}

#NEED TO CREATE the folder to store output, if not presented
fOut = ROOT.TFile('../ForLimitSetting/test_'+chan+'_'+VVtype+'_noQCDreweight_newBining_syst_Peter_2025JanFeb.root','RECREATE') #root file to store all templates

#here: make a new histogram from 40-201 and 7 GeV bin width
nRebin=7
lowM = 40
highM = 201

doCustomBinning = False
xDiv = [40.,60.,80.,90.,100.,130.,160.,200.] 

doUseReweightForQCD = False 

cfg = utl.BetterConfigParser()
cfg.read('../Configs/config.ini')

use_bEnriched_BGenFilter = False

lumiS = {}
for y in years:
  lumiTmp = float(cfg.get('General','lumi_'+y))/1000.
  lumiTmp = format("%.1f" % lumiTmp)
  lumiS[y] = str(lumiTmp)
#print(lumiS) 

ss = ['JetHT','ZH_HToCC_ZToQQ','ggZH_HToCC_ZToQQ','WH_HToCC_WToQQ','ZH_HToBB_ZToQQ','WH_HToBB_WToQQ','ggZH_HToBB_ZToQQ',\
      'QCD_HT200to300_v9','QCD_HT300to500_v9','QCD_HT500to700_v9','QCD_HT700to1000_v9','QCD_HT1000to1500_v9','QCD_HT1500to2000_v9','QCD_HT2000toInf_v9',\
      'WJetsToQQ_HT-400to600','WJetsToQQ_HT-600to800','WJetsToQQ_HT-800toInf','WJetsToLNu_HT-400to600','WJetsToLNu_HT-600to800','WJetsToLNu_HT-800to1200','WJetsToLNu_HT-1200to2500','WJetsToLNu_HT-2500toInf',\
      'ZJetsToQQ_HT-400to600','ZJetsToQQ_HT-600to800','ZJetsToQQ_HT-800toInf',\
      'TTToHadronic','TTToSemiLeptonic','TTTo2L2Nu','ST_tW-channel_antitop','ST_tW-channel_top',\
      'WW','WZ','ZZ',\
      'WWTo1L1Nu2Q','WWTo4Q','WZTo4Q','WZToLNu2B','WZTo1L1Nu2Q','WZTo2Q2L','ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q'] #JetHT is used to get the observations or data distributions

systs = ['NONE','NONE_FOR_SYST','JESU','JESD','JERU','JERD','PUU','PUD','L1PREFIRINGU','L1PREFIRINGD','TAG_CCU','TAG_CCD','ELECU','ELECD','MUONU','MUOND','TRIGU','TRIGD'] #NONE_FOR_SYST is used to get the nominal/central distributions for estimating systematic variations (DO NOT REMOVE IT!!!). The setting comes from historical context. In the past (Z+b analysis), all systematic variations were estimated using post nanoAOD preprocessing with added information. During the preprocessing, usually, not all samples were completed because of failed jobs. Therefore the nominal (central) distributions should be taken from these preprocessing samples (if we used the original samples, the central distributions can be higher than using preprocessing sample causing artificial shift/bias). Now we do not use preprocessing samples so NONE_FOR_SYST = NONE basically. However NONE_FOR_SYST uses pathsyst not path (in config.ini), see lines 194-205

random.seed(16596956731)

for syst in systs:

  fOut.mkdir(syst)
  
  fNames = {}
  xSecs = {}
  lumiScales = {}
  fHist = {}

  #prepare for sample info
  for s in ss:

    fNames[s] = {}
    xSecs[s] = {}
    lumiScales[s] = {}
    fHist[s] = {}

    for y in years:
      lumi = float(cfg.get('General','lumi_'+y))
      names = cfg.get(s,'file_'+y).split(',') #multiple names is possible, for example: Single top = t-channels, s-channels ...
      #print('>>>>>>>: ', len(names))
      xSecTmps = ['1']*len(names) #each name corresponds to a cross section
      kfactor = ['1']*len(names) #each name corresponds to a cross section
      if s not in ['JetHT']:
        xSecTmps = cfg.get(s,'xSec_'+y).split(',')

      fNames[s][y] = []
      xSecs[s][y] = []
      fHist[s][y] = []

      #observation (data) taken from normal sample while MC taken from reprocessing
      p = 'path'
      #path to where systematic uncertainties are stored
      if syst != 'NONE' and s != 'JetHT':
        p = 'pathSyst'
      p = cfg.get('Paths',p)

      #now form the full name of path for syst
      if syst != 'NONE' and s != 'JetHT':
        if syst == 'NONE_FOR_SYST':
          p = p + '/NONE' #path is pathSyst/NONE (see line 197 and above explanation)
        else:
          p = p + '/'+ syst
      #change to relevent path when processing syst for MC
      for iN in names:
        path = p + '/' + iN
        fNames[s][y].append(path)
        fHist[s][y].append(ROOT.TFile.Open(fNames[s][y][-1],'READ'))
      
      #print(xSecTmps)
      for iS in xSecTmps:
        #in case there is kfactor in cross section
        if '*' in iS:
          iS = iS.split('*')
        if len(iS) == 2:
          xSecs[s][y].append(float(iS[0])*float(iS[1]))
        else:
          xSecs[s][y].append(float(iS))

      lumiScales[s][y] = [1]*len(names)
      for iN in range(len(fNames[s][y])):
        if s not in ['JetHT']:
          #print(s, y, iN, fNames[s][y][iN])
          lumiScales[s][y][iN] = utl_func.scaleToLumi1(fNames[s][y][iN],xSecs[s][y][iN],lumi,'Nevt_all_VbbHcc_boosted')
  

  plN = 'HMass' 

  for rN,r in regions.items():
    
    hN = r + '_' + plN
      
    #print(hN, plN)
    #get histograms
    hDat = getHist(hN,['JetHT'],fHist,lumiScales)
    hZHcc = getHist(hN,['ZH_HToCC_ZToQQ'],fHist,lumiScales)
    hZHbb = getHist(hN,['ZH_HToBB_ZToQQ'],fHist,lumiScales)
    hWHcc = getHist(hN,['WH_HToCC_WToQQ'],fHist,lumiScales)
    hWHbb = getHist(hN,['WH_HToBB_WToQQ'],fHist,lumiScales)
    hggZHcc = getHist(hN,['ggZH_HToCC_ZToQQ'],fHist,lumiScales)
    hggZHbb = getHist(hN,['ggZH_HToBB_ZToQQ'],fHist,lumiScales)
    hQCD = getHist(hN,['QCD_HT200to300_v9','QCD_HT300to500_v9','QCD_HT500to700_v9','QCD_HT700to1000_v9','QCD_HT1000to1500_v9','QCD_HT1500to2000_v9','QCD_HT2000toInf_v9'],fHist,lumiScales)
    if rN=='SR' and doUseReweightForQCD:
      hQCD = getHist(hN.replace("PN_med","PN_med_xccWeight"),['QCD_HT200to300_v9','QCD_HT300to500_v9','QCD_HT500to700_v9','QCD_HT700to1000_v9','QCD_HT1000to1500_v9','QCD_HT1500to2000_v9','QCD_HT2000toInf_v9'],fHist,lumiScales)
    
    hWJ = getHist(hN,['WJetsToQQ_HT-400to600','WJetsToQQ_HT-600to800','WJetsToQQ_HT-800toInf','WJetsToLNu_HT-400to600','WJetsToLNu_HT-600to800','WJetsToLNu_HT-800to1200','WJetsToLNu_HT-1200to2500','WJetsToLNu_HT-2500toInf'],fHist,lumiScales)
    hZJ = getHist(hN,['ZJetsToQQ_HT-400to600','ZJetsToQQ_HT-600to800','ZJetsToQQ_HT-800toInf'],fHist,lumiScales)
    hTT = getHist(hN,['TTToHadronic','TTToSemiLeptonic','TTTo2L2Nu'],fHist,lumiScales)
    hST = getHist(hN,['ST_tW-channel_antitop','ST_tW-channel_top'],fHist,lumiScales)
    if VVtype=='LO_VV':
      hWW = getHist(hN,['WW'],fHist,lumiScales)
      hWZ = getHist(hN,['WZ'],fHist,lumiScales)
      hZZ = getHist(hN,['ZZ'],fHist,lumiScales)
      rTmp = r + '_VZcc'
      hVVcc = getHist(hN.replace(r,rTmp),['WZ','ZZ'],fHist,lumiScales) 
      rTmp = r + '_VZbb'
      hVVbb = getHists(hN.replace(r,rTmp),['WZ','ZZ'],fHist,lumiScales) 
      rTmp = r + '_VZother'
      hVVother = getHists(hN.replace(r,rTmp),['WZ','ZZ'],fHist,lumiScales)
      for ytmp in years:
          hVVother[ytmp].Add(hWW[ytmp])
    if VVtype=='NLO_VV':
      hWW = getHist(hN,['WWTo1L1Nu2Q','WWTo4Q'],fHist,lumiScales)
      hWZ = getHist(hN,['WZTo1L1Nu2Q','WZTo2Q2L','WZToLNu2B','WZTo4Q'],fHist,lumiScales)
      hZZ = getHist(hN,['ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q'],fHist,lumiScales)
      rTmp = r + '_VZcc'
      hVVcc = getHist(hN.replace(r,rTmp),['WZTo1L1Nu2Q','WZTo2Q2L','WZToLNu2B','WZTo4Q','ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q'],fHist,lumiScales) 
      rTmp = r + '_VZbb'
      hVVbb = getHist(hN.replace(r,rTmp),['WZTo1L1Nu2Q','WZTo2Q2L','WZToLNu2B','WZTo4Q','ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q'],fHist,lumiScales) 
      rTmp = r + '_VZother'
      hVVother = getHist(hN.replace(r,rTmp),['WZTo1L1Nu2Q','WZTo2Q2L','WZToLNu2B','WZTo4Q','ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q'],fHist,lumiScales) 
      for ytmp in years:
          hVVother[ytmp].Add(hWW[ytmp])
    
    hVHcc = {}
    hVHbb = {}
    hTop = {} 
    hVJ = {}
    hVV = {}
    for y in years:
        #group processes
        hZHcc[y].Add(hggZHcc[y])
        hZHbb[y].Add(hggZHbb[y])
        hVHcc[y] = hZHcc[y].Clone("VHcc_"+y)
        hVHcc[y].Add(hWHcc[y])
        hVHbb[y] = hZHbb[y].Clone("VHbb_"+y)
        hVHbb[y].Add(hWHbb[y])
        hTop[y] = hTT[y].Clone("Top_"+y)
        hTop[y].Add(hST[y])
        hVJ[y] = hZJ[y].Clone("VJ_"+y)
        hVJ[y].Add(hWJ[y])
        hVV[y] = hWW[y].Clone("VV_"+y)
        hVV[y].Add(hWZ[y])
        hVV[y].Add(hZZ[y])
        
        #rebinning
        hDat[y] = Rebinning(hDat[y],[lowM,highM])
        hDat[y].Rebin(nRebin)
        hQCD[y] = Rebinning(hQCD[y],[lowM,highM])
        hQCD[y].Rebin(nRebin)
        hZHcc[y] = Rebinning(hZHcc[y],[lowM,highM])
        hZHcc[y].Rebin(nRebin)
        hZHbb[y] = Rebinning(hZHbb[y],[lowM,highM])
        hZHbb[y].Rebin(nRebin)
        hWHcc[y] = Rebinning(hWHcc[y],[lowM,highM])
        hWHcc[y].Rebin(nRebin)
        hWHbb[y] = Rebinning(hWHbb[y],[lowM,highM])
        hWHbb[y].Rebin(nRebin)
        hVHcc[y] = Rebinning(hVHcc[y],[lowM,highM])
        hVHcc[y].Rebin(nRebin)
        hVHbb[y] = Rebinning(hVHbb[y],[lowM,highM])
        hVHbb[y].Rebin(nRebin)
        hTop[y] = Rebinning(hTop[y],[lowM,highM])
        hTop[y].Rebin(nRebin)
        hVJ[y] = Rebinning(hVJ[y],[lowM,highM])
        hVJ[y].Rebin(nRebin)
        hVV[y] = Rebinning(hVV[y],[lowM,highM])
        hVV[y].Rebin(nRebin)
        hVVcc[y] = Rebinning(hVVcc[y],[lowM,highM])
        hVVcc[y].Rebin(nRebin)
        hVVbb[y] = Rebinning(hVVbb[y],[lowM,highM])
        hVVbb[y].Rebin(nRebin)
        hVVother[y] = Rebinning(hVVother[y],[lowM,highM])
        hVVother[y].Rebin(nRebin)

        if doCustomBinning:
          hDat[y] = utl_func.customBin(hDat[y],xDiv)
          hQCD[y] = utl_func.customBin(hQCD[y],xDiv)
          hZHcc[y] = utl_func.customBin(hZHcc[y],xDiv)
          hZHbb[y] = utl_func.customBin(hZHbb[y],xDiv)
          hWHcc[y] = utl_func.customBin(hWHcc[y],xDiv)
          hWHbb[y] = utl_func.customBin(hWHbb[y],xDiv)
          hVHcc[y] = utl_func.customBin(hVHcc[y],xDiv)
          hVHbb[y] = utl_func.customBin(hVHbb[y],xDiv)
          hTop[y] = utl_func.customBin(hTop[y],xDiv)
          hVJ[y] = utl_func.customBin(hVJ[y],xDiv)
          hVV[y] = utl_func.customBin(hVV[y],xDiv)
          hVVcc[y] = utl_func.customBin(hVVcc[y],xDiv)
          hVVbb[y] = utl_func.customBin(hVVbb[y],xDiv)
          hVVother[y] = utl_func.customBin(hVVother[y],xDiv)
        
        #change name and save histogram
        fOut.cd(syst)
        if syst == 'NONE':
          hDat[y].SetName("data_obs_"+syst+"_"+rN+"_"+y)
          hDat[y].Write()
        
        tmp = []
        hQCD[y].SetName("QCD_"+syst+"_"+rN+"_"+y)
        tmp.append(hQCD[y])
        hZHcc[y].SetName("ZHcc_"+syst+"_"+rN+"_"+y)
        tmp.append(hZHcc[y])
        hZHbb[y].SetName("ZHbb_"+syst+"_"+rN+"_"+y)
        tmp.append(hZHbb[y])
        hWHcc[y].SetName("WHcc_"+syst+"_"+rN+"_"+y)
        tmp.append(hWHcc[y])
        hWHbb[y].SetName("WHbb_"+syst+"_"+rN+"_"+y)
        tmp.append(hWHbb[y])
        hVHcc[y].SetName("VHcc_"+syst+"_"+rN+"_"+y)
        tmp.append(hVHcc[y])
        hVHbb[y].SetName("VHbb_"+syst+"_"+rN+"_"+y)
        tmp.append(hVHbb[y])
        hTop[y].SetName("Top_"+syst+"_"+rN+"_"+y)
        tmp.append(hTop[y])
        hVJ[y].SetName("VJ_"+syst+"_"+rN+"_"+y)
        tmp.append(hVJ[y])
        hVV[y].SetName("VV_"+syst+"_"+rN+"_"+y)
        tmp.append(hVV[y])
        hVVcc[y].SetName("VVcc_"+syst+"_"+rN+"_"+y)
        tmp.append(hVVcc[y])
        hVVbb[y].SetName("VVbb_"+syst+"_"+rN+"_"+y)
        tmp.append(hVVbb[y])
        hVVother[y].SetName("VVother_"+syst+"_"+rN+"_"+y)
        tmp.append(hVVother[y])
        
        #now do final modification before writting like remove negative bin
        for hTmp in tmp:
          for i in range(1,hTmp.GetNbinsX()+1):
              if hTmp.GetBinContent(i) < 0:
                  hTmp.SetBinContent(i,0)
                  hTmp.SetBinError(i,0)

        hZHcc[y].Write()
        hZHbb[y].Write()
        hWHcc[y].Write()
        hWHbb[y].Write()
        #TEMP
        #hVHcc[y].Scale(10)
        #hVHbb[y].Scale(10)
        hVHcc[y].Write()
        hVHbb[y].Write()
        hQCD[y].Write()
        hTop[y].Write()
        hVJ[y].Write()
        hVV[y].Write()
        hVVcc[y].Write()
        hVVbb[y].Write()
        hVVother[y].Write()

    ############################
    #Add plots for all years
    ############################
    hDatA = hDat['16'].Clone(hDat['16'].GetName()+'_all')
    hZHccA = hZHcc['16'].Clone(hZHcc['16'].GetName()+'_all')
    hZHbbA = hZHbb['16'].Clone(hZHbb['16'].GetName()+'_all')
    hWHccA = hZHcc['16'].Clone(hWHcc['16'].GetName()+'_all')
    hWHbbA = hZHbb['16'].Clone(hWHbb['16'].GetName()+'_all')
    hVHccA = hVHcc['16'].Clone(hVHcc['16'].GetName()+'_all')
    hVHbbA = hVHbb['16'].Clone(hVHbb['16'].GetName()+'_all')
    hQCDA = hQCD['16'].Clone(hQCD['16'].GetName()+'_all')
    hTopA = hTop['16'].Clone(hTop['16'].GetName()+'_all')
    hVJA = hVJ['16'].Clone(hVJ['16'].GetName()+'_all')
    hVVA = hVV['16'].Clone(hVV['16'].GetName()+'_all')
    hVVccA = hVVcc['16'].Clone(hVVcc['16'].GetName()+'_all')
    hVVbbA = hVVbb['16'].Clone(hVVbb['16'].GetName()+'_all')
    hVVotherA = hVVother['16'].Clone(hVVother['16'].GetName()+'_all')
    
    #sum up all years
    for y in ['17','18']:
      hDatA.Add(hDat[y])
      hZHccA.Add(hZHcc[y])
      hZHbbA.Add(hZHbb[y])
      hWHccA.Add(hWHcc[y])
      hWHbbA.Add(hWHbb[y])
      hVHccA.Add(hVHcc[y])
      hVHbbA.Add(hVHbb[y])
      hQCDA.Add(hQCD[y])
      hTopA.Add(hTop[y])
      hVJA.Add(hVJ[y])
      hVVA.Add(hVV[y])
      hVVccA.Add(hVVcc[y])
      hVVbbA.Add(hVVbb[y])
      hVVotherA.Add(hVVother[y])
     
    #save histograms
    fOut.cd(syst)
    #only save data for nominal
    if syst == 'NONE':
      hDatA.SetName("data_obs_"+syst+"_"+rN)
      hDatA.Write()
    
    print(rN,' ',syst, ": ", hZHccA.Integral())
    hQCDA.SetName("QCD_all_"+syst+"_"+rN)
    hZHccA.SetName("ZHcc_all_"+syst+"_"+rN)
    hZHbbA.SetName("ZHbb_all_"+syst+"_"+rN)
    hWHccA.SetName("WHcc_all_"+syst+"_"+rN)
    hWHbbA.SetName("WHbb_all_"+syst+"_"+rN)
    hVHccA.SetName("VHcc_all_"+syst+"_"+rN)
    hVHbbA.SetName("VHbb_all_"+syst+"_"+rN)
    hTopA.SetName("Top_all_"+syst+"_"+rN)
    hVJA.SetName("VJ_all_"+syst+"_"+rN)
    hVVA.SetName("VV_all_"+syst+"_"+rN)
    hVVccA.SetName("VVcc_all_"+syst+"_"+rN)
    hVVbbA.SetName("VVbb_all_"+syst+"_"+rN)
    hVVotherA.SetName("VVother_all_"+syst+"_"+rN)
    hQCDA.Write()
    hZHccA.Write()
    hZHbbA.Write()
    hWHccA.Write()
    hWHbbA.Write()
    hVHccA.Write()
    hVHbbA.Write()
    hTopA.Write()
    hVJA.Write()
    hVVA.Write()
    hVVccA.Write()
    hVVbbA.Write()
    hVVotherA.Write()