#!/usr/bin/tclsh

# Tcl script to extract the enum types from the C source files
# and spit them out translated into BE map definitions.
#
# Give it the filenames on the command line:
#
#  ./extract_enums.tcl *.h *.c > maps.inc
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

            # Ideally replace . with index
            puts "map $enum_def_name\n{"
            foreach enum_entry $current_enum_entries {
                puts "  \"$enum_entry\" ."
            }
            puts "}\n"

            set current_enum ""
            set current_enum_entries [list]
            continue
        }

        # Look for enum entries
        if { $current_enum ne "" } {
            # This needs to understand an optional '=n' on the end of each line
            if { [regexp {^\s*([^,]+),} $line unused enum_entry_name] } {
                lappend current_enum_entries $enum_entry_name
                # puts "enum entry: $enum_entry_name"
            }
        }

        # puts ">>>>>>> $line"
    }

    close $handle
}


foreach filename $argv {
    process_file $filename
}