#!/bin/bash 

#make FORMAT=MC_2016 SUBFORMAT=MC_2016 SAMPLE=MC_NOTVZ PROCESSING=NORMAL NANOAOD=NANOAODV9 INPUT=TCHAIN
#./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2016.txt -out Tmp/output_testNONE_ZH_HToCC_ZToQQ_MC_2016.root -data 0 -year 2016 -syst NONE -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 -lastentry 1000 
#make FORMAT=MC_2016PRE SUBFORMAT=MC_2016PRE SAMPLE=MC_NOTVZ PROCESSING=NORMAL NANOAOD=NANOAODV9 INPUT=TCHAIN
#./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2016_preVFP.txt -out Tmp/output_testNONE_ZH_HToCC_ZToQQ_MC_2016_preVFP.root -data 0 -year 2016PRE -syst NONE -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 -lastentry 10000 
#make FORMAT=MC_2017 SUBFORMAT=MC_2017 SAMPLE=MC_NOTVZ PROCESSING=NORMAL NANOAOD=NANOAODV9 INPUT=TCHAIN
#./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2017.txt -out Tmp/output_testNONE_ZH_HToCC_ZToQQ_MC_2017.root -data 0 -year 2017 -syst NONE -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 -lastentry 10000 
make FORMAT=MC_2018 SUBFORMAT=MC_2018 SAMPLE=MC_NOTVZ PROCESSING=NORMAL NANOAOD=NANOAODV9 INPUT=TCHAIN
./main -filelist FileLists/QCD_HT1500to2000_v9_MC_2018.txt -out Tmp/output_testNONE_QCD_HT1500to2000_v9_MC_2018.root -data 0 -year 2018 -syst NONE -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 -lastentry 100000
#./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2018.txt -out Tmp/output_testNONE_ZH_HToCC_ZToQQ_MC_2018.root -data 0 -year 2018 -syst NONE -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 -lastentry 10000 
#./main -filelist FileLists/ZJetsToQQ_HT-400to600_MC_2018.txt -out Tmp/output_testNONE_ZJetsToQQ_MC_2018.root -data 0 -year 2018 -syst NONE -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 -lastentry 10000

