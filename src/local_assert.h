#ifndef __LOCAL_ASSERT_H
#define __LOCAL_ASSERT_H

#ifdef NDEBUG

#define local_assert(exp) ((void)0)

#else

void local_assert_bp(void);
#define local_assert(exp) if( !exp ) { local_assert_bp(); }

#endif

#endif
