#!/bin/sh

# take a chosen backup weight, and compute one result image

if [ "$#" -ne 1 ]; then
  echo "usage: $0 <backup_num>"
  exit 0
fi

BACKUP_NUM=$1

BACKUP_FILE=backup/bl-unet_$BACKUP_NUM.weights

echo "Processing file: $BACKUP_FILE"

./dk-build/bl-darknet rebalance test cfg/sources.cfg cfg/bl-unet.cfg $BACKUP_FILE -backup-num $BACKUP_NUM

cp training/test-pred.png training/one_test-pred-$BACKUP_NUM.png
