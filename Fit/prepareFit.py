#!/usr/bin/env python
# coding: utf-8

from __future__ import print_function, division
import sys
import os
import rhalphalib as rl
import json
import numpy as np
import scipy.stats
import pickle
import ROOT
rl.util.install_roofit_helpers()
rl.ParametericSample.PreferRooParametricHist = False
ROOT.gROOT.SetBatch(True)


import sys
import logging

eps=0.0000001

#t_readFile
def histToNp(h):
    a = []
    for iB in range(1,h.GetNbinsX()+1):
        a.append(h.GetBinContent(iB))
    return np.array(a)

def makeSample(h, obs):
    np_from_hist = histToNp(h)
    sumw2 = []
    for i in range(1,h.GetNbinsX()+1):
        sumw2 += [h.GetBinError(i)*h.GetBinError(i)]
    return (np_from_hist,obs.binning,obs.name,np.array(sumw2))

#not used currently
def getSyst(s,r,syst,f,y): #s: sample, r:region, sys: systematic, f: file
    #hN = s+'_NONE_' + r
    #hC = f.Get('NONE/'+hN)
    #TEMP use NONE_FOR_SYST as central
    hN = s+'_NONE_FOR_SYST_' + r + '_' + y
    #print(f, hN)
    hC = f.Get('NONE_FOR_SYST/'+hN)
    nC = hC.Integral()
    hN = s+'_'+syst+'U_' + r + '_' + y
    hSU = f.Get(syst+'U/'+hN)
    nSU = hSU.Integral()
    sU = 0.0001
    if nSU > 0: sU = (nSU - nC)/nSU
    hN = s+'_'+syst+'D_' + r + '_' + y
    hSD = f.Get(syst+'D/'+hN)
    nSD = hSD.Integral()
    sD = -0.0001
    if nSD > 0: sD = (nSD - nC)/nSD
    systEff = (abs(sU)+abs(sD))/2.
    #print(syst, sU, sD, systEff)
    return systEff

# Read the histogram
def get_template(f, sName, r, y, obs, syst):
    """
    Read msd template from root file
    """
    #Jet 1 ParticleNet bb pass/failing region
    #ZHcc_JESU_topCR_pass_17
    name='{}/{}_{}_{}_{}'.format(syst,sName, syst, r, y)
    print("Extracting ... ", name)
    h = f.Get(name)

    sumw = []
    sumw2 = []
    
    for i in range(1, h.GetNbinsX()+1):
        sumw += [h.GetBinContent(i)]
        sumw2 += [h.GetBinError(i)*h.GetBinError(i)]

    return (np.array(sumw), obs.binning, obs.name, np.array(sumw2))

# Tell me if my sample is too small to care about
def badtemp_ma(hvalues, mask=None):
    # Need minimum size & more than 1 non-zero bins                                                                                                                                   
    tot = np.sum(hvalues[mask])

    count_nonzeros = np.sum(hvalues[mask] > 0)
    if (tot < eps) or (count_nonzeros < 2):
        print(">>>>>>>:",hvalues) 
        print(">>>>:", tot, count_nonzeros)
        return True
    else: return False

def shape_to_num(var, nom, clip=1.5):
    """
    Don't get any systematics get bigger than 50%
    """
    nom_rate = np.sum(nom)
    var_rate = np.sum(var)

    if abs(var_rate/nom_rate) > clip:
        var_rate = clip*nom_rate

    if var_rate < 0:
        var_rate = 0

    return var_rate/nom_rate

def makeCards(dir_model,p_tfMC,p_tfRes,y,channel,VVtype,fTempNames,breakVV=True,doSyst=True):
  #TEMP not include topCR
  includeTopCR = True
  #dir_model = 'Models'
  ptCutL = 450 
  ptCutH = 1200 
  if includeTopCR: 
      topNormSF = rl.IndependentParameter('topNormSF%s%s'%(y,channel), 1., 0, 10)
  ptbins = np.array([ptCutL, ptCutH])
  npt = len(ptbins) - 1
  msdbins = np.linspace(40, 201, 24)
  msd = rl.Observable('msd', msdbins)
#msdbins
# here we derive these all at once with 2D array
  ptpts, msdpts = np.meshgrid(ptbins[:-1] + 0.3 * np.diff(ptbins), msdbins[:-1] + 0.5 * np.diff(msdbins), indexing='ij')
  rhopts = 2*np.log(msdpts/ptpts)
  ptscaled = (ptpts - ptCutL) / (ptCutH - ptCutL)
  rhoscaled = (rhopts - (-6)) / ((-2.1) - (-6))
  validbins = (rhoscaled >= 0) & (rhoscaled <= 1)
  rhoscaled[~validbins] = 1  # we will mask these out later

  fSyst = ROOT.TFile.Open(fTempNames[channel],'READ') 

  #get QCD fail and pass m_sd
  hP = fSyst.Get('NONE/QCD_NONE_SR_'+y)
  np_pass = histToNp(hP)
  hF = fSyst.Get('NONE/QCD_NONE_CR_qcd_'+y)
  np_fail = histToNp(hF)
  
  yTmp = y.replace('_','')

  ####################################################
  #QCD Model
  ####################################################
  qcdmodel = rl.Model("qcdmodel")
  qcdpass, qcdfail = 0., 0.
  for ptbin in range(npt):
      failCh = rl.Channel("ptbin%d%s%s%s" % (ptbin, 'fail',yTmp,channel))
      passCh = rl.Channel("ptbin%d%s%s%s" % (ptbin, 'pass',yTmp,channel))
      qcdmodel.addChannel(failCh)
      qcdmodel.addChannel(passCh)
      failCh.setObservation((np_fail,msd.binning,msd.name))
      passCh.setObservation((np_pass,msd.binning,msd.name))
      qcdfail += failCh.getObservation().sum()
      qcdpass += passCh.getObservation().sum()

  qcdeff = qcdpass / qcdfail
  #print(qcdeff)
  #tf_MCtempl = rl.BernsteinPoly("tf_MCtempl", p_tfMC, ['pt', 'rho'], limits=(0, 10))
  with open(f'initial_vals.json') as f: initial_vals = np.array(json.load(f)['initial_vals'])
  print("Initial fit values: ", initial_vals)
  print("Poly Shape: ", (initial_vals.shape[0]-1, initial_vals.shape[1]-1))
  tf_MCtempl = rl.BasisPoly("tf_MCtempl_%s%s"%(yTmp,channel),
                                (initial_vals.shape[0]-1,initial_vals.shape[1]-1), #shape
                                ["pt", "rho"], #variable names
                                basis='Bernstein', #type of polys
                                init_params=initial_vals, #initial values
                                limits=(0, 10), #limits on poly coefficients
                                coefficient_transform=None)

  tf_MCtempl_params = qcdeff * tf_MCtempl(ptscaled, rhoscaled) #each pt and rho bin, there is a para
  #print( tf_MCtempl_params )

  #add channels
  for ptbin in range(npt):
          failCh = qcdmodel['ptbin%dfail%s%s' % (ptbin,yTmp,channel)]
          passCh = qcdmodel['ptbin%dpass%s%s' % (ptbin,yTmp,channel)]
          failObs = failCh.getObservation()
          qcdparams = np.array([rl.IndependentParameter('qcdparam_ptbin%d_msdbin%d%s%s' % (ptbin, i,yTmp,channel), 0) for i in range(msd.nbins)])
          sigmascale = 10.
          scaledparams = failObs * (1 + sigmascale/np.maximum(1., np.sqrt(failObs)))**qcdparams
          fail_qcd = rl.ParametericSample('ptbin%dfail%s%s_qcd' % (ptbin,yTmp,channel), rl.Sample.BACKGROUND, msd, scaledparams)
          failCh.addSample(fail_qcd)
          pass_qcd = rl.TransferFactorSample('ptbin%dpass%s%s_qcd' % (ptbin,yTmp,channel), rl.Sample.BACKGROUND, tf_MCtempl_params[ptbin, :], fail_qcd)
          passCh.addSample(pass_qcd)
          failCh.mask = validbins[ptbin]
          passCh.mask = validbins[ptbin]


  #qcdFit to get TF functions from MC
  qcdfit_ws = ROOT.RooWorkspace('qcdfit_ws')
  simpdf, obs = qcdmodel.renderRoofit(qcdfit_ws)
  qcdfit = simpdf.fitTo(obs,
                            ROOT.RooFit.Extended(True),
                            ROOT.RooFit.SumW2Error(True),
                            ROOT.RooFit.Strategy(2),
                            ROOT.RooFit.Save(),
                            ROOT.RooFit.Minimizer('Minuit2', 'migrad'),
                            ROOT.RooFit.PrintLevel(-1),
                            )
  qcdfit_ws.add(qcdfit)
  qcdfit.status()

  qcdfit_ws.writeToFile(os.path.join(str(dir_model), 'testModel_qcdfit.root'))
  print(">>>>Status is: ", qcdfit.status())
  if qcdfit.status() != 0:
      raise RuntimeError('Could not fit qcd')


#t_plotQcdFit
  if True:
    channel_cats = ROOT.RooCategory("qcdmodel_channel", "channel_cat")
    channel_cats.defineType("ptbin0fail%s%s"%(yTmp,channel))
    channel_cats.defineType("ptbin0pass%s%s"%(yTmp,channel))
    channel_cats.Print("v")
    
    x = qcdfit_ws.allVars()['msd'] 
    frame12 = x.frame(ROOT.RooFit.Title("QCD fail"))
    frame13 = x.frame(ROOT.RooFit.Title("QCD pass"))

    obs.plotOn(frame12,ROOT.RooFit.Cut('qcdmodel_channel==qcdmodel_channel::ptbin0fail%s%s'%(yTmp,channel)))
    simpdf.plotOn(frame12,ROOT.RooFit.Slice(channel_cats, "ptbin0fail%s%s"%(yTmp,channel)),ROOT.RooFit.ProjWData(channel_cats,obs))
    obs.plotOn(frame13,ROOT.RooFit.Cut('qcdmodel_channel==qcdmodel_channel::ptbin0pass%s%s'%(yTmp,channel)))
    simpdf.plotOn(frame13,ROOT.RooFit.Slice(channel_cats, "ptbin0pass%s%s"%(yTmp,channel)),ROOT.RooFit.ProjWData(channel_cats, obs),ROOT.RooFit.LineColor(2))
    
    c = ROOT.TCanvas("rf501_simultaneouspdf1", "rf501_simultaneouspdf1", 800, 400)
    c.Divide(2)
    c.cd(1) 
    ROOT.gPad.SetLeftMargin(0.15)
    frame12.GetYaxis().SetTitleOffset(1.4)
    frame12.Draw()
    c.cd(2)
    ROOT.gPad.SetLeftMargin(0.15)
    frame13.GetYaxis().SetTitleOffset(1.4)
    frame13.Draw()

    from ROOT import gROOT 
    gROOT.GetListOfCanvases().Draw()
    
    c.Print('Plots/'+c.GetName()+'_'+y+'_'+channel+'.pdf')
    c.Print('Plots/'+c.GetName()+'_'+y+'_'+channel+'.C')



  #TF parameters
  param_names = [p.name for p in tf_MCtempl.parameters.reshape(-1)]
  decoVector = rl.DecorrelatedNuisanceVector.fromRooFitResult(tf_MCtempl.name + '_deco', qcdfit, param_names)
  tf_MCtempl.parameters = decoVector.correlated_params.reshape(tf_MCtempl.parameters.shape)
  tf_MCtempl_params_final = tf_MCtempl(ptscaled, rhoscaled)
  tf_dataResidual = rl.BernsteinPoly("tf_dataResidual_%s%s"%(yTmp,channel), p_tfRes, ['pt', 'rho'], limits=(0, 10))
  tf_dataResidual_params = tf_dataResidual(ptscaled, rhoscaled)
  tf_params = qcdeff * tf_MCtempl_params_final * tf_dataResidual_params
  #sys uncertainty
  sys_lumi = {'16':rl.NuisanceParameter('lumi_16', 'lnN'),'16_preVFP':rl.NuisanceParameter('lumi_16', 'lnN'),'17':rl.NuisanceParameter('lumi_17', 'lnN'),'18':rl.NuisanceParameter('lumi_18', 'lnN')}
  sys_lumi_cor_161718 = rl.NuisanceParameter('lumi_correlated', 'lnN')
  sys_lumi_cor_1718   = rl.NuisanceParameter('lumi_correlated_1718', 'lnN')
  sys_JES              = rl.NuisanceParameter('jes', 'shape')
  sys_JER              = rl.NuisanceParameter('jer', 'shape')
  sys_PU               = rl.NuisanceParameter('pu', 'shape')
  sys_l1prefire        = rl.NuisanceParameter('l1prefire_1617', 'shape') #only apply to 2016,2017
  sys_trig             = rl.NuisanceParameter('trigger', 'shape')
  sys_elec             = rl.NuisanceParameter('ele_veto', 'lnN')
  sys_muon             = rl.NuisanceParameter('muon_veto', 'lnN')
  sys_tagcc            = rl.NuisanceParameter('tagcc', 'lnN')

  systs = {'JES':sys_JES,'JER':sys_JER,'PU':sys_PU,'L1PREFIRING':sys_l1prefire,'TRIG':sys_trig,'ELEC':sys_elec,'MUON':sys_muon,'TAG_CC':sys_tagcc}
  #TEMP turn of JER for now
  #systs = {'JES':sys_JES,'PU':sys_PU,'L1PREFIRING':sys_l1prefire,'TRIG':sys_trig,'ELEC':sys_elec,'MUON':sys_muon,'TAG_CC':sys_tagcc}
  #systs = {'JES':sys_JES}

  #lumi_dict = {
  #      "16": 1.0026,
  #      "17": 1.006,
  #      "18": 1.0065
  #}
  #lumi_corr_161718 = 1.013
  #lumi_corr_1718 = 1.0027
  if not doSyst: systs = {}
  
  lumi_unc_dict = {
    "16": {"uncorrelated": 1.010, "correlated": 1.006, "correlated_1718": 0},
    "16_preVFP": {"uncorrelated": 1.010, "correlated": 1.006, "correlated_1718": 0},
    "17": {"uncorrelated": 1.020, "correlated": 1.009, "correlated_1718": 1.006},
    "18": {"uncorrelated": 1.015, "correlated": 1.020, "correlated_1718": 1.002}
  }
  
  # build actual fit model now
  model = rl.Model("model_%s_%s"%(yTmp,channel))
  sNames = ['VHcc','VHbb','Top','VJ','VV','qcd']
  if breakVV:
      sNames = ['VHcc','VHbb','Top','VJ','VVcc','VVbb','VVother','qcd']
  for ptbin in range(npt):
    for region in ['pass','fail']: #pass = SR, fail = CR_qcd
        ch = rl.Channel("ptbin%d%s%s%s" % (ptbin, region, yTmp,channel))
        model.addChannel(ch)

        isPass = region == 'pass'
        templates = {}    
        #add samples
        for sName in sNames:
            if sName == 'qcd': continue
            if isPass:
                h = fSyst.Get('NONE/'+sName+'_NONE_SR_'+y)
                hNomForSyst = fSyst.Get('NONE_FOR_SYST/'+sName+'_NONE_FOR_SYST_SR_'+y)
            else:
                h = fSyst.Get('NONE/'+sName+'_NONE_CR_qcd_'+y)
                hNomForSyst = fSyst.Get('NONE_FOR_SYST/'+sName+'_NONE_FOR_SYST_CR_qcd_'+y)
            #print(">>>>>",hNomForSyst) 
            nominal = makeSample(hNomForSyst,msd)[0]
            
            #print(">>>>>>>>>>",nominal)
            if(badtemp_ma(nominal)):
                    print("Sample {} is too small, skipping".format(sName))
                    continue

            templ = makeSample(h,msd)        
            stype = rl.Sample.SIGNAL if sName == 'VHcc' else rl.Sample.BACKGROUND
            sample = rl.TemplateSample(ch.name + '_' + sName, stype, templ)
            
            #stat and syst
            sample.autoMCStats(lnN=True)
            
            sample.setParamEffect(sys_lumi[y], lumi_unc_dict[y]['uncorrelated'])
            sample.setParamEffect(sys_lumi_cor_161718, lumi_unc_dict[y]['correlated'])
            sample.setParamEffect(sys_lumi_cor_1718, lumi_unc_dict[y]['correlated_1718'])

            if includeTopCR:
                if sName == 'Top':
                    sample.setParamEffect(topNormSF, 1*topNormSF)
             
            for syst,nui in systs.items():
                if y == '18' and syst == 'L1PREFIRING': continue #prefiring only affect 16,17 
                #print(syst,nui)
                rName = 'SR' if isPass else 'CR_qcd'
                histName = '{}/{}_{}_{}_{}'.format(syst+'U',sName, syst+'U', rName, y)
                #print(histName)
                hU = fSyst.Get(histName)
                syst_up = makeSample(hU,msd)[0]
                histName = '{}/{}_{}_{}_{}'.format(syst+'D',sName, syst+'D', rName, y)
                hD = fSyst.Get(histName)
                syst_do = makeSample(hD,msd)[0]

                #syst_up = get_template(f=fSyst, sName=sName, r=rName, y=y, obs=msd, syst=syst+'U')[0]
                #syst_do = get_template(f=fSyst, sName=sName, r=rName, y=y, obs=msd, syst=syst+'D')[0]

                eff_up = shape_to_num(syst_up,nominal)
                eff_do = shape_to_num(syst_do,nominal)

                sample.setParamEffect(nui, eff_up, eff_do)

                '''
                systEff = 1+getSyst(sName,r,syst,fSyst,y)
                #FIXME: assign artificial small unc of 0.1% for zero syst effect as a place holder
                if systEff == 1: systEff = 1.001
                sample.setParamEffect(nui,systEff)
                print(sName,r,y,systEff)
                '''

            ch.addSample(sample)
        

        #data
        if isPass:
            h = fSyst.Get('NONE/data_obs_NONE_SR_'+y)
        else:
            h = fSyst.Get('NONE/data_obs_NONE_CR_qcd_'+y)
        #h.Print("all")
        #print(histToNp(h))
        ch.setObservation((histToNp(h),msd.binning,msd.name))
        
        # drop bins outside rho validity
        mask = validbins[ptbin]
        # blind bins 11, 12, 13
        # mask[11:14] = False
        ch.mask = mask
    

    failCh = model['ptbin%dfail%s%s' % (ptbin,yTmp,channel)]
    passCh = model['ptbin%dpass%s%s' % (ptbin,yTmp,channel)]

    qcdparams = np.array([rl.IndependentParameter('qcdparam_ptbin%d_msdbin%d%s%s' % (ptbin, i, y,channel), 0) for i in range(msd.nbins)])
    initial_qcd = failCh.getObservation().astype(float)  # was integer, and numpy complained about subtracting float from it
    #print(initial_qcd)
    #subtract all nonQCD bkgr
    for sample in failCh:
        #print(sample)
        #print(sample.getExpectation(nominal=True))
        initial_qcd -= sample.getExpectation(nominal=True)
    
    #print(initial_qcd)
    if np.any(initial_qcd < 0.):
        raise ValueError("initial_qcd negative for some bins..", initial_qcd)
    sigmascale = 10  # to scale the deviation from initial
    scaledparams = initial_qcd * (1 + sigmascale/np.maximum(1., np.sqrt(initial_qcd)))**qcdparams #see Javier talk
    fail_qcd = rl.ParametericSample('ptbin%dfail%s%s_qcd' % (ptbin,yTmp,channel), rl.Sample.BACKGROUND, msd, scaledparams)
    failCh.addSample(fail_qcd)
    pass_qcd = rl.TransferFactorSample('ptbin%dpass%s%s_qcd' % (ptbin,yTmp,channel), rl.Sample.BACKGROUND, tf_params[ptbin, :], fail_qcd)
    passCh.addSample(pass_qcd)
    
    #topCR
    if includeTopCR: 
        #top norm
        failCh['Top'].setParamEffect(topNormSF, 1*topNormSF)
        passCh['Top'].setParamEffect(topNormSF, 1*topNormSF)
    
        ####################################
        #topCR
        ####################################
        ch = rl.Channel("topCR%d%s%s" % (ptbin,yTmp,channel))
        model.addChannel(ch)

        templates = {}
        for sName in sNames: #add all processes here
            #TEMP add VH to topCR
            #if 'VH' in sName: continue
            hName = sName
            if sName == 'qcd': hName = 'QCD'
            h = fSyst.Get('NONE/'+hName+'_NONE_topCR_pass_'+y)
            hNomForSyst = fSyst.Get('NONE_FOR_SYST/'+hName+'_NONE_FOR_SYST_topCR_pass_'+y)
            nominal = makeSample(hNomForSyst,msd)[0]

            #TEMP change qcd name here to qcd1 so that independent with other qcd
            if sName == "qcd": sName = "qcdTopCR"
            templ = makeSample(h,msd)        
            stype = rl.Sample.SIGNAL if sName == 'VHcc' else rl.Sample.BACKGROUND
            #stype = rl.Sample.BACKGROUND
            sample = rl.TemplateSample(ch.name + '_' + sName, stype, templ)
            #uncertainty
            sample.autoMCStats(lnN=True)
            sample.setParamEffect(sys_lumi[y], lumi_unc_dict[y]['uncorrelated'])
            sample.setParamEffect(sys_lumi_cor_161718, lumi_unc_dict[y]['correlated'])
            sample.setParamEffect(sys_lumi_cor_1718, lumi_unc_dict[y]['correlated_1718'])
            #sample.setParamEffect(sys_lumi_uncor_16, lumi_dict['2016'])
            #sample.setParamEffect(sys_lumi_uncor_17, lumi_dict['2017'])
            #sample.setParamEffect(sys_lumi_uncor_18, lumi_dict['2018'])
            #sample.setParamEffect(sys_lumi_cor_161718, lumi_corr_161718)
            #sample.setParamEffect(sys_lumi_cor_1718, lumi_corr_1718)
            
            for syst,nui in systs.items():
                if y == '18' and syst == 'L1PREFIRING': continue #prefiring only affect 16,17 
                #print(syst,nui)
                histName = '{}/{}_{}_{}_{}'.format(syst+'U',hName, syst+'U', 'topCR_pass', y)
                #print(histName)
                hU = fSyst.Get(histName)
                syst_up = makeSample(hU,msd)[0]
                histName = '{}/{}_{}_{}_{}'.format(syst+'D',hName, syst+'D', 'topCR_pass', y)
                hD = fSyst.Get(histName)
                syst_do = makeSample(hD,msd)[0]

                #syst_up = get_template(f=fSyst, sName=sName, r=rName, y=y, obs=msd, syst=syst+'U')[0]
                #syst_do = get_template(f=fSyst, sName=sName, r=rName, y=y, obs=msd, syst=syst+'D')[0]

                eff_up = shape_to_num(syst_up,nominal)
                eff_do = shape_to_num(syst_do,nominal)

                sample.setParamEffect(nui, eff_up, eff_do) 
                '''
                systEff = 1+getSyst(hName,'topCR_pass',syst,fSyst,y)
                #FIXME: assign artificial small unc of 0.1% for zero syst effect as a place holder
                if systEff == 1: systEff = 1.001
                sample.setParamEffect(nui,systEff)
                print(sName,r,y,systEff)
                '''

            ch.addSample(sample)
    
        #top norm
        ch['Top'].setParamEffect(topNormSF, 1*topNormSF)
    
        #data
        h = fSyst.Get('NONE/data_obs_NONE_topCR_pass_'+y)
        ch.setObservation((histToNp(h),msd.binning,msd.name))

  with open(os.path.join(str(dir_model), 'model_%s_%s.pkl'%(yTmp,channel)), "wb") as fout:
          pickle.dump(model, fout)

  model.renderCombine(os.path.join(str(dir_model), 'model'))

##########################################################################
#main program starts here
##########################################################################

#p_tfRes = [(0,0),(0,1),(0,2)] #BernsteinPoly order, first is for pt and second is for rho
p_tfRes = [(0,0)] #BernsteinPoly order
#p_tfMC = [(0,0),(0,1),(0,2)]
p_tfMC = [(0,0)]
VVtype = 'NLO_VV' #LO_VV
breakVV= False
doSyst = True

channels = ["ZccHcc",'VHcc'] #"ZccHcc" #VHcc

#Folders to store datacards, etc. (dir_model), and for running the fit (dir_fit)
dir_model = 'Models_'+VVtype+'_Peter_2025FebJan'
dir_fit = 'DoFit_'+VVtype+'_Peter_2025FebJan'
if breakVV:
  dir_model = 'Models_'+VVtype+'_breakVV_Peter_2025FebJan_tmp'
  dir_fit = 'DoFit_'+VVtype+'_breakVV_Peter_2025FebJan_tmp'

#templates
fTempNames = {'ZccHcc':'../ForLimitSetting/test_ZccHcc_'+VVtype+'_noQCDreweight_newBining_syst_Peter_2025JanFeb.root',\
              'VHcc':'../ForLimitSetting/test_VHcc_'+VVtype+'_noQCDreweight_newBining_syst_Peter_2025JanFeb.root'}

years = ['16_preVFP','16','17','18']
for channel in channels:
  for pMC in p_tfMC:
    for pRes in p_tfRes:
        for y in years:

            #make datacards
            yTmp = y.replace('_','') #this is because rl does not like '_' in some case
            modelName = 'tfMC_' + str(pMC[0]) + str(pMC[1]) + '_tfRes_' + str(pRes[0]) + str(pRes[1])
            dir_model_tmp = dir_model + '/' + modelName + '/' + y + '_' + channel    
            print(dir_model_tmp)
            os.system('rm -r '+dir_model_tmp)
            os.system('mkdir -p '+dir_model_tmp)
            makeCards(dir_model_tmp,pMC,pRes,y,channel,VVtype,fTempNames,breakVV,doSyst)
            os.system('chmod u+x ' + dir_model_tmp + '/model/build.sh')
            wDir = os.getcwd()
            os.chdir(dir_model_tmp+'/model')
            os.system('./build.sh')
            
            #copy datacards to fit folder
            print(os.getcwd())
            dir_fit_final = wDir+'/'+dir_fit+'/'+modelName+'/'+y+'_'+channel
            os.system('rm -r '+dir_fit_final)
            os.system('mkdir -p '+dir_fit_final)
            os.system('cp model_%s_%s.root'%(yTmp,channel) + ' ' + dir_fit_final)
            os.system('cp model_combined.txt '+dir_fit_final+'/model_combined_'+y+'_'+channel+'.txt')
            os.system('cp model_combined.root '+dir_fit_final+'/model_combined_'+y+'_'+channel+'.root')
            os.chdir(wDir)
