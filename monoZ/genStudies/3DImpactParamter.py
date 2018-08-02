# import ROOT in batch mode
import math
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

#Useful functions
dphi = ROOT.Math.VectorUtil.DeltaPhi
deltaR = ROOT.Math.VectorUtil.DeltaR

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

def Histo (events):

   histogram = ROOT.TH1F("3DIP_{sig}", "3DIP_{sig}", 140, -2, 5)

   for iev,event in enumerate(events):#event loop
       if iev >= 2000: break         #limit event loop
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
       for i,j in enumerate(jets.product()):
           count = 0
           IPSigTot=0
           Max_IP = -10
           IPSig=-1000
           #if j.pt() < 20 and abs(j.eta()) > 2.4 and j.neutralHadronEnergyFraction() > 0.90 and j.neutralEmEnergyFraction() > 0.90 and j.chargedHadronEnergyFraction() == 0 and j.muonEnergyFraction() > 0.8 and j.chargedEmEnergyFraction() > 0.90 and (j.chargedMultiplicity()+j.neutralMultiplicity()) < 2 and j.chargedMultiplicity() < 1: continue
           if j.pt() < 20: continue
           if abs(j.eta()) > 2.4: continue
           if j.neutralHadronEnergyFraction() > 0.90: continue
           if j.neutralEmEnergyFraction() > 0.90: continue
           if j.chargedHadronEnergyFraction() == 0: continue
           if j.muonEnergyFraction() > 0.8: continue
           if j.chargedEmEnergyFraction() > 0.90: continue
           if (j.chargedMultiplicity()+j.neutralMultiplicity()) < 2: continue
           if j.chargedMultiplicity() < 1: continue
           #print "jet %3d: pt %5.1f (raw pt %5.1f, matched-calojet pt %5.1f), eta %+4.2f, btag run1(CSV) %.3f, run2(pfCSVIVFV2) %.3f, pileup mva disc %+.2f" % (i, j.pt(), j.pt()*j.jecFactor('Uncorrected'), j.userFloat("caloJetMap:pt"), j.eta(), max(0,j.bDiscriminator("combinedSecondaryVertexBJetTags")), max(0,j.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")), j.userFloat("pileupJetId:fullDiscriminant"))
   #        if i == 0: # for the first jet, let's print the leading constituents
           print "the neutral hadron energy fraction is %5f:,j eta %3f"% (j.neutralHadronEnergyFraction(),j.eta())
           constituents = [ j.daughter(i2) for i2 in xrange(j.numberOfDaughters()) ]
           constituents.sort(key = lambda c:c.pt(), reverse=True)
           Alpha_num = 0
           Alpha_den = 0
           for i2, cand in enumerate(constituents):
               count=count+1
               if cand.pt() > 1 :
#                  if abs(cand.dxy(PV.position())) <0.4:
#                  if (cand.dPhi(PV.position()) * cand.dPhi(PV.position()) + cand.deta(PV.position()) * cand.deta(PV.position()) )**(.5) <0.4:
                  dr=deltaR(cand.p4(),j.p4())
#                  if deltaR(cand.p4(),j.p4())<0.2:
                  if math.hypot(cand.dxy(PV.position()),cand.dz(PV.position()))<.5: 
                     Alpha_num = Alpha_num + cand.pt()
                     Alpha_den = Alpha_den + cand.pt()
                  distance = math.hypot(cand.dxy(PV.position()),cand.dz(PV.position()))
#                  histogram.Fill(distance)
                  #histogram.Fill(cand.dxy(PV.position()))
                  #histogram.Fill(count)
#a               if i2 > 4: 
                #print "         ....."
                #break
                  IPSigNum = math.hypot(cand.dxy(PV.position()),cand.dz(PV.position()))
                  IPSigDen = math.hypot(cand.dxyError(),cand.dzError())
                  if IPSigDen > 0 and IPSigNum > 0:
                     if IPSigNum/IPSigDen > 0:
                        IPSig = math.log10(IPSigNum / IPSigDen)
                        IPSigTot=IPSigTot+IPSig
                        histogram.Fill(IPSig)
                  if IPSig>Max_IP:
                     Max_IP=IPSig
                  print "         constituent %3d: pt %6.2f, dz(pv) %+.3f, pdgId %+3d,dR %+.3f" % (i2,cand.pt(),cand.dz(PV.position()),cand.pdgId(),dr) 
           IPSigAve=IPSigTot/count
           #histogram.Fill(IPSigAve)       #histogram.Fill(IPSig)
   return histogram;

#simplified dark matter signal
#events = Events('root://cms-xrd-global.cern.ch//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_NLO_Vector_Mx-150_Mv-500_gDM1_gQ0p25_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/60000/1E74A3C2-6FD2-E611-BB27-24BE05CEEB31.root')

#ctau=10
#events = Events('root://cms-xrd-global.cern.ch//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_NLO_Vector_Mx2-50_Mv-500_gDM1_gQ0p25_TuneCUETP8M1_Mx1-1_ctau-10_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/40000/28B81DD6-0ECA-E711-8BD2-C45444922BFE.root')

#ctau=1 Heavy mass
#events = Events('root://cms-xrd-global.cern.ch//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_NLO_Vector_Mx2-150_Mv-500_gDM1_gQ0p25_TuneCUETP8M1_Mx1-1_ctau-1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/30000/08FAE9F3-C0BB-E711-A700-0025901D4B0E.root')

#ctau=1 light mass
#events = Events('root://cms-xrd-global.cern.ch//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_NLO_Vector_Mx2-1_Mv-500_gDM1_gQ0p25_TuneCUETP8M1_Mx1-0p1_ctau-1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/30000/1477451B-7AB5-E711-82AB-E0071B7A45D0.root')

# TT background
#events = Events('root://cms-xrd-global.cern.ch//store/mc/RunIISummer16MiniAODv2/TTTo2L2Nu_TuneCUETP8M2_ttHtranche3_13TeV-powheg-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/0030B9D6-72C1-E611-AE49-02163E00E602.root')

#DY Inclusive
events = Events('root://cms-xrd-global.cern.ch//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext2-v1/100000/00099D43-77ED-E611-8889-5065F381E1A1.root')

histogram = Histo(events)

c = ROOT.TCanvas ( "c" , "c" , 800, 800 )
c.cd()
c.SetLogy()
norm = histogram.GetEntries()
histogram.Scale(1/norm)
histogram.SetLineWidth(3)
histogram.GetXaxis().SetTitle("log_{10}(IP_{sig}^{3D})")
histogram.Draw()

# TT background
events = Events('root://cms-xrd-global.cern.ch//store/mc/RunIISummer16MiniAODv2/TTTo2L2Nu_TuneCUETP8M2_ttHtranche3_13TeV-powheg-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/0030B9D6-72C1-E611-AE49-02163E00E602.root')
histogram2 = Histo(events)

norm = histogram2.GetEntries();
histogram2.Scale(1/norm);
histogram2.SetLineColor(1)
histogram2.SetLineWidth(3)
histogram2.Draw("same")

#ctau=1 Heavy mass
events = Events('root://cms-xrd-global.cern.ch//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_NLO_Vector_Mx2-150_Mv-500_gDM1_gQ0p25_TuneCUETP8M1_Mx1-1_ctau-1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/30000/08FAE9F3-C0BB-E711-A700-0025901D4B0E.root')
histogram3 = Histo(events)

norm = histogram3.GetEntries();
histogram3.Scale(1/norm);
histogram3.SetLineColor(2)
histogram3.SetLineWidth(3)
histogram3.Draw("same")

#ctau=10
events = Events('root://cms-xrd-global.cern.ch//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_NLO_Vector_Mx2-50_Mv-500_gDM1_gQ0p25_TuneCUETP8M1_Mx1-1_ctau-10_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/40000/28B81DD6-0ECA-E711-8BD2-C45444922BFE.root')
histogram4 = Histo(events)

norm = histogram4.GetEntries();
histogram4.Scale(1/norm);
histogram4.SetLineColor(3)
histogram4.SetLineWidth(3)
histogram4.Draw("same")


#TLegend* legend = new TLegend(0.1,0.7,0.48,0.9);
legend = ROOT.TLegend(0.1,0.8,0.3,0.9)
legend.SetHeader("Legend")
legend.AddEntry(histogram,"Drell-Yan","f")
legend.AddEntry(histogram2,"top","f")
legend.AddEntry(histogram3,"ctau=1","f")
legend.AddEntry(histogram4,"ctau=10","f")
legend.Draw("same")

c.SaveAs("2DIP.root")

