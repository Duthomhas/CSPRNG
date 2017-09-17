#! /usr/bin/env wish

# Friendly BUILD Script

# Copyright 2017 Michael Thomas Greer<br>
# Distributed under the Boost Software License, Version 1.0.<br>
# (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt .)

catch {console hide}

if {($tcl_platform(platform) ne "windows")} {
  if {$tcl_platform(os) ne "Darwin"} {
    ttk::style theme use clam
} }

proc scale {} {
  #tk scaling -displayof . [expr {[tk scaling -displayof .] * 1.5}]
  foreach font [font names] {
    font configure $font -size [expr {int([font configure $font -size] * 1.5)}]
  }
}

proc clean.gui {} {
  ttk::label  .lbl   -text "Clean what?" 
  ttk::button .build -text "Build Cache"         -command clean.build
  ttk::button .all   -text "All Generated Files" -command clean.all
  
  pack .lbl   -side top -fill x
  pack .build -side top -fill x
  pack .all             -fill x
  
  wm geometry . 250x100+[expr {[winfo screenwidth .] / 2 - 125}]+[expr {[winfo screenheight .] / 10 * 4}]
  
  scale
  
  bind . <Key-Escape> exit
  bind . <Key-Return> {[focus] invoke}
  bind . <Key-Down>   {if {[focus] eq ".build"} {focus .all} {focus .build}}
  bind . <Key-Up>     [bind . <Key-Down>]
  
  after 150 focus -force .build
}

proc clean.build {} {file delete -force -- _build; exit}
proc clean.all   {} {file delete -force -- _build bin lib dll bin32 lib32 dll32; exit}

proc clean {} {
  if {[file exists _build]} clean.gui clean.all
}

set TARGETS {
  "directly compiled into the exe" direct
  "linked as a static library"     static
}

proc build.gui {} {
  # Get all valid generator names
  set data [exec [auto_execok cmake] --help]
  set names [list]
  regexp -- "\nGenerators.*\$" $data data
  foreach line [split $data \n] {
    if {[regexp -- "^  (\[^ \]\[^=\]+)" $line _ name]} {
      if {[string first " - " $name] < 0} {
        lappend names [string map {[arch] {}} $name]
      }
    }
  }

  # Create the GUI
  label         .gl -text "Toolchain Generator"
  ttk::combobox .g -height [llength $names] -values $names -state readonly -cursor hand2
  label         .al -text "Architecture bits"
  ttk::combobox .a -height 3 -values [list default 32 64] -state readonly -cursor hand2
  label         .tl -text "Linkage type"
  ttk::combobox .t -height x -values [dict keys $::TARGETS] -state readonly -cursor hand2
  ttk::button   .go -text Build -command build -default active
  
  # Initialize with defaults or cached values
  .g current 0
  .a current 0
  .t current 0
  catch {
    if {[file exists BUILD.cache]} {
      set f [open BUILD.cache r]
      lassign [read $f] g a t
      close $f
      foreach n {g a t} {
        catch {
          set v [set $n]
          if {[string is integer -strict $v] && ($v >= 0) && ($v < [llength [.$n cget -values]])} {
            .$n current $v
    } } } }
  }

  # Pack GUI
  foreach w {.gl .g .al .a .tl .t} {
    pack $w -side top -fill x
  }
  pack .go -expand yes -fill both -padx 40 -pady 25

  # Display
  wm geometry . 400x300+[expr {[winfo screenwidth .] / 2 - 150}]+[expr {[winfo screenheight .] / 10 * 3}]

  scale
  
  bind . <Key-Escape> exit
  bind . <Key-Return> {.go invoke}
  
  after 150 focus -force .
}

proc save.cache {} {
  catch {
    set f [open BUILD.cache w]
    puts $f "[.g current] [.a current] [.t current]"
    close $f
  }
}

proc log.output {text {tags {}}} {
  .build.output configure -state normal
  .build.output insert end $text $tags
  .build.output configure -state disabled
  .build.output see end
}

proc initialize.output {} {
  wm withdraw .

  toplevel .build
  ttk::scrollbar .build.sb -orient vertical -command {.build.output yview}
  text .build.output -state disabled -yscrollcommand {.build.sb set} -bg #056 -fg #EEE -font Courier

  pack .build.sb -side right -fill y
  pack .build.output -fill both -expand yes

  .build.output tag configure error -background #555 -foreground #DDA
  .build.output tag configure build -background #EEE -foreground #056

  set h [expr {min( [winfo screenheight .], 900 )}]
  wm geometry .build 800x$h+[expr {[winfo screenwidth .] / 2 - 400}]+[expr {([winfo screenheight .] - $h) / 3}]
  wm protocol .build WM_DELETE_WINDOW exit

  scale
  
  bind .build <Key-Escape> exit
}

proc shell.output cmd {
  set ::done 0
  lassign [chan pipe] rerr werr
  set stdio [open |[concat $cmd [list 2>@ $werr]] a+]
  foreach {chan tags} [list $stdio "" $rerr error] {
    chan configure $chan -buffering line -blocking 0
    chan event $chan readable [list apply {{chan tags} {
      log.output [chan read $chan] $tags
      if {[chan eof $chan]} {
        log.output "Done\n" error
        chan close $chan
        set ::done 1
      }
    }} $chan $tags]
  }
  vwait ::done
}

proc build {} {
  save.cache

  set gen  [string trim [.g get]]
  set arch [string trim [.a get]]
  set type [string trim [.t get]]
  set isvs [expr {[string first "Visual Studio" $gen] >= 0}]
  
  if {$arch eq "32"} {set bindir ../bin32} {set bindir ../bin}

  if {$isvs} \
    then { if {$arch eq "64"} {set gen "$gen Win64"} {set bindir ../bin32}; set arch "" } \
    else { if {$arch ne "default"} {set arch -DARCH=$arch} {set arch ""} }

  initialize.output

  file mkdir _build
  cd _build

  # Call CMake
  log.output "Building Makefiles... \n" build
  set cmd "[auto_execok cmake] -G \"$gen\" $arch -DLINKAGE=[dict get $::TARGETS $type] .."
  shell.output $cmd
  log.output "\n"
  after 750

  # Build
  log.output "Compiling... \n" build
  if {!$isvs} \
    then {set cmd "[auto_execok cmake] --build . --config Release"} \
    else {set cmd "[auto_execok cmake] --build . --target ALL_BUILD --config Release"}
  shell.output $cmd
  
  # Strip, since I can't get CMake to do this...
  if {!$isvs} {
    catch {
      cd $bindir
      foreach name [glob *] {
        catch {exec {*}[auto_execok strip] $name}
  } } }

  # Execute
  catch {
    foreach exename [lsort [glob -directory $bindir c*-example-[dict get $::TARGETS $type]*]] {
      after 750
      log.output "\n"
      log.output "Running [file tail $exename] ... \n" build
      shell.output [file join $bindir [file tail $exename]]
    }
  }

  log.output "\n"
  log.output "All Done!\n" error
}

if {$argv eq "clean"} \
  clean \
  build.gui
