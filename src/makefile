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
APPMAKE=appmake
TARGET=+zx
VERBOSITY=-vn
CRT=31

PRAGMA_FILE=zpragma.inc

#C_OPT_FLAGS=-SO3 --max-allocs-per-node200000 -DNDEBUG --std-c99 --list
C_OPT_FLAGS=-no-cleanup --c-code-in-asm --std-c99 --list

CFLAGS=$(TARGET) $(VERBOSITY) -c $(C_OPT_FLAGS) -preserve -compiler sdcc -clib=sdcc_iy -pragma-include:$(PRAGMA_FILE)
LDFLAGS=$(TARGET) $(VERBOSITY) -m -clib=sdcc_iy -pragma-include:$(PRAGMA_FILE)
ASFLAGS=$(TARGET) $(VERBOSITY) -c

CPP_FLAGS=$(TARGET) $(VERBOSITY) -c -compiler sdcc -clib=sdcc_iy -pragma-include:$(PRAGMA_FILE) -E

SYMBOLS_GENERATOR=./generate_symbols.pl
MAP=wonky.map

TAGGABLE_SRC_GENERATOR=./generate_taggable_src.pl
TAGGABLE_SRC=wonky.taggable_src

BE_MAP_EXTRACTOR=./zxspectrum_be/extract_enums.tcl
BE_ENUMS=./zxwonkyonekey_enums.inc
BE_ENUMS_LIST=./enums_list.lis
BE_STRUCT_EXTRACTOR=./zxspectrum_be/extract_structs.tcl
BE_STRUCTS=./zxwonkyonekey_structs.inc
BE_STATICS_EXTRACTOR=./zxspectrum_be/extract_statics.tcl
BE_STATICS=./zxwonkyonekey_statics.inc

TAGS_GENERATOR=etags
TAGS=TAGS

MEM_FREE=./how_much_memory_left.sh

EXEC=wonky.tap
EXEC_OUTPUT=wonky
SYM_OUTPUT=wonky.sym

OBJECTS = gameloop.o \
          levels.o \
          key_action.o \
          main.o \
          int.o \
          runner.o \
          collectable.o \
          door.o \
          slowdown_pill.o \
          runner_sprite.o \
          tracetable.o \
          local_assert.o \
          collision.o \
          levels_graphics.o \
          levels_maps.o \
          scoring.o \
          small_utoa.o \
          initialisation.o \
          sound.o \
          background_music.o

# Objects built from C files (as opposed to ASMs)
C_OBJECTS = gameloop.o \
            levels.o \
            key_action.o \
            main.o \
            int.o \
            runner.o \
            collectable.o \
            door.o \
            slowdown_pill.o \
            tracetable.o \
            local_assert.o \
            collision.o \
            scoring.o \
            sound.o

# A .cpre is the output of the C preprocessor
PREPROCESSED = $(C_OBJECTS:.o=.cpre)

# For now, if any header changes, recompile the lot. This list is used for
# BE structures file creation so order is important. Forward references
# in the structures need to be avoided.
HEADERS = action.h \
          collision.h \
          collectable.h \
          slowdown_pill.h \
          scoring.h \
          gameloop.h \
          game_state.h \
          int.h \
          teleporter.h \
          door.h \
          key_action.h \
          levels.h \
          local_assert.h \
          runner.h \
          tracetable.h \
          utils.h


# Run the preprocessor on *.c files to get *.cpre files
%.cpre: %.c $(PRAGMA_FILE) $(HEADERS)
	$(CC) $(CPP_FLAGS) -o $@ $<

%.o: %.c $(PRAGMA_FILE) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.asm
	$(AS) $(ASFLAGS) -o $@ $<

all : clean_tmp $(EXEC) $(SYM_OUTPUT) $(TAGGABLE_SRC) $(BE_ENUMS) $(BE_STRUCTS) $(BE_STATICS) $(TAGS) report

# Level maps, except the first, are in files built with the level designer
levels_maps.o : level1_map.inc.asm \
                level2_map.inc.asm \
                level3_map.inc.asm \
                level4_map.inc.asm

# Rule to build the executable. zcc's -create-app can't quite manage this
# because I've got a data block in low memory below the ORG point of the
# main code. So I use appmake to glue the pieces together. The glue line
# uses the map file to create a wonky__.bin, which is all the pieces
# (init ASM, data ASM and compiled code) glued together (with my initial
# between them as an eye catcher). The second call takes that file, places
# it at the ORiGin in a TAP file with on-tape blocks named "wonky".
$(EXEC) : $(OBJECTS)
	$(CC) $(LDFLAGS) -startup=$(CRT) $(OBJECTS) -o $(EXEC_OUTPUT)
	$(APPMAKE) +glue -b wonky --filler 0xDF --clean
	$(APPMAKE) +zx -b wonky__.bin --org 24950 --blockname wonky -o $(EXEC)

# Build the symbols table after the executable
$(SYM_OUTPUT): $(EXEC)
	$(SYMBOLS_GENERATOR) < $(MAP) > $(SYM_OUTPUT)

# Tagged source needs symbol table
$(TAGGABLE_SRC): $(SYM_OUTPUT)
	$(TAGGABLE_SRC_GENERATOR) $(SYM_OUTPUT) *.c.lis > $(TAGGABLE_SRC)

# BE enums extractor runs after a successful build
$(BE_ENUMS): $(EXEC)
	$(BE_MAP_EXTRACTOR) --enum-list $(BE_ENUMS_LIST) *.h *.c > $(BE_ENUMS)

# BE structs extractor runs after a successful build, plus it requires the enum list
$(BE_STRUCTS): $(EXEC) $(BE_ENUMS)
	$(BE_STRUCT_EXTRACTOR)  --enum-list $(BE_ENUMS_LIST) $(HEADERS) *.c > $(BE_STRUCTS)

# Statics processing needs preprocessor output plus the symbols table
$(BE_STATICS): $(PREPROCESSED) $(SYM_OUTPUT)
	$(BE_STATICS_EXTRACTOR) --symbols-file $(SYM_OUTPUT) *.cpre > $(BE_STATICS)

# TAGS file generated each time the binary is rebuilt for any reason
$(TAGS): $(EXEC)
	$(TAGS_GENERATOR) *.h *.c

# After everything, report the exec size
.PHONY: report
report: $(SYM_OUTPUT) $(TAGGABLE_SRC) $(BE_ENUMS) $(BE_STRUCTS) $(BE_STATICS) $(TAGS)
	$(MEM_FREE)

.PHONY: clean_tmp
clean_tmp:
	rm -f /tmp/tmpXX* 

.PHONY: clean
clean:
	rm -f *.o *.cpre *.err *.bin *.tap *.map *.sym *.lis zxwonkyonekey*.inc zcc_opt.def *~ $(BE_ENUMS) $(TAGGABLE_SRC) TAGS /tmp/tmpXX*
	rm -rf __pycache__
