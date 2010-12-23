#!/bin/csh
#\
exec ./sp -f $0 $*


set width 640
set height 480
set input_file [lindex $argv 0]
set basename [lindex [split [file tail $input_file] .] 0]
set dir ${basename}.frames
set frame_no 1
set time 1
set frames_per_second 12.0
set every_seconds [expr 1000.0/$frames_per_second]
set write_frames 1
set debug_output 1

if {$argc == 0} {
  puts stderr "Usage: $argv0 script_file \[options\]"
  puts stderr "options: [-viewonly] [-no_debug] [-geom wxh]"
  exit
}

for {set i 1} {$i < $argc} {incr i} {
  set option [lindex $argv $i]
  puts $option
  if {$option == "-viewonly"} {
    set write_frames 0
  } elseif {$option == "-no_debug"} {
    set debug_output 0
  } elseif {$option == "-geom"} {
    incr i
    set geom [split [lindex $argv $i] "x"]
    set width [lindex $geom 0]
    set height [lindex $geom 1]
  }
}


if {$write_frames} {
  exec rm -rf $dir
  exec mkdir $dir
}



frame .frame -bd 2m -relief sunken
frame .p -bd 2m -relief ridge
set prog [canvas .p.progress -height 20]
pack $prog -expand yes -fill both 
pack .frame -side top -expand yes -fill both -padx 2m -pady 2m
pack .p -side top -expand yes -fill both -padx 2m -pady 2m

set raster .frame.raster

togl $raster \
    -rgba true \
    -redsize 2 \
    -greensize 2 \
    -bluesize 2 \
    -depth true \
    -depthsize 12 \
    -double true \
    -width $width \
    -height $height
pack $raster -padx 1m -pady 1m -fill both -expand yes 

bind . <Q> {exit}
bind . <q> [bind . <Q>]

puts $argv


update

set prog_w [winfo width $prog]
set prog_h [winfo height $prog]


set input [open $input_file]
while {[gets $input line] >=0 && $line != ""} {
  set last_line $line
}
close $input

set total_frames [expr int(floor([lindex $last_line 0] / $every_seconds))]



puts "Expecting $total_frames frames"
set input [open $input_file]

set changes 1
set fname {}

while {[gets $input line] >=0} {
  set wait 10
  while {[lindex $line 0] > [expr $time * $every_seconds]} {
    incr wait -1
    if {$wait == 0} {
      set time [expr int(floor([lindex $line 0]/$every_seconds))]
    }
    if {$debug_output} {
      puts "FRAME $frame_no"
    }
    if {$write_frames} {
      set prev_fname $fname 
      set fname $dir/${basename}_$frame_no.png
      if {$changes} {
        gr_save $fname
      } else {
        file copy $prev_fname $fname
      }
      set changes 0
    }
    $prog delete bar
    set ratio [expr $time / $total_frames.0]
    $prog create rect 0 0 [expr $prog_w * $ratio] [expr $prog_h - 1] \
        -fill blue -tag bar
    $prog create text [expr $prog_w / 2] [expr $prog_h / 2]  \
        -fill black -text "[format %.2f [expr 100 * $ratio]]%" -tag bar
    update
    incr frame_no
    incr time
  }
#if {[lindex $line 1] != $raster} {
  if {$debug_output} {
    puts $line
  }
  eval [lrange $line 1 end]
  set changes 1
#}
}

close $input

set fname $dir/${basename}_$frame_no.png
gr_save $fname

puts "Generated $total_frames frames"

exit

