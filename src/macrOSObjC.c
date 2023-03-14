#include <macrOS/macrOSObjC.h>

#if !defined(NON_FRAGILE_API) || (!defined(MAC_OS_X_VERSION_10_7) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_7)
Class NSAutoreleasePoolClass;
SEL drainSel;
#ifdef CAN_CACHE_OBJC_METHODS
void(*drainMethod)(id, SEL) = NULL;
#endif
#endif
#if !defined(NON_FRAGILE_API) || (!defined(MAC_OS_X_VERSION_10_10) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_10)
SEL autoreleaseSel;
SEL allocSel;
#endif
SEL initSel;

macrOS_NOT_COMMONLY_CALLED void initObjCRuntime(void) {
    /* TODO Thread safety, possibly use grand dispatch */
    static bool isObjcRuntimeInit = false;

    if (!isObjcRuntimeInit) {
#if !defined(NON_FRAGILE_API) || (!defined(MAC_OS_X_VERSION_10_7) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_7)
        NSAutoreleasePoolClass = (Class) class("NSAutoreleasePool");
        drainSel = sel("drain");
#ifdef CAN_CACHE_OBJC_METHODS
        drainMethod = (void(*)(id, SEL))method_getImplementation(class_getInstanceMethod(NSAutoreleasePoolClass, drainSel));
#endif
#endif
#if !defined(NON_FRAGILE_API) || (!defined(MAC_OS_X_VERSION_10_10) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_10)
        autoreleaseSel = sel("autorelease");
        allocSel = sel("alloc");
#endif
        initSel = sel("init");
        isObjcRuntimeInit = true;
    }
}

#if !defined(NON_FRAGILE_API) && (!defined(MAC_OS_X_VERSION_10_4) || MAC_OS_X_VERSION_MIN_REQUIRED <= MAC_OS_X_VERSION_10_4)

#include <macrOS/FindVersion.h>
#include <stdlib.h>
#include <string.h>

BOOL class_addMethod(Class class, SEL name, IMP imp, const char* types);

BOOL wrapper_addClassMethod(Class class, SEL name, IMP imp, const char* types) {
    /* Weak linking doesn't work on macOS 10.1 and below. TODO Dynamically load this? */
#if defined(MAC_OS_X_VERSION_10_2) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2
    if (macrOS_RUNTIME_HAS_MAC_OS_MM(10, 5)) {
        return class_addMethod(class, name, imp, types);
    } else
#endif
    {
        /* Manually allocate the method list */
        struct objc_method_list* methodList;
        methodList = calloc(1, sizeof(struct objc_method_list));

        if (macrOS_UNLIKELY(methodList == NULL)) {
            return FALSE;
        }

        methodList->method_list[0].method_name = name;
        methodList->method_list[0].method_types = types;
        methodList->method_list[0].method_imp = imp;
        methodList->method_count = 1;
        class_addMethods(class, methodList);
    }

    return TRUE;
}

static Class wrapper_objc_class_alloc(const char* name, Class super_class) {
    Class createdClass = NULL, isa = NULL;
    char* dupedString = NULL;
    /* Find root class */
    Class rootClass = super_class;

    while (rootClass->super_class != NULL) {
        rootClass = rootClass->super_class;
    }

    /** Manually allocate a class */
    createdClass = calloc(1, sizeof(struct objc_class));

    if (macrOS_UNLIKELY(createdClass == NULL)) {
        goto fail;
    }

    isa = calloc(1, sizeof(struct objc_class));

    if (macrOS_UNLIKELY(isa == NULL)) {
        goto fail;
    }

    dupedString = strdup(name);

    if (macrOS_UNLIKELY(dupedString == NULL)) {
        goto fail;
    }

    /* TODO Find more documentation on this */
    createdClass->isa = isa;
    createdClass->super_class = super_class;
    createdClass->name = dupedString;
    createdClass->info = CLS_CLASS;
    createdClass->instance_size = super_class->instance_size;
    isa->isa = rootClass->isa;
    isa->super_class = super_class->isa;
    isa->name = dupedString;
    isa->info = CLS_META;
    isa->instance_size = super_class->isa->instance_size;
    return createdClass;
fail:

    if (createdClass != NULL) {
        free(createdClass);
    }

    if (isa != NULL) {
        free(isa);
    }

    if (dupedString != NULL) {
        free(dupedString);
    }

    return NULL;
}

Class wrapper_makeClass(const char* name, const char* super) {
    /* Weak linking doesn't work on macOS 10.1 and below. TODO Dynamically load this? */
#if defined(MAC_OS_X_VERSION_10_2) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2
    if (macrOS_RUNTIME_HAS_MAC_OS_MM(10, 5)) {
        return objc_allocateClassPair((Class)objc_getClass(super), name, 0);
    } else
#endif
    {
        return wrapper_objc_class_alloc(name, (Class)objc_getClass(super));
    }
}

void wrapper_finaliseClass(Class class) {
    /* Weak linking doesn't work on macOS 10.1 and below. TODO Dynamically load this? */
#if defined(MAC_OS_X_VERSION_10_2) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2
    if (macrOS_RUNTIME_HAS_MAC_OS_MM(10, 5)) {
        objc_registerClassPair(class);
    } else
#endif
    {
        objc_addClass(class);
    }
}

#endif
