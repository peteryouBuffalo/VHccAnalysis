#include <fstream>
#include <iostream>
//#include <boost/algorithm/string.hpp>

#include "StdArg.hpp"
#include "src/Reader.h" 
#include "src/Processor.h"
#include "src/Selector.h"
#include "src/VbbHcc_selector.h"
#include "src/VbbHcc_selector_unc.h"
#include "src/VbbHcc_triggerSel.h"
#include "src/Efficiency_selector.h"

#include "src/Global.h"

#include "correction.h"

std::vector<std::string> splitNames(const std::string& files, std::string sep = ",")
{
  std::vector<std::string> fileList;
  for (size_t i=0,n; i <= files.length(); i=n+1)
  {
    n = files.find_first_of(sep,i);
    if (n == string::npos)
      n = files.length();
    std::string tmp = files.substr(i,n-i);
    std::string ttmp;
    for(unsigned int j=0; j<tmp.size(); j++)
    {
      if(tmp[j]==' ' || tmp[j]=='\n') continue;
      ttmp+=tmp[j];
    }
    fileList.push_back(ttmp);
  }
  return fileList;

}

void SetParameters(std::string fName, glob::Parameters& para) {
  std::string line;
  std::ifstream myfile (fName);
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
      
      //skip comment line start with "//"
      if (line.find("//") != std::string::npos) continue ;
      
      std::vector<std::string> cont ;
      std::vector<std::string> cont_no_space ;
      //std::string delim(" ") ;
      
      //boost::split(cont, line, boost::is_any_of(delim));
      std::stringstream ss(line);
      std::string token;
      char delimiter = ' ';
      while (getline(ss, token, delimiter)) {
        cont.push_back(token);
      }
      
      for (size_t i = 0 ; i < cont.size() ; ++i) {
        //std::cout << "\n" << cont[i] << cont[i].find(" ") << " " << std::string::npos;
        if (cont[i].find_first_not_of(' ') != std::string::npos) {
          cont_no_space.push_back(cont[i]) ;
        }
      }

      if (cont_no_space.size() != 3) {
        std::cout << "\n Need name, value, value type. Will skip this \"" << line << "\"" << std::endl ;
      }
      else {
        std::string name = cont_no_space[0] ;
        if (cont_no_space[2] == "int") para.Set(name, std::stoi(cont[1])) ;
        if (cont_no_space[2] == "float") para.Set(name, std::stof(cont_no_space[1])) ;
        if (cont_no_space[2] == "string") para.SetStr(name, cont_no_space[1]) ;
      }
    }

    myfile.close();
  }
  
  else cout << "Unable to open file"; 

}



int main(int argc, char *argv[]) {
  
  StdArg arg(argc,argv);
  std::cout << "======================================================================" << std::endl;
  std::cout << "ZH analysis call:" << std::endl;
  for (int i=0;i<argc;i++) std::cout << argv[i] << " ";
  std::cout << endl;
  std::cout << "======================================================================" << std::endl;
  arg.keys << "-in" << "-filelist" << "-cfg" << "-out" << "-data" << "-year" << "-xccEffFileName" << "-syst" << "-centralGenWeight" 
           << "-firstentry" << "-lastentry" ; 
  arg.Process();

  std::vector<std::string> filenames;
  if ( arg.Key("-in")) 
    { 
      string inputfilenames;
      inputfilenames  = arg.Get<string>("-in"); 
      filenames = splitNames(inputfilenames);
    }
  else 
    { 
      string inputfilename;
      if (arg.Key("-filelist"))
	{
	  inputfilename  = arg.Get<string>("-filelist");
	  std::ifstream fList(inputfilename.c_str());
	  if (!fList){
	    cerr << ">>Can't open file " << inputfilename << endl;
	    return 1;
	  }
	  char lineFromFile[10000];
	  while (fList.getline(lineFromFile,10000)){
	    if ( strlen(lineFromFile)>0 ) filenames.push_back(string(lineFromFile));
	  }
	}
    }

  std::string cfgfilename =  "Configs/inputParameters.txt";
  if (arg.Key("-cfg")) cfgfilename = arg.Get<string>("-cfg");
  else std::cout << "\nUsing default configuration " << cfgfilename << std::endl; 
  
  std::string outputfilename =  "output.root";
  if (arg.Key("-out")) outputfilename = arg.Get<string>("-out");
  
  bool isData(false) ;
  
  int intisdata=0;
  int intfirstentry=0;
  long int intlastentry=-1;
  string syst = "NONE";
  string year = "";
  double centralGenWeight = 0;

  if (arg.Key("-data")) intisdata = arg.Get<int>("-data");
  if (arg.Key("-year")) year = arg.Get<string>("-year");
  if (arg.Key("-centralGenWeight")) centralGenWeight = arg.Get<double>("-centralGenWeight");
  if (arg.Key("-firstentry")) intfirstentry = arg.Get<int>("-firstentry");
  if (arg.Key("-lastentry")) intlastentry = arg.Get<int>("-lastentry");
  if (arg.Key("-syst")) syst = arg.Get<string>("-syst");

  std::string xccEffFileName = "";
  if (arg.Key("-xccEffFileName")) xccEffFileName = arg.Get<string>("-xccEffFileName");

  if(intisdata!=0) isData=true;
  
  std::cout << "\n=================================" << std::endl ;
  std::cout << "\nIs data:              " << isData ;
  std::cout << "\nYear:                 " << year ;
  std::cout << "\nCentral gen weight    " << centralGenWeight;
  std::cout << "\nFirst and last entry: " << intfirstentry << " " << intlastentry ;
  std::cout << "\nSystematic:           " << syst ;
  
  std::cout << std::endl ;

  SetParameters(cfgfilename,CUTS) ;

#if defined(TFILE)
  TFile* f = TFile::Open(filenames[0].c_str());
  TTree* chain = (TTree*)f->Get("Events");
#endif
#if defined(TCHAIN)
  std::string chainName("Events") ;
  TChain* chain = new TChain(chainName.c_str()) ;
  for ( std::vector<std::string>::const_iterator it = filenames.begin();it != filenames.end(); it++) {
    cout << "reading file " << it->c_str() << endl;
    int retcode = chain->Add(it->c_str(),-1);
    if ( retcode == 0 ) throw std::invalid_argument("the file "+*it+" does not exist of does not contain the tree named "+chainName);
  }
#endif 
  
  std::cout << "\n Number of events: " << chain->GetEntries() ;
  if (intlastentry == -1) intlastentry = chain->GetEntries() ;
  
  Processor ana ;
  ana.setOutPutFileName(outputfilename) ;
  ana.SetDataInfo(isData,year) ;
  
  //collection of all selectors
  std::vector<Selector*> sels;
  
  //Selection for VH 
  //VH_selection sel ;

  VbbHcc_selector VbbHcc_sel ;
  //VbbHcc_selector_unc VbbHcc_sel_unc ;
  
  VbbHcc_sel.SetDataInfo(isData,year); //need to unify SetDataInfo for ana and this
  //VbbHcc_sel_unc.SetDataInfo(isData,year); //need to unify SetDataInfo for ana and this
  //VbbHcc_triggerSel VbbHcc_triggerSel;

  Efficiency_selector Eff_sel ;
  std::string tmp = year;
  if (year == "2016PRE") tmp = "2016preVFP";
  std::string fName_btagSF("CalibData/btagging_"+tmp+"_UL.json.gz");
  std::string fName_btagEff("CalibData/effM.root");

  std::string fName_puSF;
  std::string fName_PUjetID_SF;
  std::string fName_PUjetID_eff;
  std::string fName_xbb_xcc_sf("CalibData/xcc_qcd_weights_looseWP.root"); //to calculate the weight for QCD xcc xbb
  std::string fName_xbb_xcc_eff(xccEffFileName); //to calculate the weight for QCD xcc xbb
  std::string fName_triggerSF("CalibData/trigger_turn_on.root"); //to calculate the weight for QCD xcc xbb

  if (year == "2016PRE") tmp = "2016preVFP";
  if (year == "2016") tmp = "2016postVFP";
  std::string fName_jec("CalibData/jme/"+tmp+"_UL/fatJet_jerc.json.gz");

  std::string tmp2 = year;
  if (year == "2016PRE") tmp2 = "2016";
  std::string fName_msd("CalibData/jme/"+tmp+"_UL/msdcorr_" + year + ".json"); 
  
  //Syst
  //if (syst == "L1PREFIRINGU") sel.SetL1prefiring("l1prefiringu");
  //if (syst == "L1PREFIRINGD") sel.SetL1prefiring("l1prefiringd");
  if (syst == "L1PREFIRINGU") {
    VbbHcc_sel.SetL1prefiring("l1prefiringu");
    //VbbHcc_sel_unc.SetL1prefiring("l1prefiringu");
  }
  if (syst == "L1PREFIRINGD") {
    VbbHcc_sel.SetL1prefiring("l1prefiringd");
    //VbbHcc_sel_unc.SetL1prefiring("l1prefiringd");
  }
  std::string jetPUidUncType = "central";
  if (syst == "JETPUIDU") jetPUidUncType = "up";
  if (syst == "JETPUIDD") jetPUidUncType = "down";
  std::string btagUncType = "central";
  if (syst == "BTAGU") btagUncType = "up";
  if (syst == "BTAGD") btagUncType = "down";
  if (syst == "BTAGLU") btagUncType = "light_up";
  if (syst == "BTAGLD") btagUncType = "light_down";
  if (syst == "BTAGBCU") btagUncType = "bc_up";
  if (syst == "BTAGBCD") btagUncType = "bc_down";

#if defined(MC_2016PRE) || defined(MC_2016) 
  fName_puSF = "CalibData/2016_pileup_ratio.root";
  if (syst == "PUU") fName_puSF = "CalibData/2016_pileup_ratio_up.root";
  if (syst == "PUD") fName_puSF = "CalibData/2016_pileup_ratio_down.root";
#endif
#ifdef MC_2017
  fName_puSF = "CalibData/2017_pileup_ratio.root";
  if (syst == "PUU") fName_puSF = "CalibData/2017_pileup_ratio_up.root";
  if (syst == "PUD") fName_puSF = "CalibData/2017_pileup_ratio_down.root";
#endif

#ifdef MC_2018
  fName_puSF = "CalibData/2018_pileup_ratio.root";
  if (syst == "PUU") fName_puSF = "CalibData/2018_pileup_ratio_up.root";
  if (syst == "PUD") fName_puSF = "CalibData/2018_pileup_ratio_down.root";
#endif
  VbbHcc_sel.SetJetMetSyst("central");
  //VbbHcc_sel_unc.SetJetMetSyst("central");
  if (syst == "JESU") VbbHcc_sel.SetJetMetSyst("jesu");
  if (syst == "JESD") VbbHcc_sel.SetJetMetSyst("jesd");
  if (syst == "JERU") VbbHcc_sel.SetJetMetSyst("jeru");
  if (syst == "JERD") VbbHcc_sel.SetJetMetSyst("jerd");
  VbbHcc_sel.SetHFtagUncType("central");
  //VbbHcc_sel_unc.SetHFtagUncType("central");
  //if (syst == "TAG_BBU") VbbHcc_sel_unc.SetHFtagUncType("bbup");
  //if (syst == "TAG_BBD") VbbHcc_sel_unc.SetHFtagUncType("bbdown");
  //if (syst == "TAG_CCU") VbbHcc_sel_unc.SetHFtagUncType("ccup");
  //if (syst == "TAG_CCD") VbbHcc_sel_unc.SetHFtagUncType("ccdown");

#if defined(DATA_2016PRE) || defined(DATA_2016)
  //std::string fName_lumiMaskFilter("CalibData/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt");
  std::string fName_lumiMaskFilter("CalibData/Cert_271036-284044_13TeV_Legacy2016_Collisions16_JSON.txt");
#endif
#if defined(DATA_2017)
  //std::string fName_lumiMaskFilter("CalibData/Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON.txt");
  std::string fName_lumiMaskFilter("CalibData/Cert_294927-306462_13TeV_UL2017_Collisions17_GoldenJSON.txt");
#endif
#if defined(DATA_2018)
  //std::string fName_lumiMaskFilter("CalibData/Cert_314472-325175_13TeV_17SeptEarlyReReco2018ABC_PromptEraD_Collisions18_JSON.txt");
  std::string fName_lumiMaskFilter("CalibData/Cert_314472-325175_13TeV_Legacy2018_Collisions18_JSON.txt");
#endif


#if defined(MC_2016PRE) || defined(MC_2016) || defined(MC_2017) || defined(MC_2018)
  //sel.SetCentralGenWeight(centralGenWeight);
  //sel.SetPileupSF(fName_puSF);
  VbbHcc_sel.SetCentralGenWeight(centralGenWeight);
  VbbHcc_sel.SetPileupSF(fName_puSF);
  //VbbHcc_sel_unc.SetCentralGenWeight(centralGenWeight);
  //VbbHcc_sel_unc.SetPileupSF(fName_puSF);
  //VbbHcc_triggerSel.SetCentralGenWeight(centralGenWeight);
  //VbbHcc_triggerSel.SetPileupSF(fName_puSF);
  
  fName_PUjetID_SF = "CalibData/scalefactorsPUID_81Xtraining.root";
  fName_PUjetID_eff = "CalibData/effcyPUID_81Xtraining.root";
  //sel.SetPUjetidCalib(fName_PUjetID_SF,fName_PUjetID_eff,jetPUidUncType); //pileup jet ID SF
  VbbHcc_sel.SetPUjetidCalib(fName_PUjetID_SF,fName_PUjetID_eff,jetPUidUncType); //pileup jet ID SF
  VbbHcc_sel.SetXbbXccEff(fName_xbb_xcc_sf);
  VbbHcc_sel.SetXbbXccEff1D(fName_xbb_xcc_eff);
  VbbHcc_sel.SetTriggerSF(fName_triggerSF);
  VbbHcc_sel.SetBtagCalib(fName_btagSF,fName_btagEff,btagUncType);
  //if (CUTS.GetStr("jet_main_btagWP")=="deepFlavBM") VbbHcc_sel.SetBtagCalib(fName_btagSF,"DeepJet","CalibData/effM.root",btagUncType);
  //VbbHcc_sel_unc.SetPUjetidCalib(fName_PUjetID_SF,fName_PUjetID_eff,jetPUidUncType); //pileup jet ID SF
  //VbbHcc_sel_unc.SetXbbXccEff(fName_xbb_xcc_eff);
  //VbbHcc_sel_unc.SetTriggerSF(fName_triggerSF);
  //VbbHcc_triggerSel.SetPUjetidCalib(fName_PUjetID_SF,fName_PUjetID_eff,jetPUidUncType); //pileup jet ID SF
  Eff_sel.SetCentralGenWeight(centralGenWeight);
  Eff_sel.SetPileupSF(fName_puSF);
#endif
#if defined(DATA_2016PRE) || defined(DATA_2016) || defined(DATA_2017) || defined(DATA_2018)
  //sel.SetLumiMaskFilter(fName_lumiMaskFilter);
  VbbHcc_sel.SetLumiMaskFilter(fName_lumiMaskFilter);
  //VbbHcc_sel_unc.SetLumiMaskFilter(fName_lumiMaskFilter);
  //VbbHcc_triggerSel.SetLumiMaskFilter(fName_lumiMaskFilter);
#endif

  VbbHcc_sel.SetJECcorr(fName_jec);                                                         
  VbbHcc_sel.SetMSDcorr(fName_msd); 
  
  std::string fName_n2b1_cut = "CalibData/n2b1_cut.root"; 
  
  VbbHcc_sel.Setn2b1Cut(fName_n2b1_cut);
  //VbbHcc_sel_unc.Setn2b1Cut(fName_n2b1_cut);
  //VbbHcc_triggerSel.Setn2b1Cut(fName_n2b1_cut);

  sels.push_back(&VbbHcc_sel);
  //sels.push_back(&Eff_sel);
  //sels.push_back(&VbbHcc_triggerSel);

  //sels.push_back(&sel) ;
//#if !defined(POSTPROC)
//  sels.push_back(&VbbHcc_sel);
//  sels.push_back(&VbbHcc_triggerSel);
//#else
//  sels.push_back(&VbbHcc_sel_unc);
//#endif


  //add all selectors to processors
  for (std::vector<Selector*>::iterator it = sels.begin() ; it != sels.end() ; it++) ana.addSelector(*it) ;
  std::cout << "\n Selections added" << std::endl; 
  try {
    chain->Process(&ana,"",intlastentry,intfirstentry) ;
    std::cout << "\n End processing" << std::endl;
    chain->Reset(); //Close file now to prevent crash on TNetX...
  }
  catch (const std::exception& e) {
    std::cout << "Exception " << e.what() << endl;
  }

  return 0 ;
}
