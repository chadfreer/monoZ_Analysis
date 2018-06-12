# Usage examples:
# . submit.sh twoLepton-tranche4.txt | grep 'DYJets' | . /dev/stdin
# . submit.sh twoLepton-data.txt | . /dev/stdin

#. submit.sh chad.txt | . /dev/stdin
grep -v -e '^#' -e '^ *$' $1 | while read dataset
do
  # uncomment to record nevents
  # grep -q "${dataset}" nevents.txt && continue
  # echo $dataset $(das_client.py --format=json --query="dataset dataset=${dataset}" | grep -o "nevents[^,]*," -m 1)

  echo crab submit -c crab.py Data.inputDataset=$dataset
done
