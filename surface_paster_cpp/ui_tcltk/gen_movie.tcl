set files [exec ls]
foreach file $files {

  if {[regexp .*.png $file png_file dummy]} {
    regsub {([^\.]*)\.png$} $png_file {\1.ppm} out_file
    set cmd [list convert $png_file $out_file]
    puts $cmd
    eval exec $cmd
#regexp {([^0-9]*)} $png_file base huy
#puts $base
  }
}
exit
