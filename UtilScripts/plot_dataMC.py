import ROOT
import sys,os
import copy
import math
sys.path.append('/uscms_data/d3/duong/CMSSW/CMSSW_7_6_5/src/ZplusC/python3/myutils/')
import configparser
import util_funcs as utl_func
import myutils as utl




ROOT.gROOT.Macro(os.path.expanduser('~/rootLogOn_forPyROOT.C' ))
ROOT.gROOT.SetBatch(True)

def getHist(pN,samName,fH,lS): #samName = ['Electron'],['DY_0J','DY_1J','DY_2J']...
  hOut = {}
  for y in years:
  #for y in ['17']:
    #print(fH[samName[0]][y][0],' ',pN)
    hOut[y] = fH[samName[0]][y][0].Get(pN).Clone() #first sample, first file
    if samName[0] not in ['JetHT']:
      hOut[y].Scale(lS[samName[0]][y][0])

    for iS in range(len(samName)):
      for fi in range(len(fH[samName[iS]][y])):
        if iS == 0 and fi == 0: continue #first sample and first file is already included above 
        h = fH[samName[iS]][y][fi].Get(pN).Clone()
        if samName[iS] not in ['JetHT']:
          h.Scale(lS[samName[iS]][y][fi])
        hOut[y].Add(h)
      
  return hOut

def getHists(pN,pN1s,samName,fH,lS):
    tmp = pN + '_' + pN1s[0]
    h = getHist(tmp,samName,fH,lS)
    for i in range(1,len(pN1s)):
      tmp = pN + '_' + pN1s[i]
      htmp = getHist(tmp,samName,fH,lS)
      for y in years:
        h[y].Add(htmp[y])
    
    return h


def getHistVV(hNin, vv_con, vvName, f, lumi):
    hTmp = getHist(hNin,[vv_con[vvName][0]],fHist,lumiScales)
    for i in range(1,vv_con[vvName].size()):
        hTmp.Add(getHist(hNin,[vv_con[vvName][i]],fHist,lumiScales))
    return hTmp

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

def addBkgr(n,proc,procs,regions):
  for r in regions:
    for y in ['16_preVFP','16','17','18','All']:
      s = 0
      s_err = 0
      for p in procs:
        s += nums[r][y][p][0]
        s_err += nums[r][y][p][1]*nums[r][y][p][1]
      n[r][y][proc] = [s,math.sqrt(s_err)]

def getFilterEff(fName):
  effs = {}
  lines = open(fName).readlines()
  for l in lines:
    l = l.replace("\n","").replace("(","").replace(")","").replace("'","")
    l = l.split(",")
    sN = l[0].replace(".txt","").replace("_MC_2016_preVFP","").replace("_MC_2016","").replace("_MC_2017","").replace("_MC_2018","")
    eff = float(l[2])
    effs[sN] = eff
  return effs



##########################
#Main
##########################
#Frequently use settings

years = ['16_preVFP','16','17','18']

blindDataOption = 1 #0: do not blind data, 1: blind data for signal regions only, 2: blind data for all regions listed in "regions" list
blindRange = [75,140]

#use to get event yield tables. Only count events within lowM-highM
lowM = 50
highM = 200

doCustomBinning_MH = True 
#xDiv_MH = [40.,60.,80.,90.,100.,130.,160.,200.]
xDiv_MH = [40,60,80,100,120,140,160,180,200]

#list here regions you want to make plot for. The list of plots for each region listed in ../Configs/config.ini in [Plot] section
regions = ['ZccHcc_boosted_PN_med','ZccHcc_boosted_PN_med_topCR_pass','ZccHcc_boosted_PN_med_qcdCR','VHcc_boosted_PN_med','VHcc_boosted_PN_med_topCR_pass','VHcc_boosted_PN_med_qcdCR']
summary_eventCount_name = 'summary_eventCount_VH_tmp.txt'

cfg = utl.BetterConfigParser()
cfg.read('../Configs/config.ini')

use_NLO_VV = True 
breakVV = True #this is use to separate VV=VZcc,VZbb, and "other VV" = VZqq(not including cc and bb) and WW

#create directory to store plots
plotFolder = '../Plots_NONE_fromPeter_2025JanFeb'
aff1 = ''
aff2 = ''
if use_NLO_VV: aff1 = 'NLO_VV'
if breakVV: aff2 = 'breakVV'
if aff1 != '': plotFolder = plotFolder + '_' + aff1
if aff2 != '': plotFolder = plotFolder + '_' + aff2
plotFolder = plotFolder + '/'

#NOT IN USE, DO NOT CHANGE THEM
makePostfit_MH = False
binContent_MH = [571.812301697, 572.658544275, 201.083748486, 165.396563785, 535.790231977, 336.389680618, 163.075072714]
binError_MH = [14.524979842, 14.6705438854, 9.64396617479, 10.4968651727, 16.9819213256, 11.6571979069, 8.3184613374]
correctFilterEff = False #only used for postProcessing 
filterEff_name = "../Configs/filterEff.txt"
use_bEnriched_BGenFilter = False  
doUseReweightForQCD = False
if doUseReweightForQCD: plotFolder = '../Plots_tmp_qcdReweight_improveWeighting_lepVeto_msoftdrop/'
if use_bEnriched_BGenFilter: plotFolder = '../Test_bEnriched_BGenFilter/'


#End frequently use settings
##################################################

plotFolders = {}
for y in years:
  plotFolders[y] = plotFolder+'/'+y
  os.system('mkdir -p '+plotFolders[y])
plotFolders['All'] = plotFolder+'/All'
os.system('mkdir -p '+plotFolders['All'])

#file to store cross check plots
fCheck = ROOT.TFile(plotFolder+"/check.root","RECREATE") 

#NOT IN USE, DO NOT CHANGE. File to store histograms for limit settings and qcd background estimation
fOut = ROOT.TFile(plotFolder+"/qcd_estimate.root","RECREATE") 

lumiS = {}
for y in years:
  lumiTmp = float(cfg.get('General','lumi_'+y))/1000.
  lumiTmp = format("%.1f" % lumiTmp)
  lumiS[y] = str(lumiTmp)
print(lumiS) 

#list all processes you want to plot here. For VV, list both LO and NLO.
#ss = ['JetHT','ZH_HToCC_ZToQQ','ggZH_HToCC_ZToQQ','ZH_HToBB_ZToQQ','ggZH_HToBB_ZToQQ','QCD_HT500to700','QCD_HT700to1000','QCD_HT1000to1500','QCD_HT1500to2000','QCD_HT2000toInf']
#ss = ['JetHT','ZH_HToCC_ZToQQ','ggZH_HToCC_ZToQQ','ZH_HToBB_ZToQQ','ggZH_HToBB_ZToQQ','QCD_HT300to500_v9','QCD_HT500to700_v9','QCD_HT700to1000_v9','QCD_HT1000to1500_v9','QCD_HT1500to2000_v9','QCD_HT2000toInf_v9','WJetsToQQ_HT-400to600','WJetsToQQ_HT-600to800','WJetsToQQ_HT-800toInf','WJetsToLNu_HT-400to600','WJetsToLNu_HT-600to800','WJetsToLNu_HT-800to1200','WJetsToLNu_HT-1200to2500','WJetsToLNu_HT-2500toInf','ZJetsToQQ_HT-400to600','ZJetsToQQ_HT-600to800','ZJetsToQQ_HT-800toInf','TTToHadronic','TTToSemiLeptonic','TTTo2L2Nu','ST_t-channel_antitop','ST_t-channel_top','ST_tW-channel_antitop','ST_tW-channel_top','WW','WZ','ZZ']
#ss = ['JetHT','ZH_HToCC_ZToQQ','ggZH_HToCC_ZToQQ','ZH_HToBB_ZToQQ','ggZH_HToBB_ZToQQ','WH_HToCC_WToQQ','WH_HToBB_WToQQ','QCD_HT200to300_v9','QCD_HT300to500_v9','QCD_HT500to700_v9','QCD_HT700to1000_v9','QCD_HT1000to1500_v9','QCD_HT1500to2000_v9','QCD_HT2000toInf_v9','WJetsToQQ_HT-400to600','WJetsToQQ_HT-600to800','WJetsToQQ_HT-800toInf','WJetsToLNu_HT-400to600','WJetsToLNu_HT-600to800','WJetsToLNu_HT-800to1200','WJetsToLNu_HT-1200to2500','WJetsToLNu_HT-2500toInf','ZJetsToQQ_HT-400to600','ZJetsToQQ_HT-600to800','ZJetsToQQ_HT-800toInf','TTToHadronic','TTToSemiLeptonic','TTTo2L2Nu','ST_tW-channel_antitop','ST_tW-channel_top','WW','WZ','ZZ']
#TEMP: no ST_tW-channel_antitop for now since no available from current processing
#ss = ['JetHT','ZH_HToCC_ZToQQ','ggZH_HToCC_ZToQQ','ZH_HToBB_ZToQQ','ggZH_HToBB_ZToQQ','WH_HToCC_WToQQ','WH_HToBB_WToQQ','QCD_HT200to300_v9','QCD_HT300to500_v9','QCD_HT500to700_v9','QCD_HT700to1000_v9','QCD_HT1000to1500_v9','QCD_HT1500to2000_v9','QCD_HT2000toInf_v9','WJetsToQQ_HT-400to600','WJetsToQQ_HT-600to800','WJetsToQQ_HT-800toInf','WJetsToLNu_HT-400to600','WJetsToLNu_HT-600to800','WJetsToLNu_HT-800to1200','WJetsToLNu_HT-1200to2500','WJetsToLNu_HT-2500toInf','ZJetsToQQ_HT-400to600','ZJetsToQQ_HT-600to800','ZJetsToQQ_HT-800toInf','TTToHadronic','TTToSemiLeptonic','TTTo2L2Nu','ST_tW-channel_top','WW','WZ','ZZ']

ss = [
  'JetHT',
  'ZH_HToCC_ZToQQ','ggZH_HToCC_ZToQQ','ZH_HToBB_ZToQQ','ggZH_HToBB_ZToQQ',
  'WH_HToCC_WToQQ','WH_HToBB_WToQQ',
  'QCD_HT200to300_v9','QCD_HT300to500_v9','QCD_HT500to700_v9','QCD_HT700to1000_v9','QCD_HT1000to1500_v9','QCD_HT1500to2000_v9','QCD_HT2000toInf_v9',
  'WJetsToQQ_HT-400to600','WJetsToQQ_HT-600to800','WJetsToQQ_HT-800toInf',
  'WJetsToLNu_HT-400to600','WJetsToLNu_HT-600to800','WJetsToLNu_HT-800to1200','WJetsToLNu_HT-1200to2500','WJetsToLNu_HT-2500toInf',
  'ZJetsToQQ_HT-400to600','ZJetsToQQ_HT-600to800','ZJetsToQQ_HT-800toInf',
  'TTToHadronic','TTToSemiLeptonic','TTTo2L2Nu',
  'ST_tW-channel_top','ST_tW-channel_antitop','ST_t-channel_top','ST_t-channel_antitop',
  'WW','WZ','ZZ',
  'WWTo1L1Nu2Q','WWTo4Q','WZTo4Q','WZToLNu2B','WZTo1L1Nu2Q','WZTo2Q2L','ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q']

#TMP not use WZTo2Q2L for now missing 2017 
#ss = ['JetHT','ZH_HToCC_ZToQQ','ggZH_HToCC_ZToQQ','ZH_HToBB_ZToQQ','ggZH_HToBB_ZToQQ','WH_HToCC_WToQQ','WH_HToBB_WToQQ','QCD_HT200to300_v9','QCD_HT300to500_v9','QCD_HT500to700_v9','QCD_HT700to1000_v9','QCD_HT1000to1500_v9','QCD_HT1500to2000_v9','QCD_HT2000toInf_v9','WJetsToQQ_HT-400to600','WJetsToQQ_HT-600to800','WJetsToQQ_HT-800toInf','WJetsToLNu_HT-400to600','WJetsToLNu_HT-600to800','WJetsToLNu_HT-800to1200','WJetsToLNu_HT-1200to2500','WJetsToLNu_HT-2500toInf','ZJetsToQQ_HT-400to600','ZJetsToQQ_HT-600to800','ZJetsToQQ_HT-800toInf','TTToHadronic','TTToSemiLeptonic','TTTo2L2Nu','ST_tW-channel_top','WW','WZ','ZZ','WWTo1L1Nu2Q','WWTo4Q','WZTo4Q','WZToLNu2B','WZTo1L1Nu2Q','ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q']

#vv = {'WW':['WW'],'WZ':['WZ'],'ZZ':['ZZ']}
#if use_NLO_VV:
#  vv = {'WW':['WWTo1L1Nu2Q','WWTo4Q'],\
#      'WZ':['WZTo4Q','WZToLNu2B','WZTo1L1Nu2Q','WZTo2Q2L'],\
#      'ZZ':['ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q']
#       }

#WWTo1L1Nu2Q_NLO": 50.9,
#WWTo4Q_NLO": 51.57,
#WZTo4Q_NLO": 23.43,
#WZToLNu2B_NLO": 2.525,
#WZTo1L1Nu2Q_NLO": 9.152,
#WZTo2Q2L_NLO": 6.422,
#ZZTo2Q2L_NLO": 3.705,
#ZZTo2Nu2Q_NLO": 4.498,
#ZZTo4Q_NLO": 3.295,

if use_bEnriched_BGenFilter: 
  ss = ['JetHT','ZH_HToCC_ZToQQ','ggZH_HToCC_ZToQQ','ZH_HToBB_ZToQQ','ggZH_HToBB_ZToQQ','QCD_bEnriched_HT300to500','QCD_bEnriched_HT500to700','QCD_bEnriched_HT700to1000','QCD_bEnriched_HT1000to1500','QCD_bEnriched_HT1500to2000','QCD_bEnriched_HT2000toInf','QCD_HT300to500_BGenFilter','QCD_HT500to700_BGenFilter','QCD_HT700to1000_BGenFilter','QCD_HT1000to1500_BGenFilter','QCD_HT1500to2000_BGenFilter','QCD_HT2000toInf_BGenFilter','WJetsToQQ_HT-400to600','WJetsToQQ_HT-600to800','WJetsToQQ_HT-800toInf','WJetsToLNu_HT-400to600','WJetsToLNu_HT-600to800','WJetsToLNu_HT-800to1200','WJetsToLNu_HT-1200to2500','WJetsToLNu_HT-2500toInf','ZJetsToQQ_HT-400to600','ZJetsToQQ_HT-600to800','ZJetsToQQ_HT-800toInf','TTToHadronic','TTToSemiLeptonic','TTTo2L2Nu','ST_t-channel_antitop','ST_t-channel_top','ST_tW-channel_antitop','ST_tW-channel_top','WW','WZ','ZZ']

filter_effs = getFilterEff(filterEff_name)
print(filter_effs)

fNames = {}
xSecs = {}
lumiScales = {}
fHist = {}


for s in ss:

  fNames[s] = {}
  xSecs[s] = {}
  lumiScales[s] = {}
  fHist[s] = {}

  for y in years:
    lumi = float(cfg.get('General','lumi_'+y))
    names = cfg.get(s,'file_'+y).split(',') #multiple names is possible, for example: Single top = t-channels, s-channels ...
    print('>>>>>>>: ', len(names))
    xSecTmps = ['1']*len(names) #each name corresponds to a cross section
    kfactor = ['1']*len(names) #each name corresponds to a cross section
    if s not in ['JetHT']:
      xSecTmps = cfg.get(s,'xSec_'+y).split(',')

    fNames[s][y] = []
    xSecs[s][y] = []
    fHist[s][y] = []
    for iN in names:
      #if 'JetHT' in iN: fNames[s][y].append(cfg.get('Paths','pathData') + '/' + iN)
      #else: fNames[s][y].append(cfg.get('Paths','pathMC') + '/' + iN)
      if 'JetHT' in iN: fNames[s][y].append(cfg.get('Paths','path') + '/' + iN)
      else: fNames[s][y].append(cfg.get('Paths','path') + '/' + iN)
      fHist[s][y].append(ROOT.TFile.Open(fNames[s][y][-1],'READ'))
    
    print(xSecTmps)
    for iS in xSecTmps:
      filterEff = 1.
      if 'JetHT' not in s and correctFilterEff: filterEff = filter_effs[s]
      #in case there is kfactor in cross section
      if '*' in iS:
        iS = iS.split('*')
      if len(iS) == 2:
        xSecs[s][y].append(float(iS[0])*float(iS[1])*filterEff)
      else:
        xSecs[s][y].append(float(iS)*filterEff)

    lumiScales[s][y] = [1]*len(names)
    for iN in range(len(fNames[s][y])):
      if s not in ['JetHT']:
        print(s, y, iN, fNames[s][y][iN])
        try:
          lumiScales[s][y][iN] = utl_func.scaleToLumi1(fNames[s][y][iN],xSecs[s][y][iN],lumi,'Nevt_all_VbbHcc_boosted')
        except AttributeError:
          print("\n Can not find lumiScale for ", s, " ", y, " ", iN, " .Set lumiScale to 0")
          lumiScales[s][y][iN] = 0
 

nums = {}

for r in regions:
  
  nums[r] = {}
  
  plotNames = cfg.get('Plots',r + '_plot').split(',')

  for plN in plotNames:
    
    #TEMP: need to check what else have break VV, only HMass?
    #if breakVV and 'HMass' not in plN: continue
    
    #FIX?: is this fine for checking other variables
    has_desired_var = (
      ('HMass' in plN) or ('ZMass' in plN) or
      ('HPt' in plN) or ('ZPt' in plN) or
      ('HFlav' in plN) or ('ZFlav' in plN) or
      ('DPhiZH' in plN) or ('DEtaZH' in plN) or
      ('CutFlow' in plN) or
      ('pt_jet' in plN) or ('pt_jet0' in plN) or ('pt_jet1' in plN) or
      ('m_jet' in plN) or ('m_jet0' in plN) or ('m_jet1' in plN)
    )
    if breakVV and not has_desired_var: continue
    
    hN = r + '_' + plN
    if plN == 'CutFlow':
      hN = plN + '_' + r
     
    print(hN, plN)
    
    if 'qcd' in r and 'CutFlow' in plN: continue

    hDat = getHist(hN,['JetHT'],fHist,lumiScales)
    print(" ", hN)
    hZHcc = getHist(hN,['ZH_HToCC_ZToQQ'],fHist,lumiScales)
    hZHbb = getHist(hN,['ZH_HToBB_ZToQQ'],fHist,lumiScales)
    hggZHcc = getHist(hN,['ggZH_HToCC_ZToQQ'],fHist,lumiScales)
    hggZHbb = getHist(hN,['ggZH_HToBB_ZToQQ'],fHist,lumiScales)
    hWHcc = getHist(hN,['WH_HToCC_WToQQ'],fHist,lumiScales)
    hWHbb = getHist(hN,['WH_HToBB_WToQQ'],fHist,lumiScales)

    #hQCD = getHist(hN,['QCD_HT500to700','QCD_HT700to1000','QCD_HT1000to1500','QCD_HT1500to2000','QCD_HT2000toInf'],fHist,lumiScales)
    if not use_bEnriched_BGenFilter:
        if not doUseReweightForQCD or 'CutFlow' in plN:
            hQCD = getHist(hN,['QCD_HT200to300_v9','QCD_HT300to500_v9','QCD_HT500to700_v9','QCD_HT700to1000_v9','QCD_HT1000to1500_v9','QCD_HT1500to2000_v9','QCD_HT2000toInf_v9'],fHist,lumiScales)
        else:
            hQCD = getHist(hN.replace("PN_med","PN_med_xccWeight"),['QCD_HT200to300_v9','QCD_HT300to500_v9','QCD_HT500to700_v9','QCD_HT700to1000_v9','QCD_HT1000to1500_v9','QCD_HT1500to2000_v9','QCD_HT2000toInf_v9'],fHist,lumiScales)
    else:
      hQCD = getHist(hN,['QCD_bEnriched_HT300to500','QCD_bEnriched_HT500to700','QCD_bEnriched_HT700to1000','QCD_bEnriched_HT1000to1500','QCD_bEnriched_HT1500to2000','QCD_bEnriched_HT2000toInf'],fHist,lumiScales)
      hQCD_BGenFilter = getHist(hN,['QCD_HT300to500_BGenFilter','QCD_HT500to700_BGenFilter','QCD_HT700to1000_BGenFilter','QCD_HT1000to1500_BGenFilter','QCD_HT1500to2000_BGenFilter','QCD_HT2000toInf_BGenFilter'],fHist,lumiScales)
      #hQCD = getHist(hN,['QCD_HT300to500_BGenFilter','QCD_HT500to700_BGenFilter','QCD_HT700to1000_BGenFilter','QCD_HT1000to1500_BGenFilter','QCD_HT1500to2000_BGenFilter','QCD_HT2000toInf_BGenFilter'],fHist,lumiScales)
      for y in years:
        hQCD[y].Add(hQCD_BGenFilter[y])
    
    hWJ = getHist(hN,['WJetsToQQ_HT-400to600','WJetsToQQ_HT-600to800','WJetsToQQ_HT-800toInf','WJetsToLNu_HT-400to600','WJetsToLNu_HT-600to800','WJetsToLNu_HT-800to1200','WJetsToLNu_HT-1200to2500','WJetsToLNu_HT-2500toInf'],fHist,lumiScales)
    hZJ = getHist(hN,['ZJetsToQQ_HT-400to600','ZJetsToQQ_HT-600to800','ZJetsToQQ_HT-800toInf'],fHist,lumiScales)
    hTT = getHist(hN,['TTToHadronic','TTToSemiLeptonic','TTTo2L2Nu'],fHist,lumiScales)
    #hST = getHist(hN,['ST_t-channel_antitop','ST_t-channel_top','ST_tW-channel_antitop','ST_tW-channel_top'],fHist,lumiScales)
    #hST = getHist(hN,['ST_tW-channel_antitop','ST_tW-channel_top'],fHist,lumiScales)
    #TEMP: just use what we have for now no available from current processing
    hST = getHist(hN,['ST_tW-channel_top'],fHist,lumiScales)
    if not use_NLO_VV:
      hWW = getHist(hN,['WW'],fHist,lumiScales)
      hWZ = getHist(hN,['WZ'],fHist,lumiScales)
      hZZ = getHist(hN,['ZZ'],fHist,lumiScales)

      if breakVV and 'HMass' in plN:
          #hVVcc = getHists(hN,['VZqqcc','VZqccc','VZcccc'],['WW','WZ','ZZ'],fHist,lumiScales) 
          #hVVbb = getHists(hN,['VZqqbb','VZqcbb','VZbbbb'],['WW','WZ','ZZ'],fHist,lumiScales) 
          #hVVbbcc = getHists(hN,['VZbbcc'],['WW','WZ','ZZ'],fHist,lumiScales) 
          #hVVqqqq = getHists(hN,['VZqqqq'],['WW','WZ','ZZ'],fHist,lumiScales)
          #hVVcc = getHists(hN,['VZqqcc','VZqccc','VZcccc','VZbbcc'],['WZ','ZZ'],fHist,lumiScales) 
          #hVVbb = getHists(hN,['VZqqbb','VZqcbb','VZbbbb'],['WZ','ZZ'],fHist,lumiScales) 
          #hVVother = getHists(hN,['VZqqqq'],['WZ','ZZ'],fHist,lumiScales)
          rTmp = r + '_VZcc'
          hVVcc = getHist(hN.replace(r,rTmp),['WZ','ZZ'],fHist,lumiScales) 
          rTmp = r + '_VZbb'
          hVVbb = getHists(hN.replace(r,rTmp),['WZ','ZZ'],fHist,lumiScales) 
          rTmp = r + '_VZother'
          hVVother = getHists(hN.replace(r,rTmp),['WZ','ZZ'],fHist,lumiScales)
          for ytmp in years:
              hVVother[ytmp].Add(hWW[ytmp])
    if use_NLO_VV:
      hWW = getHist(hN,['WWTo1L1Nu2Q','WWTo4Q'],fHist,lumiScales)
      hWZ = getHist(hN,['WZTo1L1Nu2Q','WZTo2Q2L','WZToLNu2B','WZTo4Q'],fHist,lumiScales)
      hZZ = getHist(hN,['ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q'],fHist,lumiScales)
      if breakVV and 'HMass' in plN:
          #hVVcc = getHists(hN,['VZqqcc','VZqccc','VZcccc','VZbbcc'],['WZTo1L1Nu2Q','WZTo2Q2L','WZToLNu2B','WZTo4Q','ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q'],fHist,lumiScales) 
          #hVVbb = getHists(hN,['VZqqbb','VZqcbb','VZbbbb'],['WZTo1L1Nu2Q','WZTo2Q2L','WZToLNu2B','WZTo4Q','ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q'],fHist,lumiScales) 
          #hVVother = getHists(hN,['VZqqqq'],['WZTo1L1Nu2Q','WZTo2Q2L','WZToLNu2B','WZTo4Q','ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q'],fHist,lumiScales) 
          rTmp = r + '_VZcc'
          hVVcc = getHist(hN.replace(r,rTmp),['WZTo1L1Nu2Q','WZTo2Q2L','WZToLNu2B','WZTo4Q','ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q'],fHist,lumiScales) 
          rTmp = r + '_VZbb'
          hVVbb = getHist(hN.replace(r,rTmp),['WZTo1L1Nu2Q','WZTo2Q2L','WZToLNu2B','WZTo4Q','ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q'],fHist,lumiScales) 
          rTmp = r + '_VZother'
          hVVother = getHist(hN.replace(r,rTmp),['WZTo1L1Nu2Q','WZTo2Q2L','WZToLNu2B','WZTo4Q','ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q'],fHist,lumiScales) 
          for ytmp in years:
              hVVother[ytmp].Add(hWW[ytmp])
          #hVVcc = getHists(hN,['VZqqcc','VZqccc','VZcccc'],['WWTo1L1Nu2Q','WWTo4Q','WZTo1L1Nu2Q','WZTo2Q2L','WZToLNu2B','WZTo4Q','ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q'],fHist,lumiScales) 
          #hVVbb = getHists(hN,['VZqqbb','VZqcbb','VZbbbb'],['WWTo1L1Nu2Q','WWTo4Q','WZTo1L1Nu2Q','WZTo2Q2L','WZToLNu2B','WZTo4Q','ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q'],fHist,lumiScales) 
          #hVVbbcc = getHists(hN,['VZbbcc'],['WWTo1L1Nu2Q','WWTo4Q','WZTo1L1Nu2Q','WZTo2Q2L','WZToLNu2B','WZTo4Q','ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q'],fHist,lumiScales) 
          #hVVqqqq = getHists(hN,['VZqqqq'],['WWTo1L1Nu2Q','WWTo4Q','WZTo1L1Nu2Q','WZTo2Q2L','WZToLNu2B','WZTo4Q','ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q'],fHist,lumiScales) 

    ##########################
    #stack plots for each year 
    ##########################

    for y in years:
      #get number of events
      if 'HMass' in plN:
        nums[r][y] = {}
        nums[r][y]['JetHT'] = getHistIntegral(hDat[y],lowM,highM)
        nums[r][y]['ZH_HToCC_ZToQQ'] = getHistIntegral(hZHcc[y],lowM,highM)
        nums[r][y]['ZH_HToBB_ZToQQ'] = getHistIntegral(hZHbb[y],lowM,highM)
        nums[r][y]['ggZH_HToCC_ZToQQ'] = getHistIntegral(hggZHcc[y],lowM,highM)
        nums[r][y]['ggZH_HToBB_ZToQQ'] = getHistIntegral(hggZHbb[y],lowM,highM)
        nums[r][y]['WH_HToCC_WToQQ'] = getHistIntegral(hWHcc[y],lowM,highM)
        nums[r][y]['WH_HToBB_WToQQ'] = getHistIntegral(hWHbb[y],lowM,highM)
        nums[r][y]['QCD'] = getHistIntegral(hQCD[y],lowM,highM)
        nums[r][y]['WJ'] = getHistIntegral(hWJ[y],lowM,highM)
        nums[r][y]['ZJ'] = getHistIntegral(hZJ[y],lowM,highM)
        nums[r][y]['TT'] = getHistIntegral(hTT[y],lowM,highM)
        nums[r][y]['ST'] = getHistIntegral(hST[y],lowM,highM)
        nums[r][y]['WW'] = getHistIntegral(hWW[y],lowM,highM)
        nums[r][y]['WZ'] = getHistIntegral(hWZ[y],lowM,highM)
        nums[r][y]['ZZ'] = getHistIntegral(hZZ[y],lowM,highM)
        if breakVV and 'HMass' in plN:
            nums[r][y]['VVcc'] = getHistIntegral(hVVcc[y],lowM,highM)
            nums[r][y]['VVbb'] = getHistIntegral(hVVbb[y],lowM,highM)
            nums[r][y]['VVother'] = getHistIntegral(hVVother[y],lowM,highM)
            #nums[r][y]['VVbbcc'] = getHistIntegral(hVVbbcc[y],lowM,highM)
            #nums[r][y]['VVqqqq'] = getHistIntegral(hVVqqqq[y],lowM,highM)

      #for y in ['17']:
      tmps = cfg.get(plN,'xAxisRange').split(',')
      xA_range = []
      for i in [0,1]:
        if 'Pi' in tmps[i]: xA_range.append(ROOT.TMath.Pi())
        else: xA_range.append(float(tmps[i]))
      
      xA_title = cfg.get(plN,'xAxisTitle')
      nRebin = int(cfg.get(plN,'rebin'))
      
      hData_clone = hDat[y].Clone(hDat[y].GetName()+"clone").Rebin(nRebin)
      #if doBlindData and "HMass" in plN and ('select4' in r or 'select3' in r or 'PN_med' in r):
      if "HMass" in plN and ((blindDataOption == 1 and not('topCR' in r or 'qcdCR' in r)) or blindDataOption == 2):
        nBin1 = hData_clone.GetXaxis().FindFixBin(blindRange[0])
        nBin2 = hData_clone.GetXaxis().FindFixBin(blindRange[1])
        for i in range(nBin1,nBin2+1):
          hData_clone.SetBinContent(i,-1)
          hData_clone.SetBinError(i,0)
      
      if not breakVV:
        plots_process = [hData_clone,hQCD[y].Clone().Rebin(nRebin),hST[y].Clone().Rebin(nRebin),hTT[y].Clone().Rebin(nRebin),hZJ[y].Clone().Rebin(nRebin),hWJ[y].Clone().Rebin(nRebin),hWW[y].Clone().Rebin(nRebin),hWZ[y].Clone().Rebin(nRebin),hZZ[y].Clone().Rebin(nRebin),hZHbb[y].Clone().Rebin(nRebin),hggZHbb[y].Clone().Rebin(nRebin),hWHbb[y].Clone().Rebin(nRebin),hZHcc[y].Clone().Rebin(nRebin),hggZHcc[y].Clone().Rebin(nRebin),hWHcc[y].Clone().Rebin(nRebin)]
      else:
        #plots_process = [hData_clone,hQCD[y].Clone().Rebin(nRebin),hST[y].Clone().Rebin(nRebin),hTT[y].Clone().Rebin(nRebin),hZJ[y].Clone().Rebin(nRebin),hWJ[y].Clone().Rebin(nRebin),hVVcc[y].Clone().Rebin(nRebin),hVVbb[y].Clone().Rebin(nRebin),hVVbbcc[y].Clone().Rebin(nRebin),hVVqqqq[y].Clone().Rebin(nRebin),hZHbb[y].Clone().Rebin(nRebin),hggZHbb[y].Clone().Rebin(nRebin),hWHbb[y].Clone().Rebin(nRebin),hZHcc[y].Clone().Rebin(nRebin),hggZHcc[y].Clone().Rebin(nRebin),hWHcc[y].Clone().Rebin(nRebin)]
        plots_process = [hData_clone,hQCD[y].Clone().Rebin(nRebin),hST[y].Clone().Rebin(nRebin),hTT[y].Clone().Rebin(nRebin),hZJ[y].Clone().Rebin(nRebin),hWJ[y].Clone().Rebin(nRebin),hVVother[y].Clone().Rebin(nRebin),hVVbb[y].Clone().Rebin(nRebin),hVVcc[y].Clone().Rebin(nRebin),hZHbb[y].Clone().Rebin(nRebin),hggZHbb[y].Clone().Rebin(nRebin),hWHbb[y].Clone().Rebin(nRebin),hZHcc[y].Clone().Rebin(nRebin),hggZHcc[y].Clone().Rebin(nRebin),hWHcc[y].Clone().Rebin(nRebin)]
      #custom bining
      #for iPl in range(0,len(plots_process)):
      #  plots_process[iPl] = utl_func.customBin(plots_process[iPl], xDiv)

      #plots_process = [hDat[y].Clone().Rebin(nRebin),hZHbb[y].Clone().Rebin(nRebin),hggZHbb[y].Clone().Rebin(nRebin),hZHcc[y].Clone().Rebin(nRebin),hggZHcc[y].Clone().Rebin(nRebin)]
      
      plotNames_process = []
      dataTitle = 'Data (JetHT, 20'+y+')' 
      #plotNames_process = [dataTitle, 'ZHbb', 'ggZHbb', 'ZHcc', 'ggZHcc']
      if not breakVV:
        plotNames_process = [dataTitle, 'QCD', 'Single top', 't#bar{t}','Z + jets', 'W + jets', 'WW', 'WZ', 'ZZ', 'ZH (H#rightarrow bb)', 'ggZH (H#rightarrow bb)','WH (H#rightarrow bb)','ZH (H#rightarrow cc)', 'ggZH (H#rightarrow cc)','WH (H#rightarrow cc)']
      else:
        plotNames_process = [dataTitle, 'QCD', 'Single top', 't#bar{t}','Z + jets', 'W + jets', 'VV (others)','VZ (Z#rightarrow bb)','VZ (Z#rightarrow cc)', 'ZH (H#rightarrow bb)', 'ggZH (H#rightarrow bb)','WH (H#rightarrow bb)','ZH (H#rightarrow cc)', 'ggZH (H#rightarrow cc)','WH (H#rightarrow cc)']

      #utl_func.makeStackPlot(plots_process, plotNames_process, plN + '_' + r +'_'+y, plotFolder + '/20'+y, xA_title, xA_range, 'MC unc. (stat.)', False, lumi=lumiS[y])
      logY=False
      #if 'CutFlow' in plN or 'ccTagDis' in plN or 'bbTagDis' in plN or 'bbPN' in plN or 'ccPN' in plN: logY=True
      if 'CutFlow' in plN or 'ccTagDis' in plN or 'bbTagDis' in plN: logY=True
      #utl_func.makeStackPlot(plots_process, plotNames_process, plN + '_' + r +'_'+y, plotFolder + '/20'+y+'_QCDv9', xA_title, xA_range, 'MC unc. (stat.)', False, logY=logY, lumi=lumiS[y])
      normBinWidth = -1
      if makePostfit_MH and plN=="HMass": normBinWidth = 1
      print(">>>>>>>>>>>>>>", normBinWidth)
      utl_func.makeStackPlot(plots_process, plotNames_process, plN + '_' + r +'_'+y, plotFolders[y], xA_title, xA_range, 'MC unc. (stat.)', False, logY=logY, normBinWidth=normBinWidth,lumi=lumiS[y])

      #save check histograms
      fCheck.cd()
      hQCD[y].Write()
    
    ############################
    #Plot control plot for all years
    ############################
    print(">>>>>>>>>>>>>>")
    print(hDat)
      
    hDatA = hDat['16'].Clone(hDat['16'].GetName()+'_all')
    hZHccA = hZHcc['16'].Clone(hZHcc['16'].GetName()+'_all')
    hZHbbA = hZHbb['16'].Clone(hZHbb['16'].GetName()+'_all')
    hggZHccA = hggZHcc['16'].Clone(hggZHcc['16'].GetName()+'_all')
    hggZHbbA = hggZHbb['16'].Clone(hggZHbb['16'].GetName()+'_all')
    hWHccA = hWHcc['16'].Clone(hWHcc['16'].GetName()+'_all')
    hWHbbA = hWHbb['16'].Clone(hWHbb['16'].GetName()+'_all')
    hQCDA = hQCD['16'].Clone(hQCD['16'].GetName()+'_all')
    hSTA = hST['16'].Clone(hST['16'].GetName()+'_all')
    hTTA = hTT['16'].Clone(hTT['16'].GetName()+'_all')
    hZJA = hZJ['16'].Clone(hZJ['16'].GetName()+'_all')
    hWJA = hWJ['16'].Clone(hWJ['16'].GetName()+'_all')
    hWWA = hWW['16'].Clone(hWW['16'].GetName()+'_all')
    hWZA = hWZ['16'].Clone(hWZ['16'].GetName()+'_all')
    hZZA = hZZ['16'].Clone(hZZ['16'].GetName()+'_all')
    if breakVV and 'HMass' in plN:
        hVVccA = hVVcc['16'].Clone(hVVcc['16'].GetName()+'_all')
        hVVbbA = hVVbb['16'].Clone(hVVbb['16'].GetName()+'_all')
        hVVotherA = hVVother['16'].Clone(hVVother['16'].GetName()+'_all')
    
    #sum up all years
    for y in years:
      if y == '16':continue
      hDatA.Add(hDat[y])
      hZHccA.Add(hZHcc[y])
      hZHbbA.Add(hZHbb[y])
      hggZHccA.Add(hggZHcc[y])
      hggZHbbA.Add(hggZHbb[y])
      hWHccA.Add(hWHcc[y])
      hWHbbA.Add(hWHbb[y])
      hQCDA.Add(hQCD[y])
      hSTA.Add(hST[y])
      hTTA.Add(hTT[y])
      hZJA.Add(hZJ[y])
      hWJA.Add(hWJ[y])
      hWWA.Add(hWW[y])
      hWZA.Add(hWZ[y])
      hZZA.Add(hZZ[y])
      if breakVV and 'HMass' in plN:
          hVVccA.Add(hVVcc[y])
          hVVbbA.Add(hVVbb[y])
          hVVotherA.Add(hVVother[y])
    #save HMass MC QCD histograms used to estimate QCD
    if "HMass" in plN and ("PN_med" in r or "PN_med_CR_qcd" in r or "PN_med_VR_qcd" in r or "PN_med_CR1_qcd"):
      print("\n I will save histograms for QCD background estimation")
      qcdName = ""
      dataName = ""
      otherName = ""
      #signal region
      if "PN_med" in r:
        qcdName = "QCD_SR_" + r
        dataName = "Data_SR_" + r
        otherName = "Other_SR_" + r
      if "PN_med_VR_qcd" in r:
        qcdName = "QCD_VR_"+r
        dataName = "Data_VR_"+r
        otherName = "Other_VR_"+r
      if "PN_med_CR_qcd" in r:
        qcdName = "QCD_CR_"+r
        dataName = "Data_CR_"+r
        otherName = "Other_CR_"+r
      if "PN_med_CR1_qcd" in r:
        qcdName = "QCD_CR1_"+r
        dataName = "Data_CR1_"+r
        otherName = "Other_CR1_"+r

      
      #save contributions from other processes to do subtraction
      hOther = hTTA.Clone(otherName)
      hOther.Add(hSTA)
      hOther.Add(hZJA)
      hOther.Add(hWJA)
      #FIXME update to breakVV for consistency
      hOther.Add(hWWA)
      hOther.Add(hWZA)
      hOther.Add(hZZA)

      fOut.cd()
      hQCDA_clone = hQCDA.Clone(qcdName)
      hQCDA_clone.Write()
      hDatA_clone = hDatA.Clone(dataName)
      hDatA_clone.Write()
      hOther.Write()

    if 'HMass' in plN:
        nums[r]['All'] = {}
        nums[r]['All']['JetHT'] = getHistIntegral(hDatA,lowM,highM)
        nums[r]['All']['ZH_HToCC_ZToQQ'] = getHistIntegral(hZHccA,lowM,highM)
        nums[r]['All']['ZH_HToBB_ZToQQ'] = getHistIntegral(hZHbbA,lowM,highM)
        nums[r]['All']['ggZH_HToCC_ZToQQ'] = getHistIntegral(hggZHccA,lowM,highM)
        nums[r]['All']['ggZH_HToBB_ZToQQ'] = getHistIntegral(hggZHbbA,lowM,highM)
        nums[r]['All']['WH_HToCC_WToQQ'] = getHistIntegral(hWHccA,lowM,highM)
        nums[r]['All']['WH_HToBB_WToQQ'] = getHistIntegral(hWHbbA,lowM,highM)
        nums[r]['All']['QCD'] = getHistIntegral(hQCDA,lowM,highM)
        nums[r]['All']['WJ'] = getHistIntegral(hWJA,lowM,highM)
        nums[r]['All']['ZJ'] = getHistIntegral(hZJA,lowM,highM)
        nums[r]['All']['TT'] = getHistIntegral(hTTA,lowM,highM)
        nums[r]['All']['ST'] = getHistIntegral(hSTA,lowM,highM)
        nums[r]['All']['WW'] = getHistIntegral(hWWA,lowM,highM)
        nums[r]['All']['WZ'] = getHistIntegral(hWZA,lowM,highM)
        nums[r]['All']['ZZ'] = getHistIntegral(hZZA,lowM,highM)
        if breakVV and 'HMass' in plN:
            nums[r]['All']['VVcc'] = getHistIntegral(hVVccA,lowM,highM)
            nums[r]['All']['VVbb'] = getHistIntegral(hVVbbA,lowM,highM)
            nums[r]['All']['VVother'] = getHistIntegral(hVVotherA,lowM,highM)
    
    hData_clone = hDatA.Clone(hDatA.GetName()+"clone").Rebin(nRebin)
    #if doBlindData and "HMass" in plN and ('select4' in r or 'select3' in r or ('PN_med' in r and 'CR' not in r and 'VR' not in r)):
    if "HMass" in plN and ((blindDataOption == 1 and not ('topCR' in r or 'qcdCR' in r)) \
                            or blindDataOption == 2):
        nBin1 = hData_clone.GetXaxis().FindFixBin(blindRange[0])
        nBin2 = hData_clone.GetXaxis().FindFixBin(blindRange[1])
        for i in range(nBin1,nBin2+1):
          hData_clone.SetBinContent(i,-1)
          hData_clone.SetBinError(i,0)

    if not breakVV:
      plots_process = [hData_clone,
        hQCDA.Clone("QCD_All_Clone").Rebin(nRebin),
        hSTA.Clone().Rebin(nRebin),
        hTTA.Clone().Rebin(nRebin),
        hZJA.Clone().Rebin(nRebin),
        hWJA.Clone().Rebin(nRebin),
        hWWA.Clone().Rebin(nRebin),
        hWZA.Clone().Rebin(nRebin),
        hZZA.Clone().Rebin(nRebin),
        hZHbbA.Clone().Rebin(nRebin),
        hggZHbbA.Clone().Rebin(nRebin),
        hWHbbA.Clone().Rebin(nRebin),
        hZHccA.Clone().Rebin(nRebin),
        hggZHccA.Clone().Rebin(nRebin),
        hWHccA.Clone().Rebin(nRebin)]
    else:
      plots_process = [hData_clone,
        hQCDA.Clone("QCD_All_Clone").Rebin(nRebin),
        hSTA.Clone().Rebin(nRebin),
        hTTA.Clone().Rebin(nRebin),
        hZJA.Clone().Rebin(nRebin),
        hWJA.Clone().Rebin(nRebin),
        hVVotherA.Clone().Rebin(nRebin),
        hVVbbA.Clone().Rebin(nRebin),
        hVVccA.Clone().Rebin(nRebin),
        hZHbbA.Clone().Rebin(nRebin),
        hggZHbbA.Clone().Rebin(nRebin),
        hWHbbA.Clone().Rebin(nRebin),
        hZHccA.Clone().Rebin(nRebin),
        hggZHccA.Clone().Rebin(nRebin),
        hWHccA.Clone().Rebin(nRebin)]

    
    if doCustomBinning_MH and 'HMass' in plN and 'VR' in r: 
      for iH in range(len(plots_process)):
        plots_process[iH] = utl_func.customBin(plots_process[iH],xDiv_MH)
        xA_range[0] = xDiv_MH[0]
        xA_range[1] = xDiv_MH[-1]
    
    if makePostfit_MH and 'HMass' in plN and 'VR' in r: 
      for iH in range(len(plots_process)):
        if 'QCD' in plots_process[iH].GetName():
          for iB in range(1,plots_process[iH].GetNbinsX()+1):
            plots_process[iH].SetBinContent(iB,binContent_MH[iB-1])
            plots_process[iH].SetBinError(iB,binError_MH[iB-1])
          break

    
    plotNames_process = []
    dataTitle = 'Data (JetHT)' 
    #plotNames_process = [dataTitle, 'ZHbb', 'ggZHbb', 'ZHcc', 'ggZHcc']
    #plotNames_process = [dataTitle, 'QCD', 'ZHbb', 'ggZHbb', 'ZHcc', 'ggZHcc']
    if not breakVV:
        plotNames_process = [dataTitle, 'QCD', 'Single top', 't#bar{t}','Z + jets', 'W + jets', 'WW', 'WZ', 'ZZ', 'ZH (H#rightarrow bb)', 'ggZH (H#rightarrow bb)', 'WH (H#rightarrow bb)', 'ZH (H#rightarrow cc)', 'ggZH (H#rightarrow cc)','WH (H#rightarrow cc)']
    else:
        plotNames_process = [dataTitle, 'QCD', 'Single top', 't#bar{t}','Z + jets', 'W + jets', 'VV (others)', 'VZ (Z#rightarrow bb)', 'VZ (Z#rightarrow cc)','ZH (H#rightarrow bb)', 'ggZH (H#rightarrow bb)', 'WH (H#rightarrow bb)', 'ZH (H#rightarrow cc)', 'ggZH (H#rightarrow cc)', 'WH (H#rightarrow cc)']

    logY=False
    #if 'CutFlow' in plN or 'ccTagDis' in plN or 'bbTagDis' in plN or 'bbPN' in plN or 'ccPN' in plN: logY=True
    if 'CutFlow' in plN or 'ccTagDis' in plN or 'bbTagDis' in plN: logY=True
    outPlotName = plN + '_' + r
    if doCustomBinning_MH and 'HMass' in plN and 'VR' in r: 
      outPlotName = plN + '_' + r +'_customBin'
    if makePostfit_MH and 'HMass' in plN and 'VR' in r: 
      outPlotName = outPlotName + '_postFit'
    outPlotName = outPlotName + '_all'

    normBinWidth = -1
    if makePostfit_MH and plN=="HMass": normBinWidth = 1
    utl_func.makeStackPlot(plots_process, plotNames_process, outPlotName, plotFolders['All'], xA_title, xA_range, 'MC unc. (stat.)', False, logY=logY, normBinWidth = normBinWidth, lumi='138',minY_forLog = 0.3)

    #save histograms to check
    fCheck.cd()
    hQCDA.Write()

#adding process together
addBkgr(nums,'WWWZ',['WW','WZ'],regions)
addBkgr(nums,'VZ',['WZ','ZZ'],regions)
addBkgr(nums,'ZHBB',['ggZH_HToBB_ZToQQ','ZH_HToBB_ZToQQ'],regions)
addBkgr(nums,'ZHCC',['ggZH_HToCC_ZToQQ','ZH_HToCC_ZToQQ'],regions)
addBkgr(nums,'Bkgr',['QCD','WJ','ZJ','TT','ST','WW','WZ','ZZ','ggZH_HToBB_ZToQQ','ZH_HToBB_ZToQQ','WH_HToBB_WToQQ'],regions)

for r in regions:
  for y in ['16_preVFP','16','17','18','All']:
    nums[r][y]['S/sqrt(B)'] = [(nums[r][y]['ZHCC'][0]+nums[r][y]['WH_HToCC_WToQQ'][0])/math.sqrt(nums[r][y]['Bkgr'][0]),0]

print(nums)

#print number of events
fLatex = open(plotFolder+'/'+summary_eventCount_name,'w')
fLatex.write('\documentclass[12pt]{article}\n')
fLatex.write('\\usepackage{graphicx}\n')
fLatex.write('\\title{Control plots}\n')
fLatex.write('\\begin{document}\n')

if not breakVV:
  labels = ['QCD','WJ','ZJ','TT','ST','WW','VZ','ZHBB','WH_HToBB_WToQQ','Bkgr','ggZH_HToCC_ZToQQ','ZH_HToCC_ZToQQ','WH_HToCC_WToQQ','S/sqrt(B)','JetHT']
  label_translate = {'QCD':'QCD','WJ':'W+jets','ZJ':'Z+jets','TT':'t$\\bar{t}$','ST':'Single top','WW':'WW','VZ':'VZ','ZHBB':'ZH(H$\\rightarrow$bb)','WH_HToBB_WToQQ':'WH(H$\\rightarrow$bb)','Bkgr':'Total background','ggZH_HToCC_ZToQQ':'ggZH(H$\\rightarrow$cc)','ZH_HToCC_ZToQQ':'ZH(H$\\rightarrow$cc)','WH_HToCC_WToQQ':'WH(H$\\rightarrow$cc)','S/sqrt(B)':'S/$\sqrt{B}$','JetHT':'Data'}
else:
  labels = ['QCD','WJ','ZJ','TT','ST','VVcc','VVbb','VVother','WH_HToBB_WToQQ','Bkgr','ggZH_HToCC_ZToQQ','ZH_HToCC_ZToQQ','WH_HToCC_WToQQ','S/sqrt(B)','JetHT']
  label_translate = {'QCD':'QCD','WJ':'W+jets','ZJ':'Z+jets','TT':'t$\\bar{t}$','ST':'Single top','VVcc':'VZ (Z$\\rightarrow$cc)','VVbb':'VZ (Z$\\rightarrow$bb)','VVother':'VV (other)','ZHBB':'ZH(H$\\rightarrow$bb)','WH_HToBB_WToQQ':'WH(H$\\rightarrow$bb)','Bkgr':'Total background','ggZH_HToCC_ZToQQ':'ggZH(H$\\rightarrow$cc)','ZH_HToCC_ZToQQ':'ZH(H$\\rightarrow$cc)','WH_HToCC_WToQQ':'WH(H$\\rightarrow$cc)','S/sqrt(B)':'S/$\sqrt{B}$','JetHT':'Data'}


for r in regions:
  #print nums
  fLatex.write('\n')
  fLatex.write('\\begin{table}[h]\n')
  fLatex.write('  \\caption{'+r.replace('_','\_')+'}\n')
  fLatex.write('  \\label{tab:}\n')
  fLatex.write('  \\centering\n')
  fLatex.write('  \\begin{tabular}{lccccc}\n')
  fLatex.write('  \\hline\n')
  fLatex.write('  \\hline\n')
  l = ['', '2016\_preVFP', '2016','2017','2018','Run 2']
  st = utl_func.makeLatexLine(l)
  fLatex.write(st)
  fLatex.write('  \\hline\n')

  for label in labels:
    if label != 'S/sqrt(B)': l = [label_translate[label], "{0:.2f}".format(nums[r]['16_preVFP'][label][0]), "{0:.2f}".format(nums[r]['16'][label][0]), "{0:.2f}".format(nums[r]['17'][label][0]),"{0:.2f}".format(nums[r]['18'][label][0]), "{0:.2f}".format(nums[r]['All'][label][0])]
    else: l = [label_translate[label], "{0:.3f}".format(nums[r]['16_preVFP'][label][0]), "{0:.3f}".format(nums[r]['16'][label][0]), "{0:.3f}".format(nums[r]['17'][label][0]),"{0:.3f}".format(nums[r]['18'][label][0]), "{0:.3f}".format(nums[r]['All'][label][0])]
    st = utl_func.makeLatexLine(l)
    fLatex.write(st)

  fLatex.write('  \\hline\n')
  fLatex.write('  \\hline\n')
  fLatex.write('\\end{tabular}\n')
  fLatex.write('\\end{table}\n')

fLatex.write('\\end{document}\n')
