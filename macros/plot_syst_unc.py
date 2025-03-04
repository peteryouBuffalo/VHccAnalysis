






# =============================================================================
# plot_syst_unc.py - This macro is designed to produce plots showing the
#                    uncertainties of certain systematics.
# =============================================================================

# == HEADERS ==================================================================

import ROOT
import os,sys
import copy
import configparser
from math import *

from my_funcs import ScaleToLumi1, makeUpDownSystPlot

# == USEFUL METHODS ===========================================================

def getHist(plot_name, sample_names, hist_files, lumiScales, syst, debug = True):

  hOut = {}

  ## Go through each year we're interested in
  for y in years:

    ## Get the first sample and the appropriate histogram
    if debug:
      print("Looking in ", sample_names[0], " for ", plot_name, "(20", y,")")

    hOut[y] = hist_files[sample_names[0]][syst][y][0].Get(plot_name).Clone()
    if sample_names[0] not in ["JetHT", "Data", "BTagCSV"]:
      hOut[y].Scale(lumiScales[sample_names[0]][syst][y][0])

    for iS in range(len(sample_names)):
      for fi in range(len(hist_files[sample_names[iS]][syst][y])):

        ## Skip the first sample (already grabbed)
        if iS == 0 and fi == 0: continue

        h = hist_files[sample_names[iS]][syst][y][fi].Get(plot_name).Clone()
        if sample_names[iS] not in ["Jet", "Data", "BTagCSV"]:
          scale = lumiScales[sample_names[iS]][syst][y][fi]
          if debug:
            print("Scaling ", sample_names[iS], ", SF = ", scale)
          h.Scale(scale)
        hOut[y].Add(h)
      #end for(fi)
    #end for(iS)

  return hOut

# == SETTINGS =================================================================

# This helps suppress visual output when producing the plots to file
ROOT.gROOT.SetBatch(True)

# Feel free to modify the following settings (if you dare...)    
debug = True

years = ['16', '16_preVFP', '17', '18']
regions = [ 'VHcc_boosted_PN_med', 'ZccHcc_boosted_PN_med', '' ]

variables_per_region = {
  'VHcc_boosted_PN_med': [ 'HMass', 'ZMass' ],
  'ZccHcc_boosted_PN_med': [ 'HMass', 'ZMass' ],
  '' : ['jet_pt', 'jet_mass']
}
#varLogY = [ False, False ]

dirpath = '../condor_results/2025Winter/'
output_dir = '../plot_results/systematics/2025Winter/'
batch_to_plot = 4      # 0 = ZH, QCD
                       # 1 = W+Jets
                       # 2 = Z+Jets, TT, ST, VV_LO
                       # 3 = WminusH, WplusH, VV_NLO
                       # 4 = custom

## If you wish to use a custom list that's not pre-defined above
## add them here
custom_sampleList = [
  'WW','WZ','ZZ',
  'WWTo1L1Nu2Q_NLO', 'WWTo4Q_NLO', 
  'WZTo1L1Nu2Q_NLO', 'WZTo2Q2L_NLO',
  'WZTo4Q_NLO', 'WZToLNu2B_NLO',
  'ZZTo2Nu2Q_NLO', 'ZZTo2Q2L_NLO'
]

custom_categories = [
  'WW_LO', 'WZ_LO', 'ZZ_LO',
  'WW_NLO', 'WZ_NLO', 'ZZ_NLO'
]
                       
colors = [ ROOT.kBlack, ROOT.kRed, ROOT.kBlue ]

########################################
# WARNING: DO NOT EDIT BELOW THIS POINT
########################################

batch0_sampleList = [
  'ZH_HToCC_ZToQQ', 'ggZH_HToCC_ZToQQ', ## Jet HT & ZH(H->CC)
  'ZH_HToBB_ZToQQ', 'ggZH_HToBB_ZToQQ', ## ZH(H->BB)    
  'QCD_HT200to300_v9', 'QCD_HT300to500_v9', ## QCD (200-Inf)   
  'QCD_HT500to700_v9', 'QCD_HT700to1000_v9', 'QCD_HT1000to1500_v9',
  'QCD_HT1500to2000_v9', 'QCD_HT2000toInf_v9'
]

batch1_sampleList = [
  'WJetsToQQ_HT-400to600', 'WJetsToQQ_HT-600to800',   ## WJ (400-Inf) 
  'WJetsToQQ_HT-800toInf', 'WJetsToLNu_HT-400to600',
  'WJetsToLNu_HT-600to800','WJetsToLNu_HT-800to1200',
  'WJetsToLNu_HT-1200to2500','WJetsToLNu_HT-2500toInf'
]

batch2_sampleList = [
  'ZJetsToQQ_HT-400to600', 'ZJetsToQQ_HT-600to800',   ## ZJ (400-Inf)
  'ZJetsToQQ_HT-800toInf',
  'TTToHadronic','TTToSemiLeptonic','TTTo2L2Nu',      ## Top (ttbar) 
  'ST_t-channel_antitop','ST_t-channel_top',          ## Top (Single Top) 
  'ST_tW-channel_antitop','ST_tW-channel_top',
  'WW','WZ','ZZ'                                      ## VV (LO)
]

batch3_sampleList = [
  'WH_HToBB_WToQQ', 'WH_HToCC_WToQQ',                 ## WH 
  'WWTo1L1Nu2Q_NLO', 'WWTo4Q_NLO',                    ## VV (NLO)
  'WZTo1L1Nu2Q_NLO', 'WZTo2Q2L_NLO',
  'WZTo4Q_NLO', 'WZToLNu2B_NLO',
  'ZZTo2Nu2Q_NLO', 'ZZTo2Q2L_NLO'
]

batch0_categories = [ "ZHcc", "ZHbb", "QCD"]
batch1_categories = [ "WJ" ]
batch2_categories = [ "ZJ", "TT", "ST", "VV_LO"]
batch3_categories = [ "WHcc", "WHbb", "VV_NLO" ]

category_samples = {
  "ZHcc": [ 'ZH_HToCC_ZToQQ', 'ggZH_HToCC_ZToQQ' ],
  "ZHbb": [ 'ZH_HToBB_ZToQQ', 'ggZH_HToBB_ZToQQ' ],
  "QCD": ['QCD_HT200to300_v9', 'QCD_HT300to500_v9',
          'QCD_HT500to700_v9', 'QCD_HT700to1000_v9', 'QCD_HT1000to1500_v9',
          'QCD_HT1500to2000_v9', 'QCD_HT2000toInf_v9'],
  "TT": ['TTToHadronic', 'TTToSemiLeptonic', 'TTTo2L2Nu'],
  "ST": ['ST_t-channel_antitop','ST_t-channel_top',
         'ST_tW-channel_antitop','ST_tW-channel_top'],
  "WJ": ['WJetsToQQ_HT-400to600', 'WJetsToQQ_HT-600to800',
         'WJetsToQQ_HT-800toInf', 'WJetsToLNu_HT-400to600',
         'WJetsToLNu_HT-600to800','WJetsToLNu_HT-800to1200',
         'WJetsToLNu_HT-1200to2500','WJetsToLNu_HT-2500toInf'],
  "ZJ": ['ZJetsToQQ_HT-400to600',
         'ZJetsToQQ_HT-600to800', 'ZJetsToQQ_HT-800toInf'], 
  "VV_LO": ['WW','WZ','ZZ'],
  "WW_LO": ['WW'], "WZ_LO": ['WZ'], "ZZ_LO": ['ZZ'],
  "WHcc": ['WH_HToCC_WToQQ'],
  "WHbb": ['WH_HToBB_WToQQ'],
  "VV_NLO": ['WWTo1L1Nu2Q_NLO', 'WWTo4Q_NLO',
             'WZTo1L1Nu2Q_NLO', 'WZTo2Q2L_NLO',
             'WZTo4Q_NLO', 'WZToLNu2B_NLO',
             'ZZTo2Nu2Q_NLO', 'ZZTo2Q2L_NLO'],
  "WW_NLO": ['WWTo1L1Nu2Q_NLO', 'WWTo4Q_NLO'],
  "WZ_NLO": ['WZTo1L1Nu2Q_NLO', 'WZTo2Q2L_NLO',
             'WZTo4Q_NLO', 'WZToLNu2B_NLO'],
  "ZZ_NLO": ['ZZTo2Nu2Q_NLO', 'ZZTo2Q2L_NLO']
}

systs_separate = [ "NONE", "JESU", "JESD", "PUU", "PUD", "ELECU", "ELECD",
                   "MUONU", "MUOND", "L1PREFIRINGU", "L1PREFIRINGD",
                   "TAG_CCU", "TAG_CCD", "TRIGU", "TRIGD", "JERU", "JERD"]
systs = [ "JES", "PU", "ELEC", "MUON", "L1PREFIRING", "TAG_CC", "TRIG", "JER"]

#systs_separate = [ "NONE", "JERU", "JERD" ]
#systs = [ "JER" ]

config_file = '../Configs/config.ini'

if debug: print("All settings set.")

# == MAIN CODE ================================================================

sampleList = batch0_sampleList
if (batch_to_plot == 1): sampleList = batch1_sampleList
elif (batch_to_plot == 2): sampleList = batch2_sampleList
elif (batch_to_plot == 3): sampleList = batch3_sampleList
elif (batch_to_plot == 4): sampleList = custom_sampleList

categories = batch0_categories
if (batch_to_plot == 1): categories = batch1_categories
elif (batch_to_plot == 2): categories = batch2_categories
elif (batch_to_plot == 3): categories = batch3_categories
elif (batch_to_plot == 4): categories = custom_categories

#################################
## Do not edit below this point
## (unless absolutely necessary)
#################################

if debug: print("Retrieving files and luminosities...")

## Load the config files         
if debug: print(">>> Loading config file...")
cfg = configparser.ConfigParser()
cfg.read(config_file)

## Get the proper lumi scales                        
if debug: print(">>> Loading lumi scales...")
lumiS = {}
for y in years:
  lumiTmp = float(cfg.get('General','lumi_'+y))/1000.0
  lumiTmp = float("%.1f" % lumiTmp)
  lumiS[y] = str(lumiTmp)
if debug: print(">>> lumi scales = ", lumiS)

## Retrieve the proper files and information related to them.          
if debug: print(">>> Retrieving information on samples...")
fileNames = {}
xSecs = {}
lumiScales = {}
histFiles = {}

for s in sampleList:

  fileNames[s] = {}
  xSecs[s] = {}
  lumiScales[s] = {}
  histFiles[s] = {}

  for sys in systs_separate:

    fileNames[s][sys] = {}
    xSecs[s][sys] = {}
    lumiScales[s][sys] = {}
    histFiles[s][sys] = {}
    
    for y in years:
    
        if debug: print("\nLooking for year 20" + y)
        ## Get the proper luminosity scale and the files                    
        ## for the given year (should be only ~1/year)   
        lumi = float(cfg.get('General', 'lumi_'+y))
        names = cfg.get(s, 'file_'+y).split(',')
        if debug: print(">>> | ", len(names), " files...") 
    
        ## Get other values of interest                                                                                                
        xSecTmps = ['1']*len(names)
        kfactor = ['1']*len(names)
        if s not in ['JetHT', 'Data']:
          xSecTmps = cfg.get(s, 'xSec_'+y).split(',')
    
        ## Get the proper information for this year                                                                                
        fileNames[s][sys][y] = []
        xSecs[s][sys][y] = []
        histFiles[s][sys][y] = []
     
        for iN in names:
          fileNames[s][sys][y].append(dirpath + '/' + sys + '/' + iN)
          histFiles[s][sys][y].append(ROOT.TFile.Open(fileNames[s][sys][y][-1],'READ'))

        if debug: print(">>> | xSec = ", xSecTmps)
        for iS in xSecTmps:
          if '*' in iS: iS = iS.split('*')
          if len(iS) == 2:
            xSecs[s][sys][y].append(float(iS[0])*float(iS[1]))
          else:
            xSecs[s][sys][y].append(float(iS))
      
        lumiScales[s][sys][y] = [1]*len(names)
        for iN in range(len(fileNames[s][sys][y])):
          if s not in ['JetHT']:
            if debug: print(">>> |", s, y, iN, fileNames[s][sys][y][iN])
            lumiScales[s][sys][y][iN] = ScaleToLumi1(fileNames[s][sys][y][iN],
                      xSecs[s][sys][y][iN], lumi, 'Nevt_all_VbbHcc_boosted')

print("\n>>> All files retrieved...")

## Now loop through each sample, year, systematic, and variable.
for r in regions:
  print("region r = ", r)

  for cat in categories:
    print(">> category cat = ", cat)

    variables = variables_per_region[r]
    
    for v in variables:
      print(">>>> variable v = ", v)

      # Histogram name
      hName = r + "_" + v
      if r == '':
        hName = v
      
      for s in systs:
        print(">>>>>> syst s = ", s)

        # Get the appropriate histograms for this var & syst
        plots_nom_by_year = getHist(hName, category_samples[cat],
                                    histFiles, lumiScales, "NONE", False)
        plots_up_by_year = getHist(hName, category_samples[cat],
                                   histFiles, lumiScales, s + "U", False)
        plots_dn_by_year = getHist(hName, category_samples[cat],
                                   histFiles, lumiScales, s + "D", False)

        nRebin = int(cfg.get(v, "rebin"))
        tmps = cfg.get(v, "xAxisRange").split(",")
        xA_range = []
        if "Pi" not in tmps[1]:
          xA_range = [float(tmps[0]), float(tmps[1])]
        else:
          xA_range = [0, ROOT.TMath.Pi()]
        xA_title = cfg.get(v, "xAxisTitle")
        
        for y in years:
          print(">>>>>>>> year y = ", y)
          
          plot_nom = plots_nom_by_year[y].Clone().Rebin(nRebin)
          plot_up  = plots_up_by_year[y].Clone().Rebin(nRebin)
          plot_dn  = plots_dn_by_year[y].Clone().Rebin(nRebin)

          plots_process = [
            plot_nom,
            plot_up,
            plot_dn
          ]

          plot_names = [
            "Nominal",
            s + " Up",
            s + " Down"
          ]

          canvas_name = v + "_" + s
          outpath = output_dir + "/20" + y + "/" + r + "/" + cat 
          colors = [ ROOT.kBlack, ROOT.kBlue, ROOT.kRed ]
          lumi = lumiS[y]
          logY = False

          if 'pt' in v:
            logY = True
          
          makeUpDownSystPlot(plots_process, plot_names,
            canvas_name, outpath, xA_title,
            xA_range, logY, lumi, custom_colors=colors)
          
        #end for(y)
      #end for(s)
    #end for(v)
  #end for(samp)
#end for(r)
# == | EOF | ==================================================================
