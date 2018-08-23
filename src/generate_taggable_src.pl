#!/usr/bin/perl -w
use strict;

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


# Load in the symbols table. The real symbols table has all the information
# but for now my previously created symbols file containing name=value pairs
# is easiest to parse.
#
my %symbols = ();

my $symbols_filename = shift( @ARGV );
open( SYM_FILE_HANDLE, $symbols_filename ) or die("No such input file \"$symbols_filename\"\n");
while( my $line = <SYM_FILE_HANDLE> ) {

  if( $line =~ /^(\w+)\s+(\w+)/ ) {
    $symbols{$1} = "0x$2";
  }
}
close( SYM_FILE_HANDLE );


# Find the function names. They're in the list files marked with lines like:
#
#  748   0000              ; Function gameloop_add_trace
#  749   0000              ; ---------------------------------
#  750   0000              _gameloop_add_trace:
#

foreach my $lis_filename (@ARGV) {

  open( LIS_FILE_HANDLE, $lis_filename ) or die("No such input file \"$lis_filename\"\n");

  my $in_function = undef;
  my $lis_file_offset;
  my $offset_at_last_symbol;
  my $last_symbol_address;
  my $rest_of_line;

  my $skip_block = 0;
  while( my $line = <LIS_FILE_HANDLE> ) {
    chomp($line);
    $rest_of_line = $line;

    # Skip over the lis file IF 0/ENDIF blocks
    #
    if( $line =~ /\d+\s+\w+\s+IF 0/ ) {
      $skip_block = 1;
    }
    elsif( $skip_block && $line =~ /\d+\s+\w+\s+ENDIF/ ) {
      $skip_block = 0;
    }
    elsif( $line =~ /^1\s+0000\s+MODULE/ ) {
      $lis_file_offset       = "0000";
      $offset_at_last_symbol = "0000";
      $last_symbol_address   = "0000";
    }
    else {

      if( ! $skip_block ) {

	# All lines should start with, for example:
	#
	#   674   000C...
	#
	# The first value is a line number in the *.lis file. The second
	# value is a hex number which indicates the hex offset in the
	# memory map of the instruction or data at the line.
	#
	if( $line =~ /^(\d+)\s+(\w+)(\s+.*)$/ ) {
	  my $lis_file_line_number = $1;
	  $lis_file_offset = $2;
	  $rest_of_line = $3;

	  # If the thing after the offset value is formatted:
	  #
	  #  _zxy:
	  #
	  # then assume it's a symbol. It should be in the symbols table.
	  #
	  if( $line =~ /^\d+\s+\w+\s+(_\w+):\s*$/ ) {
	    my $symbol = $1;
	    
	    if( exists( $symbols{$symbol} ) ) {

	      $last_symbol_address = $symbols{$symbol};
	      $offset_at_last_symbol = $lis_file_offset;
	    
#	      print "++++ ".$symbols{$symbol}." -- ".$symbol."\n";
	    }
	    else {
	      print "???? -- ".$symbol."\n";
	    }
	  }
	}	  
      }
    }

    my $address = hex($lis_file_offset) - hex($offset_at_last_symbol) + hex($last_symbol_address);
    printf("0x%04X ++ %s\n", $address, $rest_of_line);
 
  }

  close( LIS_FILE_HANDLE );
}
