import os, sys
import ROOT

## == Necessary variables =====================================================

## Data types we want to search
datatypes = [ "DATA_2016", "DATA_2017", "DATA_2017F", "DATA_2018",
              "MC_2016",   "MC_2017",   "MC_2018"]

## Variables for each year
files = {
  "DATA_2016": "root:://cmsxrootd.fnal.gov///store/data/Run2016B/JetHT/NANOAOD/ver2_HIPM_UL2016_MiniAODv2_NanoAODv9-v2/2520000/E8D052D8-0446-A046-B45F-2882BF34D4DB.root",

  "DATA_2017": "root:://cmsxrootd.fnal.gov///store/data/Run2017B/JetHT/NANOAOD/UL2017_MiniAODv2_NanoAODv9-v1/120000/62E9EA6D-8AD2-CC42-B2FD-2817FECF47F0.root",

  "DATA_2017F": "root:://cmsxrootd.fnal.gov///store/data/Run2017F/JetHT/NANOAOD/UL2017_MiniAODv2_NanoAODv9-v1/280000/8B4ECDF4-3A63-574B-9663-729A79E9CB2A.root",

  "DATA_2018": "root:://cmsxrootd.fnal.gov///store/data/Run2018B/JetHT/NANOAOD/UL2018_MiniAODv2_NanoAODv9-v1/120000/A4258CED-A995-AC4E-B50C-50CD7FCA4A94.root",

  "MC_2016": "root:://cmsxrootd.fnal.gov///store/mc/RunIISummer16NanoAODv7/ZH_HToBB_ZToQQ_M125_13TeV_powheg_pythia8/NANOAODSIM/PUMoriond17_Nano02Apr2020_102X_mcRun2_asymptotic_v8-v1/270000/480B1409-A76A-1C48-B8D1-A19616ACBAA2.root",

  "MC_2017": "root:://cmsxrootd.fnal.gov///store/mc/RunIIFall17NanoAODv7/ZH_HToBB_ZToQQ_M125_13TeV_powheg_pythia8/NANOAODSIM/PU2017_12Apr2018_Nano02Apr2020_102X_mc2017_realistic_v8-v1/100000/AB56D787-B4DC-5D45-BA4E-FC2DDB3E5B61.root",

  "MC_2018": "root:://cmsxrootd.fnal.gov///store/mc/RunIIAutumn18NanoAODv7/ZH_HToBB_ZToQQ_M125_13TeV_powheg_pythia8/NANOAODSIM/Nano02Apr2020_102X_upgrade2018_realistic_v21-v1/60000/19A53285-E3F2-344D-8939-EBE534E2C222.root",
}

## Directories
outputdir = "../VariableLists/Tmp"

## Debug options
debug = True

## == Main Code ===============================================================

## Header
print "======================================"
print "NOTE: Remember to make sure your grid"
print "certificate is valid before running  "
print "this script..."
print "======================================\n"

## For each file listed above...
for datatype in datatypes:

	## Output to let us know which file we're on
        print "Retrieving variable list for:", datatype
	filename = files[datatype]
	print "file:", filename
	
	## Retrieve the variables for this file.
	f = ROOT.TFile.Open(filename)
        tree = f.Get("Events")
	names = [b.GetName() for b in tree.GetListOfBranches()]
        nBranches = len(names)
	
	## Generate the selector so we have the code.
	tree.MakeSelector(outputdir + "/" + datatype)
	print "Created TSelector for ", datatype
	print ">>> ", outputdir + datatype + ".C"
	print ">>> ", outputdir + datatype + ".h"

	## Write these to a text file in the proper location
        outputfile = outputdir + datatype + ".txt"
	print nBranches, " variables found."
        print "Variables retrieved:", outputfile

	with open(outputfile, "w") as f:
		for i in range(nBranches):
			f.write(names[i] + "\n")
	f.close()	

	## End statements to let them know this file is completed.
	print "\n============================================================="

## Now, let them know where the files are located.
print "Variable lists created in ", outputdir
