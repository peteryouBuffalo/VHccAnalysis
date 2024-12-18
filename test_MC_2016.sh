#!/bin/bash 

make FORMAT=MC_2016 SUBFORMAT=MC_2016 PROCESSING=NORMAL NANOAOD=NANOAODV9 INPUT=TCHAIN
./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2016.txt -out Tmp/output_testNONE_ZH_HToCC_ZToQQ_MC_2016.root -data 0 -year 2016 -syst NONE -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 -lastentry 10000 
#make FORMAT=MC_2016PRE SUBFORMAT=MC_2016PRE PROCESSING=NORMAL NANOAOD=NANOAODV9 INPUT=TCHAIN
#./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2016_preVFP.txt -out Tmp/output_testNONE_ZH_HToCC_ZToQQ_MC_2016_preVFP.root -data 0 -year 2016PRE -syst NONE -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 -lastentry 10000 
#make FORMAT=MC_2017 SUBFORMAT=MC_2017 PROCESSING=NORMAL NANOAOD=NANOAODV9 INPUT=TCHAIN
#./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2017.txt -out Tmp/output_testNONE_ZH_HToCC_ZToQQ_MC_2017.root -data 0 -year 2017 -syst NONE -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 -lastentry 10000 
#make FORMAT=MC_2018 SUBFORMAT=MC_2018 PROCESSING=NORMAL NANOAOD=NANOAODV9 INPUT=TCHAIN
#./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2018.txt -out Tmp/output_testNONE_ZH_HToCC_ZToQQ_MC_2018.root -data 0 -year 2018 -syst NONE -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 -lastentry 10000 
