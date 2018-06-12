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

GenPart, genlabel = Handle("std::vector<reco::GenParticle>"), "prunedGenParticles"

#events = Events('root://cms-xrd-global.cern.ch//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_NLO_Vector_Mx2-1_Mv-500_gDM1_gQ0p25_TuneCUETP8M1_Mx1-0p1_ctau-1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/30000/880C598C-37B5-E711-843C-008CFAEEAD4C.root')

def Histo (events):

   #h = ROOT.TH1F('hrho', ';#rho [cm];Counts', 200, 0, 100)
   hmyrho = ROOT.TH1F('hrho2', ';decay distance [cm];Counts', 200, 0, 100)
   #print "wow I'm Here"

   for iev,event in enumerate(events):#event loop
       event.getByLabel(genlabel, GenPart)
       for i, p in enumerate(GenPart.product()):
           #print "my pdgId is : %3d " % (p.pdgId())
           if (abs(p.pdgId())!=2000012): continue
           #h.Fill(p.daughter(0).vertex().Rho())
           a = p.p()/p.mass()
           beta = a/math.sqrt(a*a+1)
           gamma = 1/math.sqrt(1-beta*beta)
           disp = ROOT.gRandom.Exp(1*gamma)
           #disp = p.p4().Rho() / p.p()
           theta = p.p4().theta()
           x = p.vertex().x() + disp*math.cos(theta)
           y = p.vertex().y() + disp*math.sin(theta)
           hmyrho.Fill(math.hypot(x, y))
   return hmyrho;        

#events = Events('root://cms-xrd-global.cern.ch//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_NLO_Vector_Mx2-1_Mv-500_gDM1_gQ0p25_TuneCUETP8M1_Mx1-0p1_ctau-1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/30000/880C598C-37B5-E711-843C-008CFAEEAD4C.root')

events = Events('root://cms-xrd-global.cern.ch//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-1000_Mv-1000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/CE49A2DE-0DB7-E611-9B9F-141877344D39.root')

c = ROOT.TCanvas ( "c" , "c" , 800, 800 )
c.cd() 
#h.Draw("")
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-1000_Mv-1000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-1000_Mv-10_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/C4B6943C-14B7-E611-AE8F-6CC2173D5F20.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-1000_Mv-10_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-1000_Mv-1995_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/60000/0889AC17-27B9-E611-AD12-0CC47A4D7644.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-1000_Mv-1995_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-1000_Mv-5000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/50000/22D843DD-76C6-E611-BD72-001E67792736.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-1000_Mv-5000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-10_Mv-100_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/92E61594-48B6-E611-98E9-0025901D16AC.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-10_Mv-100_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-10_Mv-10_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/104336DF-06B7-E611-B1D0-002481CFE864.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-10_Mv-10_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-10_Mv-20_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/AA52F979-13B7-E611-B220-0CC47A7E6A4C.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-10_Mv-20_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-10_Mv-5000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/110000/4C7B80A3-FCB6-E611-99AF-0025901D1668.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-10_Mv-5000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-150_Mv-10_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/60000/A239E74E-03B7-E611-8B35-008CFA1C6458.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-150_Mv-10_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-150_Mv-200_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/110000/A89893EB-0EB7-E611-A5C8-0026181D2917.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-150_Mv-200_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-150_Mv-295_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/110000/48DAB957-22B7-E611-BED5-6C3BE5B51238.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-150_Mv-295_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-150_Mv-5000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/8421E985-0FAD-E611-846B-001E674FB24D.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-150_Mv-5000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-150_Mv-500_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/562CB327-A1B4-E611-911B-008CFA1112B0.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-150_Mv-500_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-1_Mv-1000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/44516596-9DAC-E611-A2AA-78E7D1E4AF92.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-1_Mv-1000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-1_Mv-100_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/60000/3E005A27-0FB7-E611-8271-1CC1DE192766.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-1_Mv-100_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-1_Mv-10_1-gDMgQ_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/60000/EA523F4E-1DD0-E611-A80C-A0000420FE80.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-1_Mv-10_1-gDMgQ_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-1_Mv-2000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/C2D6A5CB-32B7-E611-A224-44A84225C3D0.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-1_Mv-2000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-1_Mv-200_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/D8457D0E-20B7-E611-8AAA-ECF4BBE1CF38.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-1_Mv-200_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-1_Mv-20_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/120000/3645F373-6BBA-E611-B030-0025905A608C.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-1_Mv-20_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-1_Mv-300_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/110000/006D10EA-53AC-E611-8614-001E67E5E889.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-1_Mv-300_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-1_Mv-5000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/34826533-02B2-E611-AFF5-002590E3A0FA.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-1_Mv-5000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-1_Mv-500_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/DC29ADDC-B6B7-E611-9C18-0025905A608E.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-1_Mv-500_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-1_Mv-50_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/BA9DCF8B-02B7-E611-9392-0CC47A57D066.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-1_Mv-50_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-500_Mv-10_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/110000/72AD5EB7-0CB7-E611-B6A9-FA163ECFB93B.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-500_Mv-10_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-500_Mv-2000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/60000/7285F405-0AB7-E611-B8F1-002590FD5E80.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-500_Mv-2000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-500_Mv-5000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/32FF4AB1-BFB1-E611-A0BE-0242AC130006.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-500_Mv-5000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-500_Mv-500_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/40911E1B-9ECD-E611-84A3-002590E7E01A.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-500_Mv-500_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-500_Mv-995_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/6EEF196E-2ABC-E611-9D13-0CC47A7C35B2.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-500_Mv-995_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-50_Mv-10_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/0E964099-69B2-E611-9554-001E67DDBFF7.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-50_Mv-10_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-50_Mv-200_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/C06A58CB-25B7-E611-BFFE-00266CFCC21C.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-50_Mv-200_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-50_Mv-300_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/6AC45E73-8DAC-E611-A25F-782BCB51D73A.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-50_Mv-300_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-50_Mv-5000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/110000/0045CD7C-33AA-E611-8CF2-002590207C28.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-50_Mv-5000_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-50_Mv-50_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/60000/5000A0BD-24AE-E611-81C2-0CC47AC08BD4.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-50_Mv-50_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

events = Events('root://cmsxrootd.fnal.gov//store/mc/RunIISummer16MiniAODv2/DarkMatter_MonoZToLL_V_Mx-50_Mv-95_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/88A3AFCF-19B7-E611-B30F-002590E39F2E.root')
histogram = Histo(events)
histogram.Draw("")
c.SaveAs("DarkMatter_MonoZToLL_V_Mx-50_Mv-95_gDMgQ-1_TuneCUETP8M1_13TeV-madgraph_ctau-10.png")

