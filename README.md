# VHcc Analysis
This is the re-worked VHcc analysis that reconfigures the code and github history so that we don't have an error I caused.

## Compile:
```
make FORMAT=sample_format INPUT=input_type
```
sample_format: MC_YEAR, DATA_YEAR (YEAR: 2016,2017,2018)
  
input_type: TCHAIN, TFILE (TCHAIN: uses TChain to open files, TFILE: uses TFile to open a file)\\
## Run locally
```
./main -filelist FileLists_test/JetHT_DATA_2016C_test.txt -out test.root -data 1 -year 2016 -lastentry 100
```
## Run on Condor
```
Use scripts inside SubmitToCondor to launch jobs to Condor
```
