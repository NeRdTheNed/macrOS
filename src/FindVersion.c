#include <macrOS/FindVersion.h>

#ifndef macrOS_INTERNAL_HAS_VERSION_CHECK_MACRO
/* TODO Consider using /System/Library/CoreServices/SystemVersion.plist?
   TODO Consider using _availability_version_check on macOS 10.15? */
#define macrOS_INTERNAL_HAS_VERSION_CHECK_MACRO
#include <macrOS/macrOSObjC.h>

#ifndef NSINTEGER_DEFINED
#define NSINTEGER_DEFINED 1

#if (defined(__LP64__) && __LP64__) || (defined(NS_BUILD_32_LIKE_64) && NS_BUILD_32_LIKE_64)
typedef long NSInteger;
typedef unsigned long NSUInteger;
#else
typedef int NSInteger;
typedef unsigned int NSUInteger;
#endif

#define NSIntegerMax LONG_MAX
#define NSIntegerMin LONG_MIN
#define NSUIntegerMax ULONG_MAX

#endif /* NSINTEGER_DEFINED */

typedef struct {
    NSInteger majorVersion;
    NSInteger minorVersion;
    NSInteger patchVersion;
} NSOperatingSystemVersion;

#if !defined(MAC_OS_X_VERSION_10_10) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_10
typedef UInt32 FourCharCode;
typedef FourCharCode OSType;
typedef SInt16 OSErr;
OSErr Gestalt(OSType selector, SInt32 *response);
#endif

typedef struct {
    uint32_t versionMajor;
    uint32_t versionMinor;
    uint32_t versionBugFix;
} FoundOSVersion;

macrOS_NOT_COMMONLY_CALLED static FoundOSVersion findOSVersion(void);

macrOS_NOT_COMMONLY_CALLED static FoundOSVersion findOSVersion(void) {
    FoundOSVersion foundOSVersion;
    /* macOS 10.10: use NSProcessInfo's operatingSystemVersion.
       PowerPC builds will never run on macOS 10.10, so this code is removed. */
#if !defined(__ppc64__) && !defined(__ppc__)
    initObjCRuntime();
    Class NSProcessInfoClass = (Class) class("NSProcessInfo");
    SEL operatingSystemVersionSel = sel("operatingSystemVersion");
    /* NSProcessInfo *processInfo = [NSProcessInfo processInfo]; */
    id processInfo = objc_msgSend_id(class("NSProcessInfo"), sel("processInfo"));
#if !defined(MAC_OS_X_VERSION_10_10) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_10

    /* Check if processInfo responds to the operatingSystemVersion selector */
    if (macrOS_LIKELY(objc_msgSend_t(BOOL, SEL)(processInfo, sel("respondsToSelector:"), operatingSystemVersionSel)))
#endif /* MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_10 */
    {
        /* NSOperatingSystemVersion osVersion = [processInfo operatingSystemVersion]; */
        NSOperatingSystemVersion osVersion;
        objc_msgSend_stret_t(osVersion, NSOperatingSystemVersion)(processInfo, operatingSystemVersionSel);
        /* Get the version numbers */
        foundOSVersion.versionMajor = (SInt32) osVersion.majorVersion;
        foundOSVersion.versionMinor = (SInt32) osVersion.minorVersion;
        foundOSVersion.versionBugFix = (SInt32) osVersion.patchVersion;
    }

#if !defined(MAC_OS_X_VERSION_10_10) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_10
    else
#endif /* MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_10 */
#endif /* !defined(__ppc64__) && !defined(__ppc__) */
#if !defined(MAC_OS_X_VERSION_10_10) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_10
    {
        /* Pre macOS 10.10: Use Gestalt, deprecated in 10.8. */
#if !defined(MAC_OS_X_VERSION_10_4) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_4
        SInt32 systemVersion = 0;
        Gestalt(/* gestaltSystemVersion */ 'sysv', &systemVersion);

        /* The individual version selectors are not officially supported until macOS 10.4,
           but they're required to get the correct version post macOS 10.4. */
        if (macrOS_UNLIKELY(systemVersion < 0x1040)) {
            /* Split the packed BCD version into individual components */
            foundOSVersion.versionMajor = ((systemVersion & 0xF000) >> 12) * 10 + ((systemVersion & 0x0F00) >> 8);
            foundOSVersion.versionMinor = (systemVersion & 0x00F0) >> 4;
            foundOSVersion.versionBugFix = (systemVersion & 0x000F);
        } else
#endif /* MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_4 */
        {
            /* Get the individual version numbers */
            SInt32 version = 0;
            Gestalt(/* gestaltSystemVersionMajor */ 'sys1', &version);
            foundOSVersion.versionMajor = (uint32_t) version;
            Gestalt(/* gestaltSystemVersionMinor */ 'sys2', &version);
            foundOSVersion.versionMinor = (uint32_t) version;
            Gestalt(/* gestaltSystemVersionBugFix */ 'sys3', &version);
            foundOSVersion.versionBugFix = (uint32_t) version;
        }
    }

#endif /* MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_10 */
    return foundOSVersion;
}

macrOS_PURE_FUNCT bool isAvalible(uint32_t major, uint32_t minor, uint32_t bugfix) {
    /* TODO Thread safety, possibly use grand dispatch */
    static bool hasFoundOSVersion = false;
    static FoundOSVersion foundOSVersion;

    if (macrOS_UNLIKELY(!hasFoundOSVersion)) {
        foundOSVersion = findOSVersion();
        hasFoundOSVersion = true;
    }

    return (foundOSVersion.versionMajor > major) || ((foundOSVersion.versionMajor == major) && (foundOSVersion.versionMinor > minor)) || ((foundOSVersion.versionMajor == major) && (foundOSVersion.versionMinor == minor) && (foundOSVersion.versionBugFix >= bugfix));
}
#else

/* Make the compiler happy by not leaving the source file empty */
typedef int macrOSNoAlarmsAndNoSuprises;

#endif
