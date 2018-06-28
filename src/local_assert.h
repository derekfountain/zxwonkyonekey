#ifndef __LOCAL_ASSERT_H
#define __LOCAL_ASSERT_H

#ifdef NDEBUG

#define local_assert(exp) ((void)0)

#else

void local_assert_bp(void);

/*
 * This is the local_assert(expression) macro to use in the code.
 * For no debug (-DNDEBUG) builds this boils away to nothing.
 */
#define local_assert(exp) if( !exp ) { local_assert_bp(); }

#endif

#endif
