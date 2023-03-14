#ifndef macrOS_EXTENDED_MAC_OS_AVAIL_H
#define macrOS_EXTENDED_MAC_OS_AVAIL_H

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/facilities/is_empty.hpp>

#include "baseMacros.h"

#if macrOS_HAS_INCLUDE(<Availability.h>)
#include <Availability.h>
#endif /* macrOS_HAS_INCLUDE(<Availability.h>) */

#if defined(__APPLE__) || macrOS_HAS_INCLUDE(<AvailabilityMacros.h>)
#include <AvailabilityMacros.h>
#endif /* defined(__APPLE__) || macrOS_HAS_INCLUDE(<AvailabilityMacros.h>) */

#if macrOS_HAS_INCLUDE(<AvailabilityVersions.h>)
#include <AvailabilityVersions.h>
#endif /* macrOS_HAS_INCLUDE(<AvailabilityVersions.h>) */

#include "GeneratedAvail.h"

/* Pass as MAJOR_MINOR_BUGFIX. Only works with official defined macOS SDK constants */
#define macrOS_ALWAYS_HAS_VERSION(version) BOOST_PP_IS_EMPTY(BOOST_PP_CAT(macrOS_ALWAYS_HAVE_, version))
#define macrOS_NEVER_HAS_VERSION(version) BOOST_PP_IS_EMPTY(BOOST_PP_CAT(macrOS_NEVER_HAVE_, version))

/* Specialised versions */
#define macrOS_ALWAYS_HAS_VERSION_M(major) macrOS_ALWAYS_HAS_VERSION_MM(major, 0)
#define macrOS_ALWAYS_HAS_VERSION_MM(major, minor) macrOS_ALWAYS_HAS_VERSION_MMF(major, minor, 0)
#define macrOS_ALWAYS_HAS_VERSION_MMF(major, minor, bugfix) macrOS_ALWAYS_HAS_VERSION(major ## _ ## minor ## _ ## bugfix)

#define macrOS_NEVER_HAS_VERSION_M(major) macrOS_NEVER_HAS_VERSION_MM(major, 0)
#define macrOS_NEVER_HAS_VERSION_MM(major, minor) macrOS_NEVER_HAS_VERSION_MMF(major, minor, 0)
#define macrOS_NEVER_HAS_VERSION_MMF(major, minor, bugfix) macrOS_NEVER_HAS_VERSION(major ## _ ## minor ## _ ## bugfix)

#endif /* macrOS_EXTENDED_MAC_OS_AVAIL_H */
