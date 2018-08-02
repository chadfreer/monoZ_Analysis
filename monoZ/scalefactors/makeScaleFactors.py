#!/usr/bin/env python
import ROOT
import array
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True


def float2double(hist):
    if hist.ClassName() == 'TH1F':
        new = ROOT.TH1D()
        hist.Copy(new)
    elif hist.ClassName() == 'TH2F':
        new = ROOT.TH2D()
        hist.Copy(new)
    else:
        raise Exception("Bad hist, dummy")
    return new

fScales = ROOT.TFile('scaleFactors.root', 'recreate')

if True:
    # For nTruePU reweighting
    pileupSF = ROOT.ScaleFactor("pileupSF", "Run2016B-H 36.8/fb Pileup profile over RunIISpring16 MC Scale Factor, x=NTruePU")
    pileupFile = ROOT.TFile.Open('PileupWeights/PU_Central.root')
    pileupFileUp = ROOT.TFile.Open('PileupWeights/PU_minBiasUP.root')
    pileupFileDown = ROOT.TFile.Open('PileupWeights/PU_minBiasDOWN.root')
    pileupSF.Set1DHist(pileupFile.Get('pileup'), pileupFileUp.Get('pileup'), pileupFileDown.Get('pileup'))
    fScales.cd()
    pileupSF.Write()
else:
    # For nvtx reweighting
    pileupSF = ROOT.ScaleFactor("pileupSF", "Run2016B-H ReReco 36.5/fb nvtx PU Scale Factor, x=nVertices")
    pileupFile = ROOT.TFile.Open('data/nvtxReRecoReweight.root')
    pileupSF.Set1DHist(pileupFile.Get('nvtx'))
    fScales.cd()
    pileupSF.Write()

electronIdSF = ROOT.ScaleFactor("electronIdSF", "Moriond '17 Electron Medium WP ID SF, x=Eta, y=Pt")
eidFile = ROOT.TFile.Open('data/moriond17ElectronMediumSF.root')
electronIdSF.Set2DHist(float2double(eidFile.Get('EGamma_SF2D')))
fScales.cd()
electronIdSF.Write()

electronGsfSF = ROOT.ScaleFactor("electronGsfSF", "Moriond '17 Electron GSF track reco SF, x=Eta, y=Pt")
eleGsfFile = ROOT.TFile.Open('data/moriond17ElectronRecoSF.root')
electronGsfSF.Set2DHist(float2double(eleGsfFile.Get('EGamma_SF2D')))
fScales.cd()
electronGsfSF.Write()

muonIdSF = ROOT.ScaleFactor("muonIdSF", "Moriond '17 Muon Tight WP ID SF, x=abs(Eta), y=Pt, z=run number")
midFile1 = ROOT.TFile.Open('data/moriond17MuonID_BCDEF.root')
midFile2 = ROOT.TFile.Open('data/moriond17MuonID_GH.root')
muon_ptetaratio1 = midFile1.Get('MC_NUM_TightID_DEN_genTracks_PAR_pt_eta/abseta_pt_ratio')
muon_ptetaratio2 = midFile2.Get('MC_NUM_TightID_DEN_genTracks_PAR_pt_eta/abseta_pt_ratio')
muon_allratio = float2double(muon_ptetaratio1.Clone("muon_allratio"))
for xbin in range(muon_ptetaratio1.GetNbinsX()+2):
    for ybin in range(muon_ptetaratio1.GetNbinsY()+2):
        runBFmean, runBFerr = muon_ptetaratio1.GetBinContent(xbin, ybin), muon_ptetaratio1.GetBinError(xbin, ybin)
        runGHmean, runGHerr = muon_ptetaratio2.GetBinContent(xbin, ybin), muon_ptetaratio2.GetBinError(xbin, ybin)
        allmean = (20.5*runBFmean + 16.3*runGHmean) / 36.8
        allerr = (20.5*runBFerr + 16.3*runGHerr) / 36.8
        muon_allratio.SetBinContent(xbin, ybin, allmean)
        muon_allratio.SetBinError(xbin, ybin, allerr)
muonIdSF.Set2DHist(muon_allratio)
fScales.cd()
muonIdSF.Write()

muonIsoSF = ROOT.ScaleFactor("muonIsoSF", "Moriond '17 Muon Tight Iso (0.15) WP ID SF, x=abs(Eta), y=Pt, z=run number")
misoFile1 = ROOT.TFile.Open('data/moriond17MuonIso_BCDEF.root')
misoFile2 = ROOT.TFile.Open('data/moriond17MuonIso_GH.root')
muIso_ptetaratio1 = misoFile1.Get('TightISO_TightID_pt_eta/abseta_pt_ratio')
muIso_ptetaratio2 = misoFile2.Get('TightISO_TightID_pt_eta/abseta_pt_ratio')
muIso_allratio = float2double(muon_ptetaratio1.Clone("muIso_allratio"))
for xbin in range(muIso_ptetaratio1.GetNbinsX()+2):
    for ybin in range(muIso_ptetaratio1.GetNbinsY()+2):
        runBFmean, runBFerr = muIso_ptetaratio1.GetBinContent(xbin, ybin), muIso_ptetaratio1.GetBinError(xbin, ybin)
        runGHmean, runGHerr = muIso_ptetaratio2.GetBinContent(xbin, ybin), muIso_ptetaratio2.GetBinError(xbin, ybin)
        allmean = (20.5*runBFmean + 16.3*runGHmean) / 36.8
        allerr = (20.5*runBFerr + 16.3*runGHerr) / 36.8
        muIso_allratio.SetBinContent(xbin, ybin, allmean)
        muIso_allratio.SetBinError(xbin, ybin, allerr)
muonIsoSF.Set2DHist(muIso_allratio)
fScales.cd()
muonIsoSF.Write()

# From https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation80XReReco#Supported_Algorithms_and_Operati
# Another option: https://indico.cern.ch/event/557163/contributions/2245312/attachments/1312814/1965165/btag_160720.pdf
btagData = ROOT.BTagCalibration("csvv2", "data/CSVv2_Moriond17_B_H.csv")
btagWrapped = ROOT.BTagData("btagData", "CSVv2 Moriond17 scale factors csv file")
btagWrapped.data = btagData
btagWrapped.Write()

# run makeBtagEfficiency.py
btagfile = ROOT.TFile("btagEfficiency.root")
btagEff = btagfile.Get("btagEfficiency")
fScales.cd()
btagEff.Write()

dibosonCorrector = ROOT.DibosonCorrections("dibosonCorrector", "WZ and ZZ corrections")
dibosonCorrector.loadEwkTables("../data/WZ_EwkCorrections.dat", "../data/ZZ_EwkCorrections.dat")
dibosonCorrector.Write()
