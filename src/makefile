CC=zcc
AS=zcc
TARGET=+zx
VERBOSITY=-vn
CRT=31

PRAGMA_FILE=zpragma.inc

#C_OPT_FLAGS=-SO3 --max-allocs-per-node200000
C_OPT_FLAGS=-no-cleanup --c-code-in-asm

CFLAGS=$(TARGET) $(VERBOSITY) -c $(C_OPT_FLAGS) -compiler sdcc -clib=sdcc_iy -pragma-include:$(PRAGMA_FILE)
LDFLAGS=$(TARGET) $(VERBOSITY) -m -clib=sdcc_iy -pragma-include:$(PRAGMA_FILE)
ASFLAGS=$(TARGET) $(VERBOSITY) -c

EXEC=zxrunner.tap
EXEC_OUTPUT=zxrunner

OBJECTS = levels.o \
          key_action.o \
          main.o \
          int.o \
	  runner.o \
	  runner_sprite.o \
          gameloop.o \
          tracetable.o

%.o: %.c $(PRAGMA_FILE)
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.asm
	$(AS) $(ASFLAGS) -o $@ $<

all : clean_tmp $(EXEC)

$(EXEC) : $(OBJECTS)
	 $(CC) $(LDFLAGS) -startup=$(CRT) $(OBJECTS) -o $(EXEC_OUTPUT) -create-app

.PHONY: clean_tmp
clean_tmp:
	rm -f /tmp/tmpXX* 

.PHONY: clean
clean:
	rm -f *.o *.bin *.tap *.map zcc_opt.def *~ /tmp/tmpXX* 
