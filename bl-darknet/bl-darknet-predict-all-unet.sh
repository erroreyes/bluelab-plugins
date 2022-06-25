#!/bin/sh

# take all the backup weights, and compute one result image for each

# sort the files numerically (e.g "2" < "10")
#FILES=$(ls backup/rebalance_*.weights | sort -t _ -k 2 -g)
FILES=$(find backup -name 'bl-unet_*.weights' | sort -t _ -k 2 -g)

for BACKUP_FILE in $FILES
do

    echo "Processing file: $BACKUP_FILE"
    
    # ./dk-build/bl-darknet rebalance test cfg/sources.cfg cfg/rebalance.cfg $BACKUP_FILE -score-file dbg-test-all/test-pred-all.txt
    ./dk-build/bl-darknet rebalance test cfg/sources.cfg cfg/bl-unet.cfg $BACKUP_FILE -score-file training/test-pred-all.txt

    # extract the backup num
    read BACKUP_NUM <<<${BACKUP_FILE//[^0-9]/ }
    cp training/test-pred.png dbg-test-all/test-pred-$BACKUP_NUM.png

    I=$((I+1))
done

