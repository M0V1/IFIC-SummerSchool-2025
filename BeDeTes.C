// Incluye las cabeceras necesarias
#include <TFile.h>
#include <TTree.h>
#include <TString.h>

#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"

void TMVA_BDT_Example()
{
    // Inicializa TMVA
    TMVA::Tools::Instance();

    // Crea un archivo de salida
    TFile* outputFile = TFile::Open("TMVAOutput.root", "RECREATE");

    // Crea la fábrica TMVA (modo "AnalysisType = Classification")
    TMVA::Factory *factory = new TMVA::Factory("TMVAClassification", outputFile,
        "!V:!Silent:Color:DrawProgressBar:AnalysisType=Classification");

    // DataLoader gestiona los datos y variables
    TMVA::DataLoader *dataloader = new TMVA::DataLoader("dataset");

    // Carga el archivo de señal
	TFile *dataFile= TFile::Open("ODEO_FEB2025_v0_1LMET30_data15_periodD.1LMET30.root");
	TFile *signalFile = TFile::Open("ODEO_FEB2025_v0_1LMET30_mc_346345.PhPy8EG_A14NNPDF23_NNPDF30ME_ttH125_dilep.1LMET30.root");
	TTree *signalTree = (TTree*)signalFile->Get("analysis");

	// Carga el archivo de fondo
	TFile *backgroundFile = TFile::Open("ODEO_FEB2025_v0_1LMET30_mc_700902.Sh_2214_Ztt_maxHTpTV2_Mll10_40_CFilterBVeto.1LMET30.root");
	TTree *backgroundTree = (TTree*)backgroundFile->Get("analysis");

    // Define las variables de entrada que se usarán
    dataloader->AddVariable("tau_pt[0]", 'F');
    dataloader->AddVariable("tau_n", 'I');
    dataloader->AddVariable("tau_e[0]", 'F');
    dataloader->AddVariable("tau_charge[0]", 'I');
    dataloader->AddVariable("met", 'F');
    dataloader->AddVariable("jet_e[0]", 'F');
    dataloader->AddVariable("jet_pt[0]", 'F');
    // ... añade todas las variables necesarias
        // Asocia los árboles al DataLoader
        dataloader->AddSignalTree(signalTree);
        dataloader->AddBackgroundTree(backgroundTree);

    // Prepara la base de datos: define cómo se dividen los datos
	TCut cutsignal="tau_pt[0]>30 && tau_n>1 && met>40";
	TCut cutback="tau_pt[0]<50";
    dataloader->PrepareTrainingAndTestTree(cutsignal, cutback,
        "nTrain_Signal=0.7:nTrain_Background=0.7:SplitMode=Random:NormMode=NumEvents:!V");

    // Configura el método BDT
    factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDT",
        "!H:!V:NTrees=350:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:"
        "SeparationType=GiniIndex:nCuts=10");

    // Entrena los métodos
    factory->TrainAllMethods();

    // Prueba todos los métodos
    factory->TestAllMethods();

    // Evalúa el rendimiento
    factory->EvaluateAllMethods();

    // Cierra archivo
    outputFile->Close();

    // Limpieza
    delete factory;
    delete dataloader;
}
