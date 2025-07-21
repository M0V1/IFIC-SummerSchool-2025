#include <TMVA/Reader.h>
#include <TTree.h>
#include <TFile.h>
#include <TString.h>
#include <TH1F.h>

void ApplyBDT() {
    TString data_file = "ODEO_FEB2025_v0_1LMET30_data15_periodD.1LMET30.root";
    TString tree_name = "analysis";
    float lep_pt, met, tau_pt, tau_eta, tau_phi, tau_charge, jet_pt, lep_eta;

    TMVA::Reader *reader = new TMVA::Reader("!Color:!Silent");
    reader->AddVariable("lep_pt[0]", &lep_pt);
    reader->AddVariable("met", &met);
    reader->AddVariable("tau_pt[0]", &tau_pt);
    reader->AddVariable("tau_eta[0]", &tau_eta);
    reader->AddVariable("tau_phi[0]", &tau_phi);
    reader->AddVariable("tau_charge[0]", &tau_charge);
    reader->AddVariable("jet_pt[0]", &jet_pt);
    reader->AddVariable("lep_eta[0]", &lep_eta);
    reader->BookMVA("BDT", "dataset/weights/TMVAClassification_BDT.weights.xml");

    TFile *f_data = TFile::Open(data_file);
    TTree *tree = (TTree*)f_data->Get(tree_name);

    tree->SetBranchAddress("lep_pt", &lep_pt); 
    tree->SetBranchAddress("met", &met);
    tree->SetBranchAddress("tau_pt", &tau_pt);
     tree->SetBranchAddress("tau_eta", &tau_eta);
     tree->SetBranchAddress("tau_phi", &tau_phi);
     tree->SetBranchAddress("tau_charge", &tau_charge);
     tree->SetBranchAddress("jet_pt", &jet_pt);
 tree->SetBranchAddress("lep_eta", &lep_eta);
    TH1F *histBDT = new TH1F("BDTscore", "BDT Score on data", 50, -1.0, 1.0);

    Long64_t nentries = tree->GetEntries();
    for (Long64_t i = 0; i < nentries; ++i) {
        tree->GetEntry(i);
        float score = reader->EvaluateMVA("BDT");
        histBDT->Fill(score);
    }

    TCanvas *c1 = new TCanvas("c1", "BDT Score", 800, 600);
    histBDT->Draw();
    c1->SaveAs("BDT_score_data.png");
}

