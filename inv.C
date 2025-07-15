#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TLorentzVector.h>
#include <TLegend.h>

#include <iostream>
#include <map>
#include <vector>
#include <string>

void plot_mvis() {
    std::map<std::string, std::vector<std::string>> files = {
        {"ttbar_Ztt", {"ODEO_FEB2025_v0_2J2LMET30_mc_410471.PhPy8EG_A14_ttbar_hdamp258p75_allhad.2J2LMET30.root", "ODEO_FEB2025_v0_2J2LMET30_mc_700792.Sh_2214_Ztautau_maxHTpTV2_BFilter.2J2LMET30.root"}},
        {"tautau",    {"ODEO_FEB2025_v0_2J2LMET30_mc_346343.PhPy8EG_A14NNPDF23_NNPDF30ME_ttH125_allhad.2J2LMET30.root", "ODEO_FEB2025_v0_2J2LMET30_mc_345121.PowhegPy8EG_NNLOPS_nnlo_30_ggH125_tautaulm15hp20.2J2LMET30.root", "ODEO_FEB2025_v0_2J2LMET30_mc_345122.PowhegPy8EG_NNLOPS_nnlo_30_ggH125_tautaulp15hm20.2J2LMET30.root"}},
        {"Higgs",     { "ODEO_FEB2025_v0_2J2LMET30_mc_345121.PowhegPy8EG_NNLOPS_nnlo_30_ggH125_tautaulm15hp20.2J2LMET30.root"}},
        {"Data",      {"ODEO_FEB2025_v0_2J2LMET30_data15_periodH.2J2LMET30.root", "ODEO_FEB2025_v0_2J2LMET30_data15_periodF.2J2LMET30.root", "ODEO_FEB2025_v0_2J2LMET30_data15_periodD.2J2LMET30.root"}}
    };
    std::map<std::string, TH1F*> histos;

    std::map<std::string, int> colors = {
        {"ttbar_Ztt", kRed+1},
        {"tautau",    kGreen+2},
        {"Higgs",     kBlue+1},
        {"Data",      kBlack}
    };

    for (const auto& [key, _] : files) {
        histos[key] = new TH1F(("h_mvis_" + key).c_str(),
                               ("M_{#tau#tau} - " + key + "; M_{#tau#tau} [GeV]; Events").c_str(),
                               60, 0, 300);
        histos[key]->SetLineColor(colors[key]);
        histos[key]->SetLineWidth(2);
        histos["Data"]->SetMarkerStyle(20); 
        histos["Data"]->SetMarkerSize(1.2);  
        histos["Data"]->SetLineColor(kBlack);
    }

    for (const auto& [proc_name, file_list] : files) {
        for (const std::string& fname : file_list) {
            TFile *file = TFile::Open(fname.c_str());
            TTree *tree = (TTree*)file->Get("analysis");

            std::vector<float> *tau_pt = nullptr;
            std::vector<float> *tau_eta = nullptr;
            std::vector<float> *tau_phi = nullptr;
            std::vector<float> *tau_e = nullptr;

            tree->SetBranchAddress("tau_pt", &tau_pt);
            tree->SetBranchAddress("tau_eta", &tau_eta);
            tree->SetBranchAddress("tau_phi", &tau_phi);
            tree->SetBranchAddress("tau_e", &tau_e);
            Long64_t nentries = tree->GetEntries();
            for (Long64_t i = 0; i < nentries; ++i) {
                tree->GetEntry(i);
                if (tau_pt->size() < 2) continue;
                if (std::abs(tau_eta->at(0)) > 2.5 || std::abs(tau_eta->at(1)) > 2.5) continue;

                TLorentzVector tau1, tau2;
                tau1.SetPtEtaPhiE(tau_pt->at(0), tau_eta->at(0), tau_phi->at(0), tau_e->at(0));
                tau2.SetPtEtaPhiE(tau_pt->at(1), tau_eta->at(1), tau_phi->at(1), tau_e->at(1));

                float mvis = (tau1 + tau2).M();
                histos[proc_name]->Fill(mvis);
            }

            file->Close();
        }
    }

    // Dessin
    TCanvas *c1 = new TCanvas("c1", "c1", 900, 600);
    TLegend *leg = new TLegend(0.65, 0.65, 0.88, 0.88);

    bool first = true;
    for (const auto& [proc, histo] : histos) {
    if (first) {
        if (proc == "Data") {
            histo->Draw("E1");  // Points avec barres d’erreur
        } else {
            histo->Draw("HIST");
        }
        first = false;
    } else {
        if (proc == "Data") {
            histo->Draw("E1 SAME");
        } else {
            histo->Draw("HIST SAME");
        }
    }
        leg->AddEntry(histo, proc.c_str(), (proc == "Data" ? "lep" : "l"));
    }

    leg->Draw();
    c1->SaveAs("mvis_tau_tau.png");
}

