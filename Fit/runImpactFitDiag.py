import os,sys

#Note: need to run runLimit.py first to copy relevant datacard to Allyears and All

years = ['16','16_preVFP','17','18','Allyears','All']
channels = ['ZccHcc','VHcc','All']

#note run impacts can take days
runMode = 'fitdiag' #'fitdiag' #'impact': fitdiag to make pre-post fit plots, impact to make impact plots
maps = ['VHcc','VVcc','VVbb']

dir_fit = 'DoFit_NLO_VV_breakVV_Peter_2025FebJan/tfMC_00_tfRes_00'

for channel in channels:
  
  for y in years:
    
    if (y == 'All' and channel != 'All') or (y!='All' and channel == 'All'): continue
    if y == 'Allyears' and channel == 'All': continue
    
    tmp = dir_fit+'/'+y+'_'+channel
    if y == 'All':
       tmp = dir_fit+'/'+y
    rootDir = os.getcwd()
    dir = os.chdir(tmp)
    
    for map in maps:
      tmp = 'model_combined_'+y+'_'+channel
      if y == 'Allyears':
         tmp = 'model_combined_allyears_'+channel
      if y == 'All':
         tmp = 'model_combined_all'

      tmp1 = 'r[1,-500,500]'
      #if map == 'VHcc': tmp1 = 'r[1,-1000,1000]'

      cmd = "text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel  --PO verbose --PO 'map=.*/"+map+"*:"+tmp1+"' "+tmp+".txt" 
      os.system(cmd)
      os.system('cp '+tmp+'.root model_combined_'+y+'_'+channel+'_'+map+'.root')

      if runMode == 'impact':
          os.system('rm *impacts_'+map+'*')
  
          # Do initial fit
          cmd = "combineTool.py -M Impacts -d model_combined_"+y+"_"+channel+"_"+map+".root -m 125 -n .impacts_"+map+" --expectSignal 1 --doInitialFit --robustFit 1 -t -1 -v 3 --X-rtd FITTER_DYN_STEP --cminDefaultMinimizerStrategy 0"
          #cmd = "combineTool.py -M Impacts -d model_combined_"+map+"_"+y+"_"+channel+".root -m 125 -n .impacts --doInitialFit --robustFit 1 -t -1 -v 3"
          os.system(cmd)
  
          # Do more fits
          cmd = "combineTool.py -M Impacts -d model_combined_"+y+"_"+channel+"_"+map+".root -m 125 -n .impacts_"+map+" --expectSignal 1 --doFits --robustFit 1 --exclude 'rgx{qcdparams*}' -t -1"
          os.system(cmd)
  
          cmd = "combineTool.py -M Impacts -d model_combined_"+y+"_"+channel+"_"+map+".root -m 125 -n .impacts_"+map+" --expectSignal 1 --exclude 'rgx{qcdparams*}' -o impacts_"+map+".json -t -1"
          os.system(cmd)
          
          '''
          cmd = 'combineTool.py -M Impacts -d model_combined_"+y+"_"+channel+"_"+map+".root -m 125 -n .impacts_"+map+" --setParameterRanges r=-100,100 -v 3 --expectSignal 1 --doInitialFit --robustFit 1 -t -1 --toysFrequentist --X-rtd FITTER_DYN_STEP'
          os.system(cmd)
          cmd = 'combineTool.py -M Impacts -d model_combined_"+y+"_"+channel+"_"+map+".root -m 125 -n .impacts_"+map+" --setParameterRanges r=-100,100 -v 3 --expectSignal 1 --doFit --robustFit 1 -t -1 --toysFrequentist --X-rtd FITTER_DYN_STEP'
          os.system(cmd)
          cmd = 'combineTool.py -M Impacts -d model_combined_"+y+"_"+channel+"_"+map+".root -m 125 --freezeParameters MH -n .impacts_"+map+" --setParameterRanges r=-100,100 -o impacts_part3.json'
          os.system(cmd)
          '''
          
  
          os.system('plotImpacts.py -i impacts_'+map+'.json -o impacts_'+map+'_'+y+'_'+channel)
      
      if runMode == 'fitdiag':
        os.system('rm *Diagnostics*')
        cmd = 'combine -M FitDiagnostics -m 125 model_combined_'+y+'_'+channel+"_"+map+'.root --setParameters r=1 --saveShapes --saveWithUncertainties --cminDefaultMinimizerStrategy 0 --robustFit=1 -t -1'
        os.system(cmd)
    
    print(os.getcwd())
    os.chdir(rootDir)
    print(os.getcwd())



    #cmd = 'combineTool.py -M Impacts -d model_combined_"+y+"_"+channel+".root -m 125 -n .impacts --setParameterRanges r=-100,100 -v 3 --expectSignal 1 --doInitialFit --robustFit 1 -t -1 --toysFrequentist --cminDefaultMinimizerStrategy 0 --X-rtd FITTER_DYN_STEP'
    #cmd = "combineTool.py -M Impacts -d model_combined_"+y+"_"+channel+".root -m 125 -n .impacts --doFits --robustFit 1 --setParameterRanges r=-100,100  -t -1 --toysFrequentist --X-rtd MINIMIZER_analytic --exclude 'rgx{qcdparams*}'"
    #cmd = 'combineTool.py -M Impacts -d model_combined_"+y+"_"+channel+".root -m 125 --freezeParameters MH -n .impacts --setParameterRanges r=-100,100 --doFits --robustFit 1 -t -1 --toysFrequentist --X-rtd FITTER_DYN_STEP'

#--toysFrequentist: which causes a fit to the data to be performed first (with r frozen to the --expectSignal value) and then any subsequent Asimov datasets or toys are generated using the post-fit values of the model parameters. In general this will result in a different value for the expected significance due to changes in the background normalisation and shape induced by the fit to data:
#https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/latest/part5/longexercise/?h=toysfrequen#d-calculating-the-significance
