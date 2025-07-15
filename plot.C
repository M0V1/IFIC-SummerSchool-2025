#include <TFile.h>
#include <TTree.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TGraphAsymmErrors.h>

void plot() {
    TFile *file = nullptr;
    file = TFile::Open("ODEO_FEB2025_v0_2J2LMET30_mc_410471.PhPy8EG_A14_ttbar_hdamp258p75_allhad.2J2LMET30.root");
    TTree *tree = (TTree*)file->Get("analysis");
    const int nbins = 25;
    double bin_edges[nbins + 1];
    int i = 0;
    for (; i < nbins / 2; ++i) {
      bin_edges[i] = 20 + i * 10; // Bins de 10 GeV
   }
    for (; i < nbins - 4; ++i) {
      bin_edges[i] = bin_edges[i - 1] + 20; // Bins de 50 GeV
   }
    for (; i <= nbins; ++i) {
       bin_edges[i] = bin_edges[i - 1] + 100; // Bins de 100 GeV
    }
    TH1F *h_total = new TH1F("h_total", "; p_{T}^{#tau} [GeV]; Events", nbins, bin_edges);
    h_total->SetLineColor(kBlue+2);
    h_total->SetLineWidth(2);

    tree->Draw("tau_pt >> h_total", "", "goff");

    TCanvas *c1 = new TCanvas("c1", "Canvas", 800, 600);
//    c1->SetLogy(); 
    h_total->Draw("HIST");

    TLegend *leg = new TLegend(0.6, 0.75, 0.88, 0.88);
    leg->AddEntry(h_total, "ttbar (tau_{had})", "l");
    leg->Draw();

    c1->SaveAs("tau_pt.png");
}


