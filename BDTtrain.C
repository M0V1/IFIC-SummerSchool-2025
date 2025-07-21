#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TLegend.h>
#include <TMVA/Factory.h>
#include <TMVA/DataLoader.h>
#include <TMVA/Tools.h>

void TrainBDT() {
    TString signal_file     = "ODEO_FEB2025_v0_1LMET30_mc_346343.PhPy8EG_A14NNPDF23_NNPDF30ME_ttH125_allhad.1LMET30.root";
    TString background_file = "ODEO_FEB2025_v0_1LMET30_mc_700792.Sh_2214_Ztautau_maxHTpTV2_BFilter.1LMET30.root";
    TString data_file       = "ODEO_FEB2025_v0_1LMET30_data15_periodD.1LMET30.root";
    TString tree_name       = "analysis";

    TMVA::Tools::Instance();

    auto outputFile = TFile::Open("TMVAOutput.root", "RECREATE");

    TMVA::Factory *factory = new TMVA::Factory("TMVAClassification", outputFile,
        "!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification");

    TMVA::DataLoader *dataloader = new TMVA::DataLoader("dataset");

    TFile *f_sig = TFile::Open(signal_file);
    TFile *f_bkg = TFile::Open(background_file);

    TTree *tsig = (TTree*)f_sig->Get(tree_name);
    TTree *tbkg = (TTree*)f_bkg->Get(tree_name);

    dataloader->AddVariable("lep_pt[0]", 'F');
    dataloader->AddVariable("met", 'F');
    dataloader->AddVariable("tau_pt[0]", 'F');
    dataloader->AddVariable("tau_eta[0]", 'F');
    dataloader->AddVariable("tau_phi[0]", 'F');
    dataloader->AddVariable("tau_charge[0]", 'F');
    dataloader->AddVariable("jet_pt[0]", 'F');
    dataloader->AddVariable("lep_eta[0]", 'F');
    dataloader->AddSignalTree(tsig, 1.0);
    dataloader->AddBackgroundTree(tbkg, 1.0);

    TCut cutSig = "tau_pt[0]>25";
    TCut cutBkg = "met[0]>40";
    dataloader->PrepareTrainingAndTestTree(cutSig, cutBkg,
        "SplitMode=Random:NormMode=NumEvents:!V");

    factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDT",
        "!H:!V:NTrees=50:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20");

    factory->TrainAllMethods();
    factory->TestAllMethods();
    factory->EvaluateAllMethods();

    outputFile->Close();

}

