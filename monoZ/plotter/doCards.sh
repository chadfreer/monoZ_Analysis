emStack="--stack Nonresonant,ZZ2l2nu,WZ3lnu,Other"
lllStack="--stack WZ3lnu,Other3l,NonPromptDY"
llllStack="--stack ZZ4l,Other4l"
sigStack="--stack Nonresonant,ZZ2l2nu,WZ3lnu,Other,DrellYanBinned"
lumiResult="--lumi 35867 MonoZSelector-monoZ_11Jan2017-gd0b8a16.root"
#lumiResult="--lumi 35867 MonoZSelector-monoZ_11Jan2017-g48e23d3.root"
rm -rf shapePlots/*
./makeCards $lumiResult --rebin 100,120,150,180,200,250,300,350,400,500,600 $emStack -c em --outputFile em_workspace.root
./makeCards $lumiResult --rebin 100,120,150,180,200,250,300,350,400,500,600 $lllStack -c lll --outputFile lll_workspace.root
./makeCards $lumiResult --rebin 100,120,150,180,200,250,300,350,400,500,600 $llllStack -c llll --outputFile llll_workspace.root
if ./makeCards --asimov $lumiResult --rebin 100,120,150,180,200,250,300,350,400,500,600  $sigStack --signal $1 -c ee -c mm --outputFile ll_workspace.root; then
  cp shapePlots/* ~/www/monoZ/shapePlots/
  combineCards.py ee=card_ee.dat mm=card_mm.dat em=card_em.dat lll=card_lll.dat llll=card_llll.dat > card_combined.dat
  text2workspace.py --channel-masks card_combined.dat
  combine -M Asymptotic -t -1 card_combined.dat.root | tee result/${1}.txt
else
  echo "Skipping $1"
fi
