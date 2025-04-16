import os,sys
import subprocess

def writeToFile(inputStr, grepStr, output, fOutN):
   filtered_lines = [line for line in output.decode().split("\n") if grepStr in line]
   tmp = inputStr
   for l in filtered_lines: tmp = tmp + ' ' + l 
   with open(fOutN,'a') as file:
      file.write(tmp + '\n')
   

years = ['16','16_preVFP','17','18']
channels = ['VHcc','ZccHcc']
breakVV = False
VVtype = 'NLO_VV'

fOutN = 'limit_significance.txt'
os.system('rm ' + fOutN)

modelName = 'tfMC_00_tfRes_00'

#mapping signal strengths
pDir = 'DoFit_'+VVtype+'_Peter_2025FebJan'
text2ws_setting = ' -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel --PO verbose  --PO \'map=.*/VHcc:rVH[1,-200,200]\'  --PO \'map=.*/VV:rVV[1,-10,10]\''
if breakVV:
    pDir = 'DoFit_'+VVtype+'_breakVV_Peter_2025FebJan_tmp'
    text2ws_setting = ' -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel --PO verbose  --PO \'map=.*/VHcc:rVH[1,-200,200]\'\
                --PO \'map=.*/VVcc:rVVcc[1,-10,10]\' --PO \'map=.*/VVbb:rVVbb[1,-10,10]\' --PO \'map=.*/VVother:rVVother[1,-10,10]\''

cDir = os.getcwd()
fOutN = cDir + '/' + fOutN

#each channel
for chan in channels:
    
    #each year and each channel
    for y in years: 
        dir = pDir+'/'+modelName+'/'+y+'_'+chan
        os.chdir(dir)
        
        print('Running expected limit: ', chan, ' ', y)
        dataCard = 'model_combined_%s_%s.txt' % (y,chan)
        cmd = 'combine -M AsymptoticLimits --run blind ' + dataCard
        process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output, error = process.communicate()
        print(output.decode())
        tmp = chan + ' ' + y + ' '
        writeToFile(tmp, 'Expected 50.0%:', output, fOutN)
        
        #mapping
        cmd = 'text2workspace.py ' + dataCard + text2ws_setting
        os.system(cmd)
        
        if not breakVV:
            cmd = 'combine -M Significance -m 90 --signif ' + dataCard.replace('.txt','.root') + ' --cminDefaultMinimizerStrategy 0 --redefineSignalPOI rVV --setParameters rVV=1 --verbose 1 -t -1'
            process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            output, error = process.communicate()
            tmp = chan + ' ' + y + ' rVV '
            writeToFile(tmp, 'Significance:', output, fOutN)
                        
        if breakVV:
            cmd = 'combine -M Significance -m 90 --signif ' + dataCard.replace('.txt','.root') + ' --cminDefaultMinimizerStrategy 0 --redefineSignalPOI rVVcc --setParameters rVVcc=1 --verbose 1 -t -1'
            process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            print("Command running:", process.args)
            output, error = process.communicate()
            print(output.decode())
            tmp = chan + ' ' + y + ' rVVcc '
            writeToFile(tmp, 'Significance:', output, fOutN)
            
            print('====================Now gettting significance==============')
            cmd = 'combine -M Significance -m 90 --signif ' + dataCard.replace('.txt','.root') + ' --cminDefaultMinimizerStrategy 0 --redefineSignalPOI rVVbb --setParameters rVVbb=1 --verbose 1 -t -1'
            process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            print("Command running:", process.args)
            output, error = process.communicate()
            print(output.decode())
            tmp = chan + ' ' + y + ' rVVbb '
            writeToFile(tmp, 'Significance:', output, fOutN)
            
        os.chdir(cDir)
 
    
    #now run all years for each channels
    dir = pDir+'/'+modelName+'/Allyears_'+chan
    os.system('rm -rf ' +dir)
    os.system('mkdir -p ' + dir)
    for y in years:
        dirTmp = pDir+'/'+modelName+'/'+y+'_'+chan
        os.system('cp '+dirTmp+'/model_*.* ' + dir)
    os.chdir(dir)
    
    #combine data cards
    dataCard = 'model_combined_'+'allyears_'+chan+'.txt'
    os.system('combineCards.py model_combined_*_'+chan+'.txt > ' + dataCard)
    
    #cmd = 'combine -M AsymptoticLimits --run blind model_combined_%s_%s.txt' % (chan, 'allyears')
    cmd = 'combine -M AsymptoticLimits --run blind ' + dataCard
    process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output, error = process.communicate()
    print(output.decode())
    tmp = chan + ' AllYear '
    writeToFile(tmp, 'Expected 50.0%:', output, fOutN)
    
    #map signal strength
    cmd = 'text2workspace.py ' + dataCard + text2ws_setting
    os.system(cmd)
    
    if not breakVV:
      cmd = 'combine -M Significance -m 90 --signif ' + dataCard.replace('.txt','.root') + ' --cminDefaultMinimizerStrategy 0 --redefineSignalPOI rVV --setParameters rVV=1 --verbose 1 -t -1'
      process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      print("Command running:", process.args)
      output, error = process.communicate()
      print(output.decode())
      tmp = chan + ' AllYear rVV '
      writeToFile(tmp, 'Significance:', output, fOutN)
    
    if breakVV:
      cmd = 'combine -M Significance -m 90 --signif ' + dataCard.replace('.txt','.root') + ' --cminDefaultMinimizerStrategy 0 --redefineSignalPOI rVVcc --setParameters rVVcc=1 --verbose 1 -t -1'
      process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      print("Command running:", process.args)
      output, error = process.communicate()
      print(output.decode())
      tmp = chan + ' AllYear rVVcc '
      writeToFile(tmp, 'Significance:', output, fOutN)

      cmd = 'combine -M Significance -m 90 --signif ' + dataCard.replace('.txt','.root') + ' --cminDefaultMinimizerStrategy 0 --redefineSignalPOI rVVbb --setParameters rVVbb=1 --verbose 1 -t -1'
      process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      print("Command running:", process.args)
      output, error = process.communicate()
      print(output.decode())
      tmp = chan + ' AllYear rVVbb '
      writeToFile(tmp, 'Significance:', output, fOutN)

    os.chdir(cDir)
    

#now combine all channels for each year
for y in years:
    dir = pDir+'/'+modelName+'/%s_%s' % (y,'allchans')
    os.system('rm -rf '+dir)
    os.system('mkdir -p '+dir)
    
    for ch in channels:
        dirTmp = pDir+'/'+modelName+'/%s_%s' % (y,ch)
        cmd = 'cp ' + dirTmp + '/model_*.* ' + dir
        print(cmd)
        os.system(cmd)

    os.chdir(dir)
    
    print('Run: %s, %s' % (y,'all channels'))

    #combine data cards
    dataCard = 'model_combined_'+y+'_allchans.txt'
    os.system('combineCards.py model_combined_'+y+'_*.txt > ' + dataCard)
    
    cmd = 'combine -M AsymptoticLimits --run blind ' + dataCard
    process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output, error = process.communicate()
    print(output.decode())
    tmp = 'AllChan ' + y + ' '
    writeToFile(tmp, 'Expected 50.0%:', output, fOutN)

    #mapping
    cmd = 'text2workspace.py ' + dataCard + text2ws_setting
    os.system(cmd)
    
    if not breakVV:
      cmd = 'combine -M Significance -m 90 --signif ' + dataCard.replace('.txt','.root') + ' --cminDefaultMinimizerStrategy 0 --redefineSignalPOI rVV --setParameters rVV=1 --verbose 1 -t -1'
      process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      print("Command running:", process.args)
      output, error = process.communicate()
      print(output.decode())
      tmp = 'AllChan ' + y + ' rVV '
      writeToFile(tmp, 'Significance:', output, fOutN)
      
    if breakVV:
      cmd = 'combine -M Significance -m 90 --signif ' + dataCard.replace('.txt','.root') + ' --cminDefaultMinimizerStrategy 0 --redefineSignalPOI rVVcc --setParameters rVVcc=1 --verbose 1 -t -1'
      process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      print("Command running:", process.args)
      output, error = process.communicate()
      print(output.decode())
      tmp = 'AllChan ' + y + ' rVVcc '
      writeToFile(tmp, 'Significance:', output, fOutN)
      cmd = 'combine -M Significance -m 90 --signif ' + dataCard.replace('.txt','.root') + ' --cminDefaultMinimizerStrategy 0 --redefineSignalPOI rVVbb --setParameters rVVbb=1 --verbose 1 -t -1'
      process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      print("Command running:", process.args)
      output, error = process.communicate()
      print(output.decode())
      tmp = 'AllChan ' + y + ' rVVbb '
      writeToFile(tmp, 'Significance:', output, fOutN)
      

    os.chdir(cDir)

#now combine all channels and years
dir = pDir+'/'+modelName+'/%s' % ('All')
os.system('rm -rf ' + dir)
os.system('mkdir -p ' + dir)

for y in years:
    for chan in channels:
      dirTmp = pDir+'/'+modelName+'/%s_%s'%(y,chan)
      cmd = 'cp ' + dirTmp + '/model_*.* ' + dir
      print(cmd)
      os.system(cmd)

os.chdir(dir)
print('Run: all')
#combine data card
dataCard = 'model_combined_all.txt'
os.system('combineCards.py model_combined_*_*.txt > ' + dataCard)

cmd = 'combine -M AsymptoticLimits --run blind ' + dataCard
process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
output, error = process.communicate()
print(output.decode())
tmp = 'AllChan AllYear '
writeToFile(tmp, 'Expected 50.0%:', output, fOutN)

#mapping
cmd = 'text2workspace.py ' + dataCard + text2ws_setting
os.system(cmd)

if not breakVV:
  print('Significance (VV): ')
  cmd = 'combine -M Significance -m 90 --signif ' + dataCard.replace('.txt','.root') + ' --cminDefaultMinimizerStrategy 0 --redefineSignalPOI rVV --setParameters rVV=1 --verbose 1 -t -1'
  process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  print("Command running:", process.args)
  output, error = process.communicate()
  print(output.decode())
  tmp = 'AllChan AllYear rVV '
  writeToFile(tmp, 'Significance:', output, fOutN)
  
if breakVV:
  print('Significance (VVcc): ')
  cmd = 'combine -M Significance -m 90 --signif ' + dataCard.replace('.txt','.root') + ' --cminDefaultMinimizerStrategy 0 --redefineSignalPOI rVVcc --setParameters rVVcc=1,rVVbb=1 --verbose 1 -t -1'
  process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  print("Command running:", process.args)
  output, error = process.communicate()
  print(output.decode())
  tmp = 'AllChan AllYear rVVcc '
  writeToFile(tmp, 'Significance:', output, fOutN)

  print('Significance (VVbb): ')
  cmd = 'combine -M Significance -m 90 --signif ' + dataCard.replace('.txt','.root') + ' --cminDefaultMinimizerStrategy 0 --redefineSignalPOI rVVbb --setParameters rVVbb=1,rVcc=1 --verbose 1 -t -1'
  process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  print("Command running:", process.args)
  output, error = process.communicate()
  print(output.decode())
  tmp = 'AllChan AllYear rVVbb '
  writeToFile(tmp, 'Significance:', output, fOutN)