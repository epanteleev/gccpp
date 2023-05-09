#pragma once
#include <cassert>

#define always_inline __attribute__((always_inline))

#ifdef NDEBUG
#define assert_linux_error(errnum) static_cast<void>(errnum)
#else
#define assert_linux_error(errnum)			                                \
  (!(errnum)								                                \
   ? static_cast<void> (0)						                            \
   : __assert_perror_fail ((errnum), __FILE__, __LINE__, __ASSERT_FUNCTION))
#endif