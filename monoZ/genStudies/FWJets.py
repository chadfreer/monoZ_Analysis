# import ROOT in batch mode
import sys
oldargv = sys.argv[:]
sys.argv = [ '-b-' ]
import ROOT
ROOT.gROOT.SetBatch(True)
sys.argv = oldargv

# load FWLite C++ libraries
ROOT.gSystem.Load("libFWCoreFWLite.so");
ROOT.gSystem.Load("libDataFormatsFWLite.so");
ROOT.AutoLibraryLoader.enable()


# load FWlite python libraries
from DataFormats.FWLite import Handle, Events

muons, muonLabel = Handle("std::vector<pat::Muon>"), "slimmedMuons"
electrons, electronLabel = Handle("std::vector<pat::Electron>"), "slimmedElectrons"
photons, photonLabel = Handle("std::vector<pat::Photon>"), "slimmedPhotons"
taus, tauLabel = Handle("std::vector<pat::Tau>"), "slimmedTaus"
jets, jetLabel = Handle("std::vector<pat::Jet>"), "slimmedJets"
fatjets, fatjetLabel = Handle("std::vector<pat::Jet>"), "slimmedJetsAK8"
mets, metLabel = Handle("std::vector<pat::MET>"), "slimmedMETs"
vertices, vertexLabel = Handle("std::vector<reco::Vertex>"), "offlineSlimmedPrimaryVertices"
verticesScore = Handle("edm::ValueMap<float>")

events = Events('root://cms-xrd-global.cern.ch//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_NLO_Vector_Mx-150_Mv-500_gDM1_gQ0p25_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/60000/1E74A3C2-6FD2-E611-BB27-24BE05CEEB31.root')

histogram = ROOT.TH1F("histogram", "histogram", 100, 0, 100)

for iev,event in enumerate(events):
    if iev >= 2000: break
    event.getByLabel(muonLabel, muons)
    event.getByLabel(electronLabel, electrons)
    event.getByLabel(photonLabel, photons)
    event.getByLabel(tauLabel, taus)
    event.getByLabel(jetLabel, jets)
    event.getByLabel(fatjetLabel, fatjets)
    event.getByLabel(metLabel, mets)
    event.getByLabel(vertexLabel, vertices)
    event.getByLabel(vertexLabel, verticesScore)

    print "\nEvent %d: run %6d, lumi %4d, event %12d" % (iev,event.eventAuxiliary().run(), event.eventAuxiliary().luminosityBlock(),event.eventAuxiliary().event())                  # Vertices
    if len(vertices.product()) == 0 or vertices.product()[0].ndof() < 4:
        print "Event has no good primary vertex."
        continue
    else:
        PV = vertices.product()[0]
        print "PV at x,y,z = %+5.3f, %+5.3f, %+6.3f, ndof: %.1f, score: (pt2 of clustered objects) %.1f" % (PV.x(), PV.y(), PV.z(), PV.ndof(),verticesScore.product().get(0))

    # Jets (standard AK4)
    count=0
    for i,j in enumerate(jets.product()):
        if j.pt() < 20: continue
        print "jet %3d: pt %5.1f (raw pt %5.1f, matched-calojet pt %5.1f), eta %+4.2f, btag run1(CSV) %.3f, run2(pfCSVIVFV2) %.3f, pileup mva disc %+.2f" % (
            i, j.pt(), j.pt()*j.jecFactor('Uncorrected'), j.userFloat("caloJetMap:pt"), j.eta(), max(0,j.bDiscriminator("combinedSecondaryVertexBJetTags")), max(0,j.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")), j.userFloat("pileupJetId:fullDiscriminant"))
#        if i == 0: # for the first jet, let's print the leading constituents
        constituents = [ j.daughter(i2) for i2 in xrange(j.numberOfDaughters()) ]
        constituents.sort(key = lambda c:c.pt(), reverse=True)
        for i2, cand in enumerate(constituents):
            count=count+1
#a            if i2 > 4: 
             #print "         ....."
             #break
            print "         constituent %3d: pt %6.2f, dz(pv) %+.3f, pdgId %+3d" % (i2,cand.pt(),cand.dz(PV.position()),cand.pdgId()) 
    histogram.Fill(count)

c = ROOT.TCanvas ( "c" , "c" , 800, 800 )
c.cd()
histogram.Draw()
c.SaveAs("PtFW.png")
    # Fat AK8 Jets
#    for i,j in enumerate(fatjets.product()):
#        print "jetAK8 %3d: pt %5.1f (raw pt %5.1f), eta %+4.2f, mass %5.1f ungroomed, %5.1f softdrop, %5.1f pruned, %5.1f trimmed, %5.1f filtered. CMS TopTagger %.1f" % (
#            i, j.pt(), j.pt()*j.jecFactor('Uncorrected'), j.eta(), j.mass(), j.userFloat('ak8PFJetsCHSSoftDropMass'), j.userFloat('ak8PFJetsCHSPrunedMass'), j.userFloat('ak8PFJetsCHSTrimmedMass'), j.userFloat('ak8PFJetsCHSFilteredMass'), j.userFloat("cmsTopTagPFJetsCHSMassAK8"))

