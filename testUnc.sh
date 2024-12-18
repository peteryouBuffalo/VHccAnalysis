#!/bin/bash 
make FORMAT=MC_2016 SUBFORMAT=MC_2016 PROCESSING=NORMAL NANOAOD=NANOAODV9 INPUT=TCHAIN
#./main -filelist sampleList_testUnc.txt -out Tmp/output_testTAG_CCU.root -data 0 -year 2016 -syst TAG_CCU -centralGenWeight 0 
./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2016.txt -out Tmp/unc/output_testNONE.root -data 0 -year 2016 -syst NONE -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 | tee Tmp/unc/logs/log_NONE.txt 
./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2016.txt -out Tmp/unc/output_testNONE_JESU.root -data 0 -year 2016 -syst JESU -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 | tee Tmp/unc/logs/log_JESU.txt
./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2016.txt -out Tmp/unc/output_testNONE_JESD.root -data 0 -year 2016 -syst JESD -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 | tee Tmp/unc/logs/log_JESD.txt
#./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2016.txt -out Tmp/unc/output_testNONE_JERU.root -data 0 -year 2016 -syst JERU -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 | tee Tmp/unc/logs/log_JERU.txt
#./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2016.txt -out Tmp/unc/output_testNONE_JERD.root -data 0 -year 2016 -syst JERD -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 | tee Tmp/unc/logs/log_JERD.txt
#./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2016.txt -out Tmp/unc/output_testNONE_L1U.root -data 0 -year 2016 -syst L1PREFIRINGU -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 | tee Tmp/unc/logs/log_L1U.txt
#./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2016.txt -out Tmp/unc/output_testNONE_L1D.root -data 0 -year 2016 -syst L1PREFIRINGD -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 | tee Tmp/unc/logs/log_L1D.txt
#./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2016.txt -out Tmp/unc/output_testNONE_JETPUIDU.root -data 0 -year 2016 -syst JETPUIDU -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 | tee Tmp/unc/logs/log_JETPUIDU.txt
#./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2016.txt -out Tmp/unc/output_testNONE_JETPUIDD.root -data 0 -year 2016 -syst JETPUIDD -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 | tee Tmp/unc/logs/log_JETPUIDD.txt
#./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2016.txt -out Tmp/unc/output_testNONE_BTAGCU.root -data 0 -year 2016 -syst BTAGBCU -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 | tee Tmp/unc/logs/log_BTAGBCU.txt
#./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2016.txt -out Tmp/unc/output_testNONE_BTAGCD.root -data 0 -year 2016 -syst BTAGBCD -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 | tee Tmp/unc/logs/log_BTAGBCD.txt
#./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2016.txt -out Tmp/unc/output_testNONE_PUU.root -data 0 -year 2016 -syst PUU -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 | tee Tmp/unc/logs/log_PUU.txt
#./main -filelist FileLists/ZH_HToCC_ZToQQ_MC_2016.txt -out Tmp/unc/output_testNONE_PUD.root -data 0 -year 2016 -syst PUD -xccEffFileName CalibData/xcc_eff_VH_looseWP.root -centralGenWeight 0 | tee Tmp/unc/logs/log_PUD.txt

