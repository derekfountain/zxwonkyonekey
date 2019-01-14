// Try to make this as memory efficient as possible. See:
//  https://www.z88dk.org/forum/viewtopic.php?pid=13301

#pragma output REGISTER_SP = 0xD000
#pragma output CRT_ENABLE_CLOSE = 0
#pragma output CLIB_EXIT_STACK_SIZE = 0
#pragma output CLIB_STDIO_HEAP_SIZE = 0
#pragma output CRT_ENABLE_RESTART = 1
