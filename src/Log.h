#ifndef _XSAITEKPANELS_LOG_H_
#define _XSAITEKPANELS_LOG_H_

#include <stdio.h>
#include <stdarg.h>

#if     defined(__GNUC__) || defined(__clang__)
#define PRINTF_ATTR(x)  __attribute__ ((format (printf, x, x + 1)))
#else
#define PRINTF_ATTR(x)
#endif  /* __GNUC__ || __clang__ */

#if     IBM
#define DIRSEP '\\'
#else   /* !IBM */
#define DIRSEP '/'
#endif  /* !IBM */

/*
 * This lets us chop out the basename (last path component) from __FILE__
 * at compile time. This works on GCC and Clang. The fallback mechanism
 * below just chops it out at compile time.
 */
#if     defined(__GNUC__) || defined(__clang__)
#define log_basename(f) (__builtin_strrchr(f, DIRSEP) ? \
    __builtin_strrchr(f, DIRSEP) + 1 : f)
#else   /* !__GNUC__ && !__clang__ */
const char *xsaitekpanels::log_basename(const char *filename);
#endif  /* !__GNUC__ && !__clang__ */

namespace xsaitekpanels {
    void logMsg_impl(const char *filename, int line, const char *fmt, ...)
        PRINTF_ATTR(3);
    void logMsg_v_impl(const char *filename, int line, const char *fmt,
        va_list ap);
    void log_backtrace();
}

#define logMsg(...) xsaitekpanels::logMsg_impl(log_basename(__FILE__), \
    __LINE__, __VA_ARGS__)

/*
 * This is an assert-like macro, except that we always perform the assertion
 * check, regardless of the setting of the NDEBUG macro. We also drop a msg
 * into the X-Plane log file to help debugging.
 */
#define VERIFY(x) \
    do { \
        if (!(x)) { \
            logMsg("assertion \"" #x "\" failed\n"); \
            fprintf(stderr, "%s:%d: assertion \"" #x "\" failed\n", \
                log_basename(__FILE__), __LINE__); \
            xsaitekpanels::log_backtrace(); \
            abort(); \
        } \
    } while (0)

#ifndef NDEBUG
#define ASSERT(x) VERIFY(x)
#else   /* NDEBUG */
#define ASSERT(x) (void)(x)
#endif  /* NDEBUG */

/*
 * This macro can be used to emit debug messages at various levels of
 * verbosity. It checks if the value of an integer variable named
 * <class>_debug is greater than or equal to <level>. If it is, it will
 * generate a log message using the remaining macro arguments.
 * Make sure to set the associated debug variable appropriately to see
 * these debug messages. See xsaitekpanels::reconfigure_all_multipanels()
 * for an example.
 */
#define dbg_log(class, level, ...) \
    do { \
        if (class ## _debug >= level) \
            logMsg(__VA_ARGS__); \
    } while (0)

#endif /* _XSAITEKPANELS_LOG_H_ */
