#!/bin/sh
#
# ASIC test BAT program 
# 10/21/09 hiro
# 11/21/09 
#
# user need to establish com4 and com8 connection between MISC and PC
# before running this program.
# use com_init.sh
# 
# use also needs to make sure connextion between Oscilloscope and PC
# is established. type tek1.sh to check it is working.
#
##########################################################################
# Initial setting
export DMVS="/home/tester/work/MISC/probe_ana2/dmvs.exe"
export SC_INI="/home/tester/work/tek/ver2/Debug/dpo4104.exe"
export SC_SET="/home/tester/work/tek/ver2_set/Debug/dpo4104.exe"
export SC_RUN="/home/tester/work/tek/ver2_noinit/Debug/dpo4104.exe"

export OUTFL="p04/p04_2"
export TYP_GDC="_globaldc"
export TYP_ADC="_testadc"
export TYP_PDC="_pixeldc"
export TYP_DIS="_ndisctest"
export TYP_OSC="_scope"
export TYP_SC="_sc"
export TYP_PR="_pr"
export TYP_SH="_sh"
export TYP_CP="_cp"
export SUF_LOG="_log.txt"
export SUF_ANA="_ana.txt"
export SUF_OSC="_osc.txt"
export SUF_BIN="_pls.BIN"


##########################################################################
# Upload md softwares
echo  `date` UPLOAD MD   | tee    $OUTFL$SUF_LOG
./mup_all.sh             | tee -a $OUTFL$SUF_LOG

##########################################################################
# DC tests
$SC_SET < tek_normal.ini

# GLOBALDC
echo  `date` GLOBALDC    | tee -a $OUTFL$SUF_LOG
echo "GLOBALDC"          | ./mupld_c.exe -p 5 &
./mterm.exe -c /dev/com4 | tee $OUTFL$TYP_GDC$SUF_LOG

$DMVS -usr $USER < $OUTFL$TYP_GDC$SUF_LOG | tee $OUTFL$TYP_GDC$SUF_ANA

# TESTADC
echo  `date` TESTADC     | tee -a $OUTFL$SUF_LOG
echo "TESTADC"           | ./mupld_c.exe -p 5 &
./mterm.exe -c /dev/com4 | tee $OUTFL$TYP_ADC$SUF_LOG

# PIXELDC
echo  `date` PIXELDC     | tee -a $OUTFL$SUF_LOG
echo "PIXELDC"           | ./mupld_c.exe -p 5 &
./mterm.exe -c /dev/com4 | tee $OUTFL$TYP_PDC$SUF_LOG

# NDISCTEST
echo  `date` NDISCTEST   | tee -a $OUTFL$SUF_LOG
echo "NDISCTEST"         | ./mupld_c.exe -p 5 &
./mterm.exe -c /dev/com4 | tee $OUTFL$TYP_DIS$SUF_LOG


##########################################################################
# SCOPE tests
echo `date` SCOPE_TEST   | tee -a $OUTFL$SUF_LOG
( ./mterm.exe | tee -a $OUTFL$TYP_OSC$SUF_LOG ) &
sleep 3

## if you start from scope test run following otherwise comment out
##./mupld_c.exe -I misc_scini.txt
##sleep 3

# Preamp mode
./mupld_c.exe -I misc_scpre.txt
$SC_SET < tek_preamp.ini  > $OUTFL$TYP_PR$SUF_OSC
sleep 5
./xysel.sh "1 TRY5 " 400 >> $OUTFL$TYP_PR$SUF_OSC

# Shaper mode
./mupld_c.exe -I misc_scsha.txt
$SC_SET < tek_shaper.ini  > $OUTFL$TYP_SH$SUF_OSC
sleep 5
./xysel.sh "1 TRY5 " 400 >> $OUTFL$TYP_SH$SUF_OSC

# CP mode
./mupld_c.exe -I misc_sccpm.txt
$SC_SET < tek_cpmode.ini  > $OUTFL$TYP_CP$SUF_OSC
sleep 5
./xysel.sh "1 TRY7"  400 >> $OUTFL$TYP_CP$SUF_OSC


##########################################################################
# PULSE tests
sleep 3
./mupld_c.exe -I misc_pulse.txt

echo
echo `date` CALON       | tee -a $OUTFL$SUF_LOG
echo save bin data to $OUTFL$SUF_BIN  | tee -a $OUTFL$SUF_LOG

./msave.exe -s $OUTFL$SUF_BIN -e 1200 &
echo "CAL BUILD CALON " |./mupld_c.exe
sleep 3
echo
echo ":q " |./mupld_c.exe
echo


exit
