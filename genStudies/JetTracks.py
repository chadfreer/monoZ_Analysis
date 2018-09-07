from DataFormats.FWLite import Handle, Events
import ROOT
#events = Events('root://cms-xrd-global.cern.ch//store/data/Run2017B/Charmonium/MINIAOD/PromptReco-v1/000/297/046/00000/88DF6C6A-4556-E711-A5C0-02163E01A630.root')
events = Events('root://cms-xrd-global.cern.ch//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_NLO_Vector_Mx-150_Mv-500_gDM1_gQ0p25_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/60000/1E74A3C2-6FD2-E611-BB27-24BE05CEEB31.root')

#tracks = Handle("std::vector")
#tracks = Handle('vector<reco::GenParticle>')
#tracks = Handle('vector<pat::Jet>')
tracks = Handle('vector<reco::GenJet>')
#tracks = Handle('BXVector<l1t::Jet>')
histogram = ROOT.TH1F("histogram", "histogram", 100, 0, 100)

i = 0
for event in events:
    print "Event", i 
   event.getByLabel("packedPFCandidates", tracks)
#    event.getByLabel("slimmedJets", tracks)
    j = 0
    for track in tracks.product():
        print "    Track", j, track.charge() / track.pt(), track.phi(), track.eta(), track.mass(), track.fromPV()
        histogram.Fill(track.pt())
        j += 1
    i += 1
    if i >= 5: break


c = ROOT.TCanvas ( "c" , "c" , 800, 800 )
c.cd()
histogram.Draw()
c.SaveAs("PtFW.png")
