
// ROOT-related Headers
#include "TROOT.h"  // Settings
#include "TColor.h" // Colors
#include "TFile.h"  // Files
#include "TCanvas.h" // Canvas
#include "TPad.h"   // Pads
#include "TLine.h"  // Line

// Standard C++ Headers
#include <algorithm>  // search within vector
#include <cmath>      // math
#include <string>     // strings
#include <unordered_map>  // maps
#include <vector>     // lists

// ///////////////////////////////////////////////////////////////////////

float roundToDecimals(float value, int decimals)
{
  float scale = std::pow(10, decimals);
  return std::round(value*scale)/scale;
}

void ProducePlot(std::vector<TH1D*> plots, std::vector<std::string> names,
		 std::string canvas_name, std::string output_path,
		 std::string x_axis_title, std::vector<float> x_axis_range,
		 bool logY, int* colors)
{
  gStyle->Reset();
  gStyle->SetOptStat(0); // remove stat box

  // Make the canvas
  TCanvas *c = new TCanvas("", "", 600, 600);
  c->SetLeftMargin(0.15f);
  c->SetFillColor(kWhite);
  c->SetFrameFillColor(kWhite);
  c->cd();

  // Prepare the legend.
  float x0 = 0.53f, x1 = 0.89f;
  TLegend* l = new TLegend(x0, 0.70f, x1, 0.87f);
  l->SetLineWidth(2);
  l->SetBorderSize(0);
  l->SetTextFont(42);
  l->SetTextSize(0.035f);

  // Modify the plots as necessary.
  float max_value = -1.0f;
  for (int i = 0; i < plots.size(); ++i)
  {
    if (i == 0) plots[i]->SetFillColor(kGray);
    plots[i]->SetLineColor(colors[i]);
    plots[i]->SetLineWidth(2);
    l->AddEntry(plots[i], names[i].c_str(), "F");

    int bin_max = plots[i]->GetMaximumBin();
    float bin_val = plots[i]->GetBinContent(bin_max);
    if (bin_val > max_value) max_value = bin_val;
  }

  // Create a buffer between the largest bin & the top of the plot.
  if (logY) {
    float max_log = log(max_value)/log(10);
    max_value = std::pow(10, max_log+1);
  }
  else max_value += 2;

  TPad* pad1_v1 = new TPad("pad1_v1", "pad1_v1", 0.0f, 0.3f, 1.0f, 1.0f);
  pad1_v1->Draw();
  pad1_v1->cd();

  plots[0]->Draw("hist");
  for (int i = 1; i < plots.size(); ++i)
  {
    plots[i]->Draw("same hist");
  }

  l->Draw();
  c->cd();
  c->Update();

  float bin_size = plots[0]->GetBinCenter(2) - plots[0]->GetBinCenter(1);
  bin_size = roundToDecimals(bin_size, 3);
  std::string extra_bit = "";

  std::string y_title = "Events/" + std::to_string(bin_size);
  plots[0]->GetYaxis()->SetTitle(y_title.c_str());
  plots[0]->GetXaxis()->SetTitle(x_axis_title.c_str());
  plots[0]->GetXaxis()->SetRangeUser(x_axis_range[0], x_axis_range[1]);
  plots[0]->SetMaximum(max_value);

  if (logY) pad1_v1->SetLogy();

  pad1_v1->Modified();
  pad1_v1->Update();

  // Create the ratio plots
  TPad* pad1_v2 = new TPad("pad1_v2", "pad1_v2", 0.0f, 0.1f, 1.0f, 0.3f);
  pad1_v2->Draw();
  pad1_v2->cd();

  TH1D* ratio_up = (TH1D*)plots[1]->Clone();
  ratio_up->Divide(plots[0]);
  ratio_up->SetLineColor(colors[1]);

  TH1D* ratio_dn = (TH1D*)plots[2]->Clone();
  ratio_dn->Divide(plots[0]);
  ratio_dn->SetLineColor(colors[2]);

  ratio_up->Draw("hist");
  ratio_dn->Draw("same hist");

  ratio_up->SetTitle("");
  ratio_up->SetMinimum(0.8f);
  ratio_up->SetMaximum(1.2f);
  ratio_up->GetXaxis()->SetTitleSize(0.13f);
  ratio_up->GetXaxis()->SetLabelSize(0.1f);

  ratio_up->GetXaxis()->SetRangeUser(x_axis_range[0], x_axis_range[1]);
  ratio_up->GetYaxis()->SetTitle("#frac{Up/Down}{Nominal}");
  ratio_up->GetYaxis()->CenterTitle();
  ratio_up->GetYaxis()->SetTitleSize(0.12f);
  ratio_up->GetYaxis()->SetTitleOffset(0.35f);
  ratio_up->GetYaxis()->SetLabelSize(0.09f);

  TLine* line_v1 = new TLine(plots[0]->GetXaxis()->GetXmin(), 1.0f,
			     plots[0]->GetXaxis()->GetXmax(), 1.0f);
  line_v1->SetLineStyle(2);
  line_v1->Draw("same");

  pad1_v2->Modified();
  pad1_v2->Update();

  c->Update();
  std::string fullpath = output_path + "/" + canvas_name;
  if (logY) fullpath += "_logY";

  c->Print((fullpath + ".png").c_str());
  c->Print((fullpath + ".pdf").c_str());
  c->Print((fullpath + ".C").c_str());
  delete c;
}

// ///////////////////////////////////////////////////////////////////////

void plot_systematics()
{
  // == SETTINGS ========================================================

  // DO NOT MODIFY THE FOLLOWING SETTINGS:
  gROOT->SetBatch(kTRUE); // supress graphical output while running
  int colors[3] = { kBlack, kRed, kBlue };
  
  // FEEL FREE TO MODIFY THE FOLLOWING SETTINGS:
  bool debug = true;

  std::string dirpath = "../Tmp/unc/";  // path to ROOT files
  std::string outpath = "../plot_results/systematics"; // output area
  
  std::vector<std::string> years = {  // years for Run2
    "16"
  };
  
  std::vector<std::string> variables = { // variables to plot

    "ZccHcc_boosted_PN_med_HMass",
    "ZccHcc_boosted_PN_med_m_jet",
    "ZccHcc_boosted_PN_med_pt_jet",
    "ZccHcc_boosted_PN_med_ZMass",
    "ZccHcc_boosted_PN_med_HPt",
    "ZccHcc_boosted_PN_med_ZPt",
    "VHcc_boosted_PN_med_HMass",
    "VHcc_boosted_PN_med_m_jet",
    "VHcc_boosted_PN_med_pt_jet",
    "VHcc_boosted_PN_med_ZMass",
    "VHcc_boosted_PN_med_HPt",
    "VHcc_boosted_PN_med_ZPt",
    
    "jet_pt",
    "jet_mass"

  };

  std::unordered_map<std::string, int> rebins = { // rebinning for vars

    { "ZccHcc_boosted_PN_med_HMass", 10 },
    { "ZccHcc_boosted_PN_med_m_jet", 10 },
    { "ZccHcc_boosted_PN_med_pt_jet", 20 },
    { "ZccHcc_boosted_PN_med_ZMass", 10 },
    { "ZccHcc_boosted_PN_med_HPt", 20 },
    { "ZccHcc_boosted_PN_med_ZPt", 20 },

    { "VHcc_boosted_PN_med_HMass", 10 },
    { "VHcc_boosted_PN_med_m_jet", 10 },
    { "VHcc_boosted_PN_med_pt_jet", 20 },
    { "VHcc_boosted_PN_med_ZMass", 10 },
    { "VHcc_boosted_PN_med_HPt", 20 },
    { "VHcc_boosted_PN_med_ZPt", 20 },
    
    { "jet_pt", 20 },
    { "jet_mass", 10 }
    
  };

  // range for x-axis on each variable
  std::vector<float> full_range = {0.0f, 2000.0f };
  std::vector<float> small_range = {0.0f, 600.0f };
  std::unordered_map<std::string, std::vector<float>> xAxisRanges =
  {
    { "ZccHcc_boosted_PN_med_HMass", full_range },
    { "ZccHcc_boosted_PN_med_m_jet", full_range },
    { "ZccHcc_boosted_PN_med_pt_jet", full_range },
    { "ZccHcc_boosted_PN_med_ZMass", full_range },
    { "ZccHcc_boosted_PN_med_HPt", full_range },
    { "ZccHcc_boosted_PN_med_ZPt", full_range },
    
    { "VHcc_boosted_PN_med_HMass", full_range },
    { "VHcc_boosted_PN_med_m_jet", full_range },
    { "VHcc_boosted_PN_med_pt_jet", full_range },
    { "VHcc_boosted_PN_med_ZMass", full_range },
    { "VHcc_boosted_PN_med_HPt", full_range },
    { "VHcc_boosted_PN_med_ZPt", full_range },

    { "jet_pt", full_range },
    { "jet_mass", small_range }
  };

  // If a variable needs to use the logY scale, add it here:
  std::vector<std::string> vars_with_logY = {
    "jet_pt"
  };

  // Systematics we're interested in (make sure these match
  // the file name.
  std::vector<std::string> systematics = {
    "JES",
    "JER",
    "L1",
    "PU",
    "JETPUID"
  };

  std::unordered_map<std::string, std::string> alt_names = {
    { "L1", "L1Prefiring" }
  };

  // ======================================================================
  // This is the main code that loops through everything and prints out the
  // appropriate plots using the settings above. Do NOT edit below here
  // unless absolutely necessary (or you want to mess with me).
  // ======================================================================

  // == MAIN CODE =========================================================

  // ====================================
  // 1. Retrieve all the necessary files.
  // ====================================
  if (debug) std::cout << "Retrieving proper files..." << std::endl;

  std::unordered_map<std::string, TFile*> files;
  std::string filebase = dirpath + "/output_testNONE";
  std::string filepath = filebase + ".root";
  files["nominal"] = new TFile(filepath.c_str());
  if (debug) {
    std::cout << ">>> nominal file retrieved... ";
    std::cout << "(" << filepath << ")" << std::endl;
  }
    
  for (size_t i = 0; i < systematics.size(); ++i)
  {

    std::string filebase2 = filebase + "_" + systematics[i];
    for (int j = 0; j < 2; ++j) // UP & DOWN
    {
      std::string ext = (j == 0) ? "U" : "D";
      filepath = filebase2 + ext + ".root";

      std::string key = systematics[i] + ext;
      files[key] = new TFile(filepath.c_str());

      if (debug) {
	std::cout << ">>> " << key << " file retrieved... ";
	std::cout << "(" << filepath << ")" << std::endl;
      }
    }
    
  }//end-for

  if (debug)
  {
    std::cout << "Sanity Check: file keys" << std::endl;
    for (auto it = files.begin(); it != files.end(); ++it) {
      std::cout << ">> Key: " << it->first << std::endl;
    }
  }

  // ==============================
  // 2. Loop through each variable.
  // ==============================
  if (debug) std::cout << "Starting actual ploting..." << std::endl;

  // Loop through the variables.
  for (size_t v = 0; v < variables.size(); ++v)
  {
    if (debug) std::cout << ">>> variable = " << variables[v] << std::endl;

    int rebin = rebins[variables[v]];

    // Get the nominal plot which will be used for all systematics.
    TH1D* hNONE = (TH1D*)(files["nominal"]->Get(variables[v].c_str()))->Clone();
    hNONE->Rebin(rebin);
    
    // Loop through each systematic.
    for (size_t s = 0; s < systematics.size(); ++s)
    {

      if (debug) std::cout << ">>> | syst = " << systematics[s] << std::endl;

      // Get the systematic keys.
      std::string keyu = systematics[s] + "U";
      std::string keyd = systematics[s] + "D";

      // Get the up & down plots.
      TH1D* hUP = (TH1D*)files[keyu]->Get(variables[v].c_str())->Clone();
      hUP->Rebin(rebin);
      TH1D* hDOWN = (TH1D*)files[keyd]->Get(variables[v].c_str())->Clone();
      hDOWN->Rebin(rebin);

      // Produce the plots.
      std::vector<TH1D*> plots = { hNONE, hUP, hDOWN };
      std::string syst_name = (alt_names.find(systematics[s]) != alt_names.end()) ?
	alt_names[systematics[s]] : systematics[s];
      std::vector<std::string> names = {
        "Nominal",
	syst_name + " Up",
	syst_name + " Down"
      };

      bool logY = (std::find(
	vars_with_logY.begin(),
	vars_with_logY.end(),
	variables[v]
      ) != vars_with_logY.end());
      
      ProducePlot(plots, names,
		  variables[v] + "_" + syst_name, outpath,
		  "", xAxisRanges[variables[v]],
		  logY, colors);
      
    }//end-for-syst
  }//end-for-var
}
