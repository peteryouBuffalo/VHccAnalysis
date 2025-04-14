# This script was written using ChatGPT by feeding it launch_v2.py (which was 
# written with Python2.7 in mind) and telling it to rewrite the script using
# Python3. NOTE: This did not work fully well and I have to re-add a lot at
# the end by hand.
#
# created/revised: Peter Young, May 8, 2024

# /////////////////////////////////////////////////////////////////////////////
# Imported Libraries
# /////////////////////////////////////////////////////////////////////////////
import os
import sys
import json
import time

# /////////////////////////////////////////////////////////////////////////////
# Defined Functions
# /////////////////////////////////////////////////////////////////////////////

# Write the condor configuration that we need to run the jobs.
def write_condor_config(workDir, sample_format, sample_subformat, VZsample, nanoaod_format, 
  data_name, name_output_dir, nJob, syst='none', centralGenWeight=0, debug=False):
    
  exe_fileName = 'exe_' + data_name + '.sh'
  if syst != 'none':
    exe_fileName = 'exe_' + syst + '_' + data_name + '.sh'
  
  f = open(workDir + '/condor_config.script', 'w')
  f.write('universe = vanilla \n')
  f.write('Executable = '+exe_fileName+' \n')
  f.write('Arguments = $(Process) \n') 
  f.write('Should_Transfer_Files = YES \n')
  f.write('WhenToTransferOutput = ON_EXIT \n')
  f.write('Transfer_Input_Files = input.tar \n')
  f.write('Output = ctagana_$(Cluster)_$(Process).stdout \n')
  f.write('Error = ctagana_$(Cluster)_$(Process).stderr \n')
  f.write('Log = ctagana_$(Cluster)_$(Process).log \n')
  f.write('request_memory = 4100 \n')
  f.write('notify_user = ${LOGNAME}@FNAL.GOV \n')
  f.write('+LENGTH="SHORT" \n')
  #f.write('x509userproxy = /tmp/x509up_u12772 \n')
  f.write('x509userproxy = $ENV(X509_USER_PROXY) \n')
  tmp = 'Queue ' + str(nJob) + '\n'
  f.write(tmp)
  f.close()
  
  f = open(workDir + '/' + exe_fileName, 'w')
  f.write('#!/bin/bash \n')
  f.write('echo $SHELL \n')
  f.write('source /cvmfs/cms.cern.ch/cmsset_default.sh \n')
  f.write('export SCRAM_ARCH=el9_amd64_gcc12 \n')
  f.write('export X509_CERT_DIR=/cvmfs/grid.cern.ch/etc/grid-security/certificates/ \n')
  f.write('eval `scramv1 project CMSSW CMSSW_14_0_6` \n')
  f.write('cd CMSSW_14_0_6/src/ \n')
  f.write('eval `scramv1 runtime -sh` \n') # cmsenv is an alias not on the workers
  f.write('echo "CMSSW: "$CMSSW_BASE \n')
  f.write('cd ${_CONDOR_SCRATCH_DIR} \n')
  f.write('tar -xvf input.tar \n')
  f.write('make FORMAT='+sample_format+ ' SUBFORMAT=' + sample_subformat + ' SAMPLE=' + VZsample + ' PROCESSING=NORMAL NANOAOD='+nanoaod_format+' INPUT=TCHAIN'+'\n')
  
  sampleType = '0'
  if 'DATA' in sample_format:
    sampleType = '1'
  year = sample_format.split('_')[1]
  if debug:
    f.write('./main -filelist sampleList_$1.txt -out output_$1.root -data ' + sampleType + ' -year ' + year + ' -syst ' + syst+ ' -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight ' + str(centralGenWeight) + ' -lastentry 100000 \n')
  else:
    f.write('./main -filelist sampleList_$1.txt -out output_$1.root -data ' + sampleType + ' -year ' + year + ' -syst ' + syst+ ' -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight ' + str(centralGenWeight) + ' \n')
  f.write('xrdcp *.root root://cmseos.fnal.gov/' + name_output_dir+ '/ \n')
  f.write('rm *.root \n')
  f.write('echo "xrdcp *.root root://cmseos.fnal.gov/' + name_output_dir + '/" \n')
  
  f.close()
  
# Make the input file list
def make_input_file_list(nFile, outDir_file_list, file_list_name):

  lines = open(file_list_name).readlines()
  nJob = len(lines) // nFile + 1
  if len(lines) % nFile == 0:
    nJob = len(lines) // nFile
  
  iFile = 0
  for line in range(0, len(lines), nFile):
    tmp = file_list_name.split('/')
    #newFile = open(outDir_file_list + '/' + tmp[len(tmp)-1] + '_' + str(iFile) + '.txt', 'w')  
    newFile = open(outDir_file_list + '/sampleList_' + str(iFile) + '.txt', 'w')  
    
    tmpFiles = lines[line:line+nFile]
    for i in range(0, len(tmpFiles)):
      newFile.write(tmpFiles[i])
    
    iFile = iFile + 1
  
  return nJob


# /////////////////////////////////////////////////////////////////////////////
# Settings
# /////////////////////////////////////////////////////////////////////////////
runMode = 1     # 0 : submit, 1 : check output and hadd output file
submit = True   # for testing setup or executing submission
debug = False   # just run on 10,000 (UPDATE: now 100,000)
haddData = True # use to combine DATA runs back together

# I need to hadd for PUD, PUU, JESU, JESD, NONE
syst = 'NONE'   # NONE, PUU, PUD
if len(sys.argv) > 1:
  syst = sys.argv[1]
centralGenWeight = 0

# Paths, Locations (CHANGE THESE)
sourceDir = '/uscms_data/d3/peteryou/boosted_new/CMSSW_14_0_6/src/VHccAnalysis/'
condorRunDir = '/uscmst1b_scratch/lpc1/lpcphys/peteryou/Output_VHcc/2025Apr_test2/'
outputDir_eos = '/store/user/peteryou/Output_VHcc/2025Apr_test2/' + syst + '/'
outputDir_scratch = '/uscms_data/d3/peteryou/boosted_new/CMSSW_14_0_6/src/VHccAnalysis/condor_results/2025Apr_test2/' + syst + '/'

# Input data sets
dataSet_list = sourceDir+"/Dataset_lists/datasets_NANOAODv9_MC_2018.txt"
#dataSet_list = sourceDir+"/Dataset_lists/datasets_ttbar_2018.txt"
#dataSet_list = sourceDir+"/Dataset_lists/datasets_missing_WJ_2018.txt"
#dataSet_list = sourceDir+"/Dataset_lists/datasets_VV_NLO_2018.txt"
#dataSet_list = sourceDir+"/Dataset_lists/datasets_QCDv9_MC.txt"
#dataSet_list = sourceDir+"/Dataset_lists/datasets_dom_bckg_MC.txt"
#dataSet_list = sourceDir+"/Dataset_lists/datasets_ggZH_MC.txt"
#dataSet_list = sourceDir+"/Dataset_lists/datasets_WJetsToQQ.txt"
#dataSet_list = sourceDir+"/Dataset_lists/datasets_Data_combined.txt"
dataSet_list = sourceDir+"/Dataset_lists/datasets_JetHT.txt"
dataSet_lists = [sourceDir+"/Dataset_lists/datasets_NANOAODv9_MC_2018.txt"]
#dataSet_lists = [sourceDir+"/Dataset_lists/datasets_ttbar_2018.txt"]
#dataSet_lists = [sourceDir+"/Dataset_lists/datasets_missing_WJ_2018.txt"]
#dataSet_lists = [sourceDir+"/Dataset_lists/datasets_VV_NLO_2018.txt"]
#dataSet_lists = [sourceDir+"/Dataset_lists/datasets_QCDv9_MC.txt"]
#dataSet_lists = [sourceDir+"/Dataset_lists/datasets_dom_bckg_MC.txt"]
#dataSet_lists = [sourceDir+"/Dataset_lists/datasets_ggZH_MC.txt"]
#dataSet_lists = [sourceDir+"/Dataset_lists/datasets_WJetsToQQ.txt"]
#dataSet_lists = [sourceDir+"/Dataset_lists/datasets_Data_combined.txt"]
dataSet_lists = [sourceDir+"/Dataset_lists/datasets_JetHT.txt"]

nFile = 2
dir_file_list = sourceDir+'/FileLists/'
dir_file_list = sourceDir+'/FileLists_JetHT/'

# Print settings
print('=============================')
print('Run mode: ', runMode)
print('Submit:   ', submit)
print('Debug:    ', debug)
print('haddData: ', haddData)

print('Systematic:                                                 ', syst)
print('Sample list file (list of input samples):                   ')#, dataSet_list)
for dS_list in dataSet_lists:
  print('      ', dS_list)
print('Sample list folder (location of input file lists):          ', dir_file_list)
print('Number of file per jobs:                                    ', nFile)
print('Source dir:                                                 ', sourceDir)
print('Condor run dir:                                             ', condorRunDir)
print('Output location eos:                                        ', outputDir_eos)
print('Output location scratch:                                    ', outputDir_scratch)

# Wait for user input
check_for_user_input = False

if check_for_user_input:
  user_input = input("Press 'P' to proceed or 'E' to exit: ")
  
  if user_input.upper() == 'P':
    print("Continuing with the program.")
  elif user_input.upper() == 'E':
    print("Exiting the program.")
    exit()
  else:
    print("Invalid input. Please try again")

# /////////////////////////////////////////////////////////////////////////////
# Main Code (DO NOT EDIT BELOW HERE)
# /////////////////////////////////////////////////////////////////////////////
samples_input = []
if len(sys.argv) > 2:
  samples_input = sys.argv[2].split(',')

# Add all samples together
lines = []
for dS_list in dataSet_lists:
  json_file = open(dataSet_list)
  samples = json.load(json_file)
  lines = lines + list(samples.keys())

#for i in range(len(lines)):
#  print(i, ": ", lines[i])

sample_format = ''
nanoaod_format= ''

dir_affix = 'test'

for line in lines:
  if len(samples_input) > 0 and line not in samples_input:
    continue
  print('=========================')
  print('Processing sample: ', line)
  
  data_name = line
  work_dir = condorRunDir+'/condor_run/' + data_name + '_' + dir_affix
  if syst != 'none':
    work_dir = condorRunDir+'/condor_run_'+syst+'/' + data_name + '_' + dir_affix

  if 'preVFP' in line:
    sample_subformat = data_name.split('_')[-3] + '_' + data_name.split('_')[-2]
  else:
    sample_subformat = data_name.split('_')[-2] + '_' + data_name.split('_')[-1]
  
  if 'DATA' in sample_subformat:
    sample_format = sample_subformat[:-1]
    preVFP = ['DATA_2016B', 'DATA_2016C', 'DATA_2016D', 'DATA_2016E']
    if sample_subformat in preVFP or ('HIPM_DATA_2016F' in line):
      sample_format = sample_format + "PRE"
  else:
    if 'preVFP' in line:
      sample_subformat = sample_subformat + 'PRE'
    sample_format = sample_subformat
  
  #used to turn on final state break down for VZ process like qqcc, qccc, etc.
  VZsample = "MC_NOTVZ"
  if ('ZZ' in line or 'WZ' in line):
    if ('NLO' in line): VZsample = "MC_VZ"
    else: VZsample = "MC_VV_LO"
  if ('WW' in line) and not ('NLO' in line):
      VZsample = "MC_VV_LO"
    
  print(sample_format, sample_subformat)

  nanoaod_format='NANOAODV9'
  dir_final_rootFile = outputDir_eos + '/' + data_name

##########################################################
  if runMode == 0:
    if 'sherpa' in line:
      centralGenWeight = 124598120.
    else:
      centralGenWeight = 0
    
    os.system('eos root://cmseos.fnal.gov rm -r ' + dir_final_rootFile)
    os.system('eos root://cmseos.fnal.gov mkdir -p ' + dir_final_rootFile)

    os.system('mkdir -p ' + work_dir)
    os.system('rm -r ' + work_dir + '/*')
    
    os.chdir(work_dir)
    
    file_list_name = dir_file_list + '/' + data_name + '.txt' 
    print('>>>>>>> Use this file list: ', file_list_name)
 
    # break list of files to a set of input file list and estimate total jobs, njob
    nJob = make_input_file_list(nFile, work_dir, file_list_name)
    
    # prepare condor job configuration
    write_condor_config(work_dir, sample_format, sample_subformat, VZsample, nanoaod_format, data_name, dir_final_rootFile, nJob, syst, centralGenWeight, debug)
    
    # copy codes, ....
    os.system('cp '+sourceDir+'/Makefile ' + work_dir)
    os.system('cp '+sourceDir+'/Ana.cxx ' + work_dir)
    os.system('cp '+sourceDir+'/StdArg.hpp ' + work_dir)
    os.system('cp -r '+sourceDir+'/src/ ' + work_dir)
    os.system('cp -r '+sourceDir+'/Configs/ ' + work_dir)
    os.system('cp -r '+sourceDir+'/CalibData/ ' + work_dir)
    os.system('cp -r '+sourceDir+'/yaml-cpp/ ' + work_dir)
    time.sleep(1)
    os.system('tar -cf input.tar Makefile *.cxx *.hpp src/ Configs/ CalibData/ yaml-cpp/ sampleList_*.txt')
    
    # submit jobs
    if submit: 
        os.system('condor_submit condor_config.script')
    
##########################################################
  if runMode == 1:
    
    os.chdir(work_dir)
    #calculate how many jobs submitted
    os.system('ls sampleList_*.txt >| tmp.txt')
    time.sleep(1)
    lines_tmp = open('tmp.txt', 'r').readlines()
    nJob = len(lines_tmp)
    #get list of output root files
    nameTmp = 'outFileListTmp.txt'
    cmd_tmp = 'xrdfs root://cmseos.fnal.gov/ ls -u ' + dir_final_rootFile + '/ | grep ".root" >| ' + nameTmp
    os.system(cmd_tmp)
    ############check total number of root file###################
    lines_tmp = open(nameTmp).readlines()
    nJob_current = len(lines_tmp)
    if nJob_current != nJob:
      print('============================================')
      print('Warning: MISSING OUTPUT ROOT FILES, found ', nJob_current, ' files but ', nJob, ' files expected')
      print(work_dir)

    #############hadd output root files###########
    os.system('mkdir ' + outputDir_scratch)
    os.system('rm ' + outputDir_scratch + '/' + data_name + '.root')
    cmd_hadd = 'hadd -f -k ' + outputDir_scratch + '/' + data_name + '.root @' + nameTmp 
    print(cmd_hadd)
    os.system(cmd_hadd)

# now hadd data
#if haddData:
#  print("haddData = True")
#  print(">>> Checking that we're in hAdd mode... (runMode == 1)")
#  if runMode == 1:
#    print(">>> Going to hadd...")
#    yrRuns = {
#      '2016': ['B','C','D','E','F','G','H'],
#      '2017': ['C','D','E','F'],
#      '2018': ['B','C','D']
#    }
#    for y in ['2016', '2017', '2018']:
#      data_name = "SingleMuon_DATA_" + y
#      os.system('rm ' + outputDir_scratch + '/' + data_name + '.root')
#      for l in yrRuns[y]:
#        cmd_hadd = 'hadd -f -k ' + outputDir_scratch + '/' + data_name + '.root ' + outputDir_scrat#ch + '/' + data_name + l + '.root'
#      print(cmd_hadd)
#      os.system(cmd_hadd) 

#//EOF/////////////////////////////////////////////////////////////////////////
