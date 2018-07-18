#!/usr/bin/tclsh

# Tcl script to extract the enum types from the C source files
# and spit them out translated into BE map definitions.
#
# Give it the filenames on the command line:
#
#  ./extract_enums.tcl [--enum-list <file>] *.h *.c > maps.inc
#
# If you specify the enum list file, the names of the enums
# found will be written to the given file. This is handy for
# chaining the output of this command into another utility
# which is interested in what enums this found.
#
# It's not too smart. It needs the C source to look like this:
#
# typedef enum _processing_flag
# {
#   KEEP_PROCESSING,
#   STOP_PROCESSING,
# } PROCESSING_FLAG;
#
# Don't add newlines or comments or anything else which will
# confuse it.

proc process_file { filename } {

    set current_enum ""
    set current_enum_entries [list]
    set current_enum_indexes [list]

    if { [catch {set handle [open $filename "r"]} err] } {
        puts stderr "Unable to open file \"$filename\" for reading. Error \"$err\""
        exit -1
    }

    while { 1 } {

        if { [gets $handle line] == -1 } {
            break
        }
        # Comments in the lines will mess it up

        # Look for start of typedef enum
        if { [regexp {^\s*typedef\s+enum\s+(_\w+)\s*$} $line unused enum_name] } {
            set current_enum $enum_name
            # puts "Found start of enum: $enum_name"
            continue
        }

        # Looking for:
        #
        # <closebrace> ENUM_NAME;
        #
        # not:
        #
        # <closebrace>
        # ENUM_NAME;
        #
        #
        if { $current_enum ne "" && [regexp {^\s*\}\s*$} $line unused] } {
            puts stderr "Found close brace on line by itself when looking for"
            puts stderr "closure of enum $enum_name"
            exit -1
        }

        # Look for end of typedef enum
        if { $current_enum ne "" && [regexp {^\s*\}\s*(\w+)\s*;$} $line unused enum_def_name] } {
            # puts "Found end of enum: $enum_name, defined as $enum_def_name"

            puts "map $enum_def_name\n{"
            foreach enum_entry $current_enum_entries enum_index $current_enum_indexes {
                puts "  \"$enum_entry\" $enum_index"
            }
            puts "}\n"

            if { [info exists ::opts(--enum-list)] } {
                set list_handle [open $::opts(--enum-list) "a"]
                puts $list_handle $enum_def_name
                close $list_handle
            }

            set current_enum ""
            set current_enum_entries [list]
            set current_enum_indexes [list]
            continue
        }

        # Look for enum entries. Optional =n at end of declaration is supported
        if { $current_enum ne "" } {

            if { [regexp {^\s*([^,= ]+)(\s*=\s*(\d+))?,} $line unused enum_entry_name unused2 number] } {
                if { $number eq "" } {
                    lappend current_enum_entries $enum_entry_name
                    lappend current_enum_indexes "."
                } else {
                    lappend current_enum_entries $enum_entry_name
                    lappend current_enum_indexes $number
                }
                # puts "enum entry: $enum_entry_name"
            }
        }

        # puts ">>>>>>> $line"
    }

    close $handle
}

set usage "extract_enums.tcl \[--enum-list <filename>\] <files>"
if { [catch {array set opts [concat { --enum-list "" } $argv]}] } {
    puts stderr $usage
    exit -1               
}

if { $::opts(--enum-list) eq "" } {
    unset ::opts(--enum-list)
} else {
    if { [regexp {\.c$} $::opts(--enum-list)] } {
        puts stderr "Not deleting a C file because it's probably not what you meant. :)"
        exit -1
    }
    if { [regexp {\.h$} $::opts(--enum-list)] } {
        puts stderr "Not deleting a H file because it's probably not what you meant. :)"
        exit -1
    }
    file delete -force $::opts(--enum-list)
    set argv [lrange $argv 2 end]
}

foreach filename $argv {
    process_file $filename
}