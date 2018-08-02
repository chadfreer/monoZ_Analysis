//code to read evemt number
//


#include "TString.h"
#include <string>
#include <vector>
//using namespace std;

void eventnumber(){

double total = 0;
//for (int i = 1; i < 5; ++i){
	//DY 96
	//TString name = "root://cmsxrootd.fnal.gov///store/user/biwang/monoZ_01June2017/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v2/170601_183058/0000/output_" + std::to_string(i) + ".root";

	//TT 147
	//TString name = "root://cmsxrootd.fnal.gov///store/user/biwang/monoZ_01Jan2017/TTTo2L2Nu_TuneCUETP8M2_ttHtranche3_13TeV-powheg-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/170523_185021/0000/output_"+ std::to_string(i) + ".root";

	//WW 3
	//TString name = "root://cmsxrootd.fnal.gov///store/user/biwang/monoZ_01June2017/WWTo2L2Nu_13TeV-powheg/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/170601_183119/0000/output_" + std::to_string(i) + ".root";

	//WZ 4
	//TString name = "root://cmsxrootd.fnal.gov///store/user/biwang/monoZ_01June2017/WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/170613_203125/0000/output_" + std::to_string(i) + ".root";

//TString name1 = "/afs/cern.ch/work/c/cfreer/monoZ/DY_Pt-binned650-inf.root";
//TString name1 = "root://eoscms.cern.ch//eos/cms/store/user/cfreer/monoZ_Nov2017/ZZJJ_ZZTo2L2Nu_EWK_13TeV-madgraph-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/171124_224936/0000/output_";

//TString name[] = {
//"root://cmsxrootd.fnal.gov///store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Pt-50To100_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/100000/00E3D7B3-9DCE-E611-A42D-0025905A609A.root",
//"root://cmsxrootd.fnal.gov///store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Pt-50To100_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/100000/0462F883-C0CE-E611-8CEE-0CC47A4D764A.root",
//"root://cmsxrootd.fnal.gov///store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Pt-50To100_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/100000/0E1E6882-A0CE-E611-BB4A-0CC47A7452D0.root",
//"root://cmsxrootd.fnal.gov///store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Pt-50To100_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/100000/0EC51971-9FCE-E611-A1FE-0025905B85FC.root",
//"root://cmsxrootd.fnal.gov///store/mc/RunIISummer16MiniAODv2/DYJetsToLL_Pt-50To100_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/100000/109D9165-9FCE-E611-9960-0025905A6134.root"};

for (int i = 1; i < 5; ++i){
        //if (i==3) continue;
	//if (i==4 || i==31) continue;
	//if (i==28 || i==89) continue;
	//TString name1 = "root://eoscms.cern.ch//eos/cms/store/user/cfreer/monoZ_Nov2017/ZZJJ_ZZTo2L2Nu_EWK_13TeV-madgraph-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/171124_224936/0000/output_";//2 files
	//TString name1 = "root://eoscms.cern.ch//eos/cms/store/user/cfreer/monoZ_Nov2017/ZZTo2L2Nu_13TeV_powheg_pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/171124_224715/0000/output_"; //22 files
        //TString name1 = "root://eoscms.cern.ch//eos/cms/store/user/cfreer/monoZ_Nov2017/WZTo3LNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/180111_151022/0000/output_"; //19 files
	//TString name1 = "root://eoscms.cern.ch//eos/cms/store/user/cfreer/monoZ_Nov2017/WWTo2L2Nu_13TeV-powheg/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/171124_224913/0000/output_"; //3 files
        //TString name1 = "root://eoscms.cern.ch//eos/cms/store/user/cfreer/monoZ_Nov2017/TTTo2L2Nu_TuneCUETP8M2_ttHtranche3_13TeV-powheg-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/171124_224544/0000/output_"; //147 missing 28 and 89
	//TString name1 = "root://eoscms.cern.ch//eos/cms/store/user/cfreer/monoZ_Nov2017/DarkMatter_MonoZToLL_NLO_Vector_Mx2-1_Mv-500_gDM1_gQ0p25_TuneCUETP8M1_Mx1-0p1_ctau-1_13TeV-madgraph/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/171212_133459/0000/output_"; //2 files
	//TString name1 = "root://eoscms.cern.ch//eos/cms/store/user/cfreer/monoZ_Nov2017/DarkMatter_MonoZToLL_NLO_Vector_Mx2-150_Mv-500_gDM1_gQ0p25_TuneCUETP8M1_Mx1-1_ctau-1_13TeV-madgraph/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/171212_133257/0000/output_"; //2 files
	//TString name1 = "root://eoscms.cern.ch//eos/cms/store/user/cfreer/monoZ_Nov2017/DYJetsToLL_Pt-650ToInf_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/171124_224507/0000/output_"; //5 files
	//TString name1 = "root://eoscms.cern.ch//eos/cms/store/user/cfreer/monoZ_Nov2017/DYJetsToLL_Pt-400To650_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/171124_224424/0000/output_"; //2 files
        //TString name1 = "root://eoscms.cern.ch//eos/cms/store/user/cfreer/monoZ_Nov2017/DYJetsToLL_Pt-250To400_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/171124_224353/0000/output_"; //3 files
        //TString name1 = "root://eoscms.cern.ch//eos/cms/store/user/cfreer/monoZ_Nov2017/DYJetsToLL_Pt-100To250_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/171124_224329/0000/output_"; //11 files
	//TString name1 = "root://eoscms.cern.ch//eos/cms/store/user/cfreer/monoZ_Nov2017/DYJetsToLL_Pt-50To100_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/171124_224302/0000/output_"; //47 files 4 and 31 missing
	//TString name1 = "root://eoscms.cern.ch//eos/cms/store/user/cfreer/monoZ_Nov2017/DYJetsToLL_M-50_Zpt-150toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/171124_224234/0000/output_"; //6 files 3 missing
	//TString name1 = "root://eoscms.cern.ch//eos/cms/store/user/cfreer/monoZ_Nov2017/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v2/180111_152847/0000/output_"; //96 files
	TString name1 = "root://eoscms.cern.ch//eos/cms/store/user/cfreer/monoZ_Nov2017/WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/180115_120631/0000/output_"; //4 files
        name1 += i;
	name1 += ".root";
	cout << "name: " <<fixed<<setprecision(8)<< name1 << endl;
	TFile *f =TFile::Open(name1);
	double nEvents = ((TH1D*) f->Get("counters/counters"))->GetBinContent(1);
	total = total + nEvents;	
	cout << "number of events: " << total << endl;
	}
}
