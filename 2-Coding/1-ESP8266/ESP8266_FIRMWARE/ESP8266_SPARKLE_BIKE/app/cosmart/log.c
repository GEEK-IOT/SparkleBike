/*
 * log.cpp
 *
 *  Created on: 2015年8月5日
 *      Author: Cocoonshu
 */

#include "cosmart/log.h"
#include "ets_sys.h"
#include "driver/uart.h"
#include <stdio.h>
#include <math.h>

int vsprintf(char *d, const char *s, va_list ap);

///////////////
//  外部函数  //
///////////////
void ICACHE_FLASH_ATTR Log_initialize() {
	uart_init(74880, 74880);

#if LOG_BY_UART
	system_set_os_print(FALSE);
	uart0_sendStr("\r\n");
#else
	system_set_os_print(TRUE);
	os_printf("\r\n");
#endif
}

void ICACHE_FLASH_ATTR Log_printCopyright() {
#if LOG_BY_UART
	uart0_sendStr("\r\n\r\n");
	uart0_sendStr("=================================================\r\n");
	uart0_sendStr("                 CoSmart  v1.0.1                 \r\n");
	uart0_sendStr(" Copyright 2015 Cocoonshu. All rights reserved.  \r\n");
	uart0_sendStr("=================================================\r\n");
	uart0_sendStr("\r\n\r\n");
#else
	os_printf("\r\n\r\n");
	os_printf("=================================================\r\n");
	os_printf("                 CoSmart  v1.0.1                 \r\n");
	os_printf(" Copyright 2015 Cocoonshu. All rights reserved.  \r\n");
	os_printf("=================================================\r\n");
	os_printf("\r\n\r\n");
#endif
}

#if LOG_BY_UART
void ICACHE_FLASH_ATTR Log_printf(const char *fmt, ...) {
	char buffer[LOG_BUFFER_SIZE];
	va_list arg;
	va_start(arg, fmt);
	vsprintf(buffer, fmt, arg);
	va_end(arg);
	uart0_sendStr(buffer);
}

void ICACHE_FLASH_ATTR Log_printfln(const char *fmt, ...) {
	char buffer[LOG_BUFFER_SIZE];
	va_list arg;
	va_start(arg, fmt);
	vsprintf(buffer, fmt, arg);
	va_end(arg);
	uart0_sendStr(buffer);
	uart0_sendStr("\r\n");
}
#endif

#define ENDIAN_LITTLE      1234
#define ENDIAN_BIG         4321
#define ENDIAN_PDP         3412
#define ENDIAN             ENDIAN_LITTLE
#define FMT_RJUST          0
#define FMT_LJUST          1
#define FMT_RJUST0         2
#define FMT_CENTER         3
#define MAX_FCONVERSION    512
#define MAX_FRACT          39
#define _isNan(arg)        ((arg) != (arg))
#define to_digit(c)        ((c) - '0')
#define to_char(n)         ((n) + '0')
#define TESTFLAG(x)        0
typedef int32_t            register_t;
typedef unsigned int       u_int32_t;
typedef long long          quad_t;
typedef unsigned long long u_quad_t;
typedef double             rtype;

static ICACHE_FLASH_ATTR int _finite(rtype d)
{
#if ENDIAN == ENDIAN_LITTLE
    struct IEEEdp {
    unsigned manl:32;
    unsigned manh:20;
    unsigned exp:11;
    unsigned sign:1;
    } *ip;
#else
    struct IEEEdp {
    unsigned sign:1;
    unsigned exp:11;
    unsigned manh:20;
    unsigned manl:32;
    } *ip;
#endif

    ip = (struct IEEEdp *)&d;
    return (ip->exp != 0x7ff);
}

static char* ICACHE_FLASH_ATTR _getbase(char *p, int *basep) {
    if (p[0] == '0') {
        switch (p[1]) {
        case 'x':
            *basep = 16;
            break;
        case 't': case 'n':
            *basep = 10;
            break;
        case 'o':
            *basep = 8;
            break;
        default:
            *basep = 10;
            return (p);
        }
        return (p + 2);
    }
    *basep = 10;
    return (p);
}

static int ICACHE_FLASH_ATTR _atob (u_quad_t *vp, char *p, int base) {
    u_quad_t value, v1, v2;
    char *q, tmp[20];
    int digit;

    if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
        base = 16;
        p += 2;
    }

    if (base == 16 && (q = strchr (p, '.')) != 0) {
        if (q - p > sizeof(tmp) - 1)
            return (0);

        strncpy (tmp, p, q - p);
        tmp[q - p] = '\0';
        if (!_atob (&v1, tmp, 16))
            return (0);

        q++;
        if (strchr (q, '.'))
            return (0);

        if (!_atob (&v2, q, 16))
            return (0);
        *vp = (v1 << 16) + v2;
        return (1);
    }

    value = *vp = 0;
    for (; *p; p++) {
        if (*p >= '0' && *p <= '9')
            digit = *p - '0';
        else if (*p >= 'a' && *p <= 'f')
            digit = *p - 'a' + 10;
        else if (*p >= 'A' && *p <= 'F')
            digit = *p - 'A' + 10;
        else
            return (0);

        if (digit >= base)
            return (0);
        value *= base;
        value += digit;
    }
    *vp = value;
    return (1);
}

void ICACHE_FLASH_ATTR strtoupper(char *p) {
    if(!p)
        return;
    for (; *p; p++)
        *p = toupper(*p);
}

char* ICACHE_FLASH_ATTR strichr(char *p, int c) {
    char *t;

    if (p != NULL) {
        for(t = p; *t; t++);
        for (; t >= p; t--) {
            *(t + 1) = *t;
        }
        *p = c;
    }
    return (p);
}

void ICACHE_FLASH_ATTR str_fmt(char *p, int size, int fmt) {
    int n, m, len;

    len = strlen (p);
    switch (fmt) {
    case FMT_RJUST:
        for (n = size - len; n > 0; n--)
            strichr (p, ' ');
        break;
    case FMT_LJUST:
        for (m = size - len; m > 0; m--)
            strcat (p, " ");
        break;
    case FMT_RJUST0:
        for (n = size - len; n > 0; n--)
            strichr (p, '0');
        break;
    case FMT_CENTER:
        m = (size - len) / 2;
        n = size - (len + m);
        for (; m > 0; m--)
            strcat (p, " ");
        for (; n > 0; n--)
            strichr (p, ' ');
        break;
    }
}

static char* ICACHE_FLASH_ATTR c_round(double fract, int *exp, char *start, char *end, char ch, char *signp) {
    double tmp;

    if (fract)
        (void)modf(fract * 10, &tmp);
    else
        tmp = to_digit(ch);
    if (tmp > 4)
        for (;; --end) {
            if (*end == '.')
                --end;
            if (++*end <= '9')
                break;
            *end = '0';
            if (end == start) {
                if (exp) {  /* e/E; increment exponent */
                    *end = '1';
                    ++*exp;
                }
                else {      /* f; add extra digit */
                *--end = '1';
                --start;
                }
                break;
            }
        }
    /* ``"%.3f", (double)-0.0004'' gives you a negative 0. */
    else if (*signp == '-')
        for (;; --end) {
            if (*end == '.')
                --end;
            if (*end != '0')
                break;
            if (end == start)
                *signp = 0;
        }
    return (start);
}

static char* ICACHE_FLASH_ATTR exponent(char *p, int exp, int fmtch) {
    char *t;
    char expbuf[MAX_FCONVERSION];

    *p++ = fmtch;
    if (exp < 0) {
        exp = -exp;
        *p++ = '-';
    }
    else
        *p++ = '+';
    t = expbuf + MAX_FCONVERSION;
    if (exp > 9) {
        do {
            *--t = to_char(exp % 10);
        } while ((exp /= 10) > 9);
        *--t = to_char(exp);
        for (; t < expbuf + MAX_FCONVERSION; *p++ = *t++);
    }
    else {
        *p++ = '0';
        *p++ = to_char(exp);
    }
    return (p);
}

static int ICACHE_FLASH_ATTR cvt(rtype number, int prec, char *signp, int fmtch, char *startp, char *endp) {
    register char *p, *t;
    register double fract;
    double integer, tmp;
    int dotrim, expcnt, gformat;

    dotrim = expcnt = gformat = 0;
    if (number < 0) {
        number = -number;
        *signp = '-';
    } else
        *signp = 0;

    fract = modf(number, &integer);

    /* get an extra slot for rounding. */
    t = ++startp;

    /*
     * get integer portion of number; put into the end of the buffer; the
     * .01 is added for modf(356.0 / 10, &integer) returning .59999999...
     */
    for (p = endp - 1; integer; ++expcnt) {
        tmp = modf(integer / 10, &integer);
        *p-- = to_char((int)((tmp + .01) * 10));
    }
    switch (fmtch) {
    case 'f':
        /* reverse integer into beginning of buffer */
        if (expcnt)
            for (; ++p < endp; *t++ = *p);
        else
            *t++ = '0';
        /*
         * if precision required or alternate flag set, add in a
         * decimal point.
         */
        if (prec)
            *t++ = '.';
        /* if requires more precision and some fraction left */
        if (fract) {
            if (prec)
                do {
                    fract = modf(fract * 10, &tmp);
                    *t++ = to_char((int)tmp);
                } while (--prec && fract);
            if (fract)
                startp = c_round(fract, (int *)NULL, startp,
                    t - 1, (char)0, signp);
        }
        for (; prec--; *t++ = '0');
        break;
    case 'e':
    case 'E':
eformat:    if (expcnt) {
            *t++ = *++p;
            if (prec)
                *t++ = '.';
            /* if requires more precision and some integer left */
            for (; prec && ++p < endp; --prec)
                *t++ = *p;
            /*
             * if done precision and more of the integer component,
             * round using it; adjust fract so we don't re-round
             * later.
             */
            if (!prec && ++p < endp) {
                fract = 0;
                startp = c_round((double)0, &expcnt, startp,
                    t - 1, *p, signp);
            }
            /* adjust expcnt for digit in front of decimal */
            --expcnt;
        }
        /* until first fractional digit, decrement exponent */
        else if (fract) {
            /* adjust expcnt for digit in front of decimal */
            for (expcnt = -1;; --expcnt) {
                fract = modf(fract * 10, &tmp);
                if (tmp)
                    break;
            }
            *t++ = to_char((int)tmp);
            if (prec)
                *t++ = '.';
        }
        else {
            *t++ = '0';
            if (prec)
                *t++ = '.';
        }
        /* if requires more precision and some fraction left */
        if (fract) {
            if (prec)
                do {
                    fract = modf(fract * 10, &tmp);
                    *t++ = to_char((int)tmp);
                } while (--prec && fract);
            if (fract)
                startp = c_round(fract, &expcnt, startp,
                    t - 1, (char)0, signp);
        }
        /* if requires more precision */
        for (; prec--; *t++ = '0');

        /* unless alternate flag, trim any g/G format trailing 0's */
        if (gformat) {
            while (t > startp && *--t == '0');
            if (*t == '.')
                --t;
            ++t;
        }
        t = exponent(t, expcnt, fmtch);
        break;
    case 'g':
    case 'G':
        /* a precision of 0 is treated as a precision of 1. */
        if (!prec)
            ++prec;
        /*
         * ``The style used depends on the value converted; style e
         * will be used only if the exponent resulting from the
         * conversion is less than -4 or greater than the precision.''
         *  -- ANSI X3J11
         */
        if (expcnt > prec || (!expcnt && fract && fract < .0001)) {
            /*
             * g/G format counts "significant digits, not digits of
             * precision; for the e/E format, this just causes an
             * off-by-one problem, i.e. g/G considers the digit
             * before the decimal point significant and e/E doesn't
             * count it as precision.
             */
            --prec;
            fmtch -= 2;     /* G->E, g->e */
            gformat = 1;
            goto eformat;
        }
        /*
         * reverse integer into beginning of buffer,
         * note, decrement precision
         */
        if (expcnt)
            for (; ++p < endp; *t++ = *p, --prec);
        else
            *t++ = '0';
        /*
         * if precision required or alternate flag set, add in a
         * decimal point.  If no digits yet, add in leading 0.
         */
        if (prec) {
            dotrim = 1;
            *t++ = '.';
        }
        else
            dotrim = 0;
        /* if requires more precision and some fraction left */
        if (fract) {
            if (prec) {
                    do {
                    fract = modf(fract * 10, &tmp);
                    *t++ = to_char((int)tmp);
                } while(!tmp && !expcnt);
                while (--prec && fract) {
                    fract = modf(fract * 10, &tmp);
                    *t++ = to_char((int)tmp);
                }
            }
            if (fract)
                startp = c_round(fract, (int *)NULL, startp,
                    t - 1, (char)0, signp);
        }
        /* alternate format, adds 0's for precision, else trim 0's */
        if (false)
            for (; prec--; *t++ = '0');
        else if (dotrim) {
            while (t > startp && *--t == '0');
            if (*t != '.')
                ++t;
        }
    }
    return (t - startp);
}

int ICACHE_FLASH_ATTR atob(u_int32_t *vp, char *p, int base) {
    u_quad_t v;

    if (base == 0)
        p = _getbase (p, &base);
    if (_atob (&v, p, base)) {
        *vp = v;
        return (1);
    }
    return (0);
}

void ICACHE_FLASH_ATTR dtoa(char *dbuf, rtype arg, int fmtch, int width, int prec) {
    char    buf[MAX_FCONVERSION+1], *cp;
    char    sign;
    int size;

    if( !_finite(arg) ) {
        if( _isNan(arg) )
            strcpy (dbuf, "NaN");
        else if( arg < 0)
            strcpy (dbuf, "-Infinity");
        else
            strcpy (dbuf, "Infinity");
        return;
    }

    if (prec == 0)
        prec = 6;
    else if (prec > MAX_FRACT)
        prec = MAX_FRACT;

    /* leave room for sign at start of buffer */
    cp = buf + 1;

    /*
     * cvt may have to round up before the "start" of
     * its buffer, i.e. ``intf("%.2f", (double)9.999);'';
     * if the first character is still NUL, it did.
     * softsign avoids negative 0 if _double < 0 but
     * no significant digits will be shown.
     */
    *cp = '\0';
    size = cvt (arg, prec, &sign, fmtch, cp, buf + sizeof(buf));
    if (*cp == '\0')
        cp++;

    if (sign)
        *--cp = sign, size++;

    cp[size] = 0;
    os_memcpy (dbuf, cp, size + 1);
}

char* ICACHE_FLASH_ATTR btoa(char *dst, u_int value, int base) {
    char buf[34], digit;
    int i, j, rem, neg;

    if (value == 0) {
        dst[0] = '0';
        dst[1] = 0;
        return (dst);
    }

    neg = 0;
    if (base == -10) {
        base = 10;
        if (value & (1L << 31)) {
            value = (~value) + 1;
            neg = 1;
        }
    }

    for (i = 0; value != 0; i++) {
        rem = value % base;
        value /= base;
        if (rem >= 0 && rem <= 9)
            digit = rem + '0';
        else if (rem >= 10 && rem <= 36)
            digit = (rem - 10) + 'a';
        buf[i] = digit;
    }

    buf[i] = 0;
    if (neg)
        strcat (buf, "-");

    /* reverse the string */
    for (i = 0, j = strlen (buf) - 1; j >= 0; i++, j--)
        dst[i] = buf[j];
    dst[i] = 0;
    return (dst);
}

char* ICACHE_FLASH_ATTR llbtoa(char *dst, u_quad_t value, int base) {
    char buf[66], digit;
    int i, j, rem, neg;

    if (value == 0) {
        dst[0] = '0';
        dst[1] = 0;
        return (dst);
    }

    neg = 0;
    if (base == -10) {
        base = 10;
        if (value & (1LL << 63)) {
            value = (~value) + 1;
            neg = 1;
        }
    }

    for (i = 0; value != 0; i++) {
        rem = value % base;
        value /= base;
        if (rem >= 0 && rem <= 9)
            digit = rem + '0';
        else if (rem >= 10 && rem <= 36)
            digit = (rem - 10) + 'a';
        buf[i] = digit;
    }

    buf[i] = 0;
    if (neg)
        strcat (buf, "-");

    /* reverse the string */
    for (i = 0, j = strlen (buf) - 1; j >= 0; i++, j--)
        dst[i] = buf[j];
    dst[i] = 0;
    return (dst);
}

int ICACHE_FLASH_ATTR vsprintf(char *d, const char *s, va_list ap) {
    const char *t;
    char *p, *dst, tmp[40];
    unsigned int n;
    int fmt, trunc, haddot, width, base, longlong;
    double dbl;

    dst = d;
    for (; *s;) {
        if (*s == '%') {
            s++;
            fmt = FMT_RJUST;
            width = trunc = haddot = longlong = 0;
            for (; *s; s++) {
                if (strchr("bcdefgilopPrRsuxX%", *s))
                    break;
                else if (*s == '-')
                    fmt = FMT_LJUST;
                else if (*s == '0')
                    fmt = FMT_RJUST0;
                else if (*s == '~')
                    fmt = FMT_CENTER;
                else if (*s == '*') {
                    if (haddot)
                        trunc = va_arg(ap, int);
                    else
                        width = va_arg(ap, int);
                } else if (*s >= '1' && *s <= '9') {
                    for (t = s; isdigit(*s); s++);
                    strncpy(tmp, t, s - t);
                    tmp[s - t] = '\0';
                    atob(&n, tmp, 10);
                    if (haddot)
                        trunc = n;
                    else
                        width = n;
                    s--;
                } else if (*s == '.')
                    haddot = 1;
            }
            if (*s == '%') {
                *d++ = '%';
                *d = 0;
            } else if (*s == 's') {
                p = va_arg(ap, char *);

                if (p)
                    strcpy(d, p);
                else
                    strcpy(d, "(null)");
            } else if (*s == 'c') {
                n = va_arg (ap, int);
                *d = n;
                d[1] = 0;
            } else {
                if (*s == 'l') {
                    if (*++s == 'l') {
                        longlong = 1;
                        ++s;
                    }
                }
                if (strchr("bdiopPrRxXu", *s)) {
                    if (*s == 'd' || *s == 'i')
                        base = -10;
                    else if (*s == 'u')
                        base = 10;
                    else if (*s == 'x' || *s == 'X')
                        base = 16;
                    else if(*s == 'p' || *s == 'P') {
                        base = 16;
                        if (*s == 'p') {
                            *d++ = '0';
                            *d++ = 'x';
                        }
                        fmt = FMT_RJUST0;
                        if (sizeof(long) > 4) {
                            width = 16;
                            longlong = 1;
                        } else {
                            width = 8;
                        }
                    }
                    else if(*s == 'r' || *s == 'R') {
                        base = 16;
                        if (*s == 'r') {
                            *d++ = '0';
                            *d++ = 'x';
                        }
                        fmt = FMT_RJUST0;
                        if (sizeof(register_t) > 4) {
                            width = 16;
                            longlong = 1;
                        } else {
                            width = 8;
                        }
                    }
                    else if (*s == 'o')
                        base = 8;
                    else if (*s == 'b')
                        base = 2;
                    if (longlong)
                        llbtoa(d, va_arg (ap, quad_t),
                            base);
                    else
                        btoa(d, va_arg (ap, int), base);

                    if (*s == 'X')
                        strtoupper(d);
                }
                else if (strchr ("eEfgG", *s)) {
void dtoa (char *dbuf, rtype arg, int fmtch, int width, int prec);
                    dbl = va_arg(ap, double);
                    dtoa(d, dbl, *s, width, trunc);
                    trunc = 0;
                }
            }
            if (trunc)
                d[trunc] = 0;
            if (width)
                str_fmt (d, width, fmt);
            for (; *d; d++);
            s++;
        } else
            *d++ = *s++;
    }
    *d = 0;
    return (d - dst);
}
