#define step1_cxx
#include "step1.h"
#include <fstream>
#include <iostream>     // std::cout
#include <algorithm>    // std::sort
#include <TH2.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TRandom.h>
#include <TRandom3.h>
#include <sstream>
#include <string>
#include <vector>
#include <TH3.h>

#include "lwtnn/lwtnn/interface/parse_json.hh"
#include "lwtnn/lwtnn/interface/LightweightNeuralNetwork.hh"

using namespace std;

// ----------------------------------------------------------------------------
// DNN stuff
// ----------------------------------------------------------------------------

std::string dnnFile = "vlq_mlp_3by10_062819.json";
std::ifstream input_cfg( dnnFile );
lwt::JSONConfig cfg = lwt::parse_json(input_cfg);
lwt::LightweightNeuralNetwork* lwtnn = new lwt::LightweightNeuralNetwork(cfg.inputs, cfg.layers, cfg.outputs);

// ----------------------------------------------------------------------------
// Define functions
// ----------------------------------------------------------------------------

bool comparepair( const std::pair<double,int> a, const std::pair<double,int> b) { return a.first > b.first; }

TRandom3 Rand;

const double MTOP  = 173.5;
const double MW    = 80.4; 

void step1::saveHistograms()
{
  TH1D* numhist = (TH1D*)inputFile->Get("mcweightanalyzer/NumTrueHist");
  TH1D* wgthist = (TH1D*)inputFile->Get("mcweightanalyzer/weightHist");
  outputFile->cd();
  numhist->Write();
  wgthist->Write();
}

 TH2D *TTconfusionD = new TH2D("TTconfusionD",";tagged decay;true decay",10,0,10,6,0,6);
 TH2D *TTconfusionN = new TH2D("TTconfusionN",";tagged decay;true decay",10,0,10,6,0,6);
 TH2D *BBconfusionD = new TH2D("BBconfusionD",";tagged decay;true decay",7,0,7,6,0,6);
 TH2D *BBconfusionN = new TH2D("BBconfusionN",";tagged decay;true decay",7,0,7,6,0,6);

// ----------------------------------------------------------------------------
// MAIN EVENT LOOP
// ----------------------------------------------------------------------------

void step1::Loop(TString inTreeName, TString outTreeName) 
{
  
  // ----------------------------------------------------------------------------
  // Turn on input tree branches
  // ----------------------------------------------------------------------------
  

   inputTree = (TTree*)inputFile->Get(inTreeName+"/"+inTreeName);
   if (inputTree == 0) return;
   if(inputTree->GetEntries() == 0){
     std::cout << "WARNING! Found 0 events in the tree!!!" << std::endl;
     return;
   }

   Init(inputTree);
   std::map<std::string,double> myMap;
   std::map<std::string,double> varMap;
   inputTree->SetBranchStatus("*",0);

   //Event info

   inputTree->SetBranchStatus("event_CommonCalc",1);
   inputTree->SetBranchStatus("run_CommonCalc",1);
   inputTree->SetBranchStatus("lumi_CommonCalc",1);
   //   inputTree->SetBranchStatus("nPV_MultiLepCalc",1);
   inputTree->SetBranchStatus("nTrueInteractions_MultiLepCalc",1);
   inputTree->SetBranchStatus("MCWeight_MultiLepCalc",1);
   inputTree->SetBranchStatus("LHEweightids_MultiLepCalc",1);
   inputTree->SetBranchStatus("LHEweights_MultiLepCalc",1);
   inputTree->SetBranchStatus("NewPDFweights_MultiLepCalc",1);
   inputTree->SetBranchStatus("HTfromHEPUEP_MultiLepCalc",1);
   inputTree->SetBranchStatus("L1NonPrefiringProb_CommonCalc",1);
   inputTree->SetBranchStatus("L1NonPrefiringProbUp_CommonCalc",1);
   inputTree->SetBranchStatus("L1NonPrefiringProbDown_CommonCalc",1);

   inputTree->SetBranchStatus("isTHBW_TpTpCalc",1);
   inputTree->SetBranchStatus("isTHTH_TpTpCalc",1);
   inputTree->SetBranchStatus("isBWBW_TpTpCalc",1);
   inputTree->SetBranchStatus("isTZBW_TpTpCalc",1);
   inputTree->SetBranchStatus("isTZTH_TpTpCalc",1);
   inputTree->SetBranchStatus("isTZTZ_TpTpCalc",1);
   inputTree->SetBranchStatus("isBHTW_TpTpCalc",1);
   inputTree->SetBranchStatus("isBHBH_TpTpCalc",1);
   inputTree->SetBranchStatus("isTWTW_TpTpCalc",1);
   inputTree->SetBranchStatus("isBZTW_TpTpCalc",1);
   inputTree->SetBranchStatus("isBZBH_TpTpCalc",1);
   inputTree->SetBranchStatus("isBZBZ_TpTpCalc",1);
   inputTree->SetBranchStatus("tPrimePt_TpTpCalc",1);
   inputTree->SetBranchStatus("NLeptonDecays_TpTpCalc",1);
   
   //triggers
   inputTree->SetBranchStatus("vsSelMCTriggersEl_MultiLepCalc",1);
   inputTree->SetBranchStatus("viSelMCTriggersEl_MultiLepCalc",1);
   inputTree->SetBranchStatus("vsSelMCTriggersMu_MultiLepCalc",1);
   inputTree->SetBranchStatus("viSelMCTriggersMu_MultiLepCalc",1);
   inputTree->SetBranchStatus("vsSelTriggersEl_MultiLepCalc",1);
   inputTree->SetBranchStatus("viSelTriggersEl_MultiLepCalc",1);
   inputTree->SetBranchStatus("vsSelTriggersMu_MultiLepCalc",1);
   inputTree->SetBranchStatus("viSelTriggersMu_MultiLepCalc",1);
   
   //electrons
   inputTree->SetBranchStatus("elPt_MultiLepCalc",1);
   inputTree->SetBranchStatus("elEta_MultiLepCalc",1);
   inputTree->SetBranchStatus("elPhi_MultiLepCalc",1);
   inputTree->SetBranchStatus("elEnergy_MultiLepCalc",1);
   //   inputTree->SetBranchStatus("elMVAValue_MultiLepCalc",1);
   inputTree->SetBranchStatus("elMiniIso_MultiLepCalc",1);
   
   //muons
   inputTree->SetBranchStatus("muPt_MultiLepCalc",1);
   inputTree->SetBranchStatus("muEta_MultiLepCalc",1);
   inputTree->SetBranchStatus("muPhi_MultiLepCalc",1);
   inputTree->SetBranchStatus("muEnergy_MultiLepCalc",1);
   inputTree->SetBranchStatus("muMiniIso_MultiLepCalc",1);

   //missing et
   inputTree->SetBranchStatus("corr_met_MultiLepCalc",1);
   inputTree->SetBranchStatus("corr_met_phi_MultiLepCalc",1);
   inputTree->SetBranchStatus("corr_metmod_MultiLepCalc",1);
   inputTree->SetBranchStatus("corr_metmod_phi_MultiLepCalc",1);

   //boosted truth
   inputTree->SetBranchStatus("HadronicVHtPt_JetSubCalc",1);
   inputTree->SetBranchStatus("HadronicVHtEta_JetSubCalc",1);
   inputTree->SetBranchStatus("HadronicVHtPhi_JetSubCalc",1);
   inputTree->SetBranchStatus("HadronicVHtEnergy_JetSubCalc",1);
   inputTree->SetBranchStatus("HadronicVHtID_JetSubCalc",1);
   inputTree->SetBranchStatus("HadronicVHtD0Pt_JetSubCalc",1);
   inputTree->SetBranchStatus("HadronicVHtD0Eta_JetSubCalc",1);
   inputTree->SetBranchStatus("HadronicVHtD0Phi_JetSubCalc",1);
   inputTree->SetBranchStatus("HadronicVHtD0E_JetSubCalc",1);
   inputTree->SetBranchStatus("HadronicVHtD1Pt_JetSubCalc",1);
   inputTree->SetBranchStatus("HadronicVHtD1Eta_JetSubCalc",1);
   inputTree->SetBranchStatus("HadronicVHtD1Phi_JetSubCalc",1);
   inputTree->SetBranchStatus("HadronicVHtD1E_JetSubCalc",1);
   inputTree->SetBranchStatus("HadronicVHtD2Pt_JetSubCalc",1);
   inputTree->SetBranchStatus("HadronicVHtD2Eta_JetSubCalc",1);
   inputTree->SetBranchStatus("HadronicVHtD2Phi_JetSubCalc",1);
   inputTree->SetBranchStatus("HadronicVHtD2E_JetSubCalc",1);

   //JetSubCalc
   inputTree->SetBranchStatus("theJetHFlav_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetPFlav_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetPt_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetEta_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetPhi_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetEnergy_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetDeepCSVb_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetDeepCSVbb_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8DoubleB_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetBTag_bSFup_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetBTag_bSFdn_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetBTag_lSFup_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetBTag_lSFdn_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetBTag_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8Pt_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8Eta_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8Phi_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8Mass_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8Energy_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8NjettinessTau1_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8NjettinessTau2_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8NjettinessTau3_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8CHSTau1_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8CHSTau2_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8CHSTau3_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8CHSPrunedMass_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8CHSSoftDropMass_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8SoftDropRaw_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8SoftDropCorr_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8SoftDrop_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8SDSubjetNDeepCSVMSF_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8SDSubjetNDeepCSVL_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8SDSubjetHFlav_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8SDSubjetIndex_JetSubCalc",1);
   inputTree->SetBranchStatus("theJetAK8SDSubjetSize_JetSubCalc",1);

   //DeepAK8
   inputTree->SetBranchStatus("dnn_B_DeepAK8Calc",1);
   inputTree->SetBranchStatus("dnn_C_DeepAK8Calc",1);
   inputTree->SetBranchStatus("dnn_J_DeepAK8Calc",1);
   inputTree->SetBranchStatus("dnn_W_DeepAK8Calc",1);
   inputTree->SetBranchStatus("dnn_Z_DeepAK8Calc",1);
   inputTree->SetBranchStatus("dnn_H_DeepAK8Calc",1);
   inputTree->SetBranchStatus("dnn_largest_DeepAK8Calc",1);
   inputTree->SetBranchStatus("dnn_T_DeepAK8Calc",1);

   //JetSubCalc
   inputTree->SetBranchStatus("maxProb_JetSubCalc",1);

   //top
   inputTree->SetBranchStatus("ttbarMass_TTbarMassCalc",1);
   inputTree->SetBranchStatus("topMass_TTbarMassCalc",1);

   inputTree->SetBranchStatus("allTopsEnergy_TTbarMassCalc",1);
   inputTree->SetBranchStatus("allTopsEta_TTbarMassCalc",1);
   inputTree->SetBranchStatus("allTopsPhi_TTbarMassCalc",1);
   inputTree->SetBranchStatus("allTopsPt_TTbarMassCalc",1);
   inputTree->SetBranchStatus("allTopsID_TTbarMassCalc",1);
   inputTree->SetBranchStatus("allTopsStatus_TTbarMassCalc",1);
   inputTree->SetBranchStatus("isTau_MultiLepCalc",1);

   // ----------------------------------------------------------------------------
   // Create output tree and define branches
   // ----------------------------------------------------------------------------
   
   // OUTPUT FILE
   outputFile->cd();
   TTree *outputTree = new TTree(outTreeName,outTreeName);

   // Common things
   outputTree->Branch("event_CommonCalc",&event_CommonCalc,"event_CommonCalc/L");
   outputTree->Branch("run_CommonCalc",&run_CommonCalc,"run_CommonCalc/I");
   outputTree->Branch("lumi_CommonCalc",&lumi_CommonCalc,"lumi_CommonCalc/I");
   //   outputTree->Branch("nPV_MultiLepCalc",&nPV_MultiLepCalc,"nPV_MultiLepCalc/I");
   outputTree->Branch("nTrueInteractions_MultiLepCalc",&nTrueInteractions_MultiLepCalc,"nTrueInteractions_MultiLepCalc/I");
   outputTree->Branch("isElectron",&isElectron,"isElectron/I");
   outputTree->Branch("isMuon",&isMuon,"isMuon/I");
   outputTree->Branch("MCPastTrigger",&MCPastTrigger,"MCPastTrigger/I");
   outputTree->Branch("DataPastTrigger",&DataPastTrigger,"DataPastTrigger/I");
   outputTree->Branch("L1NonPrefiringProb_CommonCalc",&L1NonPrefiringProb_CommonCalc,"L1NonPrefiringProb_CommonCalc/D");
   outputTree->Branch("L1NonPrefiringProbUp_CommonCalc",&L1NonPrefiringProbUp_CommonCalc,"L1NonPrefiringProbUp_CommonCalc/D");
   outputTree->Branch("L1NonPrefiringProbDown_CommonCalc",&L1NonPrefiringProbDown_CommonCalc,"L1NonPrefiringProbDown_CommonCalc/D");

   // Tprime generator
   outputTree->Branch("isTHBW_TpTpCalc",&isTHBW_TpTpCalc,"isTHBW_TpTpCalc/O");
   outputTree->Branch("isTHTH_TpTpCalc",&isTHTH_TpTpCalc,"isTHTH_TpTpCalc/O");
   outputTree->Branch("isBWBW_TpTpCalc",&isBWBW_TpTpCalc,"isBWBW_TpTpCalc/O");
   outputTree->Branch("isTZBW_TpTpCalc",&isTZBW_TpTpCalc,"isTZBW_TpTpCalc/O");
   outputTree->Branch("isTZTH_TpTpCalc",&isTZTH_TpTpCalc,"isTZTH_TpTpCalc/O");
   outputTree->Branch("isTZTZ_TpTpCalc",&isTZTZ_TpTpCalc,"isTZTZ_TpTpCalc/O");
   outputTree->Branch("isBHTW_TpTpCalc",&isBHTW_TpTpCalc,"isBHTW_TpTpCalc/O");
   outputTree->Branch("isBHBH_TpTpCalc",&isBHBH_TpTpCalc,"isBHBH_TpTpCalc/O");
   outputTree->Branch("isTWTW_TpTpCalc",&isTWTW_TpTpCalc,"isTWTW_TpTpCalc/O");
   outputTree->Branch("isBZTW_TpTpCalc",&isBZTW_TpTpCalc,"isBZTW_TpTpCalc/O");
   outputTree->Branch("isBZBH_TpTpCalc",&isBZBH_TpTpCalc,"isBZBH_TpTpCalc/O");
   outputTree->Branch("isBZBZ_TpTpCalc",&isBZBZ_TpTpCalc,"isBZBZ_TpTpCalc/O");
   outputTree->Branch("tPrimePt_TpTpCalc",&tPrimePt_TpTpCalc,"tPrimePt_TpTpCalc/O");
   outputTree->Branch("NLeptonDecays_TpTpCalc",&NLeptonDecays_TpTpCalc,"NLeptonDecays_TpTpCalc/I");
   outputTree->Branch("MCWeight_MultiLepCalc",&MCWeight_MultiLepCalc,"MCWeight_MultiLepCalc/D");

   // weights
   outputTree->Branch("renormWeights",&renormWeights);
   outputTree->Branch("pdfWeights",&pdfWeights);
   outputTree->Branch("pdfNewWeights",&pdfNewWeights);
   outputTree->Branch("pdfNewNominalWeight",&pdfNewNominalWeight,"pdfNewNominalWeight/F");
   outputTree->Branch("pileupWeight",&pileupWeight,"pileupWeight/F");
   outputTree->Branch("pileupWeightUp",&pileupWeightUp,"pileupWeightUp/F");
   outputTree->Branch("pileupWeightDown",&pileupWeightDown,"pileupWeightDown/F");
   outputTree->Branch("HTSF_Pol",&HTSF_Pol,"HTSF_Pol/F");
   outputTree->Branch("HTSF_PolUp",&HTSF_PolUp,"HTSF_PolUp/F");
   outputTree->Branch("HTSF_PolDn",&HTSF_PolDn,"HTSF_PolDn/F");
   outputTree->Branch("topPtWeight13TeV",&topPtWeight13TeV,"topPtWeight13TeV/F");
   outputTree->Branch("EGammaGsfSF",&EGammaGsfSF,"EGammaGsfSF/F");
   outputTree->Branch("lepIdSF",&lepIdSF,"lepIdSF/F");

   // ttbar generator
   outputTree->Branch("ttbarMass_TTbarMassCalc",&ttbarMass_TTbarMassCalc,"ttbarMass_TTbarMassCalc/D");
   outputTree->Branch("genTopPt",&genTopPt,"genTopPt/F");
   outputTree->Branch("genAntiTopPt",&genAntiTopPt,"genAntiTopPt/F");

   // leptons
   outputTree->Branch("isTau_MultiLepCalc",&isTau_MultiLepCalc,"isTau_MultiLepCalc/O");
   outputTree->Branch("corr_met_MultiLepCalc",&corr_met_MultiLepCalc,"corr_met_MultiLepCalc/D");
   outputTree->Branch("corr_met_phi_MultiLepCalc",&corr_met_phi_MultiLepCalc,"corr_met_phi_MultiLepCalc/D");
   outputTree->Branch("corr_metmod_MultiLepCalc",&corr_metmod_MultiLepCalc,"corr_metmod_MultiLepCalc/D");
   outputTree->Branch("corr_metmod_phi_MultiLepCalc",&corr_metmod_phi_MultiLepCalc,"corr_metmod_phi_MultiLepCalc/D");
   outputTree->Branch("leptonPt_MultiLepCalc",&leptonPt_MultiLepCalc,"leptonPt_MultiLepCalc/F");
   outputTree->Branch("leptonEta_MultiLepCalc",&leptonEta_MultiLepCalc,"leptonEta_MultiLepCalc/F");
   outputTree->Branch("leptonPhi_MultiLepCalc",&leptonPhi_MultiLepCalc,"leptonPhi_MultiLepCalc/F");
   outputTree->Branch("leptonEnergy_MultiLepCalc",&leptonEnergy_MultiLepCalc,"leptonEnergy_MultiLepCalc/F");
   outputTree->Branch("leptonMVAValue_MultiLepCalc",&leptonMVAValue_MultiLepCalc,"leptonMVAValue_MultiLepCalc/F");
   outputTree->Branch("leptonMiniIso_MultiLepCalc",&leptonMiniIso_MultiLepCalc,"leptonMiniIso_MultiLepCalc/F");
   outputTree->Branch("MT_lepMet",&MT_lepMet,"MT_lepMet/F");
   outputTree->Branch("MT_lepMetmod",&MT_lepMetmod,"MT_lepMetmod/F");
   outputTree->Branch("minDPhi_MetJet",&minDPhi_MetJet,"minDPhi_MetJet/F");

   // AK4
   outputTree->Branch("theJetPt_JetSubCalc_PtOrdered",&theJetPt_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetEta_JetSubCalc_PtOrdered",&theJetEta_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetPhi_JetSubCalc_PtOrdered",&theJetPhi_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetEnergy_JetSubCalc_PtOrdered",&theJetEnergy_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetDeepCSVb_JetSubCalc_PtOrdered",&theJetDeepCSVb_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetDeepCSVbb_JetSubCalc_PtOrdered",&theJetDeepCSVbb_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetHFlav_JetSubCalc_PtOrdered",&theJetHFlav_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetPFlav_JetSubCalc_PtOrdered",&theJetPFlav_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetBTag_JetSubCalc_PtOrdered",&theJetBTag_JetSubCalc_PtOrdered);
   outputTree->Branch("AK4HTpMETpLepPt",&AK4HTpMETpLepPt,"AK4HTpMETpLepPt/F");
   outputTree->Branch("AK4HT",&AK4HT,"AK4HT/F");
   outputTree->Branch("NJets_JetSubCalc",&NJets_JetSubCalc,"NJets_JetSubCalc/I");
   outputTree->Branch("NJetsCSV_JetSubCalc",&NJetsCSV_JetSubCalc,"NJetsCSV_JetSubCalc/I");
   outputTree->Branch("NJetsCSVwithSF_JetSubCalc",&NJetsCSVwithSF_JetSubCalc,"NJetsCSVwithSF_JetSubCalc/I");
   outputTree->Branch("deltaR_lepMinMlb",&deltaR_lepMinMlb,"deltaR_lepMinMlb/F");
   outputTree->Branch("deltaR_lepMinMlj",&deltaR_lepMinMlj,"deltaR_lepMinMlj/F");
   outputTree->Branch("minMleppBjet",&minMleppBjet,"minMleppBjet/F");
   outputTree->Branch("minMleppJet",&minMleppJet,"mixnMleppJet/F");
   outputTree->Branch("minDR_lepJet",&minDR_lepJet,"minDR_lepJet/F");
   outputTree->Branch("ptRel_lepJet",&ptRel_lepJet,"ptRel_lepJet/F");
   outputTree->Branch("deltaR_lepJets",&deltaR_lepJets);
   outputTree->Branch("deltaR_lepBJets",&deltaR_lepBJets);

   // jet truth
   outputTree->Branch("HadronicVHtID_JetSubCalc",&HadronicVHtID_JetSubCalc);
   outputTree->Branch("HadronicVHtPt_JetSubCalc",&HadronicVHtPt_JetSubCalc);
   outputTree->Branch("HadronicVHtEta_JetSubCalc",&HadronicVHtEta_JetSubCalc);
   outputTree->Branch("HadronicVHtPhi_JetSubCalc",&HadronicVHtPhi_JetSubCalc);
   outputTree->Branch("HadronicVHtEnergy_JetSubCalc",&HadronicVHtEnergy_JetSubCalc);
   outputTree->Branch("theJetAK8Wmatch_JetSubCalc_PtOrdered",&theJetAK8Wmatch_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8Tmatch_JetSubCalc_PtOrdered",&theJetAK8Tmatch_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8Zmatch_JetSubCalc_PtOrdered",&theJetAK8Zmatch_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8Hmatch_JetSubCalc_PtOrdered",&theJetAK8Hmatch_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8MatchedPt_JetSubCalc_PtOrdered",&theJetAK8MatchedPt_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8Truth_JetSubCalc_PtOrdered",&theJetAK8Truth_JetSubCalc_PtOrdered);

   // AK8
   outputTree->Branch("NJetsAK8_JetSubCalc",&NJetsAK8_JetSubCalc,"NJetsAK8_JetSubCalc/I");
   outputTree->Branch("theJetAK8Pt_JetSubCalc_PtOrdered",&theJetAK8Pt_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8Eta_JetSubCalc_PtOrdered",&theJetAK8Eta_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8Phi_JetSubCalc_PtOrdered",&theJetAK8Phi_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8Mass_JetSubCalc_PtOrdered",&theJetAK8Mass_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8Energy_JetSubCalc_PtOrdered",&theJetAK8Energy_JetSubCalc_PtOrdered);
   outputTree->Branch("minDR_leadAK8otherAK8",&minDR_leadAK8otherAK8,"minDR_leadAK8otherAK8/F");
   outputTree->Branch("minDR_lepAK8",&minDR_lepAK8,"minDR_lepAK8/F");
   outputTree->Branch("ptRel_lepAK8",&ptRel_lepAK8,"ptRel_lepAK8/F");
   outputTree->Branch("deltaR_lepAK8s",&deltaR_lepAK8s);

   // mass + sub tagging 
   outputTree->Branch("maxProb_JetSubCalc_PtOrdered",&maxProb_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8CHSPrunedMass_JetSubCalc_PtOrdered",&theJetAK8CHSPrunedMass_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8CHSSoftDropMass_JetSubCalc_PtOrdered",&theJetAK8CHSSoftDropMass_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8SoftDropRaw_JetSubCalc_PtOrdered",&theJetAK8SoftDropRaw_PtOrdered);
   outputTree->Branch("theJetAK8SoftDropCorr_JetSubCalc_PtOrdered",&theJetAK8SoftDropCorr_PtOrdered);
   outputTree->Branch("theJetAK8DoubleB_JetSubCalc_PtOrdered",&theJetAK8DoubleB_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8SoftDrop_PtOrdered",&theJetAK8SoftDrop_PtOrdered);
   outputTree->Branch("theJetAK8NjettinessTau1_JetSubCalc_PtOrdered",&theJetAK8NjettinessTau1_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8NjettinessTau2_JetSubCalc_PtOrdered",&theJetAK8NjettinessTau2_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8NjettinessTau3_JetSubCalc_PtOrdered",&theJetAK8NjettinessTau3_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8CHSTau1_JetSubCalc_PtOrdered",&theJetAK8CHSTau1_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8CHSTau2_JetSubCalc_PtOrdered",&theJetAK8CHSTau2_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8CHSTau3_JetSubCalc_PtOrdered",&theJetAK8CHSTau3_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8SDSubjetIndex_JetSubCalc_PtOrdered",&theJetAK8SDSubjetIndex_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8SDSubjetSize_JetSubCalc_PtOrdered",&theJetAK8SDSubjetSize_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8SDSubjetNDeepCSVMSF_JetSubCalc_PtOrdered",&theJetAK8SDSubjetNDeepCSVMSF_JetSubCalc_PtOrdered);
   outputTree->Branch("theJetAK8SDSubjetNDeepCSVL_JetSubCalc_PtOrdered",&theJetAK8SDSubjetNDeepCSVL_JetSubCalc_PtOrdered);

   // DeepAK8 tagging
   outputTree->Branch("dnn_largest_DeepAK8Calc_PtOrdered",&dnn_largest_DeepAK8Calc_PtOrdered);
   outputTree->Branch("dnn_B_DeepAK8Calc_PtOrdered",&dnn_B_DeepAK8Calc_PtOrdered);
   outputTree->Branch("dnn_J_DeepAK8Calc_PtOrdered",&dnn_J_DeepAK8Calc_PtOrdered);
   outputTree->Branch("dnn_W_DeepAK8Calc_PtOrdered",&dnn_W_DeepAK8Calc_PtOrdered);
   outputTree->Branch("dnn_Z_DeepAK8Calc_PtOrdered",&dnn_Z_DeepAK8Calc_PtOrdered);
   outputTree->Branch("dnn_H_DeepAK8Calc_PtOrdered",&dnn_H_DeepAK8Calc_PtOrdered);
   outputTree->Branch("dnn_T_DeepAK8Calc_PtOrdered",&dnn_T_DeepAK8Calc_PtOrdered);
   outputTree->Branch("probSum_DeepAK8_decay",&probSum_DeepAK8_decay,"probSum_DeepAK8_decay/F");
   outputTree->Branch("probSum_DeepAK8_all",&probSum_DeepAK8_all,"probSum_DeepAK8_all/F");
   outputTree->Branch("probSum_DeepAK8_four",&probSum_DeepAK8_four,"probSum_DeepAK8_four/F");
   outputTree->Branch("nB_DeepAK8",&nB_DeepAK8,"nB_DeepAK8/I");
   outputTree->Branch("nH_DeepAK8",&nH_DeepAK8,"nH_DeepAK8/I");
   outputTree->Branch("nJ_DeepAK8",&nJ_DeepAK8,"nJ_DeepAK8/I");
   outputTree->Branch("nT_DeepAK8",&nT_DeepAK8,"nT_DeepAK8/I");
   outputTree->Branch("nW_DeepAK8",&nW_DeepAK8,"nW_DeepAK8/I");
   outputTree->Branch("nZ_DeepAK8",&nZ_DeepAK8,"nZ_DeepAK8/I");

   // VLQ reco   
   outputTree->Branch("W_mass",&W_mass,"W_mass/F");
   outputTree->Branch("W_pt",&W_pt,"W_pt/F");
   outputTree->Branch("W_dRLep",&W_dRLep,"W_dRLep/F");
   outputTree->Branch("t_mass",&t_mass,"t_mass/F");
   outputTree->Branch("t_pt",&t_pt,"t_pt/F");
   outputTree->Branch("t_dRWb",&t_dRWb,"t_dRWb/F");
   outputTree->Branch("deltaR_leptonicparticle_AK8_PtOrdered",&deltaR_leptonicparticle_AK8_PtOrdered);
   outputTree->Branch("isLeptonic_t",&isLeptonic_t,"isLeptonic_t/O");
   outputTree->Branch("isLeptonic_W",&isLeptonic_W,"isLeptonic_W/O");
   outputTree->Branch("Tprime1_DeepAK8_Mass",&Tprime1_DeepAK8_Mass,"Tprime1_DeepAK8_Mass/F");
   outputTree->Branch("Tprime2_DeepAK8_Mass",&Tprime2_DeepAK8_Mass,"Tprime2_DeepAK8_Mass/F");
   outputTree->Branch("Tprime1_DeepAK8_Pt",&Tprime1_DeepAK8_Pt,"Tprime1_DeepAK8_Pt/F");
   outputTree->Branch("Tprime2_DeepAK8_Pt",&Tprime2_DeepAK8_Pt,"Tprime2_DeepAK8_Pt/F");
   outputTree->Branch("Tprime1_DeepAK8_Eta",&Tprime1_DeepAK8_Eta,"Tprime1_DeepAK8_Eta/F");
   outputTree->Branch("Tprime2_DeepAK8_Eta",&Tprime2_DeepAK8_Eta,"Tprime2_DeepAK8_Eta/F");
   outputTree->Branch("Tprime1_DeepAK8_Phi",&Tprime1_DeepAK8_Phi,"Tprime1_DeepAK8_Phi/F");
   outputTree->Branch("Tprime2_DeepAK8_Phi",&Tprime2_DeepAK8_Phi,"Tprime2_DeepAK8_Phi/F");
   outputTree->Branch("Tprime1_DeepAK8_deltaR",&Tprime1_DeepAK8_deltaR,"Tprime1_DeepAK8_deltaR/F");
   outputTree->Branch("Tprime2_DeepAK8_deltaR",&Tprime2_DeepAK8_deltaR,"Tprime2_DeepAK8_deltaR/F");
   outputTree->Branch("isValidTTDecayMode_DeepAK8",&validTDecay_DeepAK8,"validTDecay_DeepAK8/O");
   outputTree->Branch("taggedBWBW_DeepAK8",&taggedBWBW_DeepAK8,"taggedBWBW_DeepAK8/O");
   outputTree->Branch("taggedTHBW_DeepAK8",&taggedTHBW_DeepAK8,"taggedTHBW_DeepAK8/O");
   outputTree->Branch("taggedTHTH_DeepAK8",&taggedTHTH_DeepAK8,"taggedTHTH_DeepAK8/O");
   outputTree->Branch("taggedTZBW_DeepAK8",&taggedTZBW_DeepAK8,"taggedTZBW_DeepAK8/O");
   outputTree->Branch("taggedTZTH_DeepAK8",&taggedTZTH_DeepAK8,"taggedTZTH_DeepAK8/O");
   outputTree->Branch("taggedTZTZ_DeepAK8",&taggedTZTZ_DeepAK8,"taggedTZTZ_DeepAK8/O");
   outputTree->Branch("highPtAK8Jet1_SoftDropCorrectedMass",&highPtAK8Jet1_SoftDropCorrectedMass,"highPtAK8Jet1_SoftDropCorrectedMass/F");
   outputTree->Branch("highPtAK8Jet2_SoftDropCorrectedMass",&highPtAK8Jet2_SoftDropCorrectedMass,"highPtAK8Jet2_SoftDropCorrectedMass/F");
   outputTree->Branch("highPtAK8Jet3_SoftDropCorrectedMass",&highPtAK8Jet3_SoftDropCorrectedMass,"highPtAK8Jet3_SoftDropCorrectedMass/F");
   outputTree->Branch("leptonicTprimeJetIDs_DeepAK8",&leptonicTprimeJetIDs_DeepAK8);
   outputTree->Branch("hadronicTprimeJetIDs_DeepAK8",&hadronicTprimeJetIDs_DeepAK8);

   outputTree->Branch("Bprime1_DeepAK8_Mass",&Bprime1_DeepAK8_Mass,"Bprime1_DeepAK8_Mass/F");
   outputTree->Branch("Bprime2_DeepAK8_Mass",&Bprime2_DeepAK8_Mass,"Bprime2_DeepAK8_Mass/F");
   outputTree->Branch("Bprime1_DeepAK8_Pt",&Bprime1_DeepAK8_Pt,"Bprime1_DeepAK8_Pt/F");
   outputTree->Branch("Bprime2_DeepAK8_Pt",&Bprime2_DeepAK8_Pt,"Bprime2_DeepAK8_Pt/F");
   outputTree->Branch("Bprime1_DeepAK8_Eta",&Bprime1_DeepAK8_Eta,"Bprime1_DeepAK8_Eta/F");
   outputTree->Branch("Bprime2_DeepAK8_Eta",&Bprime2_DeepAK8_Eta,"Bprime2_DeepAK8_Eta/F");
   outputTree->Branch("Bprime1_DeepAK8_Phi",&Bprime1_DeepAK8_Phi,"Bprime1_DeepAK8_Phi/F");
   outputTree->Branch("Bprime2_DeepAK8_Phi",&Bprime2_DeepAK8_Phi,"Bprime2_DeepAK8_Phi/F");
   outputTree->Branch("Bprime1_DeepAK8_deltaR",&Bprime1_DeepAK8_deltaR,"Bprime1_DeepAK8_deltaR/F");
   outputTree->Branch("Bprime2_DeepAK8_deltaR",&Bprime2_DeepAK8_deltaR,"Bprime2_DeepAK8_deltaR/F");
   outputTree->Branch("isValidBBDecayMode_DeepAK8",&validBDecay_DeepAK8,"validBDecay_DeepAK8/O");
   outputTree->Branch("taggedTWTW_DeepAK8",&taggedTWTW_DeepAK8,"taggedTWTW_DeepAK8/O");
   outputTree->Branch("taggedBHTW_DeepAK8",&taggedBHTW_DeepAK8,"taggedBZTW_DeepAK8/O");
   outputTree->Branch("taggedBZTW_DeepAK8",&taggedBZTW_DeepAK8,"taggedBHTW_DeepAK8/O");
   outputTree->Branch("leptonicBprimeJetIDs_DeepAK8",&leptonicBprimeJetIDs_DeepAK8);
   outputTree->Branch("hadronicBprimeJetIDs_DeepAK8",&hadronicBprimeJetIDs_DeepAK8);

   // VLQ dnn output
   outputTree->Branch("dnn_WJets",&dnn_WJets,"dnn_WJets/F");
   outputTree->Branch("dnn_ttbar",&dnn_ttbar,"dnn_ttbar/F");
   outputTree->Branch("dnn_Tprime",&dnn_Tprime,"dnn_Tprime/F");

   // ----------------------------------------------------------------------------
   // Define and initialize objects / cuts / efficiencies
   // ----------------------------------------------------------------------------

   // basic cuts
   float metCut=50;
   int   htCut=510;
   int   nAK8jetsCut=0;
   float lepPtCut=50;
   float elEtaCut=2.5;
   float muEtaCut=2.4;
   float jetEtaCut=2.4;
   float ak8EtaCut=2.4;
   float jetPtCut=30;
   float ak8PtCut=200;

   // counters
   int n_vectorSizeMismatch = 0;

   int n_jetstotal        = 0;
   int n_jetsnearlep      = 0;
   int n_jetspassed       = 0;
   int npass_ThreeJets    = 0;
   int npass_trigger      = 0;
   int npass_mu500        = 0;
   int npass_met          = 0;
   int npass_ht           = 0;
   int npass_nAK8jets     = 0;
   int npass_nHjets       = 0;
   int npass_lepPt        = 0;
   int npass_ElEta        = 0;
   int npass_MuEta        = 0;
   int npass_all          = 0;
   int Nelectrons         = 0;
   int Nmuons             = 0;

   // Lorentz vectors
   TLorentzVector jet_lv;
   TLorentzVector lepton_lv;
   TLorentzVector ak8_lv;
   

   // Pileup distributions -- 31Mar2018 Data vs RunIIFall17MC  PLACEHOLDER IN 2018!!!
   std::vector<float> pileupweight;
   std::vector<float> pileupweightUp;
   std::vector<float> pileupweightDn;

   pileupweight = { 2.326E-04, 2.787E-02, 3.339E-02, 5.035E-02, 6.247E-02, 9.966E-02, 1.074E-01, 1.442E-01, 1.041E-01, 3.121E-01, 4.634E-01, 6.124E-01, 6.707E-01, 6.990E-01, 7.462E-01, 8.168E-01, 9.110E-01, 1.022E+00, 1.100E+00, 1.156E+00, 1.212E+00, 1.261E+00, 1.307E+00, 1.336E+00, 1.353E+00, 1.354E+00, 1.353E+00, 1.360E+00, 1.376E+00, 1.354E+00, 1.326E+00, 1.264E+00, 1.198E+00, 1.132E+00, 1.062E+00, 9.996E-01, 9.498E-01, 9.122E-01, 8.609E-01, 8.217E-01, 8.276E-01, 8.657E-01, 9.451E-01, 1.048E+00, 1.196E+00, 1.360E+00, 1.538E+00, 1.600E+00, 1.634E+00, 1.581E+00, 1.403E+00, 1.211E+00, 9.999E-01, 7.865E-01, 5.984E-01, 4.296E-01, 3.034E-01, 2.107E-01, 1.463E-01, 1.024E-01, 7.337E-02, 5.378E-02, 4.052E-02, 3.077E-02, 2.339E-02, 1.753E-02, 1.269E-02, 1.092E-02, 9.353E-03, 9.002E-03, 7.919E-03, 4.869E-03, 6.104E-03, 4.942E-03, 3.426E-03, 2.660E-03, 2.304E-03, 2.039E-03, 1.059E-03, 5.481E-04}; // TT1L.root // 69200 ub

   pileupweightDn = { 2.433E-04, 3.083E-02, 4.356E-02, 4.868E-02, 7.199E-02, 1.111E-01, 1.120E-01, 1.615E-01, 1.504E-01, 4.638E-01, 6.765E-01, 8.507E-01, 8.766E-01, 9.189E-01, 9.942E-01, 1.101E+00, 1.207E+00, 1.297E+00, 1.343E+00, 1.368E+00, 1.393E+00, 1.414E+00, 1.443E+00, 1.464E+00, 1.479E+00, 1.470E+00, 1.448E+00, 1.427E+00, 1.412E+00, 1.360E+00, 1.304E+00, 1.216E+00, 1.126E+00, 1.042E+00, 9.624E-01, 8.965E-01, 8.479E-01, 8.166E-01, 7.822E-01, 7.706E-01, 8.146E-01, 9.020E-01, 1.038E+00, 1.189E+00, 1.364E+00, 1.507E+00, 1.608E+00, 1.537E+00, 1.415E+00, 1.218E+00, 9.548E-01, 7.261E-01, 5.290E-01, 3.691E-01, 2.511E-01, 1.628E-01, 1.050E-01, 6.726E-02, 4.356E-02, 2.873E-02, 1.960E-02, 1.382E-02, 1.011E-02, 7.537E-03, 5.685E-03, 4.266E-03, 3.114E-03, 2.716E-03, 2.361E-03, 2.300E-03, 2.039E-03, 1.256E-03, 1.565E-03, 1.252E-03, 8.515E-04, 6.450E-04, 5.424E-04, 4.641E-04, 2.322E-04, 1.153E-04}; // TT1L.root  // 66016 ub

   pileupweightUp = {2.254E-04, 2.407E-02, 2.735E-02, 5.062E-02, 5.381E-02, 8.831E-02, 1.038E-01, 1.319E-01, 8.389E-02, 2.025E-01, 3.305E-01, 4.264E-01, 5.094E-01, 5.412E-01, 5.726E-01, 6.181E-01, 6.852E-01, 7.875E-01, 8.823E-01, 9.591E-01, 1.035E+00, 1.106E+00, 1.172E+00, 1.215E+00, 1.239E+00, 1.243E+00, 1.248E+00, 1.270E+00, 1.306E+00, 1.312E+00, 1.311E+00, 1.274E+00, 1.231E+00, 1.188E+00, 1.138E+00, 1.090E+00, 1.048E+00, 1.014E+00, 9.596E-01, 9.117E-01, 9.035E-01, 9.157E-01, 9.553E-01, 1.005E+00, 1.094E+00, 1.205E+00, 1.355E+00, 1.441E+00, 1.547E+00, 1.610E+00, 1.564E+00, 1.498E+00, 1.383E+00, 1.221E+00, 1.042E+00, 8.369E-01, 6.572E-01, 5.034E-01, 3.822E-01, 2.897E-01, 2.227E-01, 1.735E-01, 1.377E-01, 1.092E-01, 8.597E-02, 6.611E-02, 4.864E-02, 4.217E-02, 3.610E-02, 3.448E-02, 2.998E-02, 1.819E-02, 2.252E-02, 1.808E-02, 1.249E-02, 9.724E-03, 8.496E-03, 7.632E-03, 4.044E-03, 2.145E-03}; // TT1L.root // 72383 ub


   // Polynominals for WJets HT scaling
   TF1 *poly2 = new TF1("poly2","max([6],[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x + [5]*x*x*x*x*x)",100,5000);
   poly2->SetParameter(0,    0.998174);  
   poly2->SetParameter(1, 8.40861e-05); 
   poly2->SetParameter(2,-6.63274e-07);
   poly2->SetParameter(3, 4.09272e-10); 
   poly2->SetParameter(4,-9.50233e-14); 
   poly2->SetParameter(5, 7.59648e-18); 
   poly2->SetParameter(6,0.402806);
   
   TF1 *poly2U = new TF1("poly2U","max([6],[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x + [5]*x*x*x*x*x)",100,5000);
   poly2U->SetParameter(0,    0.998164);  
   poly2U->SetParameter(1, 8.51652e-05); 
   poly2U->SetParameter(2,-6.62919e-07);
   poly2U->SetParameter(3,  4.1205e-10); 
   poly2U->SetParameter(4,-9.57795e-14); 
   poly2U->SetParameter(5, 7.67371e-18); 
   poly2U->SetParameter(6,0.454456);
   
   TF1 *poly2D = new TF1("poly2D","max([6],[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x + [5]*x*x*x*x*x)",100,5000);
   poly2D->SetParameter(0,    0.998183);  
   poly2D->SetParameter(1, 8.30069e-05); 
   poly2D->SetParameter(2,-6.63629e-07);
   poly2D->SetParameter(3, 4.06495e-10); 
   poly2D->SetParameter(4,-9.42671e-14); 
   poly2D->SetParameter(5, 7.51924e-18); 
   poly2D->SetParameter(6,0.351156);

   
  // ----------------------------------------------------------------------------
  // RUN THE EVENT LOOP
  // ----------------------------------------------------------------------------

   cout << "RUN CONFIG: isMC = " << isMC << endl;
   cout << "isSig = " << isSig << ", SigMass = " << SigMass << endl;
   cout << "For W's: isTT = " << isTT << ", isSTt = " << isSTt << ", isSTtW = " << isSTtW << endl;
   cout << "Fot jets & PDF: isTOP = " << isTOP << ", isMadgraphBkg = " << isMadgraphBkg << endl;
   //cout << "Pileup index: " << pileupIndex << endl;
   
   Long64_t nentries = inputTree->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = inputTree->GetEntry(jentry);   nbytes += nb;
      if (Cut(ientry) != 1) continue;
      
      //      if (ientry > 5000) continue;
      
      if(jentry % 1000 ==0) std::cout<<"Completed "<<jentry<<" out of "<<nentries<<" events"<<std::endl;

      // ----------------------------------------------------------------------------
      // Filter input file by mass or decay
      // ----------------------------------------------------------------------------

      float confusionTruth = -1;
      
      if(isTTincMtt0to700 && ttbarMass_TTbarMassCalc >= 700.) continue;
      if(isTTincMtt0to1000 && ttbarMass_TTbarMassCalc >= 1000.) continue;
      if(isTTincMtt700to1000 && (ttbarMass_TTbarMassCalc < 700. || ttbarMass_TTbarMassCalc >= 1000.)) continue;
      if(isTTincMtt1000toInf && ttbarMass_TTbarMassCalc < 1000.) continue;
      if(outBWBW && !isBWBW_TpTpCalc) {confusionTruth = 0.5; continue;}
      if(outTZBW && !isTZBW_TpTpCalc) {confusionTruth = 1.5; continue;}
      if(outTHBW && !isTHBW_TpTpCalc) {confusionTruth = 2.5; continue;}
      if(outTZTH && !isTZTH_TpTpCalc) {confusionTruth = 3.5; continue;}
      if(outTZTZ && !isTZTZ_TpTpCalc) {confusionTruth = 4.5; continue;}
      if(outTHTH && !isTHTH_TpTpCalc) {confusionTruth = 5.5; continue;}
      if(outTWTW && !isTWTW_TpTpCalc) {confusionTruth = 6.5; continue;}
      if(outBZTW && !isBZTW_TpTpCalc) {confusionTruth = 7.5; continue;}
      if(outBHTW && !isBHTW_TpTpCalc) {confusionTruth = 8.5; continue;}
      if(outBZBH && !isBZBH_TpTpCalc) {confusionTruth = 9.5; continue;}
      if(outBZBZ && !isBZBZ_TpTpCalc) {confusionTruth = 10.5; continue;}
      if(outBHBH && !isBHBH_TpTpCalc) {confusionTruth = 11.5; continue;}

      if(isSig){
	if(confusionTruth < 6.0){
	  TTconfusionD->Fill(0.5,confusionTruth);
	  TTconfusionD->Fill(1.5,confusionTruth);
	  TTconfusionD->Fill(2.5,confusionTruth);
	  TTconfusionD->Fill(3.5,confusionTruth);
	  TTconfusionD->Fill(4.5,confusionTruth);
	  TTconfusionD->Fill(5.5,confusionTruth);
	  TTconfusionD->Fill(6.5,confusionTruth);
	  TTconfusionD->Fill(7.5,confusionTruth);
	  TTconfusionD->Fill(8.5,confusionTruth);
	  TTconfusionD->Fill(9.5,confusionTruth);
	}else{
	  BBconfusionD->Fill(0.5,confusionTruth-6.0);
	  BBconfusionD->Fill(1.5,confusionTruth-6.0);
	  BBconfusionD->Fill(2.5,confusionTruth-6.0);
	  BBconfusionD->Fill(3.5,confusionTruth-6.0);
	  BBconfusionD->Fill(4.5,confusionTruth-6.0);
	  BBconfusionD->Fill(5.5,confusionTruth-6.0);
	  BBconfusionD->Fill(6.5,confusionTruth-6.0);
	}
      }

      // ----------------------------------------------------------------------------
      // Assign as electron or muon event
      // ----------------------------------------------------------------------------
      // **** If we had saveLooseLeps == True in TTsinglelep_MC_cfg.py then this needs work!
      // **** Choose an electron and muon ID for "tight" and find events with only 1 tight el or mu by that definition
      isElectron = 0;
      isMuon = 0;
      if(elPt_MultiLepCalc->size()==0 && muPt_MultiLepCalc->size()==0){std::cout << "got no leptons, something wrong" << std::endl; continue;}
      if(elPt_MultiLepCalc->size()>0 && muPt_MultiLepCalc->size()>0) std::cout << "got el and mu, something wrong" << std::endl;
      if(elPt_MultiLepCalc->size()>1 || muPt_MultiLepCalc->size()>1) std::cout << "got more than one el or mu, something wrong" << std::endl;
      if(elPt_MultiLepCalc->size()>0 && muPt_MultiLepCalc->size()==0) {isElectron = 1; isMuon = 0;}
      if(elPt_MultiLepCalc->size()==0 && muPt_MultiLepCalc->size()>0) {isElectron = 0; isMuon = 1;}
      if(isElectron==0 && isMuon==0){std::cout << "got no leptons, something wrong" << std::endl; continue;}

      if(isSM && isElectron == 1) continue;
      if(isSE && isMuon == 1) continue;

      double leppt = 0;
      double lepeta = 0;
      if(isElectron){leppt = elPt_MultiLepCalc->at(0); lepeta = elEta_MultiLepCalc->at(0);}
      if(isMuon){leppt = muPt_MultiLepCalc->at(0); lepeta = muEta_MultiLepCalc->at(0);}

      // ----------------------------------------------------------------------------
      // Check jet size alignment
      // ----------------------------------------------------------------------------

      unsigned int ak8ptsize = theJetAK8Pt_JetSubCalc->size();
      unsigned int deepak8size = dnn_H_DeepAK8Calc->size();
      if(deepak8size != ak8ptsize){
	std::cout << "Something's wrong! DeepAK8 has " << deepak8size << " jets, expected " << ak8ptsize << std::endl;
	continue;
      }

      // ----------------------------------------------------------------------------
      // Pileup weight calculation
      // ----------------------------------------------------------------------------

      pileupWeight = 1.0;
      pileupWeightUp = 1.0;
      pileupWeightDown = 1.0;
	
      if(isMC){
	if(nTrueInteractions_MultiLepCalc > 99) nTrueInteractions_MultiLepCalc = 99;
	if(nTrueInteractions_MultiLepCalc < 0) nTrueInteractions_MultiLepCalc = 0;
	pileupWeight = pileupweight[nTrueInteractions_MultiLepCalc];
	pileupWeightUp = pileupweightUp[nTrueInteractions_MultiLepCalc];
	pileupWeightDown = pileupweightDn[nTrueInteractions_MultiLepCalc];
      }

      // ----------------------------------------------------------------------------
      // Assign Lepton scale factor or efficiency weights, save trigger pass/fail
      // ----------------------------------------------------------------------------

      DataPastTrigger = 0;
      MCPastTrigger = 0;
      EGammaGsfSF = 1.0;
      lepIdSF = 1.0;

      if(isMC){ //MC triggers check
	if(isElectron){
	  std::string string_a = "Ele15_IsoVVVL_PFHT450";
	  std::string string_b = "Ele15_IsoVVVL_PFHT450_PFMET50";
	  std::string string_c = "Ele50_IsoVVVL_PFHT450";
	  std::string string_d = "Ele15_IsoVVVL_PFHT600";
	  std::string string_ORa = "Ele35_WPTight_Gsf";
	  std::string string_ORb = "Ele38_WPTight_Gsf";
	  for(unsigned int itrig=0; itrig < vsSelMCTriggersEl_MultiLepCalc->size(); itrig++){
	    if(vsSelMCTriggersEl_MultiLepCalc->at(itrig).find(string_a) != std::string::npos && viSelMCTriggersEl_MultiLepCalc->at(itrig) > 0) MCPastTrigger = 1;
//	    if(vsSelMCTriggersEl_MultiLepCalc->at(itrig).find(string_b) != std::string::npos && viSelMCTriggersEl_MultiLepCalc->at(itrig) > 0) MCPastTrigger = 1;
            if(vsSelMCTriggersEl_MultiLepCalc->at(itrig).find(string_c) != std::string::npos && viSelMCTriggersEl_MultiLepCalc->at(itrig) > 0) MCPastTrigger = 1;
            if(vsSelMCTriggersEl_MultiLepCalc->at(itrig).find(string_d) != std::string::npos && viSelMCTriggersEl_MultiLepCalc->at(itrig) > 0) MCPastTrigger = 1;
	    if(vsSelMCTriggersEl_MultiLepCalc->at(itrig).find(string_ORa) != std::string::npos && viSelMCTriggersEl_MultiLepCalc->at(itrig) > 0) MCPastTrigger = 1;
	    if(vsSelMCTriggersEl_MultiLepCalc->at(itrig).find(string_ORb) != std::string::npos && viSelMCTriggersEl_MultiLepCalc->at(itrig) > 0) MCPastTrigger = 1;
	  }
	  // Gsf Tracking scale factor: https://twiki.cern.ch/twiki/pub/CMS/EgammaIDRecipesRun2/egammaEffi.txt_egammaPlots.pdf
	  if (leppt < 45) {
	    if (lepeta < -2.0) EGammaGsfSF = 0.989;
	    else if (lepeta < -1.566) EGammaGsfSF = 0.991;
	    else if (lepeta < -1.442) EGammaGsfSF = 0.982;
	    else if (lepeta < -1.0) EGammaGsfSF = 0.988;
	    else if (lepeta < -0.5) EGammaGsfSF = 0.990;
            else if (lepeta < 0.0) EGammaGsfSF = 0.986;
	    else if (lepeta < 0.5) EGammaGsfSF = 0.983;
	    else if (lepeta < 1.0) EGammaGsfSF = 0.987;
	    else if (lepeta < 1.442) EGammaGsfSF = 0.984;
	    else if (lepeta < 1.556) EGammaGsfSF = 0.985;
	    else if (lepeta < 2.0) EGammaGsfSF = 0.989;
	    else EGammaGsfSF = 0.992; }
	  else if (leppt < 75) {
	    if (lepeta < -2.0) EGammaGsfSF = 0.985;
	    else if (lepeta < -1.556) EGammaGsfSF = 0.991;
	    else if (lepeta < -1.442) EGammaGsfSF = 0.959;
	    else if (lepeta < -1.0) EGammaGsfSF = 0.989;
            else if (lepeta < -0.5) EGammaGsfSF = 0.991;
	    else if (lepeta < 0.0) EGammaGsfSF = 0.989;
	    else if (lepeta < 0.5) EGammaGsfSF = 0.987;
	    else if (lepeta < 1.0) EGammaGsfSF = 0.989;
	    else if (lepeta < 1.442) EGammaGsfSF = 0.982;
	    else if (lepeta < 1.566) EGammaGsfSF = 0.973;
	    else if (lepeta < 2.0) EGammaGsfSF = 0.991;
	    else EGammaGsfSF = 0.986; }
	  else if (leppt < 100) {
            if (lepeta < -2.0) EGammaGsfSF = 1.001;
	    else if (lepeta < -1.566) EGammaGsfSF = 1.006;
	    else if (lepeta < -1.442) EGammaGsfSF = 1.047;
	    else if (lepeta < -1.0) EGammaGsfSF = 1.005;
            else if (lepeta < -0.5) EGammaGsfSF = 1.002;
            else if (lepeta < 0.0) EGammaGsfSF = 1.006;
            else if (lepeta < 0.5) EGammaGsfSF = 1.006;
	    else if (lepeta < 1.0) EGammaGsfSF = 1.002;
	    else if (lepeta < 1.442) EGammaGsfSF = 1.005;
	    else if (lepeta < 1.566) EGammaGsfSF = 1.047;
            else if (lepeta < 2.0) EGammaGsfSF = 1.006;
	    else EGammaGsfSF = 1.001; }
	  else {
            if (lepeta < -2.0) EGammaGsfSF = 1.007;
	    else if (lepeta < -1.566) EGammaGsfSF = 0.992;
	    else if (lepeta < -1.442) EGammaGsfSF = 0.984;
	    else if (lepeta < -1.0) EGammaGsfSF = 1.001;
	    else if (lepeta < -0.5) EGammaGsfSF = 1.001;
            else if (lepeta < 0.0) EGammaGsfSF = 0.987;
	    else if (lepeta < 0.5) EGammaGsfSF = 0.987;
	    else if (lepeta < 1.0) EGammaGsfSF = 1.001;
	    else if (lepeta < 1.442) EGammaGsfSF = 1.001;
	    else if (lepeta < 1.566) EGammaGsfSF = 0.984;
	    else if (lepeta < 2.0) EGammaGsfSF = 0.992;
	    else EGammaGsfSF = 1.007; }

          //Scale factor 2: https://twiki.cern.ch/twiki/pub/CMS/EgammaIDRecipesRun2/2018_ElectronMVA90noiso_2D.pdf 
	  if (leppt < 20) {
	    if (lepeta < -2.0) lepIdSF = 1.000;
	    else if (lepeta < -1.566) lepIdSF = 0.973;
	    else if (lepeta < -1.442) lepIdSF = 1.000;
	    else if (lepeta < -0.8) lepIdSF = 1.016;
	    else if (lepeta < 0.0) lepIdSF = 0.958;
	    else if (lepeta < 0.8) lepIdSF = 0.990;
	    else if (lepeta < 1.442) lepIdSF = 1.012;
	    else if (lepeta < 1.566) lepIdSF = 1.000;
	    else if (lepeta < 2.0) lepIdSF = 0.978;
	    else lepIdSF = 0.995; }
	  else if (leppt < 35) {
	    if (lepeta < -2.0) lepIdSF = 0.973;
	    else if (lepeta < -1.566) lepIdSF = 0.951;
	    else if (lepeta < -1.442) lepIdSF = 1.000;
	    else if (lepeta < -0.8) lepIdSF = 0.965;
	    else if (lepeta < 0.0) lepIdSF = 0.970;
	    else if (lepeta < 0.8) lepIdSF = 0.973;
	    else if (lepeta < 1.442) lepIdSF = 0.965;
	    else if (lepeta < 1.566) lepIdSF = 1.000;
	    else if (lepeta < 2.0) lepIdSF = 0.948;
	    else lepIdSF = 0.963; }
	  else if (leppt < 50) {
	    if (lepeta < -2.0) lepIdSF = 0.975;
	    else if (lepeta < -1.566) lepIdSF = 0.966;
	    else if (lepeta < -1.442) lepIdSF = 1.000;
	    else if (lepeta < -0.8) lepIdSF = 0.972;
	    else if (lepeta < 0.0) lepIdSF = 0.977;
	    else if (lepeta < 0.8) lepIdSF = 0.977;
	    else if (lepeta < 1.442) lepIdSF = 0.972;
	    else if (lepeta < 1.566) lepIdSF = 1.000;
	    else if (lepeta < 2.0) lepIdSF = 0.963;
	    else lepIdSF = 0.968; }
	  else if (leppt < 100) {
	    if (lepeta < -2.0) lepIdSF = 0.978;
	    else if (lepeta < -1.566) lepIdSF = 0.978;
	    else if (lepeta < -1.442) lepIdSF = 1.000;
	    else if (lepeta < -0.8) lepIdSF = 0.974;
	    else if (lepeta < 0.0) lepIdSF = 0.978;
	    else if (lepeta < 0.8) lepIdSF = 0.982;
	    else if (lepeta < 1.442) lepIdSF = 0.976;
	    else if (lepeta < 1.566) lepIdSF = 1.000;
	    else if (lepeta < 2.0) lepIdSF = 0.976;
	    else lepIdSF = 0.953; }
	  else if (leppt < 200) {
	    if (lepeta < -2.0) lepIdSF = 0.974;
	    else if (lepeta < -1.566) lepIdSF = 0.997;
	    else if (lepeta < -1.442) lepIdSF = 1.000;
	    else if (lepeta < -0.8) lepIdSF = 0.981;
	    else if (lepeta < 0.0) lepIdSF = 0.980;
	    else if (lepeta < 0.8) lepIdSF = 0.991;
	    else if (lepeta < 1.442) lepIdSF = 0.987;
	    else if (lepeta < 1.566) lepIdSF = 1.000;
	    else if (lepeta < 2.0) lepIdSF = 0.962;
	    else lepIdSF = 0.956; }
	  else {
	    if (lepeta < -2.0) lepIdSF = 1.071;
	    else if (lepeta < -1.566) lepIdSF = 1.046;
	    else if (lepeta < -1.442) lepIdSF = 1.000;
	    else if (lepeta < -0.8) lepIdSF = 0.979;
	    else if (lepeta < 0.0) lepIdSF = 0.999;
	    else if (lepeta < 0.8) lepIdSF = 0.960;
	    else if (lepeta < 1.442) lepIdSF = 0.979;
	    else if (lepeta < 1.566) lepIdSF = 1.000;
	    else if (lepeta < 2.0) lepIdSF = 0.912;
	    else lepIdSF = 0.885; }

	  // mini isolation scale factors:  https://indico.cern.ch/event/820185/contributions/3427809/attachments/1845276/3027274/quick_update.pdf
	  if (leppt < 30){
	    if (fabs(lepeta) < 0.8) isoSF = 0.99458;
            else if (fabs(lepeta) < 1.442) isoSF = 0.99563;
            else if (fabs(lepeta) < 1.566) isoSF = 1.10598;
            else if (fabs(lepeta) < 2) isoSF = 1.02508;
            else isoSF = 1.03456;}
          else if (leppt < 40){
            if (fabs(lepeta) < 0.8) isoSF = 0.99690;
            else if (fabs(lepeta) < 1.442) isoSF = 0.99896;
            else if (fabs(lepeta) < 1.566) isoSF = 1.04486;
            else if (fabs(lepeta) < 2) isoSF = 1.01042;
            else isoSF = 1.01663;}
          else if (leppt < 50){
            if (fabs(lepeta) < 0.8) isoSF = 0.99797;
            else if (fabs(lepeta) < 1.442) isoSF = 0.99796;
            else if (fabs(lepeta) < 1.566) isoSF = 1.02402;
            else if (fabs(lepeta) < 2) isoSF = 1.00510;
            else isoSF = 1.00919;}
          else if (leppt < 60){
            if (fabs(lepeta) < 0.8) isoSF = 0.99798;
            else if (fabs(lepeta) < 1.442) isoSF = 0.99798;
            else if (fabs(lepeta) < 1.566) isoSF = 1.03159;
            else if (fabs(lepeta) < 2) isoSF = 1.00304;
            else isoSF = 1.00710;}
          else if (leppt < 100){
            if (fabs(lepeta) < 0.8) isoSF = 0.99899;
            else if (fabs(lepeta) < 1.442) isoSF = 1.00000;
            else if (fabs(lepeta) < 1.566) isoSF = 1.03282;
            else if (fabs(lepeta) < 2) isoSF = 1.00101;
            else isoSF = 1.00201;}
          else if (leppt < 200){
            if (fabs(lepeta) < 0.8) isoSF = 1.00101;
            else if (fabs(lepeta) < 1.442) isoSF = 1.00100;
            else if (fabs(lepeta) < 1.566) isoSF = 1.02058;
            else if (fabs(lepeta) < 2) isoSF = 1.00100;
            else isoSF = 1.00301;}
          else{
            if (fabs(lepeta) < 0.8) isoSF = 1.00000;
            else if (fabs(lepeta) < 1.442) isoSF = 0.99800;
            else if (fabs(lepeta) < 1.566) isoSF = 1.00403;
            else if (fabs(lepeta) < 2) isoSF = 1.00200;
            else isoSF = 1.00301;}
	}
	if(isMuon){
	  std::string string_a = "Mu15_IsoVVVL_PFHT450";
	  std::string string_b = "Mu15_IsoVVVL_PFHT450_PFMET50";
	  std::string string_d = "Mu50_IsoVVVL_PFHT450";
	  std::string string_e = "Mu15_IsoVVVL_PFHT600";
	  std::string string_ORb = "Mu50";
	  std::string string_ORc = "TkMu50";
	  for(unsigned int itrig=0; itrig < vsSelMCTriggersMu_MultiLepCalc->size(); itrig++){
	    if(vsSelMCTriggersMu_MultiLepCalc->at(itrig).find(string_a) != std::string::npos && viSelMCTriggersMu_MultiLepCalc->at(itrig) > 0) MCPastTrigger = 1;
      //	    if(vsSelMCTriggersMu_MultiLepCalc->at(itrig).find(string_b) != std::string::npos && viSelMCTriggersMu_MultiLepCalc->at(itrig) > 0) MCPastTrigger = 1;
            if(vsSelMCTriggersMu_MultiLepCalc->at(itrig).find(string_d) != std::string::npos && viSelMCTriggersMu_MultiLepCalc->at(itrig) > 0) MCPastTrigger = 1;
            if(vsSelMCTriggersMu_MultiLepCalc->at(itrig).find(string_e) != std::string::npos && viSelMCTriggersMu_MultiLepCalc->at(itrig) > 0) MCPastTrigger = 1;
	    if(vsSelMCTriggersMu_MultiLepCalc->at(itrig).find(string_ORb) != std::string::npos && viSelMCTriggersMu_MultiLepCalc->at(itrig) > 0) MCPastTrigger = 1;
	    if(vsSelMCTriggersMu_MultiLepCalc->at(itrig).find(string_ORc) != std::string::npos && viSelMCTriggersMu_MultiLepCalc->at(itrig) > 0) MCPastTrigger = 1;
	  }
           //https://wiwong.web.cern.ch/wiwong/Muon_Eff_Plots/2018_Efficiency20_miniTight_Tight_abseta/20_miniTight_Tight_abseta_SF.json
          if(leppt < 30){ // 25-30
           if(fabs(lepeta) < 0.9) isoSF= 0.991502;
            else if(fabs(lepeta) <  1.2) isoSF= 0.990496;
            else if(fabs(lepeta) <  2.1) isoSF= 1.012578;
            else isoSF= 1.016434;
          }
          else if(leppt < 40){
            if(fabs(lepeta) < 0.9) isoSF= 0.995874;
            else if(fabs(lepeta) <  1.2) isoSF= 0.995758;
            else if(fabs(lepeta) <  2.1) isoSF= 1.007955;
            else isoSF= 1.009848;
          }
          else if(leppt < 50){
            if(fabs(lepeta) < 0.9) isoSF= 0.997380;
            else if(fabs(lepeta) <  1.2) isoSF= 0.997219;
            else if(fabs(lepeta) <  2.1) isoSF= 1.003569;
            else isoSF= 1.004825;
          }
          else if(leppt < 60){
            if(fabs(lepeta) < 0.9) isoSF= 0.998509;
            else if(fabs(lepeta) <  1.2) isoSF= 0.998332;
            else if(fabs(lepeta) <  2.1) isoSF= 1.001423;
            else isoSF= 1.001671;
          }
          else if(leppt < 120){
            if(fabs(lepeta) < 0.9) isoSF= 0.999033;
            else if(fabs(lepeta) <  1.2) isoSF= 0.998499;
            else if(fabs(lepeta) <  2.1) isoSF= 0.999867;
            else isoSF= 1.000000;
          }
          else{ //120-200
           if(fabs(lepeta) < 0.9) isoSF= 1.000047;
            else if(fabs(lepeta) <  1.2) isoSF= 0.998008;
            else if(fabs(lepeta) <  2.1) isoSF= 0.997934;
            else isoSF= 1.0;
          }
            
          //cut based tight id: https://gitlab.cern.ch/cms-muonPOG/MuonReferenceEfficiencies/blob/master/EfficienciesStudies/2018/jsonfiles/RunABCD_SF_ID.json
	  if (fabs(lepeta) > 0.00 && fabs(lepeta) < 0.90) {
	    if (leppt >20.00 && leppt < 25.00) lepIdSF = 0.9913892461286353;
            else if (leppt > 25.00 && leppt < 30.00) lepIdSF = 0.991434774563006;
            else if (leppt > 30.00 && leppt < 40.00) lepIdSF = 0.99137555469275;
            else if (leppt > 40.00 && leppt < 50.00) lepIdSF = 0.9914412620030336;
            else if (leppt > 50.00 && leppt < 60.00) lepIdSF = 0.9916861853024751;
            else {lepIdSF = 0.9905799448514033;}  
          }
          else if (fabs(lepeta) > 0.90 && fabs(lepeta) < 1.20) {
            if (leppt >20.00 && leppt < 25.00) lepIdSF = 0.985987369423937;
            else if (leppt > 25.00 && leppt < 30.00) lepIdSF = 0.983987818122965;
            else if (leppt > 30.00 && leppt < 40.00) lepIdSF = 0.983823809537526;
            else if (leppt > 40.00 && leppt < 50.00) lepIdSF = 0.9842166496308641;
            else if (leppt > 50.00 && leppt < 60.00) lepIdSF = 0.9830003119378048;
            else {lepIdSF = 0.9816888807775153;}
          }
          else if (fabs(lepeta) > 1.20 && fabs(lepeta) < 2.10) {
            if (leppt >20.00 && leppt < 25.00) lepIdSF = 0.991472765372932;
            else if (leppt > 25.00 && leppt < 30.00) lepIdSF = 0.991184702552688;
            else if (leppt > 30.00 && leppt < 40.00) lepIdSF = 0.9907979966866818;
            else if (leppt > 40.00 && leppt < 50.00) lepIdSF = 0.98999331286624;
            else if (leppt > 50.00 && leppt < 60.00) lepIdSF = 0.9907796746368196;
            else {lepIdSF = 0.9886366722532979;}
          }
          else{
            if (leppt >20.00 && leppt < 25.00) lepIdSF = 0.9762336613870103;
            else if (leppt > 25.00 && leppt < 30.00) lepIdSF = 0.9735941110201962;
            else if (leppt > 30.00 && leppt < 40.00) lepIdSF = 0.973964994647057;
            else if (leppt > 40.00 && leppt < 50.00) lepIdSF = 0.9738877279555861;
            else if (leppt > 50.00 && leppt < 60.00) lepIdSF = 0.9737076412350453;
            else {lepIdSF = 0.9677501192438909;}
          }
	}
	DataPastTrigger = 1;
      }
      else{ //Data triggers check
	if(isElectron){
	  std::string string_a = "Ele15_IsoVVVL_PFHT450";
	  std::string string_b = "Ele15_IsoVVVL_PFHT450_PFMET50";
	  std::string string_c = "Ele50_IsoVVVL_PFHT450";
	  std::string string_d = "Ele15_IsoVVVL_PFHT600";
	  std::string string_ORa = "Ele35_WPTight_Gsf";
	  std::string string_ORb = "Ele38_WPTight_Gsf";
	  for(unsigned int itrig=0; itrig < vsSelTriggersEl_MultiLepCalc->size(); itrig++){
	    if(vsSelTriggersEl_MultiLepCalc->at(itrig).find(string_a) != std::string::npos && viSelTriggersEl_MultiLepCalc->at(itrig) > 0) DataPastTrigger = 1;
	  //  if(vsSelTriggersEl_MultiLepCalc->at(itrig).find(string_b) != std::string::npos && viSelTriggersEl_MultiLepCalc->at(itrig) > 0) DataPastTrigger = 1;
            if(vsSelTriggersEl_MultiLepCalc->at(itrig).find(string_c) != std::string::npos && viSelTriggersEl_MultiLepCalc->at(itrig) > 0) DataPastTrigger = 1;
            if(vsSelTriggersEl_MultiLepCalc->at(itrig).find(string_d) != std::string::npos && viSelTriggersEl_MultiLepCalc->at(itrig) > 0) DataPastTrigger = 1;
	    if(vsSelTriggersEl_MultiLepCalc->at(itrig).find(string_ORa) != std::string::npos && viSelTriggersEl_MultiLepCalc->at(itrig) > 0) DataPastTrigger = 1;
	    if(vsSelTriggersEl_MultiLepCalc->at(itrig).find(string_ORb) != std::string::npos && viSelTriggersEl_MultiLepCalc->at(itrig) > 0) DataPastTrigger = 1;
	  }
	}



	if(isMuon){
	  std::string string_a = "Mu15_IsoVVVL_PFHT450";
	  std::string string_b = "Mu15_IsoVVVL_PFHT450_PFMET50";
	  std::string string_d = "Mu50_IsoVVVL_PFHT450";
	  std::string string_e = "Mu15_IsoVVVL_PFHT600";
	  std::string string_ORb = "Mu50";
	  std::string string_ORc = "TkMu50";
	  for(unsigned int itrig=0; itrig < vsSelTriggersMu_MultiLepCalc->size(); itrig++){
	    if(vsSelTriggersMu_MultiLepCalc->at(itrig).find(string_a) != std::string::npos && viSelTriggersMu_MultiLepCalc->at(itrig) > 0) DataPastTrigger = 1;
	 //   if(vsSelTriggersMu_MultiLepCalc->at(itrig).find(string_b) != std::string::npos && viSelTriggersMu_MultiLepCalc->at(itrig) > 0) DataPastTrigger = 1;
            if(vsSelTriggersMu_MultiLepCalc->at(itrig).find(string_d) != std::string::npos && viSelTriggersMu_MultiLepCalc->at(itrig) > 0) DataPastTrigger = 1;
            if(vsSelTriggersMu_MultiLepCalc->at(itrig).find(string_e) != std::string::npos && viSelTriggersMu_MultiLepCalc->at(itrig) > 0) DataPastTrigger = 1;
	    if(vsSelTriggersMu_MultiLepCalc->at(itrig).find(string_ORb) != std::string::npos && viSelTriggersMu_MultiLepCalc->at(itrig) > 0) DataPastTrigger = 1;
	    if(vsSelTriggersMu_MultiLepCalc->at(itrig).find(string_ORc) != std::string::npos && viSelTriggersMu_MultiLepCalc->at(itrig) > 0) DataPastTrigger = 1;
	  }
	}
	MCPastTrigger = 1;
      }
      
      if(isMC && MCPastTrigger) npass_trigger+=1;
      if(!isMC && DataPastTrigger) npass_trigger+=1;

      // ----------------------------------------------------------------------------
      // Generator-level HT correction
      // ----------------------------------------------------------------------------      

      HTSF_Pol = 1;
      HTSF_PolUp = 1;
      HTSF_PolDn = 1;

      if(isMadgraphBkg){
	// Piece-wise splice with a flat line. Uncertainty from upper/lower error bar fits
	HTSF_Pol = poly2->Eval(HTfromHEPUEP_MultiLepCalc);
	HTSF_PolUp = poly2U->Eval(HTfromHEPUEP_MultiLepCalc);
	HTSF_PolDn = poly2D->Eval(HTfromHEPUEP_MultiLepCalc);
      }

      // ----------------------------------------------------------------------------
      // Lepton 4-vectors, calculate MT and electron trigger presel value
      // ----------------------------------------------------------------------------

      // Set lepton 4-vectors
      double lepM;
      double lepphi;
      if (isMuon){ 
	lepM = 0.105658367;
	lepphi = muPhi_MultiLepCalc->at(0);
	lepton_lv.SetPtEtaPhiM(muPt_MultiLepCalc->at(0),muEta_MultiLepCalc->at(0),muPhi_MultiLepCalc->at(0),lepM);
      }
      else{
	lepM = 0.00051099891;
	lepphi = elPhi_MultiLepCalc->at(0);
	lepton_lv.SetPtEtaPhiM(elPt_MultiLepCalc->at(0),elEta_MultiLepCalc->at(0),elPhi_MultiLepCalc->at(0),lepM);
      }

      MT_lepMet = sqrt(2*leppt*corr_met_MultiLepCalc*(1 - cos(lepphi - corr_met_phi_MultiLepCalc)));
      
      // ----------------------------------------------------------------------------
      // Loop over AK4 jets for calculations and pt ordering pair
      // ----------------------------------------------------------------------------

      NJets_JetSubCalc = 0;
      AK4HT = 0;
      int nskipped = 0;
      float deltaR1 = -1; float deltaR2 = -1;
      float ptRel1 = -1; float ptRel2 = -1;
      int ind1 = -1; int ind2 = -1;
      vector<pair<double,int>> jetptindpair;

      for(unsigned int ijet=0; ijet < theJetPt_JetSubCalc->size(); ijet++){

	// ----------------------------------------------------------------------------
	// Basic cuts   NEW NEW NEW NEW 2D cut right here
	// ----------------------------------------------------------------------------

	if(theJetPt_JetSubCalc->at(ijet) < jetPtCut || fabs(theJetEta_JetSubCalc->at(ijet)) > jetEtaCut) continue;
	
	jet_lv.SetPtEtaPhiE(theJetPt_JetSubCalc->at(ijet),theJetEta_JetSubCalc->at(ijet),theJetPhi_JetSubCalc->at(ijet),theJetEnergy_JetSubCalc->at(ijet));
	float deltaR = lepton_lv.DeltaR(jet_lv);
	float ptRel = lepton_lv.P()*(jet_lv.Vect().Cross(lepton_lv.Vect()).Mag()/jet_lv.P()/lepton_lv.P());
	if(deltaR < 0.4 && ptRel < 20){
	  nskipped += 1;
	  if(deltaR1 < 0){
	    deltaR1 = deltaR;
	    ptRel1 = ptRel;
	    ind1 = ijet;
	  }else{
	    if(deltaR2 < 0){
	      deltaR2 = deltaR;
	      ptRel2 = ptRel;
	      ind2 = ijet;
	    }else std::cout << "3rd close jet!" << std::endl;
	  }
	  continue; // not ONLY checking the closest jet, but should be impossible for 2 to be that close...
	}

	// ----------------------------------------------------------------------------
	// Counts and pt ordering pair
	// ----------------------------------------------------------------------------

	jetptindpair.push_back(std::make_pair(theJetPt_JetSubCalc->at(ijet),ijet));
	NJets_JetSubCalc+=1;
	AK4HT+=theJetPt_JetSubCalc->at(ijet);
      }

      if(nskipped > 1){
	std::cout << "------------------------------------------------------------" << std::endl;
	std::cout << "Weird thing! " << nskipped << " AK4s had DR < 0.4 && PtRel < 20!" << std::endl;
	std::cout << "dr1 = " << deltaR1 << ", dr2 = " << deltaR2 << std::endl;
	std::cout << "pt1 = " << ptRel1 << ", pt2 = " << ptRel2 << std::endl;
	std::cout << "------------------------------------------------------------" << std::endl;
      }
	
      // ----------------------------------------------------------------------------
      // Loop over AK8 jets for calculations and pt ordering pair
      // ----------------------------------------------------------------------------

      NJetsAK8_JetSubCalc = 0;
      vector<pair<double,int>> jetak8ptindpair;      
      nskipped = 0;      
      deltaR1 = -1; deltaR2 = -1;
      ptRel1 = -1; ptRel2 = -1;
      for(unsigned int ijet=0; ijet < theJetAK8Pt_JetSubCalc->size(); ijet++){

        // ----------------------------------------------------------------------------                                  
        // Basic cuts    NEW NEW NEW NEW NEW 2D cut right here
        // ----------------------------------------------------------------------------                                  
	
        if(fabs(theJetAK8Eta_JetSubCalc->at(ijet)) > ak8EtaCut) continue;
        if(theJetAK8Pt_JetSubCalc->at(ijet) < ak8PtCut) continue;
        if(theJetAK8NjettinessTau1_JetSubCalc->at(ijet)==0) continue;
        if(theJetAK8NjettinessTau2_JetSubCalc->at(ijet)==0) continue;
	
	ak8_lv.SetPtEtaPhiE(theJetAK8Pt_JetSubCalc->at(ijet),theJetAK8Eta_JetSubCalc->at(ijet),theJetAK8Phi_JetSubCalc->at(ijet),theJetAK8Energy_JetSubCalc->at(ijet));
	float deltaR = lepton_lv.DeltaR(ak8_lv);
	float ptRel = lepton_lv.P()*(ak8_lv.Vect().Cross(lepton_lv.Vect()).Mag()/ak8_lv.P()/lepton_lv.P());
	if(deltaR < 0.8 && ptRel < 20){
	  nskipped += 1;
	  if(deltaR1 < 0){
	    deltaR1 = deltaR;
	    ptRel1 = ptRel;
	  }else{
	    if(deltaR2 < 0){
	      deltaR2 = deltaR;
	      ptRel2 = ptRel;
	    }else std::cout << "3rd close jet!" << std::endl;
	  }
	  continue;   // not ONLY checking the closest jet, but should be impossible for 2 to be that close...
	}

        // ----------------------------------------------------------------------------                                  
        // Counters and pt ordering pair                                                                                  
        // ----------------------------------------------------------------------------                                  

	n_jetstotal++;	
        NJetsAK8_JetSubCalc += 1;
        jetak8ptindpair.push_back(std::make_pair(theJetAK8Pt_JetSubCalc->at(ijet),ijet));
	
        // ----------------------------------------------------------------------------                                  
        // Combine DeepAK8 C and B results
        // ----------------------------------------------------------------------------                                  

	if(dnn_largest_DeepAK8Calc->at(ijet) == 6) dnn_largest_DeepAK8Calc->at(ijet) = 5;

      }

      if(nskipped > 1){
	std::cout << "------------------------------------------------------------" << std::endl;
	std::cout << "Weird thing! " << nskipped << " AK8s had DR < 0.8 && PtRel < 20!" << std::endl;
	std::cout << "dr1 = " << deltaR1 << ", dr2 = " << deltaR2 << std::endl;
	std::cout << "pt1 = " << ptRel1 << ", pt2 = " << ptRel2 << std::endl; 
	std::cout << "------------------------------------------------------------" << std::endl;
     }

      // ----------------------------------------------------------------------------
      // Apply kinematic cuts
      // ----------------------------------------------------------------------------
	                
      int isPastHTCut = 0;
      if(AK4HT >= htCut){npass_ht+=1;isPastHTCut=1;}

      int isPastNAK8JetsCut = 0;
      if(NJetsAK8_JetSubCalc >= nAK8jetsCut){npass_nAK8jets+=1;isPastNAK8JetsCut=1;}
      
      int isPastMETcut = 0;
      if(corr_met_MultiLepCalc > metCut){npass_met+=1;isPastMETcut=1;}

      int isPastLepPtCut = 0;
      if(leppt > lepPtCut){npass_lepPt+=1;isPastLepPtCut=1;}
      
      int isPastElEtaCut = 0;
      int isPastMuEtaCut = 0;
      if(isElectron && fabs(lepeta) < elEtaCut){npass_ElEta+=1;isPastElEtaCut=1;}
      if(isMuon && fabs(lepeta) < muEtaCut){npass_MuEta+=1;isPastMuEtaCut=1;}
      if(isElectron){Nelectrons+=1;}
      if(isMuon){Nmuons+=1;}
      
      AK4HTpMETpLepPt = 0;
      AK4HTpMETpLepPt = AK4HT + corr_met_MultiLepCalc + leppt;


      // ----------------------------------------------------------------------------
      // Skip failing events
      // ----------------------------------------------------------------------------
            
      if(!(isPastMETcut && isPastHTCut && isPastNAK8JetsCut && isPastLepPtCut && (isPastElEtaCut || isPastMuEtaCut))) continue;
      npass_all+=1;

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      /////////////// ONLY ON SELECTED EVENTS ////////////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////      

      if(isSig){
	if(confusionTruth < 6.0){
	  TTconfusionD->Fill(0.5,confusionTruth);
	  TTconfusionD->Fill(1.5,confusionTruth);
	  TTconfusionD->Fill(2.5,confusionTruth);
	  TTconfusionD->Fill(3.5,confusionTruth);
	  TTconfusionD->Fill(4.5,confusionTruth);
	  TTconfusionD->Fill(5.5,confusionTruth);
	  TTconfusionD->Fill(6.5,confusionTruth);
	  TTconfusionD->Fill(7.5,confusionTruth);
	  TTconfusionD->Fill(8.5,confusionTruth);
	  TTconfusionD->Fill(9.5,confusionTruth);
	}else{
	  BBconfusionD->Fill(0.5,confusionTruth-6.0);
	  BBconfusionD->Fill(1.5,confusionTruth-6.0);
	  BBconfusionD->Fill(2.5,confusionTruth-6.0);
	  BBconfusionD->Fill(3.5,confusionTruth-6.0);
	  BBconfusionD->Fill(4.5,confusionTruth-6.0);
	  BBconfusionD->Fill(5.5,confusionTruth-6.0);
	  BBconfusionD->Fill(6.5,confusionTruth-6.0);
	}
      }
      
      // ----------------------------------------------------------------------------
      // Combine lepton variables into one set
      // ----------------------------------------------------------------------------

      if(isElectron){
	leptonPt_MultiLepCalc = leppt;
	leptonEta_MultiLepCalc = lepeta;
	leptonPhi_MultiLepCalc = elPhi_MultiLepCalc->at(0);
	leptonEnergy_MultiLepCalc = elEnergy_MultiLepCalc->at(0);
	leptonMiniIso_MultiLepCalc = elMiniIso_MultiLepCalc->at(0);
	leptonMVAValue_MultiLepCalc = -99.9; //elMVAValue_MultiLepCalc->at(0);
      }
      if(isMuon){
	leptonPt_MultiLepCalc = leppt;
	leptonEta_MultiLepCalc = lepeta;
	leptonPhi_MultiLepCalc = muPhi_MultiLepCalc->at(0);
	leptonEnergy_MultiLepCalc = muEnergy_MultiLepCalc->at(0);
	leptonMiniIso_MultiLepCalc = muMiniIso_MultiLepCalc->at(0);
	leptonMVAValue_MultiLepCalc = -99.9;
      }

      // ----------------------------------------------------------------------------
      // Apply pt ordering to AK4 vectors
      // ----------------------------------------------------------------------------

      std::sort(jetptindpair.begin(), jetptindpair.end(), comparepair);
      theJetPt_JetSubCalc_PtOrdered.clear();
      theJetEta_JetSubCalc_PtOrdered.clear();
      theJetPhi_JetSubCalc_PtOrdered.clear();
      theJetEnergy_JetSubCalc_PtOrdered.clear();
      theJetDeepCSVb_JetSubCalc_PtOrdered.clear();
      theJetDeepCSVbb_JetSubCalc_PtOrdered.clear();
      theJetHFlav_JetSubCalc_PtOrdered.clear();
      theJetPFlav_JetSubCalc_PtOrdered.clear();
      theJetBTag_JetSubCalc_PtOrdered.clear();
      theJetBTag_bSFup_JetSubCalc_PtOrdered.clear();
      theJetBTag_bSFdn_JetSubCalc_PtOrdered.clear();
      theJetBTag_lSFup_JetSubCalc_PtOrdered.clear();
      theJetBTag_lSFdn_JetSubCalc_PtOrdered.clear();
      for(unsigned int ijet=0; ijet < jetptindpair.size(); ijet++){
      	theJetPt_JetSubCalc_PtOrdered.push_back(theJetPt_JetSubCalc->at(jetptindpair[ijet].second));
      	theJetEta_JetSubCalc_PtOrdered.push_back(theJetEta_JetSubCalc->at(jetptindpair[ijet].second));
      	theJetPhi_JetSubCalc_PtOrdered.push_back(theJetPhi_JetSubCalc->at(jetptindpair[ijet].second));
      	theJetEnergy_JetSubCalc_PtOrdered.push_back(theJetEnergy_JetSubCalc->at(jetptindpair[ijet].second));
      	theJetDeepCSVb_JetSubCalc_PtOrdered.push_back(theJetDeepCSVb_JetSubCalc->at(jetptindpair[ijet].second));
	theJetDeepCSVbb_JetSubCalc_PtOrdered.push_back(theJetDeepCSVbb_JetSubCalc->at(jetptindpair[ijet].second));
      	theJetHFlav_JetSubCalc_PtOrdered.push_back(theJetHFlav_JetSubCalc->at(jetptindpair[ijet].second));
      	theJetPFlav_JetSubCalc_PtOrdered.push_back(theJetPFlav_JetSubCalc->at(jetptindpair[ijet].second));
	theJetBTag_JetSubCalc_PtOrdered.push_back(theJetBTag_JetSubCalc->at(jetptindpair[ijet].second));
	theJetBTag_bSFup_JetSubCalc_PtOrdered.push_back(theJetBTag_bSFup_JetSubCalc->at(jetptindpair[ijet].second));
	theJetBTag_bSFdn_JetSubCalc_PtOrdered.push_back(theJetBTag_bSFdn_JetSubCalc->at(jetptindpair[ijet].second));
	theJetBTag_lSFup_JetSubCalc_PtOrdered.push_back(theJetBTag_lSFup_JetSubCalc->at(jetptindpair[ijet].second));
	theJetBTag_lSFdn_JetSubCalc_PtOrdered.push_back(theJetBTag_lSFdn_JetSubCalc->at(jetptindpair[ijet].second));
      }

      // ----------------------------------------------------------------------------
      // AK4 Jet - lepton associations
      // ----------------------------------------------------------------------------
         
      int ind_MinMlb = -1;
      int ind_MinMlj = -1;
      BJetLeadPt = -99;
      minMleppBjet = 1e8;
      minMleppJet = 1e8;      
      minDPhi_MetJet = 1e8;
      minDR_lepJet = 1e8;
      ptRel_lepJet = -99;
      deltaR_lepJets.clear();
      deltaR_lepBJets.clear();
      deltaR_lepMinMlb = 1e8;
      deltaR_lepMinMlj = 1e8;
      NJetsCSV_JetSubCalc = 0;
      NJetsCSVwithSF_JetSubCalc = 0;
      TLorentzVector nu;
      nu.SetPtEtaPhiE(corr_met_MultiLepCalc,0,corr_met_phi_MultiLepCalc,corr_met_MultiLepCalc);

      for(unsigned int ijet=0; ijet < theJetPt_JetSubCalc_PtOrdered.size(); ijet++){
        jet_lv.SetPtEtaPhiE(theJetPt_JetSubCalc_PtOrdered.at(ijet),theJetEta_JetSubCalc_PtOrdered.at(ijet),theJetPhi_JetSubCalc_PtOrdered.at(ijet),theJetEnergy_JetSubCalc_PtOrdered.at(ijet));
	if((lepton_lv + jet_lv).M() < minMleppJet) {
	  minMleppJet = fabs((lepton_lv + jet_lv).M());
	  deltaR_lepMinMlj = jet_lv.DeltaR(lepton_lv);
	  ind_MinMlj = ijet;
	}

	deltaR_lepJets.push_back(lepton_lv.DeltaR(jet_lv));

   	if(theJetDeepCSVb_JetSubCalc_PtOrdered.at(ijet) + theJetDeepCSVbb_JetSubCalc_PtOrdered.at(ijet) > 0.4941){
          NJetsCSV_JetSubCalc += 1;
        }

	if(isNominal && theJetBTag_JetSubCalc_PtOrdered.at(ijet) == 1){
	  NJetsCSVwithSF_JetSubCalc += 1;
          if(theJetPt_JetSubCalc_PtOrdered.at(ijet) > BJetLeadPt) BJetLeadPt = theJetPt_JetSubCalc_PtOrdered.at(ijet);
          deltaR_lepBJets.push_back(lepton_lv.DeltaR(jet_lv));
	  
          if((lepton_lv + jet_lv).M() < minMleppBjet) {
            minMleppBjet = fabs( (lepton_lv + jet_lv).M() );
	    deltaR_lepMinMlb = jet_lv.DeltaR(lepton_lv);
	    ind_MinMlb = ijet;
          }
	}
	else if(isBUp && theJetBTag_bSFup_JetSubCalc_PtOrdered.at(ijet) == 1){
	  NJetsCSVwithSF_JetSubCalc += 1;
          if(theJetPt_JetSubCalc_PtOrdered.at(ijet) > BJetLeadPt) BJetLeadPt = theJetPt_JetSubCalc_PtOrdered.at(ijet);
          deltaR_lepBJets.push_back(lepton_lv.DeltaR(jet_lv));
	  
          if((lepton_lv + jet_lv).M() < minMleppBjet) {
            minMleppBjet = fabs( (lepton_lv + jet_lv).M() );
	    deltaR_lepMinMlb = jet_lv.DeltaR(lepton_lv);
	    ind_MinMlb = ijet;
          }
	}
	else if(isBDn && theJetBTag_bSFdn_JetSubCalc_PtOrdered.at(ijet) == 1){
	  NJetsCSVwithSF_JetSubCalc += 1;
          if(theJetPt_JetSubCalc_PtOrdered.at(ijet) > BJetLeadPt) BJetLeadPt = theJetPt_JetSubCalc_PtOrdered.at(ijet);
          deltaR_lepBJets.push_back(lepton_lv.DeltaR(jet_lv));
	  
          if((lepton_lv + jet_lv).M() < minMleppBjet) {
            minMleppBjet = fabs( (lepton_lv + jet_lv).M() );
	    deltaR_lepMinMlb = jet_lv.DeltaR(lepton_lv);
	    ind_MinMlb = ijet;
          }
	}
	else if(isLUp && theJetBTag_lSFup_JetSubCalc_PtOrdered.at(ijet) == 1){
	  NJetsCSVwithSF_JetSubCalc += 1;
          if(theJetPt_JetSubCalc_PtOrdered.at(ijet) > BJetLeadPt) BJetLeadPt = theJetPt_JetSubCalc_PtOrdered.at(ijet);
          deltaR_lepBJets.push_back(lepton_lv.DeltaR(jet_lv));
	  
          if((lepton_lv + jet_lv).M() < minMleppBjet) {
            minMleppBjet = fabs( (lepton_lv + jet_lv).M() );
	    deltaR_lepMinMlb = jet_lv.DeltaR(lepton_lv);
	    ind_MinMlb = ijet;
          }
	}
	else if(isLDn && theJetBTag_lSFdn_JetSubCalc_PtOrdered.at(ijet) == 1){
	  NJetsCSVwithSF_JetSubCalc += 1;
          if(theJetPt_JetSubCalc_PtOrdered.at(ijet) > BJetLeadPt) BJetLeadPt = theJetPt_JetSubCalc_PtOrdered.at(ijet);
          deltaR_lepBJets.push_back(lepton_lv.DeltaR(jet_lv));
	  
          if((lepton_lv + jet_lv).M() < minMleppBjet) {
            minMleppBjet = fabs( (lepton_lv + jet_lv).M() );
	    deltaR_lepMinMlb = jet_lv.DeltaR(lepton_lv);
	    ind_MinMlb = ijet;
          }
	}
	
	if(jet_lv.DeltaPhi(nu) < minDPhi_MetJet) minDPhi_MetJet = jet_lv.DeltaPhi(nu);	  

 	if(deltaR_lepJets[ijet] < minDR_lepJet) {
	  minDR_lepJet = deltaR_lepJets[ijet];
	  ptRel_lepJet = lepton_lv.P()*(jet_lv.Vect().Cross(lepton_lv.Vect()).Mag()/jet_lv.P()/lepton_lv.P());
	}
      }

      // ----------------------------------------------------------------------------
      // 13TeV Top pT reweighting -- TTbarMassCalc top vectors are the wrong tops....
      // ----------------------------------------------------------------------------
      
      genTopPt = -999;
      genAntiTopPt = -999;
      topPtWeight13TeV = 1.0;
      
      if(isTT){
	int top1index = -1;
	int top2index = -1;
	for(unsigned int itop=0; itop < allTopsStatus_TTbarMassCalc->size(); itop++){
	  //	  std::cout << "Status: " << allTopsStatus_TTbarMassCalc->at(itop) << ", Pt: " << allTopsPt_TTbarMassCalc->at(itop) << std::endl;
	  if(allTopsStatus_TTbarMassCalc->at(itop) != 62) continue;
	  if(top1index < 0){
	    top1index = itop;
	    continue;
	  }
	  if(top2index < 0){
	    top2index = itop;
	    continue;
	  }
	}

	float SFtop1 = TMath::Exp(0.0615-0.0005*allTopsPt_TTbarMassCalc->at(top1index));
	float SFtop2 = TMath::Exp(0.0615-0.0005*allTopsPt_TTbarMassCalc->at(top2index));
	topPtWeight13TeV = TMath::Sqrt(SFtop1*SFtop2);

	if(allTopsID_TTbarMassCalc->at(top1index) == 6){
	  genTopPt = allTopsPt_TTbarMassCalc->at(top1index);
	  genAntiTopPt = allTopsPt_TTbarMassCalc->at(top2index);
	}
	else{
	  genAntiTopPt = allTopsPt_TTbarMassCalc->at(top1index);
	  genTopPt = allTopsPt_TTbarMassCalc->at(top2index);
	}
      }

      // ----------------------------------------------------------------------------
      // Apply pt ordering to AK8 vectors 
      // ----------------------------------------------------------------------------

      //Pt ordering for AK8
      std::sort(jetak8ptindpair.begin(), jetak8ptindpair.end(), comparepair);

      dnn_J_DeepAK8Calc_PtOrdered.clear();
      dnn_T_DeepAK8Calc_PtOrdered.clear();
      dnn_H_DeepAK8Calc_PtOrdered.clear();
      dnn_W_DeepAK8Calc_PtOrdered.clear();
      dnn_Z_DeepAK8Calc_PtOrdered.clear();
      dnn_B_DeepAK8Calc_PtOrdered.clear();

      maxProb_JetSubCalc_PtOrdered.clear();
      dnn_largest_DeepAK8Calc_PtOrdered.clear();
      theJetAK8DoubleB_JetSubCalc_PtOrdered.clear();
      theJetAK8SDSubjetNDeepCSVMSF_JetSubCalc_PtOrdered.clear();
      theJetAK8SDSubjetNDeepCSVL_JetSubCalc_PtOrdered.clear();
      theJetAK8SDSubjetIndex_JetSubCalc_PtOrdered.clear();
      theJetAK8SDSubjetSize_JetSubCalc_PtOrdered.clear();
      theJetAK8Pt_JetSubCalc_PtOrdered.clear();
      theJetAK8Eta_JetSubCalc_PtOrdered.clear();
      theJetAK8Phi_JetSubCalc_PtOrdered.clear();
      theJetAK8Energy_JetSubCalc_PtOrdered.clear();
      theJetAK8Mass_JetSubCalc_PtOrdered.clear();
      theJetAK8CHSPrunedMass_JetSubCalc_PtOrdered.clear();
      theJetAK8CHSSoftDropMass_JetSubCalc_PtOrdered.clear();
      theJetAK8SoftDropRaw_PtOrdered.clear();
      theJetAK8SoftDropCorr_PtOrdered.clear();
      theJetAK8SoftDrop_PtOrdered.clear();
      theJetAK8NjettinessTau1_JetSubCalc_PtOrdered.clear();
      theJetAK8NjettinessTau2_JetSubCalc_PtOrdered.clear();
      theJetAK8NjettinessTau3_JetSubCalc_PtOrdered.clear();
      theJetAK8CHSTau1_JetSubCalc_PtOrdered.clear();
      theJetAK8CHSTau2_JetSubCalc_PtOrdered.clear();
      theJetAK8CHSTau3_JetSubCalc_PtOrdered.clear();
      for(unsigned int ijet=0; ijet < jetak8ptindpair.size(); ijet++){
	dnn_J_DeepAK8Calc_PtOrdered.push_back(dnn_J_DeepAK8Calc->at(jetak8ptindpair[ijet].second));
	dnn_T_DeepAK8Calc_PtOrdered.push_back(dnn_T_DeepAK8Calc->at(jetak8ptindpair[ijet].second));
	dnn_H_DeepAK8Calc_PtOrdered.push_back(dnn_H_DeepAK8Calc->at(jetak8ptindpair[ijet].second));
	dnn_W_DeepAK8Calc_PtOrdered.push_back(dnn_W_DeepAK8Calc->at(jetak8ptindpair[ijet].second));
	dnn_Z_DeepAK8Calc_PtOrdered.push_back(dnn_Z_DeepAK8Calc->at(jetak8ptindpair[ijet].second));
	dnn_B_DeepAK8Calc_PtOrdered.push_back(dnn_B_DeepAK8Calc->at(jetak8ptindpair[ijet].second)+dnn_C_DeepAK8Calc->at(jetak8ptindpair[ijet].second));
	maxProb_JetSubCalc_PtOrdered.push_back(maxProb_JetSubCalc->at(jetak8ptindpair[ijet].second));
      	dnn_largest_DeepAK8Calc_PtOrdered.push_back(dnn_largest_DeepAK8Calc->at(jetak8ptindpair[ijet].second));
      	theJetAK8DoubleB_JetSubCalc_PtOrdered.push_back(theJetAK8DoubleB_JetSubCalc->at(jetak8ptindpair[ijet].second));
	theJetAK8SDSubjetIndex_JetSubCalc_PtOrdered.push_back(theJetAK8SDSubjetIndex_JetSubCalc->at(jetak8ptindpair[ijet].second));
	theJetAK8SDSubjetSize_JetSubCalc_PtOrdered.push_back(theJetAK8SDSubjetSize_JetSubCalc->at(jetak8ptindpair[ijet].second));
	theJetAK8SDSubjetNDeepCSVMSF_JetSubCalc_PtOrdered.push_back(theJetAK8SDSubjetNDeepCSVMSF_JetSubCalc->at(jetak8ptindpair[ijet].second));
	theJetAK8SDSubjetNDeepCSVL_JetSubCalc_PtOrdered.push_back(theJetAK8SDSubjetNDeepCSVL_JetSubCalc->at(jetak8ptindpair[ijet].second));
      	theJetAK8Pt_JetSubCalc_PtOrdered.push_back(theJetAK8Pt_JetSubCalc->at(jetak8ptindpair[ijet].second));
      	theJetAK8Eta_JetSubCalc_PtOrdered.push_back(theJetAK8Eta_JetSubCalc->at(jetak8ptindpair[ijet].second));
      	theJetAK8Phi_JetSubCalc_PtOrdered.push_back(theJetAK8Phi_JetSubCalc->at(jetak8ptindpair[ijet].second));
      	theJetAK8Energy_JetSubCalc_PtOrdered.push_back(theJetAK8Energy_JetSubCalc->at(jetak8ptindpair[ijet].second));
      	theJetAK8Mass_JetSubCalc_PtOrdered.push_back(theJetAK8Mass_JetSubCalc->at(jetak8ptindpair[ijet].second));
      	theJetAK8CHSPrunedMass_JetSubCalc_PtOrdered.push_back(theJetAK8CHSPrunedMass_JetSubCalc->at(jetak8ptindpair[ijet].second));
      	theJetAK8CHSSoftDropMass_JetSubCalc_PtOrdered.push_back(theJetAK8CHSSoftDropMass_JetSubCalc->at(jetak8ptindpair[ijet].second));
      	theJetAK8SoftDropRaw_PtOrdered.push_back(theJetAK8SoftDropRaw_JetSubCalc->at(jetak8ptindpair[ijet].second));
      	theJetAK8SoftDropCorr_PtOrdered.push_back(theJetAK8SoftDropCorr_JetSubCalc->at(jetak8ptindpair[ijet].second));
	theJetAK8SoftDrop_PtOrdered.push_back(theJetAK8SoftDrop_JetSubCalc->at(jetak8ptindpair[ijet].second));
      	theJetAK8NjettinessTau1_JetSubCalc_PtOrdered.push_back(theJetAK8NjettinessTau1_JetSubCalc->at(jetak8ptindpair[ijet].second));
      	theJetAK8NjettinessTau2_JetSubCalc_PtOrdered.push_back(theJetAK8NjettinessTau2_JetSubCalc->at(jetak8ptindpair[ijet].second));
      	theJetAK8NjettinessTau3_JetSubCalc_PtOrdered.push_back(theJetAK8NjettinessTau3_JetSubCalc->at(jetak8ptindpair[ijet].second));
      	theJetAK8CHSTau1_JetSubCalc_PtOrdered.push_back(theJetAK8CHSTau1_JetSubCalc->at(jetak8ptindpair[ijet].second));      	
	theJetAK8CHSTau2_JetSubCalc_PtOrdered.push_back(theJetAK8CHSTau2_JetSubCalc->at(jetak8ptindpair[ijet].second));
      	theJetAK8CHSTau3_JetSubCalc_PtOrdered.push_back(theJetAK8CHSTau3_JetSubCalc->at(jetak8ptindpair[ijet].second));
      }

      // Add probabilities
      probSum_DeepAK8_all = 0;
      nB_DeepAK8 = 0;
      nH_DeepAK8 = 0;
      nJ_DeepAK8 = 0;
      nT_DeepAK8 = 0;
      nW_DeepAK8 = 0;
      nZ_DeepAK8 = 0;
      // 0 = J, 1 = T, 2 = H, 3 = Z, 4 = W, 5 = B
      for(unsigned int ijet=0; ijet < jetak8ptindpair.size(); ijet++){
        if (dnn_B_DeepAK8Calc_PtOrdered.at(ijet) < -900) dnn_B_DeepAK8Calc_PtOrdered.at(ijet) = 0;
        if (dnn_H_DeepAK8Calc_PtOrdered.at(ijet) < -900) dnn_H_DeepAK8Calc_PtOrdered.at(ijet) = 0;
        if (dnn_J_DeepAK8Calc_PtOrdered.at(ijet) < -900) dnn_J_DeepAK8Calc_PtOrdered.at(ijet) = 0;
        if (dnn_T_DeepAK8Calc_PtOrdered.at(ijet) < -900) dnn_T_DeepAK8Calc_PtOrdered.at(ijet) = 0;
        if (dnn_W_DeepAK8Calc_PtOrdered.at(ijet) < -900) dnn_W_DeepAK8Calc_PtOrdered.at(ijet) = 0;
        if (dnn_Z_DeepAK8Calc_PtOrdered.at(ijet) < -900) dnn_Z_DeepAK8Calc_PtOrdered.at(ijet) = 0;

        probSum_DeepAK8_all += dnn_B_DeepAK8Calc_PtOrdered.at(ijet) + 2*dnn_W_DeepAK8Calc_PtOrdered.at(ijet) + 2*dnn_T_DeepAK8Calc_PtOrdered.at(ijet) + 3*dnn_Z_DeepAK8Calc_PtOrdered.at(ijet) + 4*dnn_H_DeepAK8Calc_PtOrdered.at(ijet);

        if (dnn_largest_DeepAK8Calc_PtOrdered.at(ijet) == 1) nT_DeepAK8 += 1;
        else if (dnn_largest_DeepAK8Calc_PtOrdered.at(ijet) == 2) nH_DeepAK8 += 1;
        else if (dnn_largest_DeepAK8Calc_PtOrdered.at(ijet) == 3) nZ_DeepAK8 += 1;
        else if (dnn_largest_DeepAK8Calc_PtOrdered.at(ijet) == 4) nW_DeepAK8 += 1;
	else if (dnn_largest_DeepAK8Calc_PtOrdered.at(ijet) == 5) nB_DeepAK8 += 1;
	//	else if (theJetAK8SDSubjetNDeepCSVL_JetSubCalc_PtOrdered.at(ijet) > 0) nB_DeepAK8 += 1;
        else nJ_DeepAK8 += 1;
      }

      // ----------------------------------------------------------------------------
      // Evaluate the VLQ / ttbar / WJets DNN
      // ----------------------------------------------------------------------------
      
      myMap = {
	{"Wjets",  -999},
	{"ttbar",  -999},
	{"Tprime",-999},
      };

      varMap = {
	{"corr_met_singleLepCalc", -999},
	{"AK4HTpMETpLepPt", -999},
	{"AK4HT", -999},
	{"NJets_JetSubCalc", -999},
	{"NJetsAK8_JetSubCalc", -999},
	{"jetPt_1", -999},
	{"jetPt_2", -999},
	{"jetPt_3", -999},
	{"dnnJ_1", -999},
	{"dnnJ_2", -999},
	{"dnnJ_3", -999},
      };

      dnn_WJets = -1;
      dnn_ttbar = -1;
      dnn_Tprime = -1;

      if(NJetsAK8_JetSubCalc > 1){
	float jetPt_1, jetPt_2, jetPt_3;
	float sdMass_1, sdMass_2, sdMass_3;
	float dnnLargest_2, dnnLargest_3;
	float dnnJ_1, dnnJ_2, dnnJ_3;

	jetPt_1 = theJetAK8Pt_JetSubCalc_PtOrdered.at(0);
	jetPt_2 = theJetAK8Pt_JetSubCalc_PtOrdered.at(1);
	if(NJetsAK8_JetSubCalc > 2) jetPt_3 = theJetAK8Pt_JetSubCalc_PtOrdered.at(2);
	else jetPt_3 = 0;
	
	sdMass_1 = theJetAK8SoftDropCorr_PtOrdered.at(0);
	sdMass_2 = theJetAK8SoftDropCorr_PtOrdered.at(1);
	if(NJetsAK8_JetSubCalc > 2) sdMass_3 = theJetAK8SoftDropCorr_PtOrdered.at(2);
	else sdMass_3 = 0;
	
	dnnLargest_2 = dnn_largest_DeepAK8Calc_PtOrdered.at(1);
	if(NJetsAK8_JetSubCalc > 2) dnnLargest_3 = dnn_largest_DeepAK8Calc_PtOrdered.at(2);
	else dnnLargest_3 = 10;
	
	dnnJ_1 = dnn_J_DeepAK8Calc_PtOrdered.at(0);
	dnnJ_2 = dnn_J_DeepAK8Calc_PtOrdered.at(1);
	if(NJetsAK8_JetSubCalc > 2) dnnJ_3 = dnn_J_DeepAK8Calc_PtOrdered.at(2);
	else dnnJ_3 = -9;

	varMap = {
	  {"corr_met_singleLepCalc", corr_met_MultiLepCalc},
	  {"AK4HTpMETpLepPt", AK4HTpMETpLepPt},
	  {"AK4HT", AK4HT},
	  {"NJets_JetSubCalc", NJets_JetSubCalc},
	  {"NJetsAK8_JetSubCalc", NJetsAK8_JetSubCalc},
	  {"jetPt_1", jetPt_1},
	  {"jetPt_2", jetPt_2},
	  {"jetPt_3", jetPt_3},
	  {"dnnJ_1", dnnJ_1},
	  {"dnnJ_2", dnnJ_2},
	  {"dnnJ_3", dnnJ_3},
	};
	
	myMap = lwtnn->compute(varMap);      

	dnn_WJets = myMap["Wjets"];
	dnn_ttbar = myMap["ttbar"];
	dnn_Tprime = myMap["Tprime"];
      }

      // ----------------------------------------------------------------------------
      // AK8 Jet - lepton associations and truth matching
      // ----------------------------------------------------------------------------

      deltaR_lepAK8s.clear();
      minDR_lepAK8 = 1000;
      minDR_leadAK8otherAK8 = 1000;
      ptRel_lepAK8 = -99;
      if(theJetAK8Pt_JetSubCalc_PtOrdered.size() < 1) minDR_lepAK8 = -99.0;      
      if(theJetAK8Pt_JetSubCalc_PtOrdered.size() < 2) minDR_leadAK8otherAK8 = -99.0;

      theJetAK8Wmatch_JetSubCalc_PtOrdered.clear();
      theJetAK8Hmatch_JetSubCalc_PtOrdered.clear();
      theJetAK8Zmatch_JetSubCalc_PtOrdered.clear();
      theJetAK8Tmatch_JetSubCalc_PtOrdered.clear();
      theJetAK8MatchedPt_JetSubCalc_PtOrdered.clear();
      theJetAK8Truth_JetSubCalc_PtOrdered.clear();
      DeepAK8SF_TeffUp = 1.0;
      DeepAK8SF_HeffUp = 1.0;
      DeepAK8SF_ZeffUp = 1.0;
      DeepAK8SF_WeffUp = 1.0;
      DeepAK8SF_BeffUp = 1.0;
      DeepAK8SF_JeffUp = 1.0;
      DeepAK8SF_TmisUp = 1.0;
      DeepAK8SF_HmisUp = 1.0;
      DeepAK8SF_ZmisUp = 1.0;
      DeepAK8SF_WmisUp = 1.0;
      DeepAK8SF_BmisUp = 1.0;
      DeepAK8SF_JmisUp = 1.0;
      DeepAK8SF_TeffDn = 1.0;
      DeepAK8SF_HeffDn = 1.0;
      DeepAK8SF_ZeffDn = 1.0;
      DeepAK8SF_WeffDn = 1.0;
      DeepAK8SF_BeffDn = 1.0;
      DeepAK8SF_JeffDn = 1.0;
      DeepAK8SF_TmisDn = 1.0;
      DeepAK8SF_HmisDn = 1.0;
      DeepAK8SF_ZmisDn = 1.0;
      DeepAK8SF_WmisDn = 1.0;
      DeepAK8SF_BmisDn = 1.0;
      DeepAK8SF_JmisDn = 1.0;

      ak8_lv.SetPtEtaPhiM(0,0,0,0);
      TLorentzVector leadak8;
      leadak8.SetPtEtaPhiM(0,0,0,0);

      for(unsigned int ijet=0; ijet < theJetAK8Pt_JetSubCalc_PtOrdered.size(); ijet++){

	// ----------------------------------------------------------------------------
	// AK8 - lepton and AK8 -- AK8 associations
	// ----------------------------------------------------------------------------
	
	ak8_lv.SetPtEtaPhiE(theJetAK8Pt_JetSubCalc_PtOrdered.at(ijet),theJetAK8Eta_JetSubCalc_PtOrdered.at(ijet),theJetAK8Phi_JetSubCalc_PtOrdered.at(ijet),theJetAK8Energy_JetSubCalc_PtOrdered.at(ijet));
	if(ijet == 0) leadak8 = ak8_lv;

	deltaR_lepAK8s.push_back(lepton_lv.DeltaR(ak8_lv));
	if(lepton_lv.DeltaR(ak8_lv) < minDR_lepAK8){
	  minDR_lepAK8 = lepton_lv.DeltaR(ak8_lv);
	  ptRel_lepAK8 = lepton_lv.P()*(ak8_lv.Vect().Cross(lepton_lv.Vect()).Mag()/ak8_lv.P()/lepton_lv.P());
	}

	if(ijet > 0){
	  float tempdr = leadak8.DeltaR(ak8_lv);
	  if(tempdr < minDR_leadAK8otherAK8){
	    minDR_leadAK8otherAK8 = tempdr;
	  }
	}

	if(isMC){
	  
	  // ------------------------------------------------------------------------------------------------------------------
	  // TRUTH MATCHING
	  // ------------------------------------------------------------------------------------------------------------------
	  float minDR = 1000;
	  float matchedPt= -99;
	  int matchedID = 0;
	  bool isWmatched = false;
	  bool isHmatched = false;
	  bool isZmatched = false;
	  bool isTmatched = false;
	  bool isJmatched = false;
	  bool isBmatched = false;
	  TLorentzVector trueW,d1,d2,d3;

	  for(unsigned int iW = 0; iW < HadronicVHtPt_JetSubCalc->size(); iW++){
	    trueW.SetPtEtaPhiE(HadronicVHtPt_JetSubCalc->at(iW),HadronicVHtEta_JetSubCalc->at(iW),HadronicVHtPhi_JetSubCalc->at(iW),HadronicVHtEnergy_JetSubCalc->at(iW));

	    if(ak8_lv.DeltaR(trueW) < minDR){
	      minDR = ak8_lv.DeltaR(trueW);
	      matchedPt = HadronicVHtPt_JetSubCalc->at(iW);
	      matchedID = abs(HadronicVHtID_JetSubCalc->at(iW));	      
	      d1.SetPtEtaPhiE(HadronicVHtD0Pt_JetSubCalc->at(iW),HadronicVHtD0Eta_JetSubCalc->at(iW),HadronicVHtD0Phi_JetSubCalc->at(iW),HadronicVHtD0E_JetSubCalc->at(iW));
	      d2.SetPtEtaPhiE(HadronicVHtD1Pt_JetSubCalc->at(iW),HadronicVHtD1Eta_JetSubCalc->at(iW),HadronicVHtD1Phi_JetSubCalc->at(iW),HadronicVHtD1E_JetSubCalc->at(iW));
	      d3.SetPtEtaPhiE(HadronicVHtD2Pt_JetSubCalc->at(iW),HadronicVHtD2Eta_JetSubCalc->at(iW),HadronicVHtD2Phi_JetSubCalc->at(iW),HadronicVHtD2E_JetSubCalc->at(iW));
	    }
   	  }	 
	  
	  bool WallDsInJet = false;
	  bool TallDsInJet = false;
	  if(matchedID != 6 && ak8_lv.DeltaR(d1) < 0.8 && ak8_lv.DeltaR(d2) < 0.8) WallDsInJet = true;
	  if(matchedID == 6 && ak8_lv.DeltaR(d1) < 0.8 && ak8_lv.DeltaR(d2) < 0.8 && ak8_lv.DeltaR(d3) < 0.8) TallDsInJet = true;
	  if(minDR < 0.8 && matchedID == 24 && WallDsInJet) isWmatched = true;
	  if(minDR < 0.8 && matchedID == 25 && WallDsInJet) isHmatched = true;
	  if(minDR < 0.8 && matchedID == 23 && WallDsInJet) isZmatched = true;
	  if(minDR < 0.8 && matchedID == 6 && TallDsInJet) isTmatched = true;

	  theJetAK8Wmatch_JetSubCalc_PtOrdered.push_back(isWmatched);
	  theJetAK8Hmatch_JetSubCalc_PtOrdered.push_back(isHmatched);
	  theJetAK8Zmatch_JetSubCalc_PtOrdered.push_back(isZmatched);
	  theJetAK8Tmatch_JetSubCalc_PtOrdered.push_back(isTmatched);
	  if(isWmatched || isZmatched || isHmatched || isTmatched) theJetAK8MatchedPt_JetSubCalc_PtOrdered.push_back(matchedPt);
	  else theJetAK8MatchedPt_JetSubCalc_PtOrdered.push_back(-99.0);

	  if (not (isWmatched && matchedPt > 200) && not (isZmatched && matchedPt > 200) && not (isTmatched && matchedPt > 400) && not (isHmatched && matchedPt > 300)) {
	    int firstsub = theJetAK8SDSubjetIndex_JetSubCalc_PtOrdered.at(ijet);
	    int nsubs = theJetAK8SDSubjetSize_JetSubCalc_PtOrdered.at(ijet);
	    for(int isub = firstsub; isub < firstsub + nsubs; isub++){
	      if( theJetAK8SDSubjetHFlav_JetSubCalc->at(isub) == 5 ) isBmatched = true;
	    }
	    if ( not isBmatched ) isJmatched = true;
	  }

	  if(isJmatched) theJetAK8Truth_JetSubCalc_PtOrdered.push_back(0);
	  if(isTmatched && matchedPt > 400) theJetAK8Truth_JetSubCalc_PtOrdered.push_back(1);
	  if(isHmatched && matchedPt > 300) theJetAK8Truth_JetSubCalc_PtOrdered.push_back(2);
	  if(isZmatched && matchedPt > 200) theJetAK8Truth_JetSubCalc_PtOrdered.push_back(3);
	  if(isWmatched && matchedPt > 200) theJetAK8Truth_JetSubCalc_PtOrdered.push_back(4);
	  if(isBmatched) theJetAK8Truth_JetSubCalc_PtOrdered.push_back(5);

	  // ------------------------------------------------------------------------------------------------------------------
	  // Sum up an input uncertainty for DeepAK8 scale factors
	  // ------------------------------------------------------------------------------------------------------------------

	  if (dnn_largest_DeepAK8Calc_PtOrdered.at(ijet) == 1){
	    if(isTmatched) DeepAK8SF_TeffUp += 0.05;
	    else DeepAK8SF_TmisUp += 0.05;
	  }
	  else if (dnn_largest_DeepAK8Calc_PtOrdered.at(ijet) == 2){
	    if(isHmatched) DeepAK8SF_HeffUp += 0.05;
	    else DeepAK8SF_HmisUp += 0.05;
	  }
	  else if (dnn_largest_DeepAK8Calc_PtOrdered.at(ijet) == 3){
	    if(isZmatched) DeepAK8SF_ZeffUp += 0.05;
	    else DeepAK8SF_ZmisUp += 0.05;
	  }
	  else if (dnn_largest_DeepAK8Calc_PtOrdered.at(ijet) == 4){
	    if(isWmatched) DeepAK8SF_WeffUp += 0.05;
	    else DeepAK8SF_WmisUp += 0.05;
	  }
	  else if (dnn_largest_DeepAK8Calc_PtOrdered.at(ijet) == 5){
	    if(isBmatched) DeepAK8SF_BeffUp += 0.05;
	    else DeepAK8SF_BmisUp += 0.05;
	  }
	  else{
	    if(isJmatched) DeepAK8SF_JeffUp += 0.05;
	    else DeepAK8SF_JmisUp += 0.05;
	  }
	}
      }

      // ----------------------------------------------------------------------------
      // Set the input down uncertainties for DeepAK8 scale factors
      // ----------------------------------------------------------------------------
      DeepAK8SF_TeffDn = 1.0 - (DeepAK8SF_TeffUp - 1.0);
      DeepAK8SF_HeffDn = 1.0 - (DeepAK8SF_HeffUp - 1.0);
      DeepAK8SF_ZeffDn = 1.0 - (DeepAK8SF_ZeffUp - 1.0);
      DeepAK8SF_WeffDn = 1.0 - (DeepAK8SF_WeffUp - 1.0);
      DeepAK8SF_BeffDn = 1.0 - (DeepAK8SF_BeffUp - 1.0);
      DeepAK8SF_JeffDn = 1.0 - (DeepAK8SF_JeffUp - 1.0);
      DeepAK8SF_TmisDn = 1.0 - (DeepAK8SF_TmisUp - 1.0);
      DeepAK8SF_HmisDn = 1.0 - (DeepAK8SF_HmisUp - 1.0);
      DeepAK8SF_ZmisDn = 1.0 - (DeepAK8SF_ZmisUp - 1.0);
      DeepAK8SF_WmisDn = 1.0 - (DeepAK8SF_WmisUp - 1.0);
      DeepAK8SF_BmisDn = 1.0 - (DeepAK8SF_BmisUp - 1.0);
      DeepAK8SF_JmisDn = 1.0 - (DeepAK8SF_JmisUp - 1.0);

      // ----------------------------------------------------------------------------
      // W --> l nu with mass constraint
      // ----------------------------------------------------------------------------

      double metpx = corr_met_MultiLepCalc*cos(corr_met_phi_MultiLepCalc);
      double metpy = corr_met_MultiLepCalc*sin(corr_met_phi_MultiLepCalc);
      double metpt = corr_met_MultiLepCalc;
      TLorentzVector Wlv_1, Wlv_2, Wlv, Nulv_1, Nulv_2;
      double nuPz_1;
      double nuPz_2;
      
      double Dtmp = MW*MW - lepM*lepM + 2*(lepton_lv.Px()*metpx + lepton_lv.Py()*metpy);
      double Atmp = 4.0*(lepton_lv.Energy()*lepton_lv.Energy() - lepton_lv.Pz()*lepton_lv.Pz());
      double Btmp = -4.0*Dtmp*lepton_lv.Pz();
      double Ctmp = 4.0*lepton_lv.Energy()*lepton_lv.Energy()*metpt*metpt - Dtmp*Dtmp;
      
      double DETtmp = Btmp*Btmp-4.0*Atmp*Ctmp;

      if(DETtmp >= 0) {
      	nuPz_1 = (-Btmp+TMath::Sqrt(DETtmp))/(2.0*Atmp);
      	nuPz_2 = (-Btmp-TMath::Sqrt(DETtmp))/(2.0*Atmp);
      	Nulv_1.SetPxPyPzE(metpx,metpy,nuPz_1,TMath::Sqrt((metpt)*(metpt)+(nuPz_1)*(nuPz_1)));
      	Nulv_2.SetPxPyPzE(metpx,metpy,nuPz_2,TMath::Sqrt((metpt)*(metpt)+(nuPz_2)*(nuPz_2)));
      }else{
      	nuPz_1 = -Btmp/(2.0*Atmp);
      	nuPz_2 = -Btmp/(2.0*Atmp);
	// does another quad solution for pT and scales pT in result. Reduces M, pT, DR.
      	double alpha = lepton_lv.Px()*metpx/metpt + lepton_lv.Py()*metpy/metpt;
      	double Delta = MW*MW - lepM*lepM;
      	Atmp = 4.0*(lepton_lv.Pz()*lepton_lv.Pz() - lepton_lv.Energy()*lepton_lv.Energy() + alpha*alpha);
      	Btmp = 4.0*alpha*Delta;
      	Ctmp = Delta*Delta;
      	float DETtmp2 = Btmp*Btmp-4.0*Atmp*Ctmp;
      	double pTnu_1 = (-Btmp+TMath::Sqrt(DETtmp2))/(2.0*Atmp);
      	double pTnu_2 = (-Btmp-TMath::Sqrt(DETtmp2))/(2.0*Atmp);
      	Nulv_1.SetPxPyPzE(metpx*pTnu_1/metpt,metpy*pTnu_1/metpt,nuPz_1,TMath::Sqrt(pTnu_1*pTnu_1 + nuPz_1*nuPz_1));
      	Nulv_2.SetPxPyPzE(metpx*pTnu_2/metpt,metpy*pTnu_2/metpt,nuPz_2,TMath::Sqrt(pTnu_2*pTnu_2 + nuPz_2*nuPz_2));
      }
      Wlv_1 = Nulv_1+lepton_lv;
      Wlv_2 = Nulv_2+lepton_lv;
      
      if (fabs(Wlv_1.M()-MW) < fabs(Wlv_2.M()-MW)) Wlv = Wlv_1;
      else Wlv = Wlv_2;
 
      W_mass = Wlv.M();
      W_pt = Wlv.Pt();
      W_dRLep = Wlv.DeltaR(lepton_lv);

      // ----------------------------------------------------------------------------
      // VLQ decay
      // ----------------------------------------------------------------------------                                                                                                           
      // Find closest b-tagged AK4 to the W vector
      float deltaRbW = 999;
      int bIndex = 999;
      for(unsigned int ijet=0; ijet < theJetPt_JetSubCalc_PtOrdered.size(); ijet++){
	if(isNominal && theJetBTag_JetSubCalc_PtOrdered.at(ijet) == 0) continue;
	else if(isBUp && theJetBTag_bSFup_JetSubCalc_PtOrdered.at(ijet) == 0) continue;
	else if(isBDn && theJetBTag_bSFdn_JetSubCalc_PtOrdered.at(ijet) == 0) continue;
	else if(isLUp && theJetBTag_lSFup_JetSubCalc_PtOrdered.at(ijet) == 0) continue;
	else if(isLDn && theJetBTag_lSFdn_JetSubCalc_PtOrdered.at(ijet) == 0) continue;

	jet_lv.SetPtEtaPhiE(theJetPt_JetSubCalc_PtOrdered.at(ijet),theJetEta_JetSubCalc_PtOrdered.at(ijet),theJetPhi_JetSubCalc_PtOrdered.at(ijet),theJetEnergy_JetSubCalc_PtOrdered.at(ijet));
	if(jet_lv.DeltaR(Wlv) < deltaRbW){
	  deltaRbW = jet_lv.DeltaR(Wlv);
	  bIndex = ijet;
	}
      }

      // Form a leptonic top candidate if the b is close enough
      TLorentzVector top_lv;
      t_mass = -999;
      isLeptonic_W = false;
      isLeptonic_t = false;
      if(minMleppJet > 150){  // best combo of W vs t truth match with this
	//if(deltaRbW >= 0.8 && minMleppJet > 150){  // favors tops
	isLeptonic_W = true;
      }else{
	if(deltaRbW > 0.8) bIndex = ind_MinMlj; // use a close b unless it doesn't exist
	TLorentzVector bottom_lv;
	bottom_lv.SetPtEtaPhiE(theJetPt_JetSubCalc_PtOrdered.at(bIndex),theJetEta_JetSubCalc_PtOrdered.at(bIndex),theJetPhi_JetSubCalc_PtOrdered.at(bIndex),theJetEnergy_JetSubCalc_PtOrdered.at(bIndex));
	top_lv = bottom_lv + Wlv;
	t_mass = top_lv.M();
	t_pt = top_lv.Pt();
	t_dRWb = bottom_lv.DeltaR(Wlv);
	isLeptonic_t = true;
      } 

      // Fill the vector for deltaR from leptonic particle (W or t) from all AK8 Jets
      std::vector<pair<TLorentzVector,int>> jets_lv;
      deltaR_leptonicparticle_AK8_PtOrdered.clear();
      for(unsigned int ijet=0; ijet < theJetAK8Pt_JetSubCalc_PtOrdered.size(); ijet++){
	jet_lv.SetPtEtaPhiE(theJetAK8Pt_JetSubCalc_PtOrdered.at(ijet),theJetAK8Eta_JetSubCalc_PtOrdered.at(ijet),theJetAK8Phi_JetSubCalc_PtOrdered.at(ijet),theJetAK8Energy_JetSubCalc_PtOrdered.at(ijet));
	if(isLeptonic_W) deltaR_leptonicparticle_AK8_PtOrdered.push_back(jet_lv.DeltaR(Wlv));
	if(isLeptonic_t) deltaR_leptonicparticle_AK8_PtOrdered.push_back(jet_lv.DeltaR(top_lv));

	// Get 3 highest-pT jets that are not close to t/W (deltaR > .8) and store AK8 index and 4-vector
	if(jets_lv.size() >= 3) continue;

	//	if(jet_lv.DeltaR(top_lv) > .6 and isLeptonic_t) jets_lv.push_back(std::make_pair(jet_lv,ijet)); // b-quark peak at low DR, minimum near 0.6
	//	if(jet_lv.DeltaR(Wlv) > 0 and isLeptonic_W) jets_lv.push_back(std::make_pair(jet_lv,ijet)); // no peak at low DR in all AK8, cut only very close
	if(jet_lv.DeltaR(top_lv) > 0.8 and isLeptonic_t) jets_lv.push_back(std::make_pair(jet_lv,ijet));
	if(jet_lv.DeltaR(Wlv) > 0.8 and isLeptonic_W) jets_lv.push_back(std::make_pair(jet_lv,ijet));
      }

      highPtAK8Jet1_SoftDropCorrectedMass = -999;
      highPtAK8Jet2_SoftDropCorrectedMass = -999;
      highPtAK8Jet3_SoftDropCorrectedMass = -999;

      leptonicTprimeJetIDs_DeepAK8.clear();
      hadronicTprimeJetIDs_DeepAK8.clear();
      leptonicBprimeJetIDs_DeepAK8.clear();
      hadronicBprimeJetIDs_DeepAK8.clear();
	
      Tprime1_DeepAK8_Mass = -999;
      Tprime2_DeepAK8_Mass = -999;
      Tprime1_DeepAK8_Pt = -999;
      Tprime2_DeepAK8_Pt = -999;
      Tprime1_DeepAK8_Eta = -999;
      Tprime2_DeepAK8_Eta = -999;
      Tprime1_DeepAK8_Phi = -999;
      Tprime2_DeepAK8_Phi = -999;
      Tprime1_DeepAK8_deltaR = -999;
      Tprime2_DeepAK8_deltaR = -999;
      Bprime1_DeepAK8_Mass = -999;
      Bprime2_DeepAK8_Mass = -999;
      Bprime1_DeepAK8_Pt = -999;
      Bprime2_DeepAK8_Pt = -999;
      Bprime1_DeepAK8_Eta = -999;
      Bprime2_DeepAK8_Eta = -999;
      Bprime1_DeepAK8_Phi = -999;
      Bprime2_DeepAK8_Phi = -999;
      Bprime1_DeepAK8_deltaR = -999;
      Bprime2_DeepAK8_deltaR = -999;
      
      validTDecay_DeepAK8 = false;
      validBDecay_DeepAK8 = false;
      taggedBWBW_DeepAK8 = false;
      taggedTHBW_DeepAK8 = false;
      taggedTHTH_DeepAK8 = false;
      taggedTZBW_DeepAK8 = false;
      taggedTZTH_DeepAK8 = false;
      taggedTZTZ_DeepAK8 = false;      
      taggedTWTW_DeepAK8 = false;
      taggedBZTW_DeepAK8 = false;
      taggedBHTW_DeepAK8 = false;

      probSum_DeepAK8_decay = -999;
      probSum_DeepAK8_four = -999;

      // ----------------------------------------------------------------------------
      // VLQ Decay -- 3 AK8 jets away from leptonic particle
      // ----------------------------------------------------------------------------                                                                                                           
      if(jets_lv.size() > 3) std::cout << "Problem: > 3 AK8s for Tprime reco" << std::endl;
      if(jets_lv.size() == 3){

	npass_ThreeJets = npass_ThreeJets + 1;

	// Sums of tag probabilities
	probSum_DeepAK8_decay = 0;
	probSum_DeepAK8_four = 0;
        for (unsigned int i = 0; i < jets_lv.size(); i++) {
	  // "decay" weighted sum of probabilities including B
          probSum_DeepAK8_decay += dnn_B_DeepAK8Calc_PtOrdered.at(jets_lv.at(i).second) + 2*dnn_W_DeepAK8Calc_PtOrdered.at(jets_lv.at(i).second) + 3*dnn_T_DeepAK8Calc_PtOrdered.at(jets_lv.at(i).second) + 4*dnn_Z_DeepAK8Calc_PtOrdered.at(jets_lv.at(i).second) + 5*dnn_H_DeepAK8Calc_PtOrdered.at(jets_lv.at(i).second);

	  // "four" raw sum of heavy particle probabilities
          probSum_DeepAK8_four += dnn_W_DeepAK8Calc_PtOrdered.at(jets_lv.at(i).second) + dnn_T_DeepAK8Calc_PtOrdered.at(jets_lv.at(i).second) + dnn_Z_DeepAK8Calc_PtOrdered.at(jets_lv.at(i).second) + dnn_H_DeepAK8Calc_PtOrdered.at(jets_lv.at(i).second);
	}
	
	// First guess at leptonic T particles
	if(isLeptonic_W) leptonicTprimeJetIDs_DeepAK8.push_back(4);
	if(isLeptonic_t) leptonicTprimeJetIDs_DeepAK8.push_back(1);
	
	// Save masses of the 3 jets for plotting
	highPtAK8Jet1_SoftDropCorrectedMass = theJetAK8SoftDropCorr_PtOrdered.at(jets_lv.at(0).second);
	highPtAK8Jet2_SoftDropCorrectedMass = theJetAK8SoftDropCorr_PtOrdered.at(jets_lv.at(1).second);
	highPtAK8Jet3_SoftDropCorrectedMass = theJetAK8SoftDropCorr_PtOrdered.at(jets_lv.at(2).second);
	
	// ----------------------------------------------------------------------------
	// DeepAK8 SECTION -- TT
	// ----------------------------------------------------------------------------                                                                                                   

	// get the tags
	int jet1_DeepAK8 = dnn_largest_DeepAK8Calc_PtOrdered.at(jets_lv.at(0).second);
	int jet2_DeepAK8 = dnn_largest_DeepAK8Calc_PtOrdered.at(jets_lv.at(1).second);
	int jet3_DeepAK8 = dnn_largest_DeepAK8Calc_PtOrdered.at(jets_lv.at(2).second);
	// Group all the light quarks up, boosts valids without loss of res
	//	if(jet1_DeepAK8 == 0) jet1_DeepAK8 = 5;
	//	if(jet2_DeepAK8 == 0) jet2_DeepAK8 = 5;
	//	if(jet3_DeepAK8 == 0) jet3_DeepAK8 = 5;

	// pair up the jet tag with the pT index 0,1,2 and sort by tag (orders J, T, H, Z, W, B)
	std::vector <pair<int,int>> decayJets_DeepAK8;
	decayJets_DeepAK8.push_back(std::make_pair(jet1_DeepAK8,0));
	decayJets_DeepAK8.push_back(std::make_pair(jet2_DeepAK8,1));
	decayJets_DeepAK8.push_back(std::make_pair(jet3_DeepAK8,2));
	std::sort(decayJets_DeepAK8.begin(),decayJets_DeepAK8.end());

	// Start forming 4 particle groups
	TLorentzVector Tprime1_DeepAK8_lv;
	TLorentzVector Tprime2_DeepAK8_lv;
	TLorentzVector Bprime1_DeepAK8_lv;
	TLorentzVector Bprime2_DeepAK8_lv;
	validBDecay_DeepAK8 = false;
	validTDecay_DeepAK8 = false;
	if(isLeptonic_t){

	  float massDiff1=(top_lv+jets_lv.at(decayJets_DeepAK8.at(1).second).first).M()-(jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first).M();
	  float massDiff2=(top_lv+jets_lv.at(decayJets_DeepAK8.at(2).second).first).M()-(jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(1).second).first).M();
	  float massDiff3=(top_lv+jets_lv.at(decayJets_DeepAK8.at(0).second).first).M()-(jets_lv.at(decayJets_DeepAK8.at(1).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first).M();

	  if(decayJets_DeepAK8.at(0).first==2 && decayJets_DeepAK8.at(1).first==4 && decayJets_DeepAK8.at(2).first==5){ // TT -> tH bW, BB -> tW bH
	    validTDecay_DeepAK8 = true;
	    taggedTHBW_DeepAK8 = true;
	    leptonicTprimeJetIDs_DeepAK8.push_back(2); // assign the H with the leptonic top
	    hadronicTprimeJetIDs_DeepAK8 = {4,5};      // assign the b & W as hadronic
	    Tprime1_DeepAK8_lv = top_lv+jets_lv.at(decayJets_DeepAK8.at(0).second).first; // decayJets.second gives the jets_lv index to get 4-vec
	    Tprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(1).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	    Tprime1_DeepAK8_deltaR = top_lv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(0).second).first);
	    Tprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(1).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);

	    validBDecay_DeepAK8 = true;
	    taggedBHTW_DeepAK8 = true;
	    leptonicBprimeJetIDs_DeepAK8.push_back(4); // assign the W with leptonic top
	    hadronicBprimeJetIDs_DeepAK8 = {2,5};      // assign bH hadronic
	    Bprime1_DeepAK8_lv = top_lv+jets_lv.at(decayJets_DeepAK8.at(1).second).first; // decayJets.second gives the jets_lv index to get 4-vec
	    Bprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	    Bprime1_DeepAK8_deltaR = top_lv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(1).second).first);
	    Bprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(0).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);

	  }
	  else if(decayJets_DeepAK8.at(0).first==1 && decayJets_DeepAK8.at(1).first==2 && decayJets_DeepAK8.at(2).first==2){ // TTbar --> tH and tH
	    validTDecay_DeepAK8 = true;
	    taggedTHTH_DeepAK8 = true;
	    leptonicTprimeJetIDs_DeepAK8.push_back(2); // assign an H with leptonic top
	    hadronicTprimeJetIDs_DeepAK8 = {1,2};      // assign tH hadronic
	    // options (lepTop + H1) - (T0 + H2) OR (lepTop + H2) - (T0 + H1) checking smallest
	    if(massDiff1 < massDiff2) { // (lepTop + H1) wins
	      Tprime1_DeepAK8_lv = top_lv+jets_lv.at(decayJets_DeepAK8.at(1).second).first; 
	      Tprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Tprime1_DeepAK8_deltaR = top_lv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(1).second).first);
	      Tprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(0).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	    } else { // (lepTop + H2) wins
	      Tprime1_DeepAK8_lv = top_lv+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Tprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(1).second).first;
	      Tprime1_DeepAK8_deltaR = top_lv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	      Tprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(1).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(0).second).first);
	    }
	  }
	  else if(decayJets_DeepAK8.at(0).first==1 && decayJets_DeepAK8.at(1).first==2 && decayJets_DeepAK8.at(2).first==3){ // TTbar --> tH and tZ
	    validTDecay_DeepAK8 = true;
	    taggedTZTH_DeepAK8 = true;
	    // options (lepTop + H1) - (T0 + Z2) OR (lepTop + Z2) - (T0 + H1)
	    if(massDiff1 < massDiff2) { // (lepTop + H1) wins
	      leptonicTprimeJetIDs_DeepAK8.push_back(2); // tH
	      hadronicTprimeJetIDs_DeepAK8 = {1,3}; // tZ
	      Tprime1_DeepAK8_lv = top_lv+jets_lv.at(decayJets_DeepAK8.at(1).second).first; 
	      Tprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Tprime1_DeepAK8_deltaR = top_lv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(1).second).first);
	      Tprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(0).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	    } else { // (lepTop + Z2) wins
	      leptonicTprimeJetIDs_DeepAK8.push_back(3);
	      hadronicTprimeJetIDs_DeepAK8 = {1,2};
	      Tprime1_DeepAK8_lv = top_lv+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Tprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(1).second).first;
	      Tprime1_DeepAK8_deltaR = top_lv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	      Tprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(1).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(0).second).first);
	    }
	  }
	  else if(decayJets_DeepAK8.at(0).first==3 && decayJets_DeepAK8.at(1).first==4 && decayJets_DeepAK8.at(2).first==5){ // TT -> tZ bW, BB -> tW bZ
	    validTDecay_DeepAK8 = true;
	    taggedTZBW_DeepAK8 = true;
	    leptonicTprimeJetIDs_DeepAK8.push_back(3); // tZ
	    hadronicTprimeJetIDs_DeepAK8 = {4,5}; // bW
	    Tprime1_DeepAK8_lv = top_lv+jets_lv.at(decayJets_DeepAK8.at(0).second).first;
	    Tprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(1).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	    Tprime1_DeepAK8_deltaR = top_lv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(0).second).first);
	    Tprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(1).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);

	    validBDecay_DeepAK8 = true;
	    taggedBZTW_DeepAK8 = true;
	    leptonicBprimeJetIDs_DeepAK8.push_back(4); // assign the W with the leptonic top
	    hadronicBprimeJetIDs_DeepAK8 = {3,5};      // assign the b & Z as hadronic
	    Bprime1_DeepAK8_lv = top_lv+jets_lv.at(decayJets_DeepAK8.at(1).second).first; // decayJets.second gives the jets_lv index to get 4-vec
	    Bprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	    Bprime1_DeepAK8_deltaR = top_lv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(1).second).first);
	    Bprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(0).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	  }
	  else if(decayJets_DeepAK8.at(0).first==1 && decayJets_DeepAK8.at(1).first==3 && decayJets_DeepAK8.at(2).first==3){ // TTbar --> tZ tZ
	    validTDecay_DeepAK8 = true;
	    taggedTZTZ_DeepAK8 = true;
	    leptonicTprimeJetIDs_DeepAK8.push_back(3); // tZ
	    hadronicTprimeJetIDs_DeepAK8 = {1,3}; // tZ
	    // options (lepTop + Z1) - (T0 + Z2) OR (lepTop + Z2) - (T0 + Z1)
	    if(massDiff1 < massDiff2) { // (lepTop + Z1) wins
	      Tprime1_DeepAK8_lv = top_lv+jets_lv.at(decayJets_DeepAK8.at(1).second).first; 
	      Tprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Tprime1_DeepAK8_deltaR = top_lv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(1).second).first);
	      Tprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(0).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	    } else { // (lepTop + Z2) wins
	      Tprime1_DeepAK8_lv = top_lv+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Tprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(1).second).first;
	      Tprime1_DeepAK8_deltaR = top_lv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	      Tprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(1).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(0).second).first);
	    } 
	  }
	  else if(decayJets_DeepAK8.at(0).first==1 && decayJets_DeepAK8.at(1).first==4 && decayJets_DeepAK8.at(2).first==4){ // BB/XX -> tW tW, jets t W W
	    validBDecay_DeepAK8 = true;
	    taggedTWTW_DeepAK8 = true;
	    leptonicBprimeJetIDs_DeepAK8.push_back(4); // tW
	    hadronicBprimeJetIDs_DeepAK8 = {1,4}; // tW
	    // options (lepTop + W1) - (T0 + W2) OR (lepTop + W2) - (T0 + W1)
	    if(massDiff1 < massDiff2) { // (lepTop + W1) wins
	      Bprime1_DeepAK8_lv = top_lv+jets_lv.at(decayJets_DeepAK8.at(1).second).first; 
	      Bprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Bprime1_DeepAK8_deltaR = top_lv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(1).second).first);
	      Bprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(0).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	    } else { // (lepTop + W2) wins
	      Bprime1_DeepAK8_lv = top_lv+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Bprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(1).second).first;
	      Bprime1_DeepAK8_deltaR = top_lv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	      Bprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(0).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(1).second).first);
	    }
	  }

	  if(!validTDecay_DeepAK8){ // Not a valid T decay combination
	    if(massDiff3 < massDiff1 and massDiff3 < massDiff2){  // lepTop + 0 is best
	      leptonicTprimeJetIDs_DeepAK8.push_back(decayJets_DeepAK8.at(0).first);
	      hadronicTprimeJetIDs_DeepAK8 = {decayJets_DeepAK8.at(1).first, decayJets_DeepAK8.at(2).first};
	      Tprime1_DeepAK8_lv = top_lv+jets_lv.at(decayJets_DeepAK8.at(0).second).first;
	      Tprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(1).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Tprime1_DeepAK8_deltaR = top_lv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(0).second).first);
	      Tprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(1).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	    } else if(massDiff1 < massDiff2 and massDiff1 < massDiff3){ // lepTop + 1 is best 
	      leptonicTprimeJetIDs_DeepAK8.push_back(decayJets_DeepAK8.at(1).first);
	      hadronicTprimeJetIDs_DeepAK8 = {decayJets_DeepAK8.at(0).first, decayJets_DeepAK8.at(2).first};
	      Tprime1_DeepAK8_lv = top_lv+jets_lv.at(decayJets_DeepAK8.at(1).second).first;
	      Tprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Tprime1_DeepAK8_deltaR = top_lv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(1).second).first);
	      Tprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(0).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	    } else { // lepTop + 2 is best
	      leptonicTprimeJetIDs_DeepAK8.push_back(decayJets_DeepAK8.at(2).first);
	      hadronicTprimeJetIDs_DeepAK8 = {decayJets_DeepAK8.at(0).first, decayJets_DeepAK8.at(1).first};
	      Tprime1_DeepAK8_lv = top_lv+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Tprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(1).second).first;
	      Tprime1_DeepAK8_deltaR = top_lv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	      Tprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(1).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(0).second).first);
	    }
	  }
	  if(!validBDecay_DeepAK8){ // Not a valid B decay combination
	    if(massDiff3 < massDiff1 and massDiff3 < massDiff2){  // lepTop + 0 is best
	      leptonicBprimeJetIDs_DeepAK8.push_back(decayJets_DeepAK8.at(0).first);
	      hadronicBprimeJetIDs_DeepAK8 = {decayJets_DeepAK8.at(1).first, decayJets_DeepAK8.at(2).first};
	      Bprime1_DeepAK8_lv = top_lv+jets_lv.at(decayJets_DeepAK8.at(0).second).first;
	      Bprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(1).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Bprime1_DeepAK8_deltaR = top_lv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(0).second).first);
	      Bprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(1).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	    } else if(massDiff1 < massDiff2 and massDiff1 < massDiff3){ // lepTop + 1 is best 
	      leptonicBprimeJetIDs_DeepAK8.push_back(decayJets_DeepAK8.at(1).first);
	      hadronicBprimeJetIDs_DeepAK8 = {decayJets_DeepAK8.at(0).first, decayJets_DeepAK8.at(2).first};
	      Bprime1_DeepAK8_lv = top_lv+jets_lv.at(decayJets_DeepAK8.at(1).second).first;
	      Bprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Bprime1_DeepAK8_deltaR = top_lv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(1).second).first);
	      Bprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(0).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	    } else { // lepTop + 2 is best
	      leptonicBprimeJetIDs_DeepAK8.push_back(decayJets_DeepAK8.at(2).first);
	      hadronicBprimeJetIDs_DeepAK8 = {decayJets_DeepAK8.at(0).first, decayJets_DeepAK8.at(1).first};
	      Bprime1_DeepAK8_lv = top_lv+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Bprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(1).second).first;
	      Bprime1_DeepAK8_deltaR = top_lv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	      Bprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(1).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(0).second).first);
	    }
	  }
	}else{ // isLeptonic_W

	  float massDiff1=(Wlv+jets_lv.at(decayJets_DeepAK8.at(1).second).first).M()-(jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first).M();
	  float massDiff2=(Wlv+jets_lv.at(decayJets_DeepAK8.at(2).second).first).M()-(jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(1).second).first).M();
	  float massDiff3=(Wlv+jets_lv.at(decayJets_DeepAK8.at(0).second).first).M()-(jets_lv.at(decayJets_DeepAK8.at(1).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first).M();

	  if(decayJets_DeepAK8.at(0).first==4 && decayJets_DeepAK8.at(1).first==5 && decayJets_DeepAK8.at(2).first==5){ // bW bW
	    validTDecay_DeepAK8 = true;
	    taggedBWBW_DeepAK8 = true;
	    leptonicTprimeJetIDs_DeepAK8.push_back(5); // bW
	    hadronicTprimeJetIDs_DeepAK8 = {4,5}; // bW
	    // options (lepW + b1) - (W0 + b2) OR (lepW + b2) - (W0 + b1)
	    if(massDiff1 < massDiff2) { // (lepW + b1) wins
	      Tprime1_DeepAK8_lv = Wlv+jets_lv.at(decayJets_DeepAK8.at(1).second).first;
	      Tprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Tprime1_DeepAK8_deltaR = Wlv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(1).second).first);
	      Tprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(0).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	    } else { // (lepW + b2) wins
	      Tprime1_DeepAK8_lv = Wlv+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Tprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(1).second).first;
	      Tprime1_DeepAK8_deltaR = Wlv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	      Tprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(0).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(1).second).first);
	    }
	  }
	  else if(decayJets_DeepAK8.at(0).first==1 && decayJets_DeepAK8.at(1).first==3 && decayJets_DeepAK8.at(2).first==5){ // TT -> bW tZ, BB -> tW bZ
	    validTDecay_DeepAK8 = true;
	    taggedTZBW_DeepAK8 = true;
	    leptonicTprimeJetIDs_DeepAK8.push_back(5); // bW
	    hadronicTprimeJetIDs_DeepAK8 = {1,3}; // tZ
	    Tprime1_DeepAK8_lv = Wlv+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	    Tprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(1).second).first;
	    Tprime1_DeepAK8_deltaR = Wlv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	    Tprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(0).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(1).second).first);

	    validBDecay_DeepAK8 = true;
	    taggedBZTW_DeepAK8 = true;
	    leptonicBprimeJetIDs_DeepAK8.push_back(4); // tW
	    hadronicBprimeJetIDs_DeepAK8 = {3,5}; // bZ
	    Bprime1_DeepAK8_lv = Wlv+jets_lv.at(decayJets_DeepAK8.at(0).second).first;
	    Bprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(1).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	    Bprime1_DeepAK8_deltaR = Wlv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(0).second).first);
	    Bprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(1).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	  }
	  else if(decayJets_DeepAK8.at(0).first==1 && decayJets_DeepAK8.at(1).first==2 && decayJets_DeepAK8.at(2).first==5){ // TT -> bW tH, BB -> tW bH
	    validTDecay_DeepAK8 = true;	    
	    taggedTHBW_DeepAK8 = true;
	    leptonicTprimeJetIDs_DeepAK8.push_back(5); // bW
	    hadronicTprimeJetIDs_DeepAK8 = {1,2}; // tH
	    Tprime1_DeepAK8_lv = Wlv+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	    Tprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(1).second).first;
	    Tprime1_DeepAK8_deltaR = Wlv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	    Tprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(0).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(1).second).first);

	    validBDecay_DeepAK8 = true;	    
	    taggedBHTW_DeepAK8 = true;
	    leptonicBprimeJetIDs_DeepAK8.push_back(4); // tW
	    hadronicBprimeJetIDs_DeepAK8 = {2,5}; // bH
	    Bprime1_DeepAK8_lv = Wlv+jets_lv.at(decayJets_DeepAK8.at(0).second).first;
	    Bprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(1).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	    Bprime1_DeepAK8_deltaR = Wlv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(0).second).first);
	    Bprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(2).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	  }
	  else if(decayJets_DeepAK8.at(0).first==1 && decayJets_DeepAK8.at(1).first==1 && decayJets_DeepAK8.at(2).first==4){ // BB -> tW tW, jets t t W
	    validBDecay_DeepAK8 = true;
	    taggedTWTW_DeepAK8 = true;
	    leptonicBprimeJetIDs_DeepAK8.push_back(1); // tW
	    hadronicBprimeJetIDs_DeepAK8 = {1,4}; // tW
	    // options (lepW + t0) - (W2 + t1) OR (lepW + t1) - (W2 + t0)
	    if(massDiff3 < massDiff1) { // (lepW + t0) wins
	      Bprime1_DeepAK8_lv = Wlv+jets_lv.at(decayJets_DeepAK8.at(0).second).first;
	      Bprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(1).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Bprime1_DeepAK8_deltaR = Wlv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(0).second).first);
	      Bprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(1).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	    } else { // (lepW + t1) wins
	      Bprime1_DeepAK8_lv = Wlv+jets_lv.at(decayJets_DeepAK8.at(1).second).first;
	      Bprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Bprime1_DeepAK8_deltaR = Wlv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(1).second).first);
	      Bprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(0).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	    }
	  }
	  
	  if(!validTDecay_DeepAK8){ // not a valid grouping
	    if(massDiff3 < massDiff1 and massDiff3 < massDiff2){ // lepW + 0 wins
	      leptonicTprimeJetIDs_DeepAK8.push_back(decayJets_DeepAK8.at(0).first);
	      hadronicTprimeJetIDs_DeepAK8 = {decayJets_DeepAK8.at(1).first, decayJets_DeepAK8.at(2).first};
	      Tprime1_DeepAK8_lv = Wlv+jets_lv.at(decayJets_DeepAK8.at(0).second).first;
	      Tprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(1).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Tprime1_DeepAK8_deltaR = Wlv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(0).second).first);
	      Tprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(1).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	    } else if(massDiff1 < massDiff2 and massDiff1 < massDiff3){ // lepW + 1 wins
	      leptonicTprimeJetIDs_DeepAK8.push_back(decayJets_DeepAK8.at(1).first);
	      hadronicTprimeJetIDs_DeepAK8 = {decayJets_DeepAK8.at(0).first, decayJets_DeepAK8.at(2).first};
	      Tprime1_DeepAK8_lv = Wlv+jets_lv.at(decayJets_DeepAK8.at(1).second).first;
	      Tprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Tprime1_DeepAK8_deltaR = Wlv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(1).second).first);
	      Tprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(0).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	    } else { // lepW + 2 wins
	      leptonicTprimeJetIDs_DeepAK8.push_back(decayJets_DeepAK8.at(2).first);
	      hadronicTprimeJetIDs_DeepAK8 = {decayJets_DeepAK8.at(0).first, decayJets_DeepAK8.at(1).first};
	      Tprime1_DeepAK8_lv = Wlv+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Tprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(1).second).first;
	      Tprime1_DeepAK8_deltaR = Wlv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	      Tprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(0).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	    }
	  }
	  if(!validBDecay_DeepAK8){ // not a valid grouping
	    if(massDiff3 < massDiff1 and massDiff3 < massDiff2){ // lepW + 0 wins
	      leptonicBprimeJetIDs_DeepAK8.push_back(decayJets_DeepAK8.at(0).first);
	      hadronicBprimeJetIDs_DeepAK8 = {decayJets_DeepAK8.at(1).first, decayJets_DeepAK8.at(2).first};
	      Bprime1_DeepAK8_lv = Wlv+jets_lv.at(decayJets_DeepAK8.at(0).second).first;
	      Bprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(1).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Bprime1_DeepAK8_deltaR = Wlv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(0).second).first);
	      Bprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(1).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	    } else if(massDiff1 < massDiff2 and massDiff1 < massDiff3){ // lepW + 1 wins
	      leptonicBprimeJetIDs_DeepAK8.push_back(decayJets_DeepAK8.at(1).first);
	      hadronicBprimeJetIDs_DeepAK8 = {decayJets_DeepAK8.at(0).first, decayJets_DeepAK8.at(2).first};
	      Bprime1_DeepAK8_lv = Wlv+jets_lv.at(decayJets_DeepAK8.at(1).second).first;
	      Bprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Bprime1_DeepAK8_deltaR = Wlv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(1).second).first);
	      Bprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(0).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	    } else { // lepW + 2 wins
	      leptonicBprimeJetIDs_DeepAK8.push_back(decayJets_DeepAK8.at(2).first);
	      hadronicBprimeJetIDs_DeepAK8 = {decayJets_DeepAK8.at(0).first, decayJets_DeepAK8.at(1).first};
	      Bprime1_DeepAK8_lv = Wlv+jets_lv.at(decayJets_DeepAK8.at(2).second).first;
	      Bprime2_DeepAK8_lv = jets_lv.at(decayJets_DeepAK8.at(0).second).first+jets_lv.at(decayJets_DeepAK8.at(1).second).first;
	      Bprime1_DeepAK8_deltaR = Wlv.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	      Bprime2_DeepAK8_deltaR = jets_lv.at(decayJets_DeepAK8.at(0).second).first.DeltaR(jets_lv.at(decayJets_DeepAK8.at(2).second).first);
	    }
	  }
	}
	
        if(Tprime1_DeepAK8_lv.M() != -999){
	  Tprime1_DeepAK8_Mass = Tprime1_DeepAK8_lv.M();
	  Tprime2_DeepAK8_Mass = Tprime2_DeepAK8_lv.M();
	  Tprime1_DeepAK8_Pt = Tprime1_DeepAK8_lv.Pt();
	  Tprime2_DeepAK8_Pt = Tprime2_DeepAK8_lv.Pt();
	  Tprime1_DeepAK8_Eta = Tprime1_DeepAK8_lv.Eta();
	  Tprime2_DeepAK8_Eta = Tprime2_DeepAK8_lv.Eta();
	  Tprime1_DeepAK8_Phi = Tprime1_DeepAK8_lv.Phi();
	  Tprime2_DeepAK8_Phi = Tprime2_DeepAK8_lv.Phi();
	}
        if(Bprime1_DeepAK8_lv.M() != -999){
	  Bprime1_DeepAK8_Mass = Bprime1_DeepAK8_lv.M();
	  Bprime2_DeepAK8_Mass = Bprime2_DeepAK8_lv.M();
	  Bprime1_DeepAK8_Pt = Bprime1_DeepAK8_lv.Pt();
	  Bprime2_DeepAK8_Pt = Bprime2_DeepAK8_lv.Pt();
	  Bprime1_DeepAK8_Eta = Bprime1_DeepAK8_lv.Eta();
	  Bprime2_DeepAK8_Eta = Bprime2_DeepAK8_lv.Eta();
	  Bprime1_DeepAK8_Phi = Bprime1_DeepAK8_lv.Phi();
	  Bprime2_DeepAK8_Phi = Bprime2_DeepAK8_lv.Phi();
	}

	// Numerator of confusion matrix
	if(isSig){
	  if(confusionTruth < 6.0){
	    if(taggedBWBW_DeepAK8) TTconfusionN->Fill(0.5,confusionTruth);
	    else if(taggedTZBW_DeepAK8) TTconfusionN->Fill(1.5,confusionTruth);
	    else if(taggedTHBW_DeepAK8) TTconfusionN->Fill(2.5,confusionTruth);
	    else if(taggedTZTH_DeepAK8) TTconfusionN->Fill(3.5,confusionTruth);
	    else if(taggedTZTZ_DeepAK8) TTconfusionN->Fill(4.5,confusionTruth);
	    else if(taggedTHTH_DeepAK8) TTconfusionN->Fill(5.5,confusionTruth);
	    else{
	      if(hadronicTprimeJetIDs_DeepAK8.at(0) == 4 && hadronicTprimeJetIDs_DeepAK8.at(1) == 5) TTconfusionN->Fill(6.5,confusionTruth);
	      else if(hadronicTprimeJetIDs_DeepAK8.at(0) == 1 && hadronicTprimeJetIDs_DeepAK8.at(1) == 3) TTconfusionN->Fill(7.5,confusionTruth);
	      else if(hadronicTprimeJetIDs_DeepAK8.at(0) == 1 && hadronicTprimeJetIDs_DeepAK8.at(1) == 2) TTconfusionN->Fill(8.5,confusionTruth);
	      else TTconfusionN->Fill(9.5,confusionTruth);
	    }
	  }else{
	    if(taggedTWTW_DeepAK8) BBconfusionN->Fill(0.5,confusionTruth-6.0);
	    else if(taggedBZTW_DeepAK8) BBconfusionN->Fill(1.5,confusionTruth-6.0);
	    else if(taggedBHTW_DeepAK8) BBconfusionN->Fill(2.5,confusionTruth-6.0);
	    else{
	      if(hadronicBprimeJetIDs_DeepAK8.at(0) == 1 && hadronicBprimeJetIDs_DeepAK8.at(1) == 4) BBconfusionN->Fill(3.5,confusionTruth-6.0);
	      else if(hadronicBprimeJetIDs_DeepAK8.at(0) == 3 && hadronicBprimeJetIDs_DeepAK8.at(1) == 5) BBconfusionN->Fill(4.5,confusionTruth-6.0);
	      else if(hadronicBprimeJetIDs_DeepAK8.at(0) == 2 && hadronicBprimeJetIDs_DeepAK8.at(1) == 5) BBconfusionN->Fill(5.5,confusionTruth-6.0);
	      else BBconfusionN->Fill(6.5,confusionTruth-6.0);
	    }
	  }
	}
      }

      // ----------------------------------------------------------------------------
      // PDF and Matrix Element energy scale weights
      // ----------------------------------------------------------------------------

      std::vector<double> renorm;
      std::vector<double> pdf;
      renormWeights.clear();
      pdfWeights.clear();
      pdfNewWeights.clear();
      pdfNewNominalWeight = 1.0;
      if(isSig){
	pdfNewNominalWeight = NewPDFweights_MultiLepCalc->at(0);
	// SEEMS TO APPLY TO ALL B2G MG+PYTHIA SIGNALS. NNLO 4-FLAVOR PDF
	for(unsigned int i = 0; i < LHEweightids_MultiLepCalc->size(); i++){
	  if(i > 0 && i < 101) pdfNewWeights.push_back(NewPDFweights_MultiLepCalc->at(i));
	  if(LHEweightids_MultiLepCalc->at(i) > 1 && LHEweightids_MultiLepCalc->at(i) < 10){
	    if(LHEweightids_MultiLepCalc->at(i) == 6 || LHEweightids_MultiLepCalc->at(i) == 8) continue;
	    renormWeights.push_back(LHEweights_MultiLepCalc->at(i));
	  }
	  if(LHEweightids_MultiLepCalc->at(i) > 474 && LHEweightids_MultiLepCalc->at(i) < 575){
	    pdfWeights.push_back(LHEweights_MultiLepCalc->at(i));
	  }
	}
      }
      else if(isMadgraphBkg){
	// SEEMS TO APPLY TO OTHER MG+PYTHIA BACKGROUNDS. LEADING ORDER 5-FLAVOR PDF
	for(unsigned int i = 0; i < LHEweightids_MultiLepCalc->size(); i++){
	  if(LHEweightids_MultiLepCalc->at(i) > 1 && LHEweightids_MultiLepCalc->at(i) < 10){
	    if(LHEweightids_MultiLepCalc->at(i) == 6 || LHEweightids_MultiLepCalc->at(i) == 8) continue;
	    renorm.push_back(LHEweights_MultiLepCalc->at(i));
	    renormWeights.push_back(LHEweights_MultiLepCalc->at(i));
	  }
	  if(LHEweightids_MultiLepCalc->at(i) > 10 && LHEweightids_MultiLepCalc->at(i) < 111){
	    pdf.push_back(LHEweights_MultiLepCalc->at(i));
	    pdfWeights.push_back(LHEweights_MultiLepCalc->at(i));
	  }
	  if(LHEweightids_MultiLepCalc->at(i) == 111 || LHEweightids_MultiLepCalc->at(i) == 112){
	    alphaSWeights.push_back(LHEweights_MultiLepCalc->at(i));
	  }
	}
      }
      else{
	// SEEMS TO APPLY TO ALL POWHEG AND MC@NLO BACKGROUNDS. NLO PDFs
	for(unsigned int i = 0; i < LHEweightids_MultiLepCalc->size(); i++){
	  if(LHEweightids_MultiLepCalc->at(i) > 1001 && LHEweightids_MultiLepCalc->at(i) < 1010){
	    if(LHEweightids_MultiLepCalc->at(i) == 1006 || LHEweightids_MultiLepCalc->at(i) == 1008) continue;
	    renorm.push_back(LHEweights_MultiLepCalc->at(i));
	    renormWeights.push_back(LHEweights_MultiLepCalc->at(i));
	  }
	  if(LHEweightids_MultiLepCalc->at(i) > 2000 && LHEweightids_MultiLepCalc->at(i) < 2101){
	    pdf.push_back(LHEweights_MultiLepCalc->at(i));
	    pdfWeights.push_back(LHEweights_MultiLepCalc->at(i));
	  }
	  if(LHEweightids_MultiLepCalc->at(i) == 2101 || LHEweightids_MultiLepCalc->at(i) == 2102){
	    alphaSWeights.push_back(LHEweights_MultiLepCalc->at(i));
	  }
	}
      }

      if(renormWeights.size() == 0){
	if(isVV){
	  renormWeights.push_back(0.85);
	  renormWeights.push_back(1.15);
	  renormWeights.push_back(0.85);
	  renormWeights.push_back(0.85);
	  renormWeights.push_back(1.15);
	  renormWeights.push_back(1.15);
	}else{
	  for(int irn = 0; irn < 6; irn++){
	    renormWeights.push_back(1.0);
	  }
	}
      }
      if(pdfWeights.size() == 0){
	for(int ipdf = 0; ipdf < 100; ipdf++){
	  pdfWeights.push_back(1.0);
	}
      }

      // ----------------------------------------------------------------------------
      // DONE!! Write the tree
      // ----------------------------------------------------------------------------
      
      outputTree->Fill();
   }
   std::cout<<"Nelectrons             = "<<Nelectrons<<" / "<<nentries<<std::endl;
   std::cout<<"Npassed_ElEta          = "<<npass_ElEta<<" / "<<nentries<<std::endl;
   std::cout<<"Nmuons                 = "<<Nmuons<<" / "<<nentries<<std::endl;
   std::cout<<"Npassed_MuEta          = "<<npass_MuEta<<" / "<<nentries<<std::endl;
   std::cout<<"Npassed_nAK8Jets       = "<<npass_nAK8jets<<" / "<<nentries<<std::endl;
   std::cout<<"Npassed_Trigger        = "<<npass_trigger<<" / "<<nentries<<std::endl;
   std::cout<<"Npassed_MET            = "<<npass_met<<" / "<<nentries<<std::endl;
   std::cout<<"Npassed_lepPt          = "<<npass_lepPt<<" / "<<nentries<<std::endl;
   std::cout<<"Npassed_HT             = "<<npass_ht<<" / "<<nentries<<std::endl;
   std::cout<<"Npassed_ALL            = "<<npass_all<<" / "<<nentries<<std::endl;
   std::cout<<"Npassed_ThreeJets      = "<<npass_ThreeJets<<" / "<<nentries<<std::endl;
   
   if(isSig){
     TTconfusionD->Write();
     TTconfusionN->Write();
     BBconfusionD->Write();
     BBconfusionN->Write();
   }

   outputTree->Write();

}
