#!/bin/bash 
make FORMAT=DATA_2016 SUBFORMAT=DATA_2016B SAMPLE=DATA_NOTVZ PROCESSING=NORMAL NANOAOD=NANOAODV9 INPUT=TCHAIN
./main -filelist FileLists_test/JetHT_DATA_2016B.txt -out Tmp/output_testNONE_DATA_2016B.txt.root -data 1 -year 2016PRE -syst NONE -xccEffFileName NONE -centralGenWeight 0 -lastentry 10000 
make FORMAT=DATA_2016 SUBFORMAT=DATA_2016F SAMPLE=DATA_NOTVZ PROCESSING=NORMAL NANOAOD=NANOAODV9 INPUT=TCHAIN
./main -filelist FileLists_test/JetHT_DATA_2016F.txt -out Tmp/output_testNONE_DATA_2016F.txt.root -data 1 -year 2016 -syst NONE -xccEffFileName NONE -centralGenWeight 0 -lastentry 10000 
make FORMAT=DATA_2017 SUBFORMAT=DATA_2017B SAMPLE=DATA_NOTVZ PROCESSING=NORMAL NANOAOD=NANOAODV9 INPUT=TCHAIN
./main -filelist FileLists_test/JetHT_DATA_2017B.txt -out Tmp/output_testNONE_DATA_2017B.txt.root -data 1 -year 2017 -syst NONE -xccEffFileName NONE -centralGenWeight 0 -lastentry 10000 
make FORMAT=DATA_2018 SUBFORMAT=DATA_2018B SAMPLE=DATA_NOTVZ PROCESSING=NORMAL NANOAOD=NANOAODV9 INPUT=TCHAIN
./main -filelist FileLists_test/JetHT_DATA_2018B.txt -out Tmp/output_testNONE_DATA_2018B.txt.root -data 1 -year 2018 -syst NONE -xccEffFileName NONE -centralGenWeight 0 -lastentry 10000 
