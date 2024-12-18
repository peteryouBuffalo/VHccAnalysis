
#include<iostream>
#include<string>
#include<TH1.h>
#include<TFile.h>

void draw_histogram(std::string histogram_name) 
{
  std::string filepath = "../test.root";
  //std::string histogram_name = "evt_cutflow";

  TFile *f = new TFile(filepath.c_str());
  TH1D *h = (TH1D*)f->Get(histogram_name.c_str());
  h->Draw();
}
