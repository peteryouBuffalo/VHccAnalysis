#################################################################
#
# calc_bckg_est_ratio.py - created (Apr 6, 2025)
#
# description: This macro takes the QCD MC and calculates the
#              transfer function (ratio) to be used to
#              estimate the QCD background in data.
#
#################################################################

import ROOT
import sys,os
import copy
import math
sys.path.append('/uscms_data/d3/duong/CMSSW/CMSSW_7_6_5/src/ZplusC/python3/myutils/')
import configparser
import util_funcs as utl_func
import myutils as utl
import json
import array

ROOT.gROOT.Macro(os.path.expanduser('~/rootLogOn_forPyROOT.C' ))
ROOT.gROOT.SetBatch(True)

#################################################################
# Useful Methods
#################################################################

def getHist(pN,samName,fH,lS): #samName = ['Electron'],['DY_0J','DY_1J','DY_2J']...
  hOut = {}
  for y in years:
    # first sample, first file
    hOut[y] = fH[samName[0]][y][0].Get(pN).Clone()
    if samName[0] not in ['JetHT','SingleMuon']:
      hOut[y].Scale(lS[samName[0]][y][0])

    # go through the rest of the samples and files
    # (skip the first one because it's already included)
    for iS in range(len(samName)):
      for fi in range(len(fH[samName[iS]][y])):
        if iS == 0 and fi == 0: continue 
        h = fH[samName[iS]][y][fi].Get(pN).Clone()
        if samName[iS] not in ['JetHT','SingleMuon']:
          h.Scale(lS[samName[iS]][y][fi])
        hOut[y].Add(h)
      
  return hOut


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


def make_arr(start, end, bin_width):
  bin_edges = []
  x = start
  while x <= end:
    bin_edges.append(x)
    x += bin_width
  if bin_edges[-1] < end + 1e-6:
    bin_edges.append(end)
  return bin_edges


def Rebinning(hist_in, new_name, xRange=[40,148],bin_size=4):
  print("new name = ", new_name)
  nBin = int((xRange[1]-xRange[0])/hist_in.GetBinWidth(1))
  hist_out = ROOT.TH1D(new_name, "", nBin, xRange[0], xRange[1])

  # Fill the bins in the range
  for i in range(1, hist_out.GetNbinsX()+1):
    iBin = hist_in.FindFixBin(hist_out.GetBinLowEdge(i))
    print(i, ": ", hist_in.GetBinContent(iBin))
    hist_out.SetBinContent(i, hist_in.GetBinContent(iBin))
    hist_out.SetBinError(i, hist_in.GetBinError(iBin))

  # Properly fill the overflow bin
  ofBin = hist_in.FindFixBin(xRange[1])
  nOf = 0
  nErrOf = 0
  for i in range(ofBin, hist_in.GetNbinsX() + 2):
    nOf += hist_in.GetBinContent(i)
    nErrOf += hist_in.GetBinError(i)*hist_in.GetBinError(i)
  nErrOf = math.sqrt(nErrOf)
  hist_out.SetBinContent(hist_out.GetNbinsX()+1, nOf)
  hist_out.SetBinError(hist_out.GetNbinsX()+1, nErrOf)
  
  # Properly fill the underflow bin
  ufBin = hist_in.FindFixBin(xRange[0]-hist_in.GetBinWidth(1))
  nUf = 0
  nErrUf = 0
  for i in range(0, ufBin+1):
    nUf += hist_in.GetBinContent(i)
    nErrUf += hist_in.GetBinError(i)*hist_in.GetBinError(i)
  nErrUf = math.sqrt(nErrUf)
  hist_out.SetBinContent(0, nUf)
  hist_out.SetBinError(0, nErrUf)

  # Rebin
  hist_out.Rebin(bin_size)
  
  # Return the properly rebinned histogram
  return hist_out

#################################################################
# Main
#################################################################

years = ['16_preVFP','16','17','18']
years = ['18']

regions = [
    'WTag_pass2prong',
    'WTag_fail2prong'
]

region_name = {
  'WTag_pass2prong': 'pass',
  'WTag_fail2prong': 'fail'
}

input_folder = '../condor_results/2025Apr_2prong_updated/NONE/'
output_folder = 'templates'

ss = [
  'SingleMuon',
  'ZH_HToCC_ZToQQ','ggZH_HToCC_ZToQQ','ZH_HToBB_ZToQQ','ggZH_HToBB_ZToQQ',
  'WH_HToCC_WToQQ','WH_HToBB_WToQQ',
  'QCD_HT200to300_v9','QCD_HT300to500_v9','QCD_HT500to700_v9','QCD_HT700to1000_v9',
  'QCD_HT1000to1500_v9','QCD_HT1500to2000_v9','QCD_HT2000toInf_v9',
  'WJetsToQQ_HT-400to600','WJetsToQQ_HT-600to800','WJetsToQQ_HT-800toInf',
  'WJetsToLNu_HT-400to600','WJetsToLNu_HT-600to800','WJetsToLNu_HT-800to1200',
  'WJetsToLNu_HT-1200to2500','WJetsToLNu_HT-2500toInf',
  'ZJetsToQQ_HT-400to600','ZJetsToQQ_HT-600to800','ZJetsToQQ_HT-800toInf',
  'TTToHadronic','TTToSemiLeptonic','TTTo2L2Nu',
  'ST_tW-channel_top','ST_tW-channel_antitop','ST_t-channel_top','ST_t-channel_antitop',
  'WW','WZ','ZZ',
  'WWTo1L1Nu2Q','WWTo4Q','WZTo4Q','WZToLNu2B',
  'WZTo1L1Nu2Q','WZTo2Q2L','ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q'
]

# We don't need specifics on the non-ttbar backgrounds,
# so it's just easier to lump them all into the category of "non-TT bckg"
categories = [ "Data", "nonTT_bckg", "TT" ]

samples_per_category = {
  "Data": ['SingleMuon'],
  "nonTT_bckg": [
    'ZH_HToCC_ZToQQ','ggZH_HToCC_ZToQQ','ZH_HToBB_ZToQQ','ggZH_HToBB_ZToQQ',
    'WH_HToCC_WToQQ','WH_HToBB_WToQQ',
    'QCD_HT200to300_v9','QCD_HT300to500_v9','QCD_HT500to700_v9','QCD_HT700to1000_v9',
    'QCD_HT1000to1500_v9','QCD_HT1500to2000_v9','QCD_HT2000toInf_v9',
    'WJetsToQQ_HT-400to600','WJetsToQQ_HT-600to800','WJetsToQQ_HT-800toInf',
    'WJetsToLNu_HT-400to600','WJetsToLNu_HT-600to800','WJetsToLNu_HT-800to1200',
    'WJetsToLNu_HT-1200to2500','WJetsToLNu_HT-2500toInf',
    'ZJetsToQQ_HT-400to600','ZJetsToQQ_HT-600to800','ZJetsToQQ_HT-800toInf',
    'ST_tW-channel_top','ST_tW-channel_antitop','ST_t-channel_top','ST_t-channel_antitop',
    'WWTo1L1Nu2Q','WWTo4Q','WZTo4Q','WZToLNu2B',
    'WZTo1L1Nu2Q','WZTo2Q2L','ZZTo2Q2L','ZZTo2Nu2Q','ZZTo4Q'
  ],
  "TT": [ 'TTToHadronic','TTToSemiLeptonic','TTTo2L2Nu']
}


###############################
# DO NOT CHANGE ANYTHING BELOW
###############################
cfg = utl.BetterConfigParser()
cfg.read('../Configs/config.ini')

lumiS = {}
for y in years:
  lumiTmp = float(cfg.get('General','lumi_'+y))/1000.
  lumiTmp = format("%.1f" % lumiTmp)
  lumiS[y] = str(lumiTmp)
print(lumiS)



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
    names = cfg.get(s,'file_'+y).split(',') # multiple names is possible,
                                            # for example: Single top = t-channels, s-channels ...
    print('>>>>>>>: ', len(names))
    xSecTmps = ['1']*len(names) #each name corresponds to a cross section
    kfactor = ['1']*len(names) #each name corresponds to a cross section
    if s not in ['JetHT', 'SingleMuon']:
      xSecTmps = cfg.get(s,'xSec_'+y).split(',')

    fNames[s][y] = []
    xSecs[s][y] = []
    fHist[s][y] = []
    for iN in names:
      if 'JetHT' in iN or 'SingleMuon' in iN: fNames[s][y].append(input_folder + '/' + iN)
      else: fNames[s][y].append(input_folder + '/' + iN)
      fHist[s][y].append(ROOT.TFile.Open(fNames[s][y][-1],'READ'))
                                  
    print(xSecTmps)
    for iS in xSecTmps:
      filterEff = 1.
      #if 'JetHT' not in s and correctFilterEff: filterEff = filter_effs[s]
      #in case there is kfactor in cross section
      if '*' in iS:
        iS = iS.split('*')
      if len(iS) == 2:
        xSecs[s][y].append(float(iS[0])*float(iS[1])*filterEff)
      else:
        xSecs[s][y].append(float(iS)*filterEff)
      
    lumiScales[s][y] = [1]*len(names)
    for iN in range(len(fNames[s][y])):
      if s not in ['JetHT', 'SingleMuon']:
        print(s, y, iN, fNames[s][y][iN])
        try:
          lumiScales[s][y][iN] = utl_func.scaleToLumi1(fNames[s][y][iN], \
            xSecs[s][y][iN],lumi,'Nevt_all_VbbHcc_boosted')
        except AttributeError:
          print("\n Can not find lumiScale for ", s, " ", y, " ", iN, " .Set lumiScale to 0")
          lumiScales[s][y][iN] = 0

          
# Prepare the files for the output
output_files = {}
for y in years:
  output_files[y] = ROOT.TFile.Open(output_folder + "/wtemplates_" + y + ".root", "RECREATE")
  
# Go through each region
for r in regions:

  print("r = ", r)
  
  # Get the appropriate plots. We need the following:
  # 1) Data - *_total_mass
  # 2) TT MC - *_matched_mass, *_unmatched_mass,
  # 3) non-TT MC - *_total_mass

  # For the data, we want to have just what SHOULD be ttbar.
  # We subtract off the backgrounds via MC and we can just
  # use the total_mass to make that easier.
  hN = r + "_total_mass"
  hData_total = getHist(hN,samples_per_category["Data"],fHist,lumiScales)
  hBckg_total = getHist(hN,samples_per_category["nonTT_bckg"],fHist,lumiScales)

  hN = r + "_unmatched_mass"
  hTT_unmatched = getHist(hN,samples_per_category["TT"],fHist,lumiScales)
  hN = r + "_matched_mass"
  hTT_matched   = getHist(hN,samples_per_category["TT"],fHist,lumiScales)
  
  # Go through each year
  for y in years:

    print(">> y = ", y)

    # Produce the proper data sample
    hData_in = hData_total[y]
    hBckg_in = hBckg_total[y]
    hData_in.Add(hBckg_in,-1)

    # Get the proper TT MC
    hTT_unmatched_in = hTT_unmatched[y]
    hTT_matched_in   = hTT_matched[y]

    # Rebin these plots to match the range and binning
    # that we want.
    hData_out         = Rebinning(hData_in, "data_obs_" + region_name[r] + "_nominal")
    hTT_unmatched_out = Rebinning(hTT_unmatched_in, "catp1_" + region_name[r] + "_nominal")
    hTT_matched_out   = Rebinning(hTT_matched_in, "catp2_" + region_name[r] + "_nominal")
    
    # Add these plots to the appropriate
    output_files[y].cd()
    hData_out.Write()
    hTT_unmatched_out.Write()
    hTT_matched_out.Write()

# Close all the files
for y in years:
  output_files[y].Close()
    
print("===== EOF =====")
