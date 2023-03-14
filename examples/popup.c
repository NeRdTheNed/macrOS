#include <macrOS/FindVersion.h>
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

enum {
    NSWarningAlertStyle = 0,
    NSInformationalAlertStyle = 1,
    NSCriticalAlertStyle = 2
};
typedef NSUInteger NSAlertStyle;

typedef CFStringRef NSString;

#if !defined(MAC_OS_X_VERSION_10_3) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_3
int NSRunAlertPanel(NSString* title, NSString* msgFormat, NSString* defaultButton, NSString* alternateButton, NSString* otherButton, ...) macrOS_AVAILABLE_DEPR_MACOS(10.0, 10.3);
#endif

int main (int argc, char** argv) {
    char* title = "Demo popup";
    char* text = "Hello world!";

    if (argc > 1) {
        text = argv[1];
    }

    if (argc > 2) {
        title = argv[2];
    }

    /* Initialise the Objective C runtime */
    initObjCRuntime();
    /* Display a popup message */
    Class NSStringClass = (Class) class("NSString");
    SEL stringWithUTF8StringSelector = sel("stringWithUTF8String:");
    /*NSString* messageTitle = [NSString stringWithUTF8String:title];*/
    id messageTitle = objc_msgSend_t(id, const char*)((id) NSStringClass, stringWithUTF8StringSelector, title);
    /*NSString* messageText = [NSString stringWithUTF8String:message];*/
    id messageText = objc_msgSend_t(id, const char*)((id) NSStringClass, stringWithUTF8StringSelector, text);
#if !defined(MAC_OS_X_VERSION_10_3) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_3

    if (macrOS_RUNTIME_HAS_MAC_OS_MM(10, 3)) {
#endif
        /*NSAlert* alert = [[[NSAlert alloc] init] autorelease];*/
        id alert = objc_alloc_init_autorelease((Class) class("NSAlert"));
        /*[alert setMessageText:messageTitle];*/
        objc_msgSend_void_id(alert, sel("setMessageText:"), messageTitle);
        /*[alert setInformativeText:messageText];*/
        objc_msgSend_void_id(alert, sel("setInformativeText:"), messageText);
        /*[alert setAlertStyle:NSInformationalAlertStyle];*/
        objc_msgSend_t(void, NSAlertStyle)(alert, sel("setAlertStyle:"), NSInformationalAlertStyle);
        /*[alert addButtonWithTitle:@"OK"];*/
        objc_msgSend_void_id(alert, sel("addButtonWithTitle:"), (id) CFSTR("OK"));
        {
            /* Set alert level */
            /*[[alert window] setLevel:NSModalPanelWindowLevel];*/
            id alertWindow = objc_msgSend_id(alert, sel("window"));
            objc_msgSend_t(void, int)(alertWindow, sel("setLevel:"), 8);
        }
        /*[alert runModal];*/
        objc_msgSend_t(int)(alert, sel("runModal"));
#if !defined(MAC_OS_X_VERSION_10_3) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_3
    } else {
        NSRunAlertPanel((NSString*) messageTitle, (NSString*) CFSTR("%@"), (NSString*) CFSTR("OK"), nil, nil, (NSString*) messageText);
    }

#endif
}
