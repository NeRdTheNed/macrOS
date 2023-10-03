#ifndef macrOS_OBJC_H
#define macrOS_OBJC_H

/* Heavily modified file originally based on code from Tigr,
   a public domain cross-platform graphics library.
   Most modifications were made for adding support for very old versions of macOS.
   See the original project at https://github.com/erkkah/tigr
   Thanks for demonstrating a pure C way to use Objective-C on macOS! */

#include <boost/preprocessor/debug/assert.hpp>
#include <boost/preprocessor/logical/not.hpp>
#include <boost/preprocessor/stringize.hpp>

#include "Avail.h"
#include "macMacros.h"

#include <CoreFoundation/CoreFoundation.h>

#include <stdbool.h>
#include <stddef.h>

/* Initialises cached methods and selectors for the Objective C runtime.
   This method should always be called before using any Objective C runtime functions.
   TODO Header only build */
macrOS_NOT_COMMONLY_CALLED void initObjCRuntime(void);

#if defined(MAC_OS_X_VERSION_10_5) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5
#define CAN_CACHE_OBJC_METHODS
#endif

#include <objc/objc.h>

#if macrOS_HAS_INCLUDE(<objc/objc-runtime.h>) || (!defined(MAC_OS_X_VERSION_10_4) || MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_4)
#include <objc/objc-runtime.h>
#if defined(MAC_OS_X_VERSION_10_2) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2
/* Define some methods on old SDKs */
BOOL class_addMethod(Class class, SEL name, IMP imp, const char* types) macrOS_AVAILABLE_MACOS(10.5) macrOS_WEAK_IMPORT_BEFORE(10, 5);
Class objc_allocateClassPair(Class super, const char* name, size_t bonus) macrOS_AVAILABLE_MACOS(10.5) macrOS_WEAK_IMPORT_BEFORE(10, 5);
void objc_registerClassPair(Class class) macrOS_AVAILABLE_MACOS(10.5) macrOS_WEAK_IMPORT_BEFORE(10, 5);
Protocol* objc_getProtocol(const char* name) macrOS_AVAILABLE_MACOS(10.5) macrOS_WEAK_IMPORT_BEFORE(10, 5);
BOOL class_addProtocol(Class class, Protocol* protocol) macrOS_AVAILABLE_MACOS(10.5) macrOS_WEAK_IMPORT_BEFORE(10, 5);
#endif
#else
#include <objc/message.h>
#include <objc/runtime.h>
#endif

#if defined(MAC_OS_X_VERSION_10_9) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_9
#include <objc/NSObjCRuntime.h>
#endif

#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) || defined(__arm64__)
#define NON_FRAGILE_API
#endif

/* ABI is a bit different between platforms */
#ifdef __arm64__
#define abi_objc_msgSend_stret objc_msgSend
#else
#define abi_objc_msgSend_stret objc_msgSend_stret
#endif
#ifdef __i386__
#define abi_objc_msgSend_fpret objc_msgSend_fpret
#else
#define abi_objc_msgSend_fpret objc_msgSend
#endif

/* Macros to help debug uses of objc_msgSend_stret
   TODO These are very simplified, and don't take any special cases into account.
   They're likely to be wrong or misleading, because I don't understand the ABIs.
   Just use the inbuilt ObjC "is special return type" check */

#ifdef __ppc64__
/* TODO 64 bit PowerPC */
#define OK_TO_STRET(TYPE) BOOST_PP_NOT(sizeof(TYPE) == 16)
#define ARCH_NAME "ppc64"
#elif defined(__ppc__)
/* On 32 bit PowerPC, structs are never returned in registers */
#define OK_TO_STRET(TYPE) BOOST_PP_BOOL(1)
#define ARCH_NAME "ppc32"
#elif defined(__x86_64__)
/* On 64 bit Intel processors, structs are returned in registers when they fit into "two 8 bytes" */
#define OK_TO_STRET(TYPE) BOOST_PP_BOOL(sizeof(TYPE) > 16)
#define ARCH_NAME "x86_64"
#elif defined(__i386__)
/* On 32 bit Intel processors, structs are returned in registers when they are 1, 2, 4, or 8 bytes large. */
#define OK_TO_STRET(TYPE) BOOST_PP_NOT(sizeof(TYPE) == 1 || sizeof(TYPE) == 2 || sizeof(TYPE) == 4 || sizeof(TYPE) == 8)
/* TODO I think Apple dictates that all return values are 4 byte algined, so this could really just be (sizeof(TYPE) > 8) */
#define ARCH_NAME "i386"
#else
/* TODO Non 64 bit ARM?
   Other platforms */
#define OK_TO_STRET(TYPE) BOOST_PP_BOOL(1)
#define ARCH_NAME "unknown"
#endif

#define ASSERT_OK_TO_STRET(TYPE) BOOST_PP_ASSERT_MSG(OK_TO_STRET(TYPE), "Type " BOOST_PP_STRINGIZE(TYPE) " cannot be used in objc_msgSend_stret on arch " ARCH_NAME)

/* Objective C message sending function macros, based on TIGR.
   These are called in a somewhat unusual way due to Objective C implementation details.
   Two sets of arguments are passed due to needing to cast the function calls
   to function pointers of the correct type on some platforms.

   objc_msgSend_t: send a message to an object.
   Call as: objc_msgSend_t(return type, argument types...)(id object, SEL selector, arguments...)

   objc_msgSendSuper_t: send a message to the super class of an object.
   Call as: objc_msgSendSuper_t(return type, argument types...)(struct objc_super* superclass, SEL selector, arguments...)

   objc_msgSend_stret_t: send a message to an object, which returns a struct that can't be passed in registers.
                         Due to Objective C implementation details,
                         the local variable you want the returned value to be stored in
                         is passed as the first argument of the macro.
                         This is because objc_msgSend_stret sometimes takes a struct
                         and stores the returned value in it
                         (expands to objc_msgSend_stret(&variable, arguments...)),
                         and sometimes returns a struct
                         (expands to variable = objc_msgSend_stret(arguments...)).
   Call as: objc_msgSend_stret_t(local struct variable, struct type, argument types...)(id object, SEL selector, arguments...) */

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

/* This is mostly just to allow compiling with Apple GCC 4.2 and below. TODO Try to use new prototypes / pointer type casts where possible. */
#if !(defined(__ppc64__) || defined(__ppc__)) || defined(MAC_OS_X_VERSION_10_8) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_8
/* New msgSend prototype */
#ifdef OBJC_OLD_DISPATCH_PROTOTYPES
#undef OBJC_OLD_DISPATCH_PROTOTYPES
#define OBJC_OLD_DISPATCH_PROTOTYPES 0
#endif

#define objc_msgSendSuper_t(RET, ...) ((RET(*)(struct objc_super*, SEL, ##__VA_ARGS__))objc_msgSendSuper)
#define objc_msgSend_t(RET, ...) ((RET(*)(id, SEL, ##__VA_ARGS__))objc_msgSend)
#define objc_msgSend_stret_t(STRUCT, RET, ...) STRUCT = ((RET(*)(id, SEL, ##__VA_ARGS__))abi_objc_msgSend_stret)
#else
/* Old msgSend prototype */
#define objc_msgSendSuper_t(RET, ...) (RET)objc_msgSendSuper
#define objc_msgSend_t(RET, ...) (RET)objc_msgSend
#ifdef __OBJC2__
#define objc_msgSend_stret_t(STRUCT, RET, ...) STRUCT = (RET)abi_objc_msgSend_stret
#else
/* Haha preprocessor go brr */
#define abi_objc_msgSend_stret_helper(...) __VA_ARGS__)
#define objc_msgSend_stret_t(STRUCT, RET, ...) abi_objc_msgSend_stret(&STRUCT, abi_objc_msgSend_stret_helper
#endif
#endif /* defined(MAC_OS_X_VERSION_10_8) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_8 */

/* Convenience methods for calling functions with common type signatures.
   Call like objc_msgSend_t, but omit the set of function type arguments. */

#define objc_msgSend_id objc_msgSend_t(id)
#define objc_msgSend_void objc_msgSend_t(void)
#define objc_msgSend_void_id objc_msgSend_t(void, id)
#define objc_msgSend_void_bool objc_msgSend_t(void, bool)

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

/* Convenience methods to call common Objective C runtime functions.

   SEL sel(char* name): Shorter form of sel_registerName. Returns the selector for this name.

   id class(char* name): Shorter form of objc_getClass. Returns the class with this name.
   Note this returns an id, not a Class.

   Class makeClass(const char* name, const char* super): Create a Objective C class
   with the specified name and superclass. Call finaliseClass to register it
   after adding methods, protocols etc.

   finaliseClass(Class class): Call this method to register a class
   created with makeClass to the Objective C runtime.

   addMethod(Class class, char* selectorName, IMP function, char* type signature):
   Adds a method to a class created with makeClass. */

#define sel(NAME) sel_registerName(NAME)
#define class(NAME) ((id)objc_getClass(NAME))
#if !defined(NON_FRAGILE_API) && (!defined(MAC_OS_X_VERSION_10_4) || MAC_OS_X_VERSION_MIN_REQUIRED <= MAC_OS_X_VERSION_10_4)
Class wrapper_makeClass(const char* name, const char* super);
void wrapper_finaliseClass(Class class);
#define makeClass(NAME, SUPER) wrapper_makeClass(NAME, SUPER)
#define finaliseClass(class) wrapper_finaliseClass(class)
#define addMethodToClass_wrapper wrapper_addClassMethod
BOOL wrapper_addClassMethod(Class class, SEL name, IMP imp, const char* types);
#else
#define makeClass(NAME, SUPER) objc_allocateClassPair((Class)objc_getClass(SUPER), NAME, 0)
#define finaliseClass(class) objc_registerClassPair(class)
#define addMethodToClass_wrapper class_addMethod
#endif

/* Check here to get the signature right: https://nshipster.com/type-encodings/ */
#define addMethod(CLASS, NAME, IMPL, SIGNATURE)                       \
    if (!addMethodToClass_wrapper(CLASS, sel(NAME), (IMP)(IMPL), (SIGNATURE))) \
    macrOS_ASSERT(false)

#define addIvar(CLASS, NAME, SIZE, SIGNATURE)                           \
    if (!class_addIvar(CLASS, NAME, SIZE, rint(log2(SIZE)), SIGNATURE)) \
    macrOS_ASSERT(false)

/* Convenience macros to call Objective C functions which may have specialised implementations.
   id objc_alloc(Class class): Equivalent to [Class alloc]
   id objc_init(Class class): Equivalent to [Class init]
   id objc_alloc_init(Class class): Equivalent to [[[Class alloc] init]
   id objc_autorelease(id object): Equivalent to [object autorelease]
   id objc_alloc_init_autorelease(class): Equivalent to [[[Class alloc] init] autorelease]
   void* OBJC_AUTORELEASE_POOL_PUSH(): Returns an autorelease pool.
   OBJC_AUTORELEASE_POOL_POP(void* pool): Drains an autorelease pool created with OBJC_AUTORELEASE_POOL_PUSH. */

#if macrOS_HAS_INCLUDE(<objc/objc-internal.h>)
#define HAS_INTERNAL_HEADER
#include <objc/objc-internal.h>
#endif

#if defined(NON_FRAGILE_API) && defined(MAC_OS_X_VERSION_10_10) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_10
#ifndef HAS_INTERNAL_HEADER
id objc_alloc(Class class) macrOS_AVAILABLE_MACOS(10.9);
#endif
#else
extern SEL allocSel;
#define objc_alloc(class) objc_msgSend_id((id) class, allocSel)
#endif

extern SEL initSel;
#define objc_init(x) objc_msgSend_id(x, initSel)

#if defined(NON_FRAGILE_API) && defined(MAC_OS_X_VERSION_10_14) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_14
#ifndef HAS_INTERNAL_HEADER
id objc_alloc_init(Class class) macrOS_AVAILABLE_MACOS(10.14);
#endif
#else
#define objc_alloc_init(class) objc_init(objc_alloc(class))
#endif

#if defined(NON_FRAGILE_API) && defined(MAC_OS_X_VERSION_10_10) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_10
#ifndef HAS_INTERNAL_HEADER
id objc_autorelease(id value) macrOS_AVAILABLE_MACOS(10.7);
#endif
#else
extern SEL autoreleaseSel;
#define objc_autorelease(x) objc_msgSend_id(x, autoreleaseSel)
#endif

#define objc_alloc_init_autorelease(class) objc_autorelease(objc_alloc_init(class))

#if defined(NON_FRAGILE_API) && defined(MAC_OS_X_VERSION_10_7) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_7
#define OBJC_AUTORELEASE_POOL_POP(pool) objc_autoreleasePoolPop(pool)
#define OBJC_AUTORELEASE_POOL_PUSH() objc_autoreleasePoolPush()
#ifndef HAS_INTERNAL_HEADER
void objc_autoreleasePoolPop(void *pool) macrOS_AVAILABLE_MACOS(10.7);
macrOS_RETURN_NONNULL void *objc_autoreleasePoolPush(void) macrOS_AVAILABLE_MACOS(10.7);
#endif
#else
extern Class NSAutoreleasePoolClass;
extern SEL drainSel;
#ifdef CAN_CACHE_OBJC_METHODS
extern void(*drainMethod)(id, SEL);
#define OBJC_AUTORELEASE_POOL_POP(pool) drainMethod(pool, drainSel)
#else
#define OBJC_AUTORELEASE_POOL_POP(pool) objc_msgSend_void(pool, drainSel)
#endif
/* TODO Cache implementation method */
#define OBJC_AUTORELEASE_POOL_PUSH() objc_alloc_init(NSAutoreleasePoolClass)
#endif

#endif /* macrOS_OBJC_H */
