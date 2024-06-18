#!/usr/bin/gnuplot -persist
#
#    
#    	G N U P L O T
#    	Version 4.2 patchlevel 0
#    	last modified March 2007
#    	System: CYGWIN_NT-5.1 1.5.24(0.156/4/2)
#    
#    	Copyright (C) 1986 - 1993, 1998, 2004, 2007
#    	Thomas Williams, Colin Kelley and many others
#    
#    	Type `help` to access the on-line reference manual.
#    	The gnuplot FAQ is available from
#    		http://www.gnuplot.info/faq/
#    
#    	Send comments and help requests to  <gnuplot-info@lists.sourceforge.net>
#    	Send bug reports and suggestions to <gnuplot-bugs@lists.sourceforge.net>
#    
# set terminal x11 0
# set output
unset clip points
set clip one
unset clip two
set bar 1.000000
set xdata
set ydata
set zdata
set x2data
set y2data
set timefmt x "%d/%m/%y,%H:%M"
set timefmt y "%d/%m/%y,%H:%M"
set timefmt z "%d/%m/%y,%H:%M"
set timefmt x2 "%d/%m/%y,%H:%M"
set timefmt y2 "%d/%m/%y,%H:%M"
set timefmt cb "%d/%m/%y,%H:%M"
set boxwidth
set style fill  empty border
set dummy x,y
set format x "% g"
set format y "% g"
set format x2 "% g"
set format y2 "% g"
set format z "% g"
set format cb "% g"
set angles radians
unset grid
set key title ""
unset label
unset arrow
unset style line
unset style arrow
unset logscale
set offsets 0, 0, 0, 0
set pointsize 1
set encoding default
unset polar
unset parametric
unset decimalsign
set view map
set samples 100, 100
set isosamples 10, 10
set surface
unset contour
set clabel '%8.3g'
set mapping cartesian
set datafile separator whitespace
unset hidden3d
set cntrparam order 4
set cntrparam linear
set cntrparam levels auto 5
set cntrparam points 5
set size ratio 0 1,1
set origin 0,0
set style data points
set style function lines
set xzeroaxis linetype -2 linewidth 1.000
set yzeroaxis linetype -2 linewidth 1.000
set x2zeroaxis linetype -2 linewidth 1.000
set y2zeroaxis linetype -2 linewidth 1.000
set ticslevel 0.5
set mxtics default
set mytics default
set mztics default
set mx2tics default
set my2tics default
set mcbtics default
set nox2tics
set noy2tics
set title "" 
set timestamp bottom 
set timestamp "" 
set rrange [ * : * ] noreverse nowriteback  # (currently [0.00000:10.0000] )
set trange [ * : * ] noreverse nowriteback  # (currently [-5.00000:5.00000] )
set urange [ * : * ] noreverse nowriteback  # (currently [-5.00000:5.00000] )
set vrange [ * : * ] noreverse nowriteback  # (currently [-5.00000:5.00000] )
set xlabel "column" 
set x2label "" 
set xrange [ 0.00000 : 32.0000 ] noreverse nowriteback
set x2range [ * : * ] noreverse nowriteback  # (currently [-10.0000:10.0000] )
set ylabel "row" 
set y2label "" 
set yrange [ 32.00000 : 0.0000 ] nowriteback
set y2range [ * : * ] noreverse nowriteback  # (currently [-10.0000:10.0000] )
set zlabel "" 
set zrange [ * : * ] noreverse nowriteback  # (currently [-10.0000:10.0000] )
set cblabel "" 
set cbrange [ * : * ] noreverse nowriteback  # (currently [-10.0000:10.0000] )
set zero 1e-08
set lmargin -1
set bmargin -1
set rmargin -1
set tmargin -1
set locale "C"
set pm3d implicit at s
set pm3d scansautomatic
set palette positive nops_allcF maxcolors 0 gamma 1.5 color model RGB 
set palette rgbformulae 33, 13, 10
set colorbox default
set loadpath 
set fontpath 
set fit noerrorvariables
splot "./.map.dat" w l t"" lt 2
#    EOF
