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

CC=zcc
AS=zcc
TARGET=+zx
VERBOSITY=-vn
CRT=4

PRAGMA_FILE=zpragma.inc

#C_OPT_FLAGS=-SO3 --max-allocs-per-node200000 -DNDEBUG --std-c99 --list
C_OPT_FLAGS=-no-cleanup --c-code-in-asm --std-c99 --list

CFLAGS=$(TARGET) $(VERBOSITY) -c $(C_OPT_FLAGS) -preserve -compiler sdcc -clib=sdcc_iy -pragma-include:$(PRAGMA_FILE)
LDFLAGS=$(TARGET) $(VERBOSITY) -m -clib=sdcc_iy -pragma-include:$(PRAGMA_FILE)
ASFLAGS=$(TARGET) $(VERBOSITY) -c

SYMBOLS_GENERATOR=./generate_symbols.pl
MAP=wonky.map

BE_MAP_EXTRACTOR=./be/extract_enums.tcl
BE_ENUMS=./be/be_enums.inc
BE_ENUMS_LIST=./be/enums_list.lis
BE_STRUCT_EXTRACTOR=./be/extract_structs.tcl
BE_STRUCTS=./be/be_structs.inc

TAGS_GENERATOR=etags

EXEC=wonky.tap
EXEC_OUTPUT=wonky
SYM_OUTPUT=wonky.sym

OBJECTS = gameloop.o \
          levels.o \
          key_action.o \
          main.o \
          int.o \
	  runner.o \
	  runner_sprite.o \
          tracetable.o \
	  local_assert.o \
	  collision.o \
	  trc.o

%.o: %.c $(PRAGMA_FILE)
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.asm
	$(AS) $(ASFLAGS) -o $@ $<

all : clean_tmp $(EXEC)

$(EXEC) : $(OBJECTS)
	$(CC) $(LDFLAGS) -startup=$(CRT) $(OBJECTS) -o $(EXEC_OUTPUT) -create-app
	$(SYMBOLS_GENERATOR) < $(MAP) > $(SYM_OUTPUT)
	$(BE_MAP_EXTRACTOR) --enum-list $(BE_ENUMS_LIST) *.h *.c > $(BE_ENUMS)
	$(BE_STRUCT_EXTRACTOR)  --enum-list $(BE_ENUMS_LIST) *.h *.c > $(BE_STRUCTS)
	$(TAGS_GENERATOR) *.h *.c

.PHONY: clean_tmp
clean_tmp:
	rm -f /tmp/tmpXX* 

.PHONY: clean
clean:
	rm -f *.o *.bin *.tap *.map *.sym *.lis *.ss zcc_opt.def *~ $(BE_ENUMS) ./be/*.lis /tmp/tmpXX*
	rm -rf __pycache__
