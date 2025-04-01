echo "Run2 2016"
echo ">>> 2016B"
dasgoclient --query="run dataset=/JetHT/Run2016B-ver2_HIPM_UL2016_MiniAODv1_NanoAODv2-v1/NANOAOD" | tr '\n' ',' | sed 's/,$//' > runs/runs_2016B.txt
echo ">>> 2016C"
dasgoclient --query="run dataset=/JetHT/Run2016C-HIPM_UL2016_MiniAODv2_NanoAODv9-v2/NANOAOD" | tr '\n' ',' | sed 's/,$//' > runs/runs_2016C.txt
echo ">>> 2016D"
dasgoclient --query="run dataset=/JetHT/Run2016D-HIPM_UL2016_MiniAODv2_NanoAODv9-v2/NANOAOAOD" | tr '\n' ',' | sed 's/,$//' > runs/runs_2016B.txt
echo ">>> 2016E"
dasgoclient --query="run dataset=/JetHT/Run2016E-HIPM_UL2016_MiniAODv2_NanoAODv9-v2/NANOAOAOD" | tr '\n' ',' | sed 's/,$//' > runs/runs_2016E.txt
echo ">>> 2016F_HIPM"
dasgoclient --query="run dataset=/JetHT/Run2016F-HIPM_UL2016_MiniAODv2_NanoAODv9-v2/NANOAOAOD" | tr '\n' ',' | sed 's/,$//' > runs/runs_2016F_HIPM.txt
echo ">>> 2016F"
dasgoclient --query="run dataset=/JetHT/Run2016F-UL2016_MiniAODv2_NanoAODv9-v1/NANOAOD" | tr '\n' ',' | sed 's/,$//' > runs/runs_2016F.txt
echo ">>> 2016G"
dasgoclient --query="run dataset=/JetHT/Run2016G-UL2016_MiniAODv2_NanoAODv9-v1/NANOAOD" | tr '\n' ',' | sed 's/,$//' > runs/runs_2016G.txt
echo ">>> 2016H"
dasgoclient --query="run dataset=/JetHT/Run2016H-UL2016_MiniAODv2_NanoAODv9-v1/NANOAOD" | tr '\n' ',' | sed 's/,$//' > runs/runs_2016H.txt
echo ">>> All found for 2016. See files runs_2016*.txt."
echo "Run2 2017"
echo ">>> 2017B"
dasgoclient --query="run dataset=/JetHT/Run2017B-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | tr '\n' ',' | sed 's/,$//' > runs/runs_2017B.txt
echo ">>> 2017C"
dasgoclient --query="run dataset=/JetHT/Run2017C-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | tr '\n' ',' | sed 's/,$//' > runs/runs_2017C.txt
echo ">>> 2017D"
dasgoclient --query="run dataset=/JetHT/Run2017D-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | tr '\n' ',' | sed 's/,$//' > runs/runs_2017D.txt
echo ">>> 2017E"
dasgoclient --query="run dataset=/JetHT/Run2017E-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | tr '\n' ',' | sed 's/,$//' > runs/runs_2017E.txt
echo ">>> 2017F"
dasgoclient --query="run dataset=/JetHT/Run2017F-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD" | tr '\n' ',' | sed 's/,$//' > runs/runs_2017F.txt
echo ">>> All found for 2017. See files runs_2017*.txt."
echo "Run2 2018"
echo ">>> 2018A"
dasgoclient --query="run dataset=/JetHT/Run2018A-UL2018_MiniAODv2_NanoAODv9-v2/NANOAOD" | tr '\n' ',' | sed 's/,$//' > runs/runs_2018A.txt
echo ">>> 2018B"
dasgoclient --query="run dataset=/JetHT/Run2018B-UL2018_MiniAODv2_NanoAODv9-v1/NANOAOD" | tr '\n' ',' | sed 's/,$//' > runs/runs_2018B.txt
echo ">>> 2018C"
dasgoclient --query="run dataset=/JetHT/Run2018C-UL2018_MiniAODv2_NanoAODv9-v1/NANOAOD" | tr '\n' ',' | sed 's/,$//' > runs/runs_2018C.txt
echo ">>> 2018D"
dasgoclient --query="run dataset=/JetHT/Run2018D-UL2018_MiniAODv2_NanoAODv9-v2/NANOAOD" | tr '\n' ',' | sed 's/,$//' > runs/runs_2018D.txt
echo ">>> All found for 2018. See files runs_2018*.txt."
echo "All run numbers for Run2 found."
