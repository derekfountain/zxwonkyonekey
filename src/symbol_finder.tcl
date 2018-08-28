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

# Crude little script to present an interface to finding symbols
# in my map file. Slightly quicker than grep...
#
# The input file is the map file, resulting from generate_symbols.pl

if { [llength $argv] != 1 } {
    puts stderr "Usage:"
    puts stderr "  symbol_finder.tcl <symbol_file.map>"
    exit -1
}

set symbols [dict create]
proc readSymbols { filename } {
    set ::symbols [dict create]
    if { [catch {set handle [open $filename "r"]} err] } {
        # puts stderr "Unable to open \"$filename\": $err"
    } else {
        while { [gets $handle line] >= 0 } {
            if { [regexp {^(\w+)\s+(\w+)} $line unused symbol address] } {
                dict append ::symbols $symbol "0x$address"
            }
        }
        close $handle
        set ::last_modification [file mtime $filename]
    }
}

proc find_symbol { args } {
    set filtered_symbols [dict filter $::symbols key "*${::findMe}*"]

    set result ""
    dict for {key value} $filtered_symbols {
        append result "$key : $value\n"
    }
    .t delete 0.0 end
    .t insert 0.0 $result
}

pack [entry .e -width 20 -background white -textvariable findMe] -expand false -fill x
bind .e <KeyRelease> find_symbol
focus .e
pack [text .t -width 50 -height 5 -background white] -side top -expand true -fill both

set ::last_modification 0
readSymbols [lindex $argv 0]

proc refreshSymbols { filename } {
    if { ![file exists $filename] } {
        .t delete 0.0 end
        .t insert 0.0 "$filename doesn't exist"
    } else {

        if { [file mtime $filename] ne $::last_modification } {
            readSymbols $filename
            find_symbol
        }
    }
    after 1000 [list refreshSymbols $filename]
}
refreshSymbols [lindex $argv 0]
