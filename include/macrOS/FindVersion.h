#ifndef macrOS_EXTENDED_MAC_OS_FIND_VERSION_H
#define macrOS_EXTENDED_MAC_OS_FIND_VERSION_H

#include <boost/preprocessor/control/expr_iif.hpp>
#include <boost/preprocessor/control/iif.hpp>
#include <boost/preprocessor/logical/not.hpp>

#include <stdbool.h>

#include "baseMacros.h"

#include "Avail.h"

/* macOS runtime version check macros, with additional compile time optimisations to remove checks which will always return a known value.
   For example, a check for if a program is running macOS 10.7 or higher would be replaced by a constant value of false on PowerPC,
   as macOS only supported PowerPC from 10.0 to 10.6. Another example is a check for if a program is running on a version of macOS lower than 10.2
   will be replaced with a constant value of false on non-PowerPC 32 bit platforms, because non-PowerPC 32 bit platforms cannot run macOS 10.2.
   If the program targets a higher version of macOS than it checks for, it will be replaced by a constant value of true. */

/* Runtime + compile time check for if this program is running a version of macOS lower than the arguments specified.
   When targeting newer versions of macOS than the version checked, an if (false) statment is emmited.
   On platforms which don't support running on this version of macOS, an if (false) statment is emmited.
   Otherwise, an if (macrOS_RUNTIME_HAS_MAC_OS macro) {} else statement is emmited (as Clang's __builtin_available does not support inversion). */
#define macrOS_IFR_RUNTIME_LESS_MAC_OS_MM(major, minor) BOOST_PP_IIF(macrOS_ALWAYS_HAS_VERSION_MM(major, minor), if (false), BOOST_PP_IIF(macrOS_NEVER_HAS_VERSION_MM(major, minor), if (false), if (macrOS_RUNTIME_HAS_MAC_OS_MM(major, minor)) {} else))
#define macrOS_IFR_RUNTIME_LESS_MAC_OS_MMF(major, minor, bugfix) BOOST_PP_IIF(macrOS_ALWAYS_HAS_VERSION_MMF(major, minor, bugfix), if (false), BOOST_PP_IIF(macrOS_NEVER_HAS_VERSION_MMF(major, minor, bugfix), if (false), if (macrOS_RUNTIME_HAS_MAC_OS_MMF(major, minor, bugfix)) {} else))

/* Runtime + compile time check for if this program is running a version of macOS newer than the arguments specified.
   When targeting newer versions of macOS than the version checked, nothing is emmited.
   Otherwise, an if (macrOS_RUNTIME_HAS_MAC_OS macro) statement is emmited, which performs additional optimisations. */
#define macrOS_IFR_RUNTIME_HAS_MAC_OS_MM(major, minor) BOOST_PP_EXPR_IIF(BOOST_PP_NOT(macrOS_ALWAYS_HAS_VERSION_MM(major, minor)), if (macrOS_RUNTIME_HAS_MAC_OS_MM(major, minor)))
#define macrOS_IFR_RUNTIME_HAS_MAC_OS_MMF(major, minor, bugfix) BOOST_PP_EXPR_IIF(BOOST_PP_NOT(macrOS_ALWAYS_HAS_VERSION_MMF(major, minor, bugfix)), if (macrOS_RUNTIME_HAS_MAC_OS_MMF(major, minor, bugfix)))

/* Clang doesn't allow negating the version check, so macrOS_RUNTIME_LESS_MAC_OS_MM / macrOS_RUNTIME_LESS_MAC_OS_MMF isn't possible */

/* Runtime + compile time check for if this program is running a version of macOS newer than the arguments specified.
   When targeting newer versions of macOS than the version checked, a constant value of true is emmited.
   On platforms which don't support running on this version of macOS, a constant value of false is emmited.
   Otherwise, the macrOS_RUNTIME_HAS_MAC_OS_IMP macro is emmited. */
#define macrOS_RUNTIME_HAS_MAC_OS_MM(major, minor) BOOST_PP_IIF(macrOS_ALWAYS_HAS_VERSION_MM(major, minor), true, BOOST_PP_IIF(macrOS_NEVER_HAS_VERSION_MM(major, minor), false, macrOS_RUNTIME_HAS_MAC_OS_MM_IMP(major, minor)))
#define macrOS_RUNTIME_HAS_MAC_OS_MMF(major, minor, bugfix) BOOST_PP_IIF(macrOS_ALWAYS_HAS_VERSION_MMF(major, minor, bugfix), true, BOOST_PP_IIF(macrOS_NEVER_HAS_VERSION_MMF(major, minor, bugfix), false, macrOS_RUNTIME_HAS_MAC_OS_MMF_IMP(major, minor, bugfix)))

/* Note: the runtime checks for a macOS version may not produce behaviour identical to Clang's __builtin_available, as there is no guarantee guarded functions will be weakly linked. */

#if macrOS_HAS_BUILTIN(__builtin_available)
/* Use __builtin_available */
#define macrOS_INTERNAL_HAS_VERSION_CHECK_MACRO
#define macrOS_RUNTIME_HAS_MAC_OS_MM_IMP(major, minor) __builtin_available(macOS major ## . ## minor, *)
#define macrOS_RUNTIME_HAS_MAC_OS_MMF_IMP(major, minor, bugfix) __builtin_available(macOS major ## . ## minor ## . ## bugfix, *)
#elif defined(MAC_OS_X_VERSION_10_15) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_15
/* Use the _availability_version_check API on macOS 10.15 or higher. */
#define macrOS_INTERNAL_HAS_VERSION_CHECK_MACRO
#include <stdbool.h>
#include <stdint.h>

#define macrOS_VERSION_AVAIL_CHECK_PLATFORM_MACOS 1

typedef uint32_t dyld_platform_t;

typedef struct {
    dyld_platform_t platform;
    uint32_t version;
} dyld_build_version_t;

extern bool _availability_version_check(uint32_t versionCount, dyld_build_version_t versions[]);

#define macrOS_MERGE_VERSION(major, minor, bugfix) (((major & 0xffff) << 16) | ((minor & 0xff) << 8) | (bugfix & 0xff))

#define macrOS_RUNTIME_HAS_MAC_OS_MM_IMP(major, minor) macrOS_RUNTIME_HAS_MAC_OS_MMF_IMP(major, minor, 0)
#define macrOS_RUNTIME_HAS_MAC_OS_MMF_IMP(major, minor, bugfix) _availability_version_check(1, (dyld_build_version_t[]) { { macrOS_VERSION_AVAIL_CHECK_PLATFORM_MACOS, macrOS_MERGE_VERSION(major, minor, bugfix) } })
#else
/* Use a custom version check implementation TODO Header only build */
#include <stdbool.h>
#include <stdint.h>

bool isAvalible(uint32_t major, uint32_t minor, uint32_t bugfix);
#define macrOS_RUNTIME_HAS_MAC_OS_MM_IMP(major, minor) macrOS_RUNTIME_HAS_MAC_OS_MMF_IMP(major, minor, 0)
#define macrOS_RUNTIME_HAS_MAC_OS_MMF_IMP(major, minor, bugfix) isAvalible(major, minor, bugfix)
#endif /* macrOS_HAS_BUILTIN(__builtin_available) */

#endif /* macrOS_EXTENDED_MAC_OS_FIND_VERSION_H */
