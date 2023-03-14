#ifndef macrOS_MAC_MACROS_H
#define macrOS_MAC_MACROS_H

#include <boost/preprocessor/control/expr_iif.hpp>
#include <boost/preprocessor/facilities/is_empty.hpp>
#include <boost/preprocessor/logical/not.hpp>

#include "baseMacros.h"
#include "Avail.h"
#include "FindVersion.h"

/* Wrapper for using the availability attribute if it's supported. */
#if macrOS_HAS_ATTRIBUTE(availability)
#define macrOS_AVAILABLE_MACOS(num) __attribute__((availability(macosx,introduced=num)))
#define macrOS_AVAILABLE_DEPR_MACOS(num, depr) __attribute__((availability(macosx,introduced=num,deprecated=depr)))
#else
#define macrOS_AVAILABLE_MACOS(num)
#define macrOS_AVAILABLE_DEPR_MACOS(num, depr)
#endif

/* Marks a function as weakly linked. macOS didn't support weak linking until macOS 10.2. */
#if defined(MAC_OS_X_VERSION_10_2) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2
#if macrOS_HAS_ATTRIBUTE(weak_import)
#define macrOS_WEAK_IMPORT __attribute__((weak_import))
#elif defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#define macrOS_WEAK_IMPORT __attribute__((weak_import))
#elif defined(__MWERKS__) && (__MWERKS__ >= 0x3205) && !defined(__INTEL__)
#define macrOS_WEAK_IMPORT __attribute__((weak_import))
#elif defined(WEAK_IMPORT_ATTRIBUTE) && BOOST_PP_NOT(BOOST_PP_IS_EMPTY(WEAK_IMPORT_ATTRIBUTE))
#define macrOS_WEAK_IMPORT WEAK_IMPORT_ATTRIBUTE
#elif macrOS_HAS_ATTRIBUTE(weak)
#define macrOS_WEAK_IMPORT __attribute__((weak))
#else
#define macrOS_WEAK_IMPORT
#endif /* macrOS_HAS_ATTRIBUTE(weak_import) */
#else
#define macrOS_WEAK_IMPORT
#endif /* defined(MAC_OS_X_VERSION_10_2) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2 */

/* Marks the function as weakly linked if compiling for a version of macOS older than the version specified. */
#define macrOS_WEAK_IMPORT_BEFORE(major, minor) BOOST_PP_EXPR_IIF(BOOST_PP_NOT(macrOS_ALWAYS_HAS_VERSION_MM(major, minor)), macrOS_WEAK_IMPORT)
#define macrOS_WEAK_IMPORT_BEFORE_MMF(major, minor, bugfix) BOOST_PP_EXPR_IIF(BOOST_PP_NOT(macrOS_ALWAYS_HAS_VERSION_MMF(major, minor, bugfix)), macrOS_WEAK_IMPORT)

#endif /* macrOS_MAC_MACROS_H */
