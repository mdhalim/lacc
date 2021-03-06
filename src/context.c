#include "parser/typetree.h"
#include "preprocessor/input.h"
#include <lacc/context.h>

#include <stdio.h>
#include <stdarg.h>

struct context context = {0};

/*
 * Custom implementation of printf, handling a restricted set of
 * formatters: %s, %c, %d, %lu, %ld.
 *
 * In addition, have a custom formatter for objects representing a
 * compiler-internal type object.
 *
 *  %t  : Type
 *
 */
static int vfprintf_cc(FILE *stream, const char *format, va_list ap)
{
    int n = 0;

    if (!format) {
        return n;
    }

    while (*format) {
        if (*format != '%') {
            putc(*format, stream);
            format++;
            n++;
        } else {
            switch (format[1]) {
            case 's':
                n += fputs(va_arg(ap, const char *), stream);
                break;
            case 'c':
                n += fprintf(stream, "%c", va_arg(ap, int));
                break;
            case 'd':
                n += fprintf(stream, "%d", va_arg(ap, int));
                break;
            case 'l':
                switch (*format++) {
                case 'u':
                    n += fprintf(stream, "%lu", va_arg(ap, unsigned long));
                    break;
                case 'd':
                    n += fprintf(stream, "%ld", va_arg(ap, long));
                    break;
                default:
                    format -= 2;
                }
                break;
            case 't':
                n += fprinttype(stream, va_arg(ap, Type), NULL);
                break;
            default:
                putc(*format, stream);
                putc(format[1], stream);
                n++;
                break;
            }

            format += 2;
        }
    }

    return n;
}

void verbose(const char *format, ...)
{
    if (context.verbose) {
        va_list args;
        va_start(args, format);
        vfprintf_cc(stdout, format, args);
        fputc('\n', stdout);
        va_end(args);
    }
}

void warning(const char *format, ...)
{
    va_list args;
    if (!context.suppress_warning) {
        va_start(args, format);
        fprintf(
            stderr,
            "(%s, %d) warning: ",
            str_raw(current_file_path),
            current_file_line);
        vfprintf_cc(stderr, format, args);
        fputc('\n', stderr);
        va_end(args);
    }
}

void error(const char *format, ...)
{
    va_list args;

    context.errors++;
    va_start(args, format);
    fprintf(
        stderr,
        "(%s, %d) error: ",
        str_raw(current_file_path),
        current_file_line);
    vfprintf_cc(stderr, format, args);
    fputc('\n', stderr);
    va_end(args);
}
