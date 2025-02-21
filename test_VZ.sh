#!/bin/bash 

#make FORMAT=MC_2016 SUBFORMAT=MC_2016 SAMPLE=MC_VZ PROCESSING=NORMAL NANOAOD=NANOAODV9 INPUT=TCHAIN
#./main -filelist FileLists_test/WZTo4Q_NLO_MC_2016.txt -out Tmp/output_WZTo4Q_NLO_MC_2016.root -data 0 -year 2016 -syst NONE -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 -lastentry 10000 
make FORMAT=MC_2016 SUBFORMAT=MC_2016 SAMPLE=MC_VV_LO PROCESSING=NORMAL NANOAOD=NANOAODV9 INPUT=TCHAIN
./main -filelist FileLists_test/WZ_MC_2016.txt -out Tmp/output_testNONE_WZ_MC_2016.root -data 0 -year 2016 -syst NONE -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 -lastentry 10000 
#./main -filelist FileLists_test/ZZ_MC_2016.txt -out Tmp/output_testNONE_ZZ_MC_2016.root -data 0 -year 2016 -syst NONE -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 -lastentry 10000 
#./main -filelist FileLists_test/ZH_HToCC_ZToQQ_MC_2016.txt -out Tmp/output_testNONE_ZH_HToCC_ZToQQ_MC_2016.root -data 0 -year 2016 -syst NONE -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 -lastentry 10000 
