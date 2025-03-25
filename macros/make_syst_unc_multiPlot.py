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

from my_funcs import ScaleToLumi1, makeMultiPlot

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

dirpath = '../plot_results/systematics/2025Winter/'
output_dir = '../plot_results/systematics/2025Winter/multiplots/'

categories = [
  'WW_LO', 'WZ_LO', 'ZZ_LO',
  'WW_NLO', 'WZ_NLO', 'ZZ_NLO',
  'ZHcc', 'ZHbb', 'QCD',
  'WJ', 'ZJ', 'TT', 'ST',
  'VV_LO', 'WHcc', 'WHbb', 'VV_NLO'
]

categories = [
  'WJ', 'ZHcc',
  'ZJ', 'ZHbb',
  'TT', 'WHcc',
  'ST', 'WHbb',
  'VV_NLO', 'QCD'
]

category_names = {
  'WJ': "W+jets",
  'ZJ': "Z+jets",
  'TT': "tt",
  'ST': "t",
  'VV_NLO': "VV",
  'ZHcc': "ZHcc",
  'ZHbb': "ZHbb",
  'WHcc': "WHcc",
  'WHbb': "WHbb",
  'QCD': "QCD"
}
                       
colors = [ ROOT.kBlack, ROOT.kRed, ROOT.kBlue ]

########################################
# WARNING: DO NOT EDIT BELOW THIS POINT
########################################

systs = [ "JES", "PU", "ELEC", "MUON", "L1PREFIRING", "TAG_CC", "TRIG", "JER"]
syst_names = {
  "JES": "JES",
  "PU": "Pileup",
  "ELEC": "Electron Veto",
  "MUON": "Muon Veto",
  "L1PREFIRING": "L1-Prefiring",
  "TAG_CC": "Xcc jet tagging",
  "TRIG": "jet triggers",
  "JER": "JER"
}

config_file = '../Configs/config.ini'

if debug: print("All settings set.")

# == MAIN CODE ================================================================

## Load the config files         
if debug: print(">>> Loading config file...")
cfg = configparser.ConfigParser()
cfg.read(config_file)

## Get the appropriate output files.
syst_files = {}
for y in years:
  file_name = dirpath+"/up_dn_plots_20" + y + ".root"
  if debug: print(">>> Retrieving plot file for y = ", y)
  syst_files[y] = ROOT.TFile.Open(file_name, "READ")

## Now loop through each sample, year, systematic, and variable.
for r in regions:
  print("region r = ", r)

  variables = variables_per_region[r]
    
  for v in variables:
    print(">>>> variable v = ", v)

    # Histogram name
    hName = r + "_" + v
    if r == '':
      hName = v

    nRebin = int(cfg.get(v, "rebin"))
    tmps = cfg.get(v, "xAxisRange").split(",")
    xA_range = []
    if "Pi" not in tmps[1]:
      xA_range = [float(tmps[0]), float(tmps[1])]
    else:
      xA_range = [0, ROOT.TMath.Pi()]
    xA_title = cfg.get(v, "xAxisTitle")

    # Go through each systematic
    for s in systs:
      print(">>>>>> syst s = ", s)
      
      # Go through each year
      for y in years:
        print(">>>>>>>> year y = ", y)

        plots = []
        plot_names = []
        for cat in categories:

          print(">>>>>>>>> cat = ", cat)
          plot_name = cat + "_" + hName + "_" + s
          print(">>>>>>>>> plot = ", plot_name)

          plot_up = syst_files[y].Get(plot_name + "U").Clone()
          #if "jet" in v: plot_up.Rebin(nRebin)
          plot_up.GetYaxis().SetRangeUser(0.8,1.2)
          plot_up.GetXaxis().SetRangeUser(xA_range[0], xA_range[1])
          plots.append(plot_up)
          plot_dn = syst_files[y].Get(plot_name + "D").Clone()
          #if "jet" in v: plot_dn.Rebin(nRebin)
          plot_dn.GetYaxis().SetRangeUser(0.8,1.2)
          plot_dn.GetXaxis().SetRangeUser(xA_range[0], xA_range[1])
          plots.append(plot_dn)
          plot_names.append(category_names[cat])

        plot_title = "20" + y + " " + syst_names[s]
        if y == '16_preVFP':
          plot_title = "2016APV " + syst_names[s]
        makeMultiPlot(plots, plot_names,
                      hName + "_" + s + "_" + y,
                      output_dir,
                      xA_range, xA_title,
                      plot_title,
                      plots_per_sub = 2)
        
      #end for(y)
    #end for(s)
  #end for(v)
#end for(r)
# == | EOF | ==================================================================
