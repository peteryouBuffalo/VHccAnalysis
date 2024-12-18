# Useful Import Statements

import ROOT
import sys,os
import math
from math import *
from array import array

## Useful Variables
colors = [416+1, 632+1, 600+1,
          400+1, 840+1, 860+1,
          880+1, 900+1, 432+1,
          616+1, 616+3, 800+7]
fill_colors = [46, 16, ROOT.kAzure + 1] #38
years = ["16", "17", "18"]

#############################################################################
# myText
#############################################################################
def myText(txt="CMS Preliminary", ndcX=0, ndcY=0, size=0.8):
  ROOT.gPad.Update()
  text = ROOT.TLatex()
  text.SetNDC()
  text.SetTextColor(ROOT.kBlack)
  text.SetTextSize(text.GetTextSize()*size)
  text.SetTextFont(42)
  text.DrawLatex(ndcX, ndcY, txt)
  return text

#############################################################################
# ScaleToLumi
#############################################################################
def ScaleToLumi1(fName, xSec, lumi, hist_name = 'Nevt'):
  f = ROOT.TFile.Open(fName, 'read')
  hTmp = f.Get(hist_name)
  nP = hTmp.GetBinContent(3)
  nN = hTmp.GetBinContent(1)
  if abs(nP-nN) < 0.001:
    return 1.0
  return lumi*xSec/(nP-nN)

#############################################################################
# GetHist
#############################################################################
def getHist(plotName, sample_names, hist_files, lumiScales, scale = True, debug = True):
  
  hOut = {}
  
  ## Go through each year we're interested in.
  for y in years:
    
    ## Get the first sample and the appropriate histogram
    if debug: 
      print("Looking in ", sample_names[0], " for ", plotName, "(20",y,")")
    hOut[y] = hist_files[sample_names[0]][y][0].Get(plotName).Clone()
    if sample_names[0] not in ['JetHT', 'Data', 'BTagCSV'] and scale:
      hOut[y].Scale(lumiScales[sample_names[0]][y][0])
    
    ## Add the other samples
    for iS in range(len(sample_names)):
      for fi in range(len(hist_files[sample_names[iS]][y])):
        
        ## Skip the first sample (already grabbed)
        if iS == 0 and fi == 0: continue
        
        h = hist_files[sample_names[iS]][y][fi].Get(plotName).Clone()
        if sample_names[iS] not in ["JetHT", "Data", "BTagCSV"] and scale:
          h.Scale(lumiScales[sample_names[iS]][y][fi])
        hOut[y].Add(h)
  
  return hOut

#############################################################################
# Overlap Plot
#############################################################################
def makeOverlapPlot(plots, plotNames, canvasName, outputDir, xAxisTitle,
                    xAxisRange, logY, lumi, custom_colors=colors,
                    moveLegendLeft=False):

  ## Set some useful settings
  ROOT.gStyle.SetOptStat(0)

  ## Make the canvas
  c = ROOT.TCanvas(canvasName, canvasName, 600, 600)
  c.SetLeftMargin(0.15)
  c.cd()

  ## Prepare the legend
  x0 = 0.53; x1 = 0.89
  if moveLegendLeft:
    x0 = 0.17
    x1 = 0.42

  l = ROOT.TLegend(x0, 0.70, x1, 0.87)
  l.SetLineWidth(2)
  l.SetBorderSize(0)
  l.SetTextFont(42)
  l.SetTextSize(0.035)

  ## Overlap the plots
  allStack = ROOT.THStack('st','')

  for i in range(0,len(plots)):
    if (i == 0):
      plots[i].SetFillColor(ROOT.kGray)
    plots[i].SetLineColor(custom_colors[i])
    plots[i].SetLineWidth(2)
    l.AddEntry(plots[i], plotNames[i], 'F')
    allStack.Add(plots[i])

  ## Draw everything & modify the axes
  allStack.Draw("nostack HIST")

  binSize = plots[0].GetBinCenter(2) - plots[1].GetBinCenter(1)
  binSize = round(binSize,3)
  extraBit = ''
  if ('GeV' in xAxisTitle):
    extraBit = ' GeV'
  allStack.GetXaxis().SetTitle(xAxisTitle)
  allStack.GetYaxis().SetTitle("Events/" + str(binSize) + extraBit)
  allStack.GetXaxis().SetRangeUser(xAxisRange[0], xAxisRange[1])
  l.Draw()

  ## Update the canvas & modify the y-axis if appropriate
  myText('CMS Work in Progress #sqrt{s} = 13 TeV, '+lumi+' fb^{-1}', 
    0.25, 0.937775, 0.8)

  c.Update()
  if logY: c.SetLogy()

  ## Check to make sure the directory exists &
  ## then print the proper files to the output
  dirExists = os.path.exists(outputDir)
  if not dirExists:
    print("Warning: output directory does not exist.")
    os.makedirs(outputDir)
    print(">>> output directory created.")

  ## Print out the plots
  extraBit = ''
  if logY: extraBit = '_logY'
  fullpath = outputDir + '/' + canvasName + extraBit

  c.Print(fullpath + '.png')
  c.Print(fullpath + '.pdf')
  c.Print(fullpath + '.C')

#######################################################################
## makeRatioPlot
#######################################################################
def makeRatioPlot(plots, plotNames, canvasName, outputDir, xAxisTitle,
                  xAxisRange, ratioTitle, logY, lumi,
                  moveLegendLeft=False):

  ## Set some useful settings
  ROOT.gStyle.SetOptStat(0) # remove stat box

  ## Make the canvas
  c = ROOT.TCanvas('', '', 600, 600)
  c.SetLeftMargin(0.15)
  c.cd()
  
  ## Prepare the legend                                                                              
  x0 = 0.53; x1 = 0.89
  if moveLegendLeft:
    x0 = 0.17
    x1 = 0.42

  l = ROOT.TLegend(x0, 0.70, x1, 0.87)
  l.SetLineWidth(2)
  l.SetBorderSize(0)
  l.SetTextFont(42)
  l.SetTextSize(0.035)

  ## Modify the plots as necessary                                         
  maxVal = -1
  customColors = [ ROOT.kBlack, ROOT.kBlue]
  for i in range(0,len(plots)):
    #if (i == 0):
    #  plots[i].SetFillColor(ROOT.kGray)
    plots[i].SetLineColor(customColors[i])
    plots[i].SetLineWidth(2)
    l.AddEntry(plots[i], plotNames[i], 'F')

    binmax = plots[i].GetMaximumBin()
    binval = plots[i].GetBinContent(binmax)
    if binval > maxVal:
      maxVal = binval

  if logY:
    maxLog = log(maxVal)/log(10)
    maxVal = pow(10, maxLog+1)
    print(maxVal)
  else:
    maxVal = maxVal + 2


  rat = ROOT.TRatioPlot(plots[0], plots[1])
  plots[0].GetXaxis().SetTitle(xAxisTitle)
  rat.SetH1DrawOpt("hist")
  rat.SetH2DrawOpt("hist")
  rat.Draw("hist")

  c.Update()
  c.cd()

  l.Draw("same")

  if logY: rat.GetUpperPad().SetLogy()

  rat.GetLowerRefGraph().SetMinimum(0)
  rat.GetLowerRefGraph().SetMaximum(2)

  rat.GetUpperRefXaxis().SetRange(xAxisRange[0], xAxisRange[1])
  rat.GetLowerRefXaxis().SetRange(xAxisRange[0], xAxisRange[1])
  rat.GetLowerRefYaxis().SetTitle(ratioTitle)

  binSize = plots[0].GetBinCenter(2) - plots[1].GetBinCenter(1)
  binSize = round(binSize,3)
  extraBit = ''
  if ('GeV' in xAxisTitle):
    extraBit = ' GeV'

  rat.GetUpperRefYaxis().SetTitle("Events/" + str(binSize) + extraBit)

  rat.GetUpperPad().Modified()
  rat.GetUpperPad().Update()
  rat.GetLowerPad().Modified()
  rat.GetLowerPad().Update()
  ## Check to make sure the directory exists &                                
  ## then print the proper files to the output                          

  dirExists = os.path.exists(outputDir)
  if not dirExists:
    print("Warning: output directory does not exist.")
    os.makedirs(outputDir)
    print(">>> output directory created.")

  ## Print out the plots             
  extraBit = ''
  if logY: extraBit = '_logY'
  fullpath = outputDir + '/' + canvasName + extraBit

  c.Print(fullpath + '.png')
  c.Print(fullpath + '.pdf')
  c.Print(fullpath + '.C')

  
#######################################################################
## makeUpDownSystPlot
#######################################################################
def makeUpDownSystPlot(plots, plotNames, canvasName, outputDir, xAxisTitle,
                    xAxisRange, logY, lumi, custom_colors=colors,
                    moveLegendLeft=False):

  ## Set some useful settings
  ROOT.gStyle.SetOptStat(0)  # remove stat box

  ## Make the canvas
  c = ROOT.TCanvas('', '', 600, 600)
  c.SetLeftMargin(0.15)
  c.cd()

  ## Prepare the legend
  x0 = 0.53; x1 = 0.89
  if moveLegendLeft:
    x0 = 0.17
    x1 = 0.42

  l = ROOT.TLegend(x0, 0.70, x1, 0.87)
  l.SetLineWidth(2)
  l.SetBorderSize(0)
  l.SetTextFont(42)
  l.SetTextSize(0.035)

  ## Modify the plots as necessary
  maxVal = -1
  for i in range(0,len(plots)):
    if (i == 0):
      plots[i].SetFillColor(ROOT.kGray)
    plots[i].SetLineColor(custom_colors[i])
    plots[i].SetLineWidth(2)
    l.AddEntry(plots[i], plotNames[i], 'F')

    binmax = plots[i].GetMaximumBin()
    binval = plots[i].GetBinContent(binmax)
    if binval > maxVal:
      maxVal = binval

  if logY:
    maxLog = log(maxVal)/log(10)
    maxVal = pow(10, maxLog+1)
    print(maxVal)
  else:
    maxVal = maxVal + 2
  
  ## Create the proper pads
  ## PAD #1 - the top pad (the plots)
  pad1_v1 = ROOT.TPad("pad1_v1", "pad1_v1", 0, 0.3, 1.0, 1.0)
  #pad1_v1.SetBottomMargin(0.05)
  pad1_v1.Draw()
  pad1_v1.cd()

  plots[0].Draw("hist")
  for i in range(1,len(plots)):
    plots[i].Draw("same hist")

  l.Draw()
  c.cd()

  binSize = plots[0].GetBinCenter(2) - plots[1].GetBinCenter(1)
  binSize = round(binSize,3)
  extraBit = ''
  if ('GeV' in xAxisTitle):
    extraBit = ' GeV'
  
  plots[0].GetYaxis().SetTitle("Events/" + str(binSize) + extraBit)
  plots[0].GetXaxis().SetRangeUser(xAxisRange[0], xAxisRange[1])
  plots[0].SetMaximum(maxVal)
  plots[0].GetXaxis().SetTitleOffset(1.15)
  plots[0].GetXaxis().SetTitle(xAxisTitle)

  if logY:
    pad1_v1.SetLogy()
  
  myText('CMS Work in Progress #sqrt{s} = 13 TeV', 0.5, 0.937775, 0.5)
  pad1_v1.Modified()
  pad1_v1.Update()
  
  ## PAD #2 - the bottom pad (up/dn over nominal)
  pad1_v2 = ROOT.TPad("pad1_v2", "pad1_v2", 0, 0.1, 1.0, 0.3)
  #pad1_v2.SetTopMargin(0.0)
  #pad1_v2.SetBottomMargin(0.1)
  pad1_v2.Draw()
  pad1_v2.cd()

  ratioUp = plots[1].Clone()
  ratioUp.Divide(plots[0])
  #ratioUp.SetFillColorAlpha(0.0)
  ratioUp.SetLineColor(custom_colors[1])

  #p2 = plots[2].Clone()#.Rebin(10)
  #p0 = plots[0].Clone()#.Rebin(10)
  #for i in range(1, p2.GetNbinsX()):
  #  n2 = p2.GetBinContent(i)
  #  n0 = p0.GetBinContent(i)
  #  print(n2, " | ", n0)
  
  ratioDn = plots[2].Clone()
  ratioDn.Divide(plots[0])
  ratioDn.SetLineColor(custom_colors[2])

  ratioUp.Draw("hist")
  ratioDn.Draw("same hist")

  ratioUp.SetTitle("")
  ratioUp.SetMinimum(0.8)
  ratioUp.SetMaximum(1.2)
  ratioUp.GetXaxis().SetTitleSize(0.13)
  #ratioUp.GetXaxis().SetTitleOffset(1.15)
  ratioUp.GetXaxis().SetLabelSize(0.1)
  ratioUp.GetXaxis().SetTitle(xAxisTitle)
  ratioUp.GetXaxis().SetRangeUser(xAxisRange[0], xAxisRange[1])
  ratioDn.GetXaxis().SetTitleSize(0.13)
  ratioDn.GetXaxis().SetTitle(xAxisTitle)

  ratioUp.GetYaxis().SetTitle("#frac{Up/Down}{Nominal}")
  ratioUp.GetYaxis().CenterTitle()
  ratioUp.GetYaxis().SetTitleSize(0.12)
  ratioUp.GetYaxis().SetTitleOffset(0.35)
  ratioUp.GetYaxis().SetLabelSize(0.09)
  
  line_v1 = ROOT.TLine(plots[0].GetXaxis().GetXmin(), 1, plots[0].GetXaxis().GetXmax(), 1)
  line_v1.SetLineStyle(2)
  line_v1.Draw("same")

  pad1_v2.Modified()
  pad1_v2.Update()
  #c.Update()
  
  ## Check to make sure the directory exists &                         
  ## then print the proper files to the output
  
  dirExists = os.path.exists(outputDir)
  if not dirExists:
    print("Warning: output directory does not exist.")
    os.makedirs(outputDir)
    print(">>> output directory created.")

  ## Print out the plots
  extraBit = ''
  if logY: extraBit = '_logY'
  fullpath = outputDir + '/' + canvasName + extraBit

  c.Print(fullpath + '.png')
  c.Print(fullpath + '.pdf')
  c.Print(fullpath + '.C')
  
## == |EOF| ===========================================================
