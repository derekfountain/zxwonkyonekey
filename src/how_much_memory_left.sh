#!/bin/sh

REG_SP=$(perl -ne '/^REGISTER_SP\s+=\s+\$(\w+)/ && print hex($1)' < wonky.map)
BSS_END=$(perl -ne '/^__BSS_END_tail\s+=\s+\$(\w+)/ && print hex($1)' < wonky.map)
STACK_SIZE=$(perl -ne '/^TAR__crt_stack_size\s+=\s+\$(\w+)/ && print hex($1)' < wonky.map)

TOP_STACK=$(expr $REG_SP - $STACK_SIZE)
FREE_MEM=$(expr $TOP_STACK - $BSS_END)

echo -n "\nStack comes down to "
printf "%5d  " $TOP_STACK
printf "(0x%04X)\n" $TOP_STACK

echo -n "Heap starts at      "
printf "%5d  " $BSS_END
printf "(0x%04X)\n" $BSS_END

echo "                    ====="
echo -n "Free memory         "
printf "%5d  " $FREE_MEM
printf "(0x%04X)\n" $FREE_MEM