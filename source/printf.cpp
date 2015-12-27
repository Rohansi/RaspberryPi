//
// Created by Daniel Lindberg on 2015-11-29.
//

#include <limits.h>
#include "string.h"
#include "printf.h"

#define MAX_INTEGER_SIZE 65

#define FLAG_LEFT_ADJUST   0x01
#define FLAG_ALWAYS_SIGN   0x02
#define FLAG_PREPEND_SPACE 0x04
#define FLAG_ALTERNATIVE   0x08
#define FLAG_PAD_WITH_ZERO 0x10

#define INPUT_TYPE_hh   0
#define INPUT_TYPE_h    1
#define INPUT_TYPE_none 2
#define INPUT_TYPE_l    3
#define INPUT_TYPE_ll   4
#define INPUT_TYPE_j    5
#define INPUT_TYPE_z    6
#define INPUT_TYPE_t    7
#define INPUT_TYPE_L    8

typedef struct {
    char *buffer;
    char *buffer_end;
} printf_state_t;

typedef struct {
    int flags;
    size_t min_width;
    size_t precision;
    int input_type;
    int format;
} printf_format_t;

static size_t render_integer(char *buffer, int base, char ten, unsigned long long int val) {
    size_t index = MAX_INTEGER_SIZE - 1;
    buffer[index--] = '\0';

    if (base > 10) {
        do {
            int mod = (int)(val % base);
            if (mod < 10) {
                buffer[index--] = (char)('0' + mod);
            } else {
                buffer[index--] = (char)(ten + mod - 10);
            }
            val /= base;
        } while (val != 0);
    } else {
        do {
            buffer[index--] = (char)('0' + val % base);
            val /= base;
        } while (val != 0);
    }

    return index + 1;
}

static inline const char *read_format_integer(const char *format) {
    if (*format == '*') {
        return format + 1;
    }

    while (*format >= '0' && *format <= '9') {
        format++;
    }

    return format;
}

static inline int parse_format_integer(const char *beg, const char *end) {
    int result = 0;
    int multiplier = 1;

    while (end != beg) {
        end--;
        result += multiplier * (*end - '0');
        multiplier *= 10;
    }

    return result;
}

static inline void output_char(printf_state_t *state, char c) {
    if (state->buffer != state->buffer_end) {
        *state->buffer++ = c;
    }
}

static inline void output_n_chars(printf_state_t *state, char c, size_t n) {
    size_t left = state->buffer_end - state->buffer;
    n = n > left ? left : n;
    while (n--) {
        *state->buffer++ = c;
    }
}

static inline void output_string(printf_state_t *state, const char *str, size_t n) {
    size_t left = state->buffer_end - state->buffer;
    n = n > left ? left : n;
    while (n--) {
        *state->buffer++ = *str++;
    }
}

static void write_string(printf_state_t *state, printf_format_t fmt, const char *val) {
    size_t len = strlen(val);
    size_t pad = 0;

    if (fmt.precision != UINT_MAX && len > fmt.precision) {
        len = (size_t)fmt.precision;
    }

    if (len < fmt.min_width) {
        pad = fmt.min_width - len;
    }

    if (!(fmt.flags & FLAG_LEFT_ADJUST)) {
        output_n_chars(state, ' ', pad);
    }

    output_string(state, val, len);

    if (fmt.flags & FLAG_LEFT_ADJUST) {
        output_n_chars(state, ' ', pad);
    }
}

static void write_signed(printf_state_t *state, printf_format_t fmt, long long int val) {
    char sign = '\0';
    char buffer[MAX_INTEGER_SIZE];
    size_t index;
    size_t length;
    size_t padding = 0;
    size_t extra_zeroes = 0;

    if (fmt.precision == 0 && val == 0) {
        index = MAX_INTEGER_SIZE - 1;
        length = 0;
        buffer[MAX_INTEGER_SIZE - 1] = '\0';
    } else {
        if (val < 0) {
            index = render_integer(buffer, 10, 'a', -val);
        } else {
            index = render_integer(buffer, 10, 'a', val);
        }
        length = MAX_INTEGER_SIZE - index - 1;
    }

    if (fmt.precision != UINT_MAX && length < fmt.precision) {
        extra_zeroes = fmt.precision - length;
        length += extra_zeroes;
    }

    if (val < 0 || fmt.flags & FLAG_PREPEND_SPACE || fmt.flags & FLAG_ALWAYS_SIGN) {
        length += 1;
    }

    if (length < fmt.min_width) {
        padding = fmt.min_width - length;
    }

    if (val < 0) {
        sign = '-';
    } else if (fmt.flags & FLAG_ALWAYS_SIGN) {
        sign = '+';
    } else if (fmt.flags & FLAG_PREPEND_SPACE) {
        sign = ' ';
    }

    if (!(fmt.flags & FLAG_LEFT_ADJUST)) {
        output_n_chars(state, ' ', padding);
    }

    if (sign != '\0') {
        output_char(state, sign);
    }

    output_n_chars(state, '0', extra_zeroes);
    output_string(state, buffer + index, MAX_INTEGER_SIZE - index - 1);

    if (fmt.flags & FLAG_LEFT_ADJUST) {
        output_n_chars(state, ' ', padding);
    }
}

static void write_unsigned(printf_state_t *state, printf_format_t fmt, unsigned long long int val) {
    int base = 10;
    char ten = 'a';
    char buffer[MAX_INTEGER_SIZE];
    size_t index;
    size_t length;
    size_t padding = 0;
    size_t extra_zeroes = 0;

    if (fmt.format == 'o') {
        base = 8;
    } else if (fmt.format == 'x') {
        base = 16;
    } else if (fmt.format == 'X') {
        base = 16;
        ten = 'A';
    }

    index = render_integer(buffer, base, ten, val);
    length = MAX_INTEGER_SIZE - index - 1;

    if (fmt.format == 'o' && fmt.flags & FLAG_ALTERNATIVE) {
        if (buffer[index] != '0') {
            size_t extra_zero = length + 1;
            fmt.precision = fmt.precision < extra_zero ? extra_zero : fmt.precision;
        }
    } else if (fmt.precision == 0 && val == 0) {
        index = MAX_INTEGER_SIZE - 1;
        length = 0;
        buffer[MAX_INTEGER_SIZE - 1] = '\0';
    }

    if (fmt.precision != UINT_MAX && length < fmt.precision) {
        extra_zeroes = fmt.precision - length;
        length += extra_zeroes;
    }

    if (val != 0 && base == 16 && fmt.flags & FLAG_ALTERNATIVE) {
        length += 2;
    }

    if (length < fmt.min_width) {
        padding = fmt.min_width - length;
    }

    if (!(fmt.flags & FLAG_LEFT_ADJUST)) {
        output_n_chars(state, ' ', padding);
    }

    if (val != 0 && base == 16 && fmt.flags & FLAG_ALTERNATIVE) {
        output_char(state, '0');
        output_char(state, (char) fmt.format);
    }

    output_n_chars(state, '0', extra_zeroes);
    output_string(state, buffer + index, MAX_INTEGER_SIZE - index - 1);

    if (fmt.flags & FLAG_LEFT_ADJUST) {
        output_n_chars(state, ' ', padding);
    }
}

int vsnprintf(char *buffer, size_t bufsz, const char *format, va_list arg) {
    printf_state_t state;
    state.buffer = buffer;
    state.buffer_end = buffer + bufsz - 1;

    while (1) {
        char c = *format++;
        if (c == '\0') {
            break;
        } else if (c != '%') {
            output_char(&state, c);
        } else {
            printf_format_t fmt;
            fmt.flags = 0;

            // parse flags
            while (1) {
                switch (*format) {
                    case '-': format++; fmt.flags |= FLAG_LEFT_ADJUST; continue;
                    case '+': format++; fmt.flags |= FLAG_ALWAYS_SIGN; continue;
                    case ' ': format++; fmt.flags |= FLAG_PREPEND_SPACE; continue;
                    case '#': format++; fmt.flags |= FLAG_ALTERNATIVE; continue;
                    case '0': format++; fmt.flags |= FLAG_PAD_WITH_ZERO; continue;
                }
                break;
            }

            // parse minimum width
            // TODO: duplicated code with precision
            const char *min_width_beg = format;
            format = read_format_integer(format);
            const char *min_width_end = format;
            fmt.min_width = *min_width_beg == '*' ? va_arg(arg, int) : parse_format_integer(min_width_beg, min_width_end);

            // parse precision (if there is one)
            if (*format != '.') {
                fmt.precision = UINT_MAX;
            } else {
                format++;

                const char *precision_beg = format;
                format = read_format_integer(format);
                const char *precision_end = format;
                fmt.precision = *precision_beg == '*' ? va_arg(arg, int) : parse_format_integer(precision_beg, precision_end);
            }

            // parse input type modifier
            switch (*format) {
                case 'h': format++; fmt.input_type = *format == 'h' ? INPUT_TYPE_hh : INPUT_TYPE_h; break;
                case 'l': format++; fmt.input_type = *format == 'l' ? INPUT_TYPE_ll : INPUT_TYPE_l; break;
                case 'j': format++; fmt.input_type = INPUT_TYPE_j; break;
                case 'z': format++; fmt.input_type = INPUT_TYPE_z; break;
                case 't': format++; fmt.input_type = INPUT_TYPE_t; break;
                case 'L': format++; fmt.input_type = INPUT_TYPE_L; break;
                default:
                    fmt.input_type = INPUT_TYPE_none;
                    break;
            }

            if (fmt.input_type == INPUT_TYPE_hh || fmt.input_type == INPUT_TYPE_ll) {
                format++;
            }

            char str_buf[2];
            const char *str_val;
            long long int signed_val;
            unsigned long long int unsigned_val;

            fmt.format = *format++;
            switch (fmt.format) {
                case 'c':
                    signed_val = va_arg(arg, int);
                    str_buf[0] = (char)signed_val;
                    str_buf[1] = '\0';
                    write_string(&state, fmt, str_buf);
                    break;
                case 's':
                    str_val = va_arg(arg, const char *);
                    write_string(&state, fmt, str_val);
                    break;
                case 'd': case 'i':
                    signed_val = va_arg(arg, int);
                    write_signed(&state, fmt, signed_val);
                    break;
                case 'o': case 'x': case 'X': case 'u':
                    unsigned_val = va_arg(arg, unsigned int);
                    write_unsigned(&state, fmt, unsigned_val);
                    break;
                case 'f': case 'F':
                case 'e': case 'E':
                case 'a': case 'A':
                case 'g': case 'G':
                case 'n':
                case 'p':
                    // TODO: us
                    break;
            }
        }
    }

    *state.buffer++ = '\0';
    return (int)(state.buffer - buffer);
}

int snprintf(char *buffer, size_t bufsz, const char *format, ...) {
    va_list arg;
    va_start(arg, format);
    int ret = vsnprintf(buffer, bufsz, format, arg);
    va_end(arg);
    return ret;
}
