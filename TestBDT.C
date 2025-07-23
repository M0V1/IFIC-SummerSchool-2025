#include <TMVA/Reader.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>

void ApplyBDT()
{
    TMVA::Reader *reader = new TMVA::Reader("!Color:!Silent");

    Float_t tau_pt, tau_e, met, jet_pt, jet_e;
    Float_t tau_n;

    reader->AddVariable("tau_pt[0]", &tau_pt);
    reader->AddVariable("tau_n", &tau_n);
    reader->AddVariable("tau_e[0]", &tau_e);
    reader->AddVariable("met", &met);
    reader->AddVariable("jet_e[0]", &jet_e);
    reader->AddVariable("jet_pt[0]", &jet_pt);

    reader->BookMVA("BDT", "dataset/weights/TMVAClassification_BDT.weights.xml");

    TFile *dataFile = TFile::Open("ODEO_FEB2025_v0_1LMET30_data15_periodD.1LMET30.root");
    TTree *dataTree = (TTree*)dataFile->Get("analysis");

    dataTree->SetBranchAddress("tau_pt", &tau_pt);
    dataTree->SetBranchAddress("tau_n", &tau_n);
    dataTree->SetBranchAddress("tau_e", &tau_e);
    dataTree->SetBranchAddress("met", &met);
    dataTree->SetBranchAddress("jet_e", &jet_e);
    dataTree->SetBranchAddress("jet_pt", &jet_pt);

    TFile *outFile = new TFile("BDT_applied.root", "RECREATE");
    TTree *outTree = new TTree("analysis", "Tree with BDT output");

    Float_t bdtScore;
    outTree->Branch("BDT_score", &bdtScore, "BDT_score/F");
    outTree->Branch("tau_pt", &tau_pt, "tau_pt/F");
    outTree->Branch("met", &met, "met/F");
    outTree->Branch("tau_n", &tau_n, "tau_n/F");
    outTree->Branch("tau_e", &tau_e, "tau_e/F");
    outTree->Branch("jet_pt", &jet_pt, "jet_pt/F");
     outTree->Branch("jet_e", &jet_e, "jet_e/F");
    Long64_t nEntries = dataTree->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        dataTree->GetEntry(i);
        bdtScore = reader->EvaluateMVA("BDT");
        outTree->Fill();
    }

    outFile->cd();
    outTree->Write();
    outFile->Close();
    dataFile->Close();

    delete reader;
}

