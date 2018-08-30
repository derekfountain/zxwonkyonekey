#!/usr/bin/wish

# Wonky One Key, a ZX Spectrum game featuring a single control key
# Copyright (C) 2018 Derek Fountain
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

set ::WIDTH_TILES   32
set ::HEIGHT_TILES  24

set ::BACKGROUND        "black"
set ::SOLIDH            "white"
set ::SOLIDV            "yellow"
set ::JUMPER            "red"

set ::SOLIDH_CHAR       {0x80}
set ::SOLIDV_CHAR       {0x83}
set ::JUMPER_CHAR       {0x81}

set ::HIGHLIT_OUTLINE   "yellow"
set ::UNHIGHLIT_OUTLINE "gray"

###

set ::BLOCK_SIZE    24
set ::BLOCK_SPACING 2

if { [llength $argv] != 1 } {
    puts stderr "Usage:"
    puts stderr "  level_designer.tcl <level_file.asm>"
    exit -1
}
set ::filename [lindex $argv 0]


pack [canvas .c -width  [expr {$::WIDTH_TILES  * $::BLOCK_SIZE + $WIDTH_TILES  * $::BLOCK_SPACING}] \
	        -height [expr {$::HEIGHT_TILES * $::BLOCK_SIZE + $HEIGHT_TILES * $::BLOCK_SPACING}] \
     ]


proc generateOutput {} {

    set h [open $::filename "w"]

    for { set y 0 } { $y < $::HEIGHT_TILES } { incr y } {
	for { set x 0 } { $x < $::WIDTH_TILES } { incr x } {

	    set fill [.c itemcget "_${y}x${x}" -fill]
	    if { $fill ne $::BACKGROUND } {

		puts $h [format "        defb 0x16, 0x%02X, 0x%02X   ; AT %d,%d" $y $x $y $x]

		switch $fill \
		    $::SOLIDH [list puts $h [format "        defb 0x%02X ; solidh" $::SOLIDH_CHAR]] \
		    $::SOLIDV [list puts $h [format "        defb 0x%02X ; solidv" $::SOLIDV_CHAR]] \
		    $::JUMPER [list puts $h [format "        defb 0x%02X ; jumper" $::JUMPER_CHAR]] \
	    }

	}
    }

    close $h

}

proc changeChar { keySym } {
    if { [string is integer $keySym] } {
	# Not required, as it turned out
    } elseif { $keySym eq "question" } {
	generateOutput
    }
}

bind all <KeyPress> [list changeChar %K]

proc tileEnter { widget tagName } {
    $widget itemconfigure $tagName -outline $::HIGHLIT_OUTLINE
}

proc tileLeave { widget tagName } {
    $widget itemconfigure $tagName -outline $::UNHIGHLIT_OUTLINE
}

proc tileSelect { widget tagName action } {
    if { $action eq "set solidh" } {
	$widget itemconfigure $tagName -fill $::SOLIDH
    } elseif { $action eq "set solidv" } {
	$widget itemconfigure $tagName -fill $::SOLIDV
    } elseif { $action eq "set background" } {
	$widget itemconfigure $tagName -fill $::BACKGROUND
    } elseif { $action eq "set jumper" } {
	$widget itemconfigure $tagName -fill $::JUMPER
    } else {
	return
    }

    generateOutput
}

set existingLayout [dict create]
if { [file exists $::filename] } {

    set tagName ""

    set h [open $filename "r"]
    while { [gets $h line] > 0 } {

	if { [regexp {\s+defb 0x16, (0x\w\w), (0x\w\w)} $line unused yHex xHex] } {
	    scan $yHex %x y
	    scan $xHex %x x
	    set tagName "_${y}x${x}"
	} elseif { [regexp {\s+defb 0x\w\w ; (\w+)} $line unused charType] } {
	    dict append existingLayout $tagName $charType
	}

    }
    close $h
}

for { set x 0 } { $x < $::WIDTH_TILES } { incr x } {
    for { set y 0 } { $y < $::HEIGHT_TILES } { incr y } {
	set tagName "_${y}x${x}"
	.c create rectangle [expr {$x*($::BLOCK_SIZE+2)}] \
	                    [expr {$y*($::BLOCK_SIZE+2)}] \
	                    [expr {$x*($::BLOCK_SIZE+2)+$::BLOCK_SIZE}] \
                            [expr {$y*($::BLOCK_SIZE+2)+$::BLOCK_SIZE}] \
	                    -tags $tagName \
	                    -outline $::UNHIGHLIT_OUTLINE -width 1 -fill $::BACKGROUND
	.c bind $tagName <Enter>   [list tileEnter  .c $tagName]
	.c bind $tagName <Leave>   [list tileLeave  .c $tagName]

	.c bind $tagName <Button-1>        [list tileSelect .c $tagName "set solidh"]	
	.c bind $tagName <Button-2>        [list tileSelect .c $tagName "set solidv"]	
	.c bind $tagName <Button-3>        [list tileSelect .c $tagName "set background"]	
	.c bind $tagName <Double-Button-1> [list tileSelect .c $tagName "set jumper"]

	if { [dict exists $existingLayout $tagName] } {
	    tileSelect .c $tagName "set [dict get $existingLayout $tagName]"
	}
    }
}