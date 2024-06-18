#!/bin/bash
# Probestation ASIC test script.
# by hiro
#
#
# v1.1 11-24-09 use dialog
# v1.8 12-11-09 stable verion for DELL laptop
# v1.9 03-09-10 add auto file upload and e-mail notification. 
#               tune for YGG (desktop PC).
##########################################################################
# Initial setting
DMVS="/home/tester/work/MISC/probe_ana2/dmvs.exe"
SC_INI="/home/tester/work/tek/release/dpo4104.exe"
SC_SET="/home/tester/work/tek/release/dpo4104_set.exe"
SC_RUN="/home/tester/work/tek/release/dpo4104_noinit.exe"
NET_HD="/cygdrive/z/data/ASIC/"

TYP_GDC="_globaldc"
TYP_ADC="_testadc"
TYP_PDC="_pixeldc"
TYP_DIS="_ndisctest"
TYP_OSC="_scope"
TYP_SC="_sc"
TYP_PR="_pr"
TYP_SH="_sh"
TYP_CP="_cp"
SUF_LOG="_log.txt"
SUF_ANA="_ana.txt"
SUF_OSC="_osc.txt"
SUF_MIS="_mis.txt"
SUF_BIN="_pls.BIN"
V4VSEL="_4vsel.dat"

STIMTIME="1200"
#STIMTIME="1600"
#STIMTIME="1800"
SSCAN="y"
DATDIR="./data/"


# Full scan tests
UPMD="y"
GLDC="y"
TADC="y"
PIXD="y"
NDIS="y"
RUNM="y"
SPRE="y"
SSHA="y"
SCPM="y"
SPLS="y"
UPFL="y"
SDML="y"

fq="n"

##########################################################################
# Set directory
#[ "$#" != 3 ] && { \
#    echo "Usage: pstest1.sh OUTDIR ASIC_ID run_level"
#    exit
#}

# ----- set ASIC_ID ------------
dialog --title "Probestation ASIC test" --inputbox "Enter ASIC ID" 9 40 2> _1.txt
[ $? != 0 ] && exit
ASIC=$(cat _1.txt)

# ----- set Run Level ----------
dialog --title "Probestation ASIC test" --menu "Select $ASIC test type" 15 40 4 \
	1 "Power on test" 2 "CP 4VSEL" 3 "Full scan" 4 "Scope test" 2> _1.txt
[ $? != 0 ] && exit
RUN_LVL=$(cat _1.txt)
if [ $RUN_LVL == "3" ]; then
   dialog --title "Probestation ASIC test" --checklist "$ASIC full test step:" 20 45 21 \
       UPLOADMD  1  on \
       GLOBALDC  2  on \
       TESTADC   3  on \
       PIXELDC   4  on \
       NDISCTEST 5  on \
       RUN_MISC  6  on \
       SCOPE_PRE 7  on \
       SCOPE_SHA 8  on \
       SCOPE_CPM 9  on \
       STIM_PLS  10 on \
       UPLD_FIL  11 on \
       SENDMAIL  12 on \
       2> _1.txt

   grep "UPLOADMD" _1.txt > _2.txt
   [ $? != 0 ] && UPMD="n"
   grep "GLOBALDC" _1.txt > _2.txt
   [ $? != 0 ] && GLDC="n"
   grep "TESTADC" _1.txt > _2.txt
   [ $? != 0 ] && TADC="n"
   grep "PIXELDC" _1.txt > _2.txt
   [ $? != 0 ] && PIXD="n"
   grep "NDISCTEST" _1.txt > _2.txt
   [ $? != 0 ] && NDIS="n"
   grep "RUN_MISC" _1.txt > _2.txt
   [ $? != 0 ] && RUNM="n"
   grep "SCOPE_PRE" _1.txt > _2.txt
   [ $? != 0 ] && SPRE="n"
   grep "SCOPE_SHA" _1.txt > _2.txt
   [ $? != 0 ] && SSHA="n"
   grep "SCOPE_CPM" _1.txt > _2.txt
   [ $? != 0 ] && SCPM="n"
   grep "STIM_PLS" _1.txt > _2.txt
   [ $? != 0 ] && SPLS="n"
   grep "UPLD_FIL" _1.txt > _2.txt
   [ $? != 0 ] && UPFL="n"
   grep "SENDMAIL" _1.txt > _2.txt
   [ $? != 0 ] && SDML="n"

   echo $UPMD $GLDC $TADC $PIXD $NDIS $RUNM $SPRE $SSHA $SCPM $SPLS $UPFL $SDML
fi
if [ $RUN_LVL == "4" ]; then
   # individual scope out mode
   dialog --title "Probestation ASIC test" --menu "Select $ASIC scope test type: " 15 40 5 \
	1 "preamp (missed) " 2 "shaper (missed) " 3 "CP preamp (missed) " \
	4 "preamp (scan all) " 5 "shaper (scan all) " 6 "CP preamp (scan all) " 2> _1.txt
   [ $? != 0 ] && exit
   RUN_OSC=$(cat _1.txt)
   dialog --title "Probestation ASIC test" --yesno "Is MISC already booted? [y/n]" \
   15 40
   if [ $? != 0 ]; then
      MISC_ON="n"
   else
      MISC_ON="y"
   fi
fi

# ----- set Data directory -----
ddir=$(ls -lat $DATDIR | grep "drwx" | awk '{print $8}' | grep -v "\.")
#ddir=$(ls -la $DATDIR | grep "drwx" | awk '{print $9}' | grep -v "\.")
#echo $ddir
ndir=$(echo $ddir | awk '{print NF}')
adir="1 NewDirectory"
for (( i=1 ; i<=$ndir ; i++ ))
do
        j=`expr $i + 1`
	sdir=$(echo $ddir | awk '{print $'$i'}')
	adir=$(echo $adir $j $sdir)
done
#echo $adir
dialog --title "Probestation ASIC test" --menu "Select $ASIC data directory:" 15 40 10 \
	$adir 2> _1.txt
[ $? != 0 ] && exit
i=$(cat _1.txt)
if [ $i == "1" ]; then
   # new directory
   dialog --title "Probestation ASIC test" --inputbox "Enter $ASIC data directory name: " 9 40 \
  	 2> _1.txt
   if [ $? != 0 ]; then
	   dialog --clear
	   exit 0
   fi
   OUTDIR=$(cat _1.txt)
else
   j=`expr $i - 1`
   OUTDIR=$(echo $ddir | awk '{print $'$j'}')
fi
OUTDIR=$(echo $DATDIR$OUTDIR)

# ------------------------------

echo $ASIC $RUN_LVL $OUTDIR

#OUTDIR=$ASIC
#export ASIC=$2
#export RUN_LVL=$3
NDIR=1
OUTFL=$OUTDIR/$ASIC"_"$RUN_LVL
OUTFL2=$OUTDIR/$ASIC
LOGFIL=$OUTFL$SUF_LOG
echo $NDIR
echo $OUTFL
echo $LOGFIL

if [ -f $LOGFIL ]; then
   dialog --title "Probestation ASIC test" --yesno "Data already exist. Overwrite the data? " 9 30
   if [ $? != 0 ]; then
	   dialog --clear
	   exit 0
   fi
else
   [ -d $OUTDIR ] || mkdir $OUTDIR
fi

echo `date ` ": " $0 $ASIC $RUN_LVL $OUTDIR | tee -a $OUTDIR"/run_log.txt"
dialog --clear
echo "============================================="
echo "Start testing ...."
echo
echo


##########################################################################
# Upload md softwares
upload_md() {
	echo  `date` UPLOAD MD   | tee -a $OUTFL$SUF_LOG
	./mup_all.sh             | tee -a $OUTFL$SUF_LOG
}

kill_misc() {
	./zfill.sh
	sleep 8
}

kill_mterm() {
	./killmterm.sh
	sleep 3
}

##########################################################################
# DC tests
init_scope() {
	$SC_SET < tek_normal.ini
}

# 4VSEL set
set_4vsel() {
   if [ -f $OUTFL2$V4VSEL ]; then
       v4vsel=$(cat $OUTFL2$V4VSEL)
       cat $OUTFL2$V4VSEL misc_4vsel.txt > _1.txt
   else
       v4vsel=$(cat 4vsel_def.ini)
       cat 4vsel_def.ini  misc_4vsel.txt > _1.txt
   fi
   echo `date` 4VSEL $v4vsel   | tee -a $OUTFL$SUF_LOG
   ./mupld_c.exe -p 10 -I _1.txt &
   #echo "pass2"
   ./mterm.exe -d -c /dev/com4 | tee -a $OUTFL$SUF_LOG
}

conf_4vsel() {
   if [ -f $OUTFL2$V4VSEL ]; then
       v4vsel=$(cat $OUTFL2$V4VSEL)
   else
       v4vsel=$(cat 4vsel_def.ini)
   fi
   dialog --title "Probestation ASIC test" --inputbox "Confirm $ASIC 4VSEL value :" 9 40 \
         $v4vsel 2> $OUTFL2$V4VSEL
}


# GLOBALDC
run_globaldc() {
   echo  `date` GLOBALDC    | tee -a $OUTFL$SUF_LOG
   echo "GLOBALDC"          | ./mupld_c.exe -p 5 &
   ./mterm.exe -d -c /dev/com4 | tee $OUTFL$TYP_GDC$SUF_LOG

   $DMVS -usr $USER < $OUTFL$TYP_GDC$SUF_LOG | tee $OUTFL$TYP_GDC$SUF_ANA
}

# TESTADC
run_testadc() {
   echo  `date` TESTADC     | tee -a $OUTFL$SUF_LOG
   echo "TESTADC"           | ./mupld_c.exe -p 5 &
   ./mterm.exe -d -c /dev/com4 | tee $OUTFL$TYP_ADC$SUF_LOG
}

# PIXELDC
run_pixeldc() {
   echo  `date` PIXELDC     | tee -a $OUTFL$SUF_LOG
   echo "PIXELDC"           | ./mupld_c.exe -p 5 &
   ./mterm.exe -d -c /dev/com4 | tee $OUTFL$TYP_PDC$SUF_LOG
}

# NDISCTEST
run_ndisctest() {
   echo  `date` NDISCTEST   | tee -a $OUTFL$SUF_LOG
   echo "NDISCTEST"         | ./mupld_c.exe -p 5 &
   ./mterm.exe -d -c /dev/com4 | tee $OUTFL$TYP_DIS$SUF_LOG
}


##########################################################################
# SCOPE tests
run_mterm() {
   echo `date` START_MTERM  | tee -a $OUTFL$SUF_LOG
   ( ./mterm.exe -d | tee -a $OUTFL$TYP_OSC$SUF_LOG ) &
   sleep 3
}

## if you start from scope test run following otherwise comment out
init_misc() {
   ./mupld_c.exe -I misc_scini.txt
   sleep 3
}


# Preamp mode
run_scope_pre() {
   echo `date` SCOPE_PRE    | tee -a $OUTFL$SUF_LOG
   $SC_INI > _1.txt
   sleep 8

   ./mupld_c.exe -I misc_scpre.txt
   #$SC_SET < tek_preamp.ini  > $OUTFL$TYP_PR$SUF_OSC
   #$SC_SET < tek_normal.ini  > $OUTFL$TYP_PR$SUF_OSC
   $SC_SET < tek_preamp_2.ini  > $OUTFL$TYP_PR$SUF_OSC
   if [ "$SSCAN" = "y" ]; then
      sleep 6
      ./single_xysel.sh 4 TRY5 0 0
      #./xysel_prsh.sh "0 TRY5 " 400 >> $OUTFL$TYP_PR$SUF_OSC
      #./xysel_prsh.sh "0 TRY5 " 300 >> $OUTFL$TYP_PR$SUF_OSC
      ./xysel_pr.sh "2 TRY5 " 700    >> $OUTFL$TYP_PR$SUF_OSC
      ./scope.exe -c < $OUTFL$TYP_PR$SUF_OSC | tee $OUTFL2$TYP_PR$SUF_MIS
   fi
}

run_single_pre() {
   echo `date` SINGLE_PRE   | tee -a $OUTFL$SUF_LOG

   ./mupld_c.exe -I misc_scpre.txt
   #$SC_SET < tek_normal.ini
   $SC_SET < tek_preamp_2.ini

   sleep 3
   cat $OUTFL2$TYP_PR$SUF_MIS > _1.txt
   ctmp=$(cat _1.txt |grep -v "Non" |awk '{print $1}')
   nmis=$(echo $ctmp |awk '{print NF}')
   if [ $fq = "n" ]; then
      if [ $nmis = 0 ]; then
   	   echo "On pixel to re-take the data."
	   echo "now exiting."
	   exit
      fi
      if [ $nmis -gt "20" ]; then
	   echo "There are $nmis pixels to re-take the data."
	   echo "Are you sure you want to do this? [y/n]"
           read ans
           [ "$ans" = "n" ] && exit
           [ "$ans" = "N" ] && exit
      fi
      for (( i=1 ; i<=$nmis ; i++ ))
      do
           j=`expr $i + 1`
	   xytxt=$(head _1.txt -n $j | tail - -n 1)
	   xtxt=$(echo $xytxt| awk '{print $1}')
	   ytxt=$(echo $xytxt| awk '{print $2}')
	   single_file=$OUTFL$TYP_PR"_"$xtxt"_"$ytxt".txt"
           ./single_xysel.sh 4 TRY5 $xytxt | tee $single_file
	   sleep 3
      done
   fi
   if [ $fq = "y" ]; then
      if [ $nmis -le "20" ]; then
         for (( i=1 ; i<=$nmis ; i++ ))
         do
           j=`expr $i + 1`
	   xytxt=$(head _1.txt -n $j | tail - -n 1)
	   xtxt=$(echo $xytxt| awk '{print $1}')
	   ytxt=$(echo $xytxt| awk '{print $2}')
	   single_file=$OUTFL$TYP_PR"_"$xtxt"_"$ytxt".txt"
           ./single_xysel.sh 4 TRY5 $xytxt | tee $single_file
	   sleep 3
         done
      fi
   fi
}

# Shaper mode
run_scope_sha() {
   echo `date` SCOPE_SHA    | tee -a $OUTFL$SUF_LOG
   $SC_INI > _1.txt
   sleep 8

   ./mupld_c.exe -I misc_scsha.txt
   #$SC_SET < tek_shaper.ini  > $OUTFL$TYP_SH$SUF_OSC
   $SC_SET < tek_shaper_2.ini  > $OUTFL$TYP_SH$SUF_OSC
   if [ "$SSCAN" = "y" ]; then
      sleep 6
      ./single_xysel.sh 4 TRY5 0 0
      #./xysel_prsh.sh "2 TRY5 " 700 >> $OUTFL$TYP_SH$SUF_OSC
      #./xysel_prsh.sh "1 TRY5 " 550 >> $OUTFL$TYP_SH$SUF_OSC
      ./xysel_sh.sh "2 TRY5 " 700 >> $OUTFL$TYP_SH$SUF_OSC
      ./scope.exe -c < $OUTFL$TYP_SH$SUF_OSC | tee $OUTFL2$TYP_SH$SUF_MIS
   fi
}
run_single_sha() {
   echo `date` SINGLE_SHA   | tee -a $OUTFL$SUF_LOG

   ./mupld_c.exe -I misc_scsha.txt
   #$SC_SET < tek_shaper.ini
   $SC_SET < tek_shaper_2.ini
   sleep 3
   cat $OUTFL2$TYP_SH$SUF_MIS > _1.txt
   ctmp=$(cat _1.txt |grep -v "Non" |awk '{print $1}')
   nmis=$(echo $ctmp |awk '{print NF}')
   if [ $fq = "n" ]; then
      if [ $nmis = 0 ]; then
	   echo "On pixel to re-take the data."
	   echo "now exiting."
	   exit
      fi
      if [ $nmis -gt "20" ]; then
	   echo "There are $nmis pixels to re-take the data."
	   echo "Are you sure you want to do this? [y/n]"
           read ans
           [ "$ans" = "n" ] && exit
           [ "$ans" = "N" ] && exit
      fi
      for (( i=1 ; i<=$nmis ; i++ ))
      do
           j=`expr $i + 1`
	   xytxt=$(head _1.txt -n $j | tail - -n 1)
	   xtxt=$(echo $xytxt| awk '{print $1}')
	   ytxt=$(echo $xytxt| awk '{print $2}')
	   single_file=$OUTFL$TYP_SH"_"$xtxt"_"$ytxt".txt"
           ./single_xysel.sh 3 TRY5 $xytxt | tee $single_file
	   sleep 3
      done
   fi
   if [ $fq = "y" ]; then
      if [ $nmis -le "20" ]; then
         for (( i=1 ; i<=$nmis ; i++ ))
         do
           j=`expr $i + 1`
	   xytxt=$(head _1.txt -n $j | tail - -n 1)
	   xtxt=$(echo $xytxt| awk '{print $1}')
	   ytxt=$(echo $xytxt| awk '{print $2}')
	   single_file=$OUTFL$TYP_SH"_"$xtxt"_"$ytxt".txt"
           ./single_xysel.sh 3 TRY5 $xytxt | tee $single_file
	   sleep 3
         done
      fi
   fi
}

# CP mode
run_scope_cpm() {
   echo `date` SCOPE_CPM    | tee -a $OUTFL$SUF_LOG
   $SC_INI > _1.txt
   sleep 8

   ./mupld_c.exe -I misc_sccpm.txt
   #$SC_SET < tek_cpmode.ini  > $OUTFL$TYP_CP$SUF_OSC
   $SC_SET < tek_cpmode_2.ini  > $OUTFL$TYP_CP$SUF_OSC
   if [ "$SSCAN" = "y" ]; then
      sleep 5
      ./xysel.sh "0 TRY7"  500 >> $OUTFL$TYP_CP$SUF_OSC
      ./scope.exe -c < $OUTFL$TYP_CP$SUF_OSC | tee $OUTFL2$TYP_CP$SUF_MIS
   else
      sleep 5
      ./single_xysel.sh 1 TRY5 17 17 >> $OUTFL$TYP_CP$SUF_OSC
   fi
}

run_single_cpm() {
   echo `date` SINGLE_CPM   | tee -a $OUTFL$SUF_LOG

   ./mupld_c.exe -I misc_sccpm.txt
   #$SC_SET < tek_cpmode.ini
   $SC_SET < tek_cpmode_2.ini
   sleep 3
   cat $OUTFL2$TYP_CP$SUF_MIS > _1.txt
   ctmp=$(cat _1.txt |grep -v "Non" |awk '{print $1}')
   nmis=$(echo $ctmp |awk '{print NF}')
   echo $nmis
   if [ $fq = "n" ]; then
      if [ $nmis = 0 ]; then
	   echo "On pixel to re-take the data."
	   echo "now exiting."
	   exit
      fi
      if [ $nmis -gt "20" ]; then
	   echo "There are $nmis pixels to re-take the data."
	   echo "Are you sure you want to do this? [y/n]"
           read ans
           [ "$ans" = "n" ] && exit
           [ "$ans" = "N" ] && exit
      fi
      for (( i=1 ; i<=$nmis ; i++ ))
      do
           j=`expr $i + 1`
	   xytxt=$(head _1.txt -n $j | tail - -n 1)
	   xtxt=$(echo $xytxt| awk '{print $1}')
	   ytxt=$(echo $xytxt| awk '{print $2}')
	   single_file=$OUTFL$TYP_CP"_"$xtxt"_"$ytxt".txt"
           #./single_xysel.sh 1 TRY7 $xytxt | tee $single_file
           ./single_xysel.sh 2 TRY7 $xytxt | tee $single_file
	   sleep 3
      done
   fi
   if [ $fq = "y" ]; then
      if [ $nmis -le "20" ]; then
         for (( i=1 ; i<=$nmis ; i++ ))
         do
           j=`expr $i + 1`
	   xytxt=$(head _1.txt -n $j | tail - -n 1)
	   xtxt=$(echo $xytxt| awk '{print $1}')
	   ytxt=$(echo $xytxt| awk '{print $2}')
	   single_file=$OUTFL$TYP_CP"_"$xtxt"_"$ytxt".txt"
           ./single_xysel.sh 2 TRY7 $xytxt | tee $single_file
	   sleep 3
         done
      fi
   fi
}

#--- for 4VSEL setting -----
run_mcmdr() {
   echo "Now you are directory connected to MISC."
   echo "type 4VSEL ? to see what is the current setting"
   echo "watch oscilloscope to minimize the saw tooth with adjusting 4VSEL value"
   echo "to chage 4VSEL, type ."
   echo "number 4VSEL !"
   echo "CMD!"
   echo "number should be something near 45."
   echo "To quit from this mode, type :q"
   sleep 3
   ./mcmdr.exe -s
}
set_center() {
   echo "17 17 XYSEL" |./mupld_c.exe
}

set_4vsel_scope() {
   $SC_SET < tek_cpmode2.ini
}

save_4vsel() {
   dialog --title "Probestation ASIC test" --inputbox "Enter optimal 4VSEL value :" 9 31 \
	2> $OUTFL2$V4VSEL
}
##########################################################################
# PULSE tests
run_stimtest() {
   $SC_INI > _1.txt
   sleep 8
   ./mupld_c.exe -I misc_pulse.txt

   echo
   echo `date` CALON       | tee -a $OUTFL$SUF_LOG
   echo save bin data to $OUTFL$SUF_BIN  | tee -a $OUTFL$SUF_LOG

   ./msave.exe -s $OUTFL$SUF_BIN -e $STIMTIME &
   echo "CAL BUILD CALON " |./mupld_c.exe
   sleep 3
   echo
   echo ":q " |./mupld_c.exe
   echo
}

run_hiroplot() {
   echo "Wait until STIM data collect for $STIMTIME second."
   echo
   sleep $STIMTIME
   echo
   echo "If you see orange row for first and secnd and rest as blue,"
   echo "   STIM scan worked fine."
   echo "To quit, type :e"
   echo
   ./hiroplot-db.exe -I $OUTFL$SUF_BIN -m
}

##########################################################################
# Upload files
up_files() {
   echo `date` "Sending data to network space....." | tee -a $OUTFL$SUF_LOG
   echo "cp -r $OUTDIR $NET_HD"
   cp -r $OUTDIR $NET_HD
}

# Send mail to the admin
send_mail() {
   echo `date` e-mail sent to admin | tee -a $OUTFL$SUF_LOG
   cat $OUTFL$SUF_LOG > smtp_body.txt
   #echo $ASIC | paste smtp_subject.txt - > smtp_sub2.txt
   echo $OUTFL2 | paste smtp_subject.txt - > smtp_sub2.txt
   cat smtp_header.txt smtp_sub2.txt smtp_nan.txt smtp_body.txt > smtp_mail.txt
   cat smtp_mail.txt | /usr/sbin/ssmtp.exe -t
   #$send_to=`tail -1 smtp_header.txt`
   send_to="miyasaka"
   echo "Send $ASIC data to $send_to"
}


##########################################################################

case "$RUN_LVL" in
	"1") # very short test for GLOBALDC and STIM 
	    export STIMTIME="60"
	    conf_4vsel
	    kill_mterm
	    upload_md
	    init_scope
	    set_4vsel
	    #echo "4vpass"
	    run_globaldc
	    run_mterm
	    run_stimtest
	    run_hiroplot
	    ;;
        "2") # find optimal 4VSEL
	    kill_mterm
	    upload_md
	    init_scope
	    run_mterm
	    init_misc
            export SSCAN="n"
	    run_scope_cpm
	    set_4vsel_scope
	    run_mcmdr
	    save_4vsel
	    ;; 
	"3") # full testing
	    kill_mterm
	    init_scope
	    if [ $UPMD = "y" ]; then
	       conf_4vsel
	       upload_md
	       set_4vsel
            fi
	    [ $GLDC = "y" ] && run_globaldc
	    sleep 10
	    [ $TADC = "y" ] && run_testadc
	    [ $PIXD = "y" ] && run_pixeldc
	    [ $NDIS = "y" ] && run_ndisctest
	    if [ $RUNM = "y" ]; then
               run_mterm
	       fq="y"
	       [ $GLDC = "n" ] && init_misc
	       if [ $SPRE = "y" ]; then
                  run_scope_pre
                  run_single_pre
               fi
               if [ $SSHA = "y" ]; then
                  run_scope_sha
                  run_single_sha
               fi
               if [ $SCPM = "y" ]; then
                  run_scope_cpm
                  run_single_cpm
               fi
               if [ $SPLS = "y" ]; then
                  run_stimtest
                  sleep $STIMTIME
               fi
	    fi
	    if [ $UPFL = "y" ]; then
               sleep 10
	       echo "Wait untill data transfer to data server..."
	       up_files
	       echo "Data transfer completed."
            fi
	    if [ $SDML = "y" ]; then
	       echo "Sending mail to the admin..."
	       send_mail
            fi
	    ;;
	    
	"4") # run individual scope out mode
	    if [ $MISC_ON = "n" ]; then
	       conf_4vsel
               upload_md
	       set_4vsel
	       run_mterm
	       init_misc
            fi
            case "$RUN_OSC" in
		    "1") #go into preamp mode
                         export SSCAN="n"
			 run_single_pre
			 ;;
                    "2") #go into shaper mode
                         export SSCAN="n"
			 run_single_sha
			 ;;
                    "3") #go into shaper mode
                         export SSCAN="n"
			 run_single_cpm
			 ;;
		    "4") #go into preamp mode
                         export SSCAN="y"
	                 run_scope_pre
			 ;;
		    "5") #go into shaper mode
                         export SSCAN="y"
	                 run_scope_sha
			 ;;
		    "6") #go into CP mode
                         export SSCAN="y"
	                 run_scope_cpm
			 ;;
	    esac
	    ;;
	"*") echo "Not known run level.";;
esac

exit

