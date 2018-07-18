#!/usr/bin/tclsh

# Tcl script to extract the struct types from the C source files
# and spit them out translated into BE struct definitions.
#
# Give it the filenames on the command line:
#
#  ./extract_enums.tcl *.h *.c > maps.inc
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

            puts "def $struct_def_name\n{"
            foreach struct_entry $current_struct_entries {
                puts "  \"$struct_entry\" $struct_index"
            }
            puts "}\n"

            set current_struct ""
            set current_struct_entries [list]
            continue
        }


        # puts ">>>>>>> $line"
    }

    close $handle
}


foreach filename $argv {
    process_file $filename
}