import os
import sys
import argparse
import configparser
import numpy as np
import uproot
from array import array
import ROOT

dirpath = '/uscms_data/d3/peteryou/boosted_new/CMSSW_14_0_6/src/VHccAnalysis/condor_results/2025Jan/'
config_file = '../Configs/config.ini'
regions = ['ZccHcc_boosted_PN_med', 'VHcc_boosted_PN_med']
output_path = '../Tmp/theo_unc/'

categories = {
    "QCD": [
        "QCD_HT1000to1500_v9", "QCD_HT1500to2000_v9", "QCD_HT2000toInf_v9",
        "QCD_HT200to300_v9", "QCD_HT300to500_v9", "QCD_HT500to700_v9", "QCD_HT700to1000_v9"
    ],
    "TT": [
        "TTTo2L2Nu", "TTToHadronic", "TTToSemiLeptonic"
    ],
    "ZH_HToCC": [
        "ZH_HToCC_ZToQQ", "ggZH_HToCC_ZToQQ"
    ],
    "ZH_HToBB": [
        "ZH_HToBB_ZToQQ", "ggZH_HToBB_ZToQQ"
    ],
    "WplusH": [
        "WplusH_HToBB_WToQQ", "WplusH_HToCC_WToQQ"
    ],
    "WminusH": [
        "WminusH_HToBB_WToQQ", "WminusH_HToCC_WToQQ"
    ],
    "WJets": [
        "WJetsToLNu_HT-1200to2500", "WJetsToLNu_HT-2500toInf",
        "WJetsToLNu_HT-400to600", "WJetsToLNu_HT-600to800", "WJetsToLNu_HT-800to1200",
        "WJetsToQQ_HT-400to600", "WJetsToQQ_HT-600to800", "WJetsToQQ_HT-800toInf"
    ],
    "ZJets": [
        "ZJetsToQQ_HT-400to600", "ZJetsToQQ_HT-600to800", "ZJetsToQQ_HT-800toInf"
    ],
    "ST": [
        "ST_t-channel_antitop", "ST_t-channel_top", "ST_tW-channel_antitop", "ST_tW-channel_top"
    ],
    "WZ": [
        "WZTo1L1Nu2Q_NLO", "WZTo2Q2L_NLO", "WZTo4Q_NLO", "WZToLNu2B_NLO"#, "WZ"
    ],
    "WW": [
        "WWTo1L1Nu2Q_NLO", "WWTo4Q_NLO"#, "WW"
    ],
    "ZZ": [
        "ZZTo2Nu2Q_NLO", "ZZTo2Q2L_NLO", "ZZTo4Q_NLO"#, "ZZ"
    ]
}

xsec_dict = {
    "WWTo1L1Nu2Q_NLO": 50.9,
    "WWTo4Q_NLO": 51.57,
    "WZTo4Q_NLO": 23.43,
    "WZToLNu2B_NLO": 2.525,
    "WZTo1L1Nu2Q_NLO": 9.152,
    "WZTo2Q2L_NLO": 6.422,
    "ZZTo2Q2L_NLO": 3.705,
    "ZZTo2Nu2Q_NLO": 4.498,
    "ZZTo4Q_NLO": 3.295,
    "WminusH_HToBB_WToQQ": 0.21348075,
    "WplusH_HToBB_WToQQ": 0.34215009999999996,
    "WminusH_HToBB_WToLNu": 0.10281929999999999,
    "WplusH_HToBB_WToLNu": 0.16451088,
    "WplusH_HToCC_WToQQ": 0.01711454025,
    "WminusH_HToCC_WToQQ": 0.01086317572
}

def get_cross_section(sample, year_key, config_parser, xsec_dict):
    try:
        xsec_str = config_parser[sample].get(f"xSec_{year_key}")
        return eval(xsec_str)
    except Exception as e:
        if sample in xsec_dict:
            return xsec_dict[sample]
        else:
            print(f"Warning: cross-section for sample {sample} not found; using 1.0")
            return 1.0

def rebin_array(data_array, factor):
    nbins = len(data_array)
    trim = nbins % factor
    if trim != 0:
        data_array = data_array[:-trim]
    return data_array.reshape(-1, factor).sum(axis=1)

def rebin_errors(error_array, factor):
    nbins = len(error_array)
    trim = nbins % factor
    if trim != 0:
        error_array = error_array[:-trim]
    return np.sqrt(np.sum(error_array.reshape(-1, factor)**2, axis=1))

def get_rebinned_edges(bin_edges, factor):
    if bin_edges is None:
        return None
    bin_edges = bin_edges[:-1]
    trim = len(bin_edges) % factor
    if trim != 0:
        bin_edges = bin_edges[:-trim]
    bin_edges_rebinned = bin_edges.reshape(-1, factor)
    new_bin_edges = bin_edges_rebinned[:, 0]
    step = bin_edges[1] - bin_edges[0]
    last_edges = bin_edges_rebinned[:, -1]
    last_edge = last_edges[-1] + step * factor
    new_bin_edges = np.append(new_bin_edges, last_edge)
    return new_bin_edges

def produce_pdf_uncertainties(year, rebin_factor, config_file, dirpath, categories, xsec_dict, regions, normalized):
    output_file = ROOT.TFile(output_path + f"PDF_uncertainties_{year}.root", "RECREATE")
    config_parser = configparser.ConfigParser()
    config_parser.read(config_file)
    systs = ['PDFG0', 'PDFG1', 'PDFG2']
    
    for group_name, samples in categories.items():
        print(f"Processing group: {group_name} for PDF+alpha uncertainties")
        for region in regions:
            print(f"  Region: {region}")
            combined_sigma_k = None
            combined_err_sigma_k = None
            bin_edges = None
            first_sample = True
            for sample in samples:
                weight = get_cross_section(sample, year[-2:], config_parser, xsec_dict)
                sigma_k_list = []
                err_sigma_k_list = []
                for s in systs:
                    file_path = os.path.join(dirpath, s, f"{sample}_MC_{year}.root")
                    if not os.path.exists(file_path):
                        print(f"    File {file_path} not found for sample {sample} in systematic {s}. Filling zeros.")
                        if s == 'PDFG0':
                            indices = range(0, 35)
                        elif s == 'PDFG1':
                            indices = range(35, 70)
                        elif s == 'PDFG2':
                            indices = range(70, 103)
                        for _ in indices:
                            sigma_k_list.append(np.zeros(300))
                            err_sigma_k_list.append(np.zeros(300))
                        continue
                    try:
                        file = uproot.open(file_path)
                    except Exception as e:
                        print(f"    Error opening {file_path}: {e}")
                        continue
                    for x in range(103):
                        condition = ((s == 'PDFG0' and x < 35) or
                                     (s == 'PDFG1' and 35 <= x < 70) or
                                     (s == 'PDFG2' and x >= 70))
                        if not condition:
                            continue
                        hist_name = f"{region}_HMass_LHEPdf_{x};1"
                        if hist_name in file:
                            hist = file[hist_name]
                            contents, edges = hist.to_numpy()
                            errors = hist.errors()
                            if bin_edges is None:
                                bin_edges = edges
                            sigma_k_list.append(contents)
                            err_sigma_k_list.append(errors)
                        else:
                            nbins = 300 if bin_edges is None else (len(bin_edges)-1)
                            sigma_k_list.append(np.zeros(nbins))
                            err_sigma_k_list.append(np.zeros(nbins))
                sigma_k = np.array(sigma_k_list)
                err_sigma_k = np.array(err_sigma_k_list)
                sigma_k_rebinned = []
                err_sigma_k_rebinned = []
                for i in range(sigma_k.shape[0]):
                    sigma_k_rebinned.append(rebin_array(sigma_k[i], rebin_factor))
                    err_sigma_k_rebinned.append(rebin_errors(err_sigma_k[i], rebin_factor))
                sigma_k_rebinned = np.array(sigma_k_rebinned)
                err_sigma_k_rebinned = np.array(err_sigma_k_rebinned)
                sigma_k_sample = sigma_k_rebinned * weight
                err_sigma_k_sample = err_sigma_k_rebinned * weight
                if first_sample:
                    combined_sigma_k = sigma_k_sample.copy()
                    combined_err_sigma_k = err_sigma_k_sample.copy()**2
                    first_sample = False
                else:
                    combined_sigma_k += sigma_k_sample
                    combined_err_sigma_k += err_sigma_k_sample**2
            if combined_sigma_k is None:
                print(f"  No samples found for group {group_name} in region {region}. Skipping.")
                continue
            if normalized:
                cat_xsec = sum(get_cross_section(sample, year[-2:], config_parser, xsec_dict) for sample in samples)
                combined_sigma_k = combined_sigma_k / cat_xsec
                combined_err_sigma_k = combined_err_sigma_k / cat_xsec
            combined_err_sigma_k = np.sqrt(combined_err_sigma_k)
            nbins = combined_sigma_k.shape[1]
            delta_PDF = np.sqrt(np.sum((combined_sigma_k[1:101] - combined_sigma_k[0])**2, axis=0))
            numerator = np.sum((combined_sigma_k[1:101] - combined_sigma_k[0]) * combined_err_sigma_k[0], axis=0)
            denominator = delta_PDF.copy()
            denominator[denominator == 0] = 1e-10
            term1 = (-1/denominator)*numerator
            term2 = np.sum(((combined_sigma_k[1:101] - combined_sigma_k[0]) / denominator) * combined_err_sigma_k[1:101], axis=0)
            err_delta_PDF = np.sqrt(term1**2 + term2**2)
            delta_alpha = (combined_sigma_k[102] - combined_sigma_k[101]) / 2.0
            err_delta_alpha = np.sqrt((combined_err_sigma_k[101]/2.0)**2 + (combined_err_sigma_k[102]/2.0)**2)
            delta_PDFalpha = np.sqrt(delta_PDF**2 + delta_alpha**2)
            delta_PDFalpha[delta_PDFalpha==0] = 1e-10
            err_delta_PDFalpha = np.sqrt(((delta_PDF/delta_PDFalpha)*err_delta_PDF)**2 +
                                         ((delta_alpha/delta_PDFalpha)*err_delta_alpha)**2)
            new_bin_edges = get_rebinned_edges(bin_edges, rebin_factor)
            if new_bin_edges is None:
                print("  Could not determine new bin edges; skipping.")
                continue
            nbins_hist = len(new_bin_edges) - 1
            h_PDF = ROOT.TH1D(f"{group_name}_{region}_PDF", f"PDF Uncertainty for {group_name} in {region}",
                              nbins_hist, array('d', new_bin_edges))
            h_alpha = ROOT.TH1D(f"{group_name}_{region}_alpha", f"alpha Uncertainty for {group_name} in {region}",
                                nbins_hist, array('d', new_bin_edges))
            h_PDFalpha = ROOT.TH1D(f"{group_name}_{region}_PDF_alpha", f"Combined PDF+alpha Uncertainty for {group_name} in {region}",
                                   nbins_hist, array('d', new_bin_edges))
            for i in range(nbins):
                h_PDF.SetBinContent(i+1, delta_PDF[i])
                h_PDF.SetBinError(i+1, err_delta_PDF[i])
                h_alpha.SetBinContent(i+1, delta_alpha[i])
                h_alpha.SetBinError(i+1, err_delta_alpha[i])
                h_PDFalpha.SetBinContent(i+1, delta_PDFalpha[i])
                h_PDFalpha.SetBinError(i+1, err_delta_PDFalpha[i])
            h_PDF.Write()
            h_alpha.Write()
            h_PDFalpha.Write()
    output_file.Close()
    print("PDF+alpha uncertainty processing complete.")

def produce_scale_uncertainties(year, rebin_factor, config_file, dirpath, categories, xsec_dict, regions, normalized):
    output_file = ROOT.TFile(output_path + f"SCALE_uncertainties_{year}.root", "RECREATE")
    config_parser = configparser.ConfigParser()
    config_parser.read(config_file)
    
    for group_name, samples in categories.items():
        print(f"Processing group: {group_name} for SCALE uncertainties")
        for region in regions:
            print(f"  Region: {region}")
            combined_sigma_k = None
            combined_err_sigma_k = None
            bin_edges = None
            first_sample = True
            for sample in samples:
                weight = get_cross_section(sample, year[-2:], config_parser, xsec_dict)
                sigma_k_list = []
                err_sigma_k_list = []
                s = "SCALE"
                file_path = os.path.join(dirpath, s, f"{sample}_MC_{year}.root")
                if not os.path.exists(file_path):
                    print(f"    File {file_path} not found for sample {sample}. Skipping sample.")
                    continue
                try:
                    file = uproot.open(file_path)
                except Exception as e:
                    print(f"    Error opening {file_path}: {e}")
                    continue
                for x in range(9):
                    hist_name = f"{region}_HMass_LHEScale_{x};1"
                    if hist_name in file:
                        hist = file[hist_name]
                        contents, edges = hist.to_numpy()
                        errors = hist.errors()
                        if bin_edges is None:
                            bin_edges = edges
                        sigma_k_list.append(contents)
                        err_sigma_k_list.append(errors)
                    else:
                        print(f"    Histogram {hist_name} not found in {file_path}. Filling zeros.")
                        nbins = 300 if bin_edges is None else (len(bin_edges)-1)
                        sigma_k_list.append(np.zeros(nbins))
                        err_sigma_k_list.append(np.zeros(nbins))
                sigma_k = np.array(sigma_k_list)
                err_sigma_k = np.array(err_sigma_k_list)
                sigma_k_rebinned = []
                err_sigma_k_rebinned = []
                for i in range(sigma_k.shape[0]):
                    sigma_k_rebinned.append(rebin_array(sigma_k[i], rebin_factor))
                    err_sigma_k_rebinned.append(rebin_errors(err_sigma_k[i], rebin_factor))
                sigma_k_rebinned = np.array(sigma_k_rebinned)
                err_sigma_k_rebinned = np.array(err_sigma_k_rebinned)
                sigma_k_sample = sigma_k_rebinned * weight
                err_sigma_k_sample = err_sigma_k_rebinned * weight
                if first_sample:
                    combined_sigma_k = sigma_k_sample.copy()
                    combined_err_sigma_k = err_sigma_k_sample.copy()**2
                    first_sample = False
                else:
                    combined_sigma_k += sigma_k_sample
                    combined_err_sigma_k += err_sigma_k_sample**2
            if combined_sigma_k is None:
                print(f"  No valid samples for group {group_name} in region {region}. Skipping.")
                continue
            if normalized:
                cat_xsec = sum(get_cross_section(sample, year[-2:], config_parser, xsec_dict) for sample in samples)
                combined_sigma_k = combined_sigma_k / cat_xsec
                combined_err_sigma_k = combined_err_sigma_k / cat_xsec
            combined_err_sigma_k = np.sqrt(combined_err_sigma_k)
            nbins = combined_sigma_k.shape[1]
            idx_central = 4
            idx_variations = [0, 1, 3, 5, 7, 8]
            sigma_central = combined_sigma_k[idx_central]
            err_sigma_central = combined_err_sigma_k[idx_central]
            sigma_variations = combined_sigma_k[idx_variations]
            err_sigma_variations = combined_err_sigma_k[idx_variations]
            max_sigma = np.max(sigma_variations, axis=0)
            min_sigma = np.min(sigma_variations, axis=0)
            sigma_central_safe = np.where(sigma_central == 0, 1e-10, sigma_central)
            max_sigma_safe = np.where(max_sigma == 0, 1e-10, max_sigma)
            min_sigma_safe = np.where(min_sigma == 0, 1e-10, min_sigma)
            SCALEup = max_sigma_safe / sigma_central_safe
            SCALEdown = min_sigma_safe / sigma_central_safe
            delta_SCALE = (SCALEup - SCALEdown) / 2.0
            if np.isnan(delta_SCALE).any() or np.isinf(delta_SCALE).any():
                print(f"delta_SCALE contains NaN or Inf for group {group_name} in region {region}. Skipping.")
                continue
            max_indices = np.argmax(sigma_variations, axis=0)
            min_indices = np.argmin(sigma_variations, axis=0)
            err_max_sigma = err_sigma_variations[max_indices, np.arange(nbins)]
            err_min_sigma = err_sigma_variations[min_indices, np.arange(nbins)]
            err_max_sigma_safe = np.where(max_sigma == 0, 0, err_max_sigma)
            err_min_sigma_safe = np.where(min_sigma == 0, 0, err_min_sigma)
            max_sigma_safe_for_error = np.where(max_sigma == 0, 1e-10, max_sigma)
            min_sigma_safe_for_error = np.where(min_sigma == 0, 1e-10, min_sigma)
            err_SCALEup = SCALEup * np.sqrt((err_max_sigma_safe / max_sigma_safe_for_error)**2 +
                                            (err_sigma_central / sigma_central_safe)**2)
            err_SCALEdown = SCALEdown * np.sqrt((err_min_sigma_safe / min_sigma_safe_for_error)**2 +
                                                (err_sigma_central / sigma_central_safe)**2)
            err_delta_SCALE = np.sqrt((err_SCALEup / 2.0)**2 + (err_SCALEdown / 2.0)**2)
            new_bin_edges = get_rebinned_edges(bin_edges, rebin_factor)
            if new_bin_edges is None:
                print("Could not determine new bin edges; skipping.")
                continue
            nbins_hist = len(new_bin_edges) - 1
            h_delta_SCALE = ROOT.TH1D(f"{group_name}_{region}_SCALE",
                                      f"Scale Uncertainty for {group_name} in {region}",
                                      nbins_hist, array('d', new_bin_edges))
            for i in range(nbins):
                h_delta_SCALE.SetBinContent(i+1, delta_SCALE[i])
                h_delta_SCALE.SetBinError(i+1, err_delta_SCALE[i])
            h_delta_SCALE.Write()
    output_file.Close()
    print("SCALE uncertainty processing complete.")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--year", choices=["2016", "2016PRE", "2017", "2018"], default="2016")
    parser.add_argument("--rebin", type=int, default=10)
    parser.add_argument("--type", choices=["pdf", "scale", "both"], default="both")
    parser.add_argument("--normalized", action="store_true")
    args = parser.parse_args()
    
    year = args.year
    if year == "2016PRE":
      year = "2016_preVFP"
    rebin_factor = args.rebin
    unc_type = args.type
    normalized = args.normalized
    
    print(f"Processing {unc_type} uncertainties for {year} samples (normalized={normalized})")
    
    if unc_type in ("pdf", "both"):
        produce_pdf_uncertainties(year, rebin_factor, config_file, dirpath, categories, xsec_dict, regions, normalized)
    if unc_type in ("scale", "both"):
        produce_scale_uncertainties(year, rebin_factor, config_file, dirpath, categories, xsec_dict, regions, normalized)

if __name__ == "__main__":
    main()

