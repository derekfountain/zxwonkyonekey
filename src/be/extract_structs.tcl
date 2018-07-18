#!/usr/bin/tclsh

# Tcl script to extract the struct types from the C source files
# and spit them out translated into BE struct definitions.
#
# Give it the filenames on the command line:
#
#  ./extract_enums.tcl  [--enum-list <file>] *.h *.c > maps.inc
#
# If you specify the enum list file, the names found in it will
# be assumed to be map definitions. This file would normally be
# created by extract_enums.tcl so this script can recognise
# enums in the structures.
#
# It's not too smart. It needs the C source to look like this:
#
# typedef struct _process_table
# {
#   values...
# } PROCESS_TABLE;;
#
# Don't add newlines or comments or anything else which will
# confuse it.
#
# TODO This is going to need some work. :)

proc process_file { filename } {

    set current_struct ""
    set current_struct_entries [list]

    if { [catch {set handle [open $filename "r"]} err] } {
        puts stderr "Unable to open file \"$filename\" for reading. Error \"$err\""
        exit -1
    }

    while { 1 } {

        if { [gets $handle line] == -1 } {
            break
        }

        # Look for start of typedef struct
        if { [regexp {^\s*typedef\s+struct\s+(_\w+)\s*$} $line unused struct_name] } {
            set current_struct $struct_name
            # puts "Found start of struct: $struct_name"
            continue
        }

        # Looking for:
        #
        # <closebrace> STRUCT_NAME;
        #
        # not:
        #
        # <closebrace>
        # STRUCT_NAME;
        #
        #

        # Look for end of typedef struct
        if { $current_struct ne "" && [regexp {^\s*\}\s*(\w+)\s*;$} $line unused struct_def_name] } {
            # puts "Found end of struct: $struct_name, defined as $struct_def_name"

            puts "def $struct_def_name struct \n{"
            foreach struct_entry $current_struct_entries {
                puts "  $struct_entry"
            }
            puts "}\n"

            set current_struct ""
            set current_struct_entries [list]
            continue
        }

        # Look for struct entries
        if { $current_struct ne "" } {

            if { [regexp {^\s*int8_t\s+([^;]+);} $line unused struct_entry_name] } {

		# int8_t
		#
                lappend current_struct_entries "n8 dec signed \"$struct_entry_name\""

            } elseif { [regexp {^\s*int16_t\s+([^;]+);} $line unused struct_entry_name] } {

		# int16_t
		#
                lappend current_struct_entries "n16 dec signed \"$struct_entry_name\""

            } elseif { [regexp {^\s*uint8_t\s+([^;]+);} $line unused struct_entry_name] } {

		# uint8_t
		#
                lappend current_struct_entries "n8 dec unsigned \"$struct_entry_name\""

            } elseif { [regexp {^\s*uint16_t\s+([^;]+);} $line unused struct_entry_name] } {

		# uint16_t
		#
                lappend current_struct_entries "n16 dec signed \"$struct_entry_name\""

            } elseif { [regexp {^\s*struct\s+(\w+)\*\s+([^;]+);} $line unused struct_ptr struct_entry_name] } {

		# struct something* ptr_var
		#
		lappend current_struct_entries "n16 ptr $struct_ptr \"$struct_entry_name\""
	    } elseif { [regexp {^\s*(\w+)\s+([^;]+);} $line unused possible_enum struct_entry_name] } {

		# typedef'ed enum
		#
                if { [lsearch -exact $::known_enums $possible_enum] } {
                    lappend current_struct_entries "n8 map $possible_enum open \"$struct_entry_name\""
                }
	    }


        }
        

        # puts ">>>>>>> $line"
    }

    close $handle
}

set usage "extract_structs.tcl \[--enum-list <filename>\] <files>"
if { [catch {array set opts [concat { --enum-list "" } $argv]}] } {
    puts stderr $usage
    exit -1               
}

set known_enums [list]
if { $::opts(--enum-list) ne "" } {
    set handle [open $::opts(--enum-list) "r"]
    while { [gets $handle line] >= 0 } {
        lappend known_enums $line
    }
    close $handle
    set argv [lrange $argv 2 end]
}

foreach filename $argv {
    process_file $filename
}