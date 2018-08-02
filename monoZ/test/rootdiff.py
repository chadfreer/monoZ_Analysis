#!/usr/bin/env python
import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
import argparse
import os

parser = argparse.ArgumentParser(description='Diffs two root files using histograms')
parser.add_argument('--dir', '-d', help='Output directory', default='diff')
parser.add_argument('left', help='Left')
parser.add_argument('right', help='Right')
args = parser.parse_args()

browseHook = r'''
#include "TObject.h"
#include "TPython.h"
#include "Python.h"

class PyTBrowserImp : public TBrowserImp {
  public:
    PyTBrowserImp(PyObject* self) : fSelf(self) {}
    virtual ~PyTBrowserImp() {}
    virtual void Add(TObject* o, const char* name, int check) {
      auto pyObj = TPython::ObjectProxy_FromVoidPtr((void *) o, o->ClassName());
      PyObject* r = PyObject_CallMethod(fSelf, const_cast<char*>("Add"), const_cast<char*>("(Osi)"), pyObj, name, check);
      if ( r == nullptr ) PyErr_Print();
      Py_XDECREF(r);
    };
  private:
    PyObject* fSelf;
};
'''
ROOT.gInterpreter.Declare(browseHook)

class Leaf:
    def __init__(self, parent, name, value):
        self.parent = parent
        self.name = name
        self.value = value
        self.children = []
        if parent:
            parent.addChild(self)

    def __repr__(self):
        return '<Leaf instance ("%s", %s)>' % (self.name, repr(self.value))

    def __str__(self):
        return self.name

    def ls(self, indent=0):
        print '  '*indent, self.name, ':', repr(self.value)
        for child in self.children:
            child.ls(indent+1)

    def addChild(self, child):
        self.children.append(child)

    def getChild(self, childName):
        for child in self.children:
            if child.name == childName:
                return child
        return None


class BlackHoleBrowser(ROOT.PyTBrowserImp):
    def __init__(self):
        ROOT.PyTBrowserImp.__init__(self, self)

    def Add(self, obj, nameCycle=None, check=-1):
        pass


class FileWalker(ROOT.PyTBrowserImp):
    # Safeguard against too much recursion, in case of infinite loop
    __maxDepth = 10
    # Dictionary methods that return the same type tend to cause loops
    # So we blacklist all browsable methods except these below
    __methodWhitelist = [
            'r()',
            'pt()',
            'eta()',
            'phi()',
            'mass2()', # mass() can throw, best ignore it
        ]

    def __init__(self):
        ROOT.PyTBrowserImp.__init__(self, self)
        self._objTree = []
        self._dirStack = [None]
        self._browser = None

    def setBrowser(self, browser):
        self._browser = browser

    def blackListed(self, obj):
        if len(self._dirStack) >= FileWalker.__maxDepth:
            return True
        if type(obj) is ROOT.TString:
            return True
        if obj.ClassName() == 'TMethodBrowsable':
            if obj.GetName() in FileWalker.__methodWhitelist:
                return False
            return True
        return False

    def getRoots(self):
        return filter(lambda leaf: leaf.parent is None, self._objTree)

    def Add(self, obj, nameCycle=None, check=-1, returnLeaf=False):
        if not self._browser:
            raise Exception("Tried to browse without setting up FileWalker")
        if not obj:
            raise Exception("Passed object that is nullptr during walk")
        if self.blackListed(obj):
            return
        name = obj.GetName()
        parent = self._dirStack[-1]
        if parent and name in parent.children:
            print "NameCycle", nameCycle, "already in directory. Will only keep first of name", name
            return
        leaf = Leaf(parent, name, obj)
        self._objTree.append(leaf)
        if obj.IsFolder():
            self._dirStack.append(leaf)
            obj.Browse(self._browser)
            self._dirStack.pop()
        if returnLeaf:
            return leaf


walker = FileWalker()
browser = ROOT.TBrowser("FileWalker", "", walker)
walker.setBrowser(browser)
blackhole = ROOT.TBrowser("Blackhole", "", BlackHoleBrowser())

leftFile = ROOT.TFile.Open(args.left)
rightFile = ROOT.TFile.Open(args.right)

leftLeaf = walker.Add(leftFile, returnLeaf=True)
rightLeaf = walker.Add(rightFile, returnLeaf=True)

leftLabel = ROOT.TPaveText(.1, .9, .5, .98, "NDC")
leftLabel.AddText(args.left)
rightLabel = ROOT.TPaveText(.1, .9, .5, .98, "NDC")
rightLabel.AddText(args.right)

ROOT.gROOT.SetStyle("Plain")
ROOT.gStyle.SetOptTitle(False)

def canDrawSame(a, b):
    if a.IsA().InheritsFrom(ROOT.TH1.Class()):
        return True
    return False

def sanitize(name):
    name = name.replace('/','_')
    if name[0] in ['.', '_']:
        name = name[1:]
    return name

def leafDiff(outputDir, left, right, name=None):
    if (len(left.children) if left else 0) + (len(right.children) if right else 0) > 0:
        # At least one is a directory, make new folder and recurse
        leafDir = os.path.join(outputDir, name) if name else outputDir
        print "mkdir", leafDir
        os.mkdir(leafDir)
        visitedChildNames = []
        if left:
            for child in left.children:
                visitedChildNames.append(child.name)
                leafDiff(leafDir, child, right.getChild(child.name) if right else None, child.name)
        if right:
            for child in right.children:
                if child.name not in visitedChildNames:
                    leafDiff(leafDir, None, child, child.name)
    # elif left and right and canDrawSame(left.value, right.value):
        # TODO: draw using same, make legend, etc.
    elif left or right:
        canvas = ROOT.TCanvas(name+"__canvas", name, 1200, 600)
        canvas.Divide(2,1)
        pad1 = canvas.cd(1)
        if left:
            left.value.Browse(blackhole)
            leftLabel.Draw()
        pad2 = canvas.cd(2)
        if right:
            right.value.Browse(blackhole)
            rightLabel.Draw()
        canvas.Print(os.path.join(outputDir, sanitize(name)+".png"))


leafDiff(args.dir, leftLeaf, rightLeaf)
