#ifndef macrOS_BASE_MACROS_H
#define macrOS_BASE_MACROS_H

/* Non-macOS specific macros. */

#if defined(__has_attribute)
#define macrOS_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
#define macrOS_HAS_ATTRIBUTE(x) 0
#endif

#if defined(__has_builtin)
#define macrOS_HAS_BUILTIN(x) __has_builtin(x)
#else
#define macrOS_HAS_BUILTIN(x) 0
#endif

#if defined __has_include
#define macrOS_HAS_INCLUDE(x) __has_include(x)
#else
#define macrOS_HAS_INCLUDE(x) 0
#endif

#if macrOS_HAS_BUILTIN(__builtin_expect)
#define macrOS_LIKELY(x) __builtin_expect(!!(x), 1)
#else
#define macrOS_LIKELY(x) (x)
#endif

#if macrOS_HAS_BUILTIN(__builtin_expect)
#define macrOS_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define macrOS_UNLIKELY(x) (x)
#endif

#if macrOS_HAS_ATTRIBUTE(returns_nonnull)
#define macrOS_RETURN_NONNULL __attribute__((returns_nonnull))
#else
#define macrOS_RETURN_NONNULL
#endif

#if macrOS_HAS_ATTRIBUTE(cold)
#define macrOS_COLD __attribute__((cold))
#else
#define macrOS_COLD
#endif

#if macrOS_HAS_ATTRIBUTE(minsize)
#define macrOS_MINSIZE __attribute__((minsize))
#elif defined(_MSC_VER)
#define macrOS_MINSIZE __pragma(optimize("s", on))
#else
#define macrOS_MINSIZE
#endif

#if macrOS_HAS_ATTRIBUTE(noinline)
#define macrOS_NEVER_INLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define macrOS_NEVER_INLINE __declspec(noinline)
#else
#define macrOS_NEVER_INLINE
#endif

#if macrOS_HAS_ATTRIBUTE(pure)
#define macrOS_PURE_FUNCT __attribute__((pure))
#else
#define macrOS_PURE_FUNCT
#endif

#ifndef macrOS_NOT_COMMONLY_CALLED
#define macrOS_NOT_COMMONLY_CALLED macrOS_MINSIZE macrOS_COLD macrOS_NEVER_INLINE
#endif

#endif /* macrOS_BASE_MACROS_H */
