from AnalysisResult import AnalysisResult
from Dataset import Dataset
from PlotGroup import PlotGroup
from SelectorResult import SelectorResult
from splitCanvas import splitCanvas
from stackUp import stackUp
import ROOT


def drawTextFrame(text, right=None):
    l = ROOT.TLatex()
    ROOT.SetOwnership(l, False)
    l.SetNDC()
    l.SetTextSize(0.5*ROOT.gPad.GetTopMargin())
    l.SetTextFont(42)
    l.SetY(1-0.9*ROOT.gPad.GetTopMargin())
    if right:
        l.SetTextAlign(31)
        l.SetX(1-ROOT.gPad.GetRightMargin())
    else:
        l.SetTextAlign(11)
        l.SetX(ROOT.gPad.GetLeftMargin())
    l.SetTitle(text)
    l.Draw()


def drawCMSstuff(lumi, grayText=''):
    cat = ROOT.gPad.GetName().split('_')[0]
    if cat in ['ee','mm','em','ll','lll','llll']:
        cat = cat.replace('m', r'#mu')
        channel = ' %s ch.,' % cat
    else:
        channel = ''
    drawTextFrame("#scale[1.2]{#font[62]{CMS}} #it{Internal}")
    drawTextFrame("#color[14]{%s} %s %.1ffb^{-1} (13TeV)" % (grayText, channel, lumi/1000.), right=True)


def addOverflowBinToLastBin(hist):
    nB = hist.GetNbinsX()
    lb, le = hist.GetBinContent(nB), hist.GetBinError(nB)
    ob, oe = hist.GetBinContent(nB + 1), hist.GetBinError(nB + 1)
    lb = lb + ob
    le = (le*le + oe*oe)**(0.5)
    hist.SetBinContent(nB, lb)
    hist.SetBinError(nB, le)
    # Zero-out overflow
    hist.SetBinContent(nB + 1, 0.)
    hist.SetBinError(nB + 1, 0.)


def getAxisPrimitive(canvas):
    # Easier way to get axis, since first primitive drawn ends up defining the axis
    for primitive in canvas.GetListOfPrimitives():
        if hasattr(primitive, 'GetYaxis'):
            return primitive
    return None

def buildCallback(args):
    import array
    callbackChain = []
    if args.projX:
        def projectionLambda(hist):
            newHist = hist.ProjectionX("ChangeME", int(args.projX[0]), int(args.projX[1]))
            newHist.GetYaxis().SetTitle(hist.GetZaxis().GetTitle())
            return newHist
        callbackChain.append(projectionLambda)
    if args.rebin:
        # Turn this into a callback function operating on a TH1
        newBinning = map(float, args.rebin.split(','))
        if not all(newBinning[i] < newBinning[i+1] for i in range(len(newBinning)-1)):
            raise Exception("Invalid binning specified: bins not in order! Check for typos : >")
        newBinning = array.array('d', newBinning)
        def rebinLambda(hist):
            newHist = hist.Rebin(len(newBinning) - 1, "YouBetterChangeMe", newBinning)
            # TODO: Get smart about this and bin-width normalize? (for combine, we must preserve poisson so no)
            # TODO: flag for add overflow?
            addOverflowBinToLastBin(newHist)
            newHist.GetYaxis().SetTitle("Counts / Bin")
            return newHist
        callbackChain.append(rebinLambda)
    if len(callbackChain) > 0:
        callback = lambda hist: reduce(lambda f, g: g(f), callbackChain, hist)
    else:
        callback = None
    return callback
