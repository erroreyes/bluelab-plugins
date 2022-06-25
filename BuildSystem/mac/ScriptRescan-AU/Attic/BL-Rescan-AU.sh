#! /bin/sh

echo "Starting BlueLab plugins rescan for AU format..."

STATUS_STR=$(spctl --status)
STATUS=0

if [[ $STATUS_STR == *"enabled"* ]]; then
  STATUS=1
fi

if [ $STATUS ]; then
   sudo spctl --master-disable
fi

auval -vt aufx BlLa
auval -vt aumf BlLa
auval -vt aumu BlLa

if [ $STATUS ]; then
   sudo spctl --master-enable
fi
   
echo "Rescan finished!"
