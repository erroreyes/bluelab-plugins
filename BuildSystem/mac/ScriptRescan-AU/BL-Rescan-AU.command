# Rescan all the BlueLab AU plugins
# while disabling temporarily "SecAssessment system policy security"
#
# Double-click on the BL-Rescan-AU.command to rescan all the BlueLab plugins
# in the AU format
#
# A Terminal window will open, then type your administrator password
#

#! /bin/sh

echo "Starting BlueLab plugins rescan for AU format..."

SPCTL_STATUS_STR=$(spctl --status)
SPCTL_STATUS=0

if [[ $SPCTL_STATUS_STR == *"enabled"* ]]; then
  SPCTL_STATUS=1
fi

if [ $SPCTL_STATUS ]; then
   sudo spctl --master-disable
fi

auval -vt aufx BlLa
auval -vt aumf BlLa
auval -vt aumu BlLa

if [ $SPCTL_STATUS ]; then
   sudo spctl --master-enable
fi
   
echo "Rescan finished!"
