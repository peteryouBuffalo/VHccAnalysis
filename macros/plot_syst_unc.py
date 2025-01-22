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

years = ['16' , '17', '18']
regions = [ 'VHcc_boosted_PN_med', 'ZccHcc_boosted_PN_med', '' ]

variables_per_region = { #[ 'HMass', 'ZMass' ]
  'VHcc_boosted_PN_med': [ 'HMass', 'ZMass' ],
  'ZccHcc_boosted_PN_med': [ 'HMass', 'ZMass' ],
  '' : ['jet_pt', 'jet_mass']
}
#varLogY = [ False, False ]

dirpath = '../condor_results/2025Jan/'
output_dir = '../plot_results/systematics/2025Jan/'

colors = [ ROOT.kBlack, ROOT.kRed, ROOT.kBlue ]

sampleList = [
  'ZH_HToCC_ZToQQ', 'ggZH_HToCC_ZToQQ', ## Jet HT & ZH(H->CC)
  #'ZH_HToBB_ZToQQ', 'ggZH_HToBB_ZToQQ', ## ZH(H->BB)    
  #'QCD_HT200to300_v9', 'QCD_HT300to500_v9', ## QCD (200-Inf)   
  #'QCD_HT500to700_v9', 'QCD_HT700to1000_v9', 'QCD_HT1000to1500_v9',
  #'QCD_HT1500to2000_v9', 'QCD_HT2000toInf_v9',
  #'WJetsToQQ_HT-200to400',
  #'WJetsToQQ_HT-400to600', 'WJetsToQQ_HT-600to800',   ## WJ (400-Inf) 
  #'WJetsToQQ_HT-800toInf', 'WJetsToLNu_HT-400to600',
  #'WJetsToLNu_HT-100to200', 'WJetsToLNu_HT-200to400',
  #'WJetsToLNu_HT-600to800','WJetsToLNu_HT-800to1200',
  #'WJetsToLNu_HT-1200to2500','WJetsToLNu_HT-2500toInf',
  #'ZJetsToQQ_HT-200to400',
  #'ZJetsToQQ_HT-400to600', 'ZJetsToQQ_HT-600to800',   ## ZJ (400-Inf)
  #'ZJetsToQQ_HT-800toInf',
  #'TTToHadronic','TTToSemiLeptonic','TTTo2L2Nu'#,      ## Top (ttbar) 
  #'ST_t-channel_antitop','ST_t-channel_top',          ## Top (Single Top) 
  #'ST_tW-channel_antitop','ST_tW-channel_top',
  #'WW','WZ','ZZ'                                      ## VV    
]

categories = [ "QCD", "TT", "ZH" ]
categories = [ "ZH" ]

category_samples = {
  "ZH": [ 'ZH_HToCC_ZToQQ', 'ggZH_HToCC_ZToQQ' ],
  "QCD": ['QCD_HT200to300_v9', 'QCD_HT300to500_v9',
  'QCD_HT500to700_v9', 'QCD_HT700to1000_v9', 'QCD_HT1000to1500_v9',
  'QCD_HT1500to2000_v9', 'QCD_HT2000toInf_v9'],
  "TT": ['TTToHadronic', 'TTToSemiLeptonic', 'TTTo2L2Nu']  
}

systs_separate = [ "NONE", "JESU", "JESD", "PUU", "PUD" ]
systs = [ "JES", "PU" ]

config_file = '../Configs/config.ini'

if debug: print("All settings set.")

# == MAIN CODE ================================================================

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
          if '*' in iS: iS.split('*')
          if len(iS) == 2:
            xSecs[s][sys][y].append(float(iS[0])*float(iS[1]))
          else:
            xSecs[s][sys][y].append(float(iS))
      
        lumiScales[s][sys][y] = [1]*len(names)
        for iN in range(len(fileNames[s][sys][y])):
          if s not in ['JetHT']:
            if debug: print(">>> |", s, y, iN, fileNames[s][sys][y][iN])
            lumiScales[s][sys][y][iN] = ScaleToLumi1(fileNames[s][sys][y][iN],
                      xSecs[s][sys][y][iN], lumi, 'Nevt_VbbHcc_boosted')

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
