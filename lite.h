/*
 * Copyright (C) 2021  liblite developers
 *
 * This file is part of liblite.
 *
 * liblite is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liblite is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with liblite.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define LITE_INHEADER static inline __attribute__((unused))

// Prevent the compiler from replacing the whole thing with a call to a function
// from <string.h>, vectorizing the loop, or emitting rep* instructions (rep*
// have significant startup overhead).
#define LITE_COMPILER_BARRIER() __asm__ volatile ("" ::: "memory")

LITE_INHEADER void *lite_memcpy_fw(void *dst, const void *src, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        ((char *) dst)[i] = ((const char *) src)[i];
        LITE_COMPILER_BARRIER();
    }
    return dst;
}

LITE_INHEADER void *lite_memcpy_bw(void *dst, const void *src, size_t n)
{
    while (n) {
        --n;
        ((char *) dst)[n] = ((const char *) src)[n];
        LITE_COMPILER_BARRIER();
    }
    return dst;
}

#define lite_memcpy lite_memcpy_fw

LITE_INHEADER void *lite_memmove(void *dst, const void *src, size_t n)
{
    uintptr_t dst_i = (uintptr_t) dst;
    uintptr_t src_i = (uintptr_t) src;
    if (dst_i < src_i) {
        lite_memcpy_fw(dst, src, n);
    } else if (dst_i > src_i) {
        lite_memcpy_bw(dst, src, n);
    }
    return dst;
}

LITE_INHEADER void *lite_memccpy(void *dst, const void *src, char c, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        if ((((char *) dst)[i] = ((const char *) src)[i]) == c) {
            return ((char *) dst) + i + 1;
        }
        LITE_COMPILER_BARRIER();
    }
    return NULL;
}

LITE_INHEADER void *lite_memset(void *p, char c, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        ((char *) p)[i] = c;
        LITE_COMPILER_BARRIER();
    }
    return p;
}

LITE_INHEADER char *lite_strcpy(char *dst, const char *src)
{
    size_t i = 0;
    while ((dst[i] = src[i]) != '\0') {
        ++i;
        LITE_COMPILER_BARRIER();
    }
    return dst;
}

LITE_INHEADER char *lite_strncpy(char *dst, const char *src, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        if ((dst[i] = src[i]) == '\0') {
            ++i;
            lite_memset(dst + i, '\0', n - i);
            break;
        }
        LITE_COMPILER_BARRIER();
    }
    return dst;
}

LITE_INHEADER char *lite_stpcpy(char *dst, const char *src)
{
    size_t i = 0;
    while ((dst[i] = src[i]) != '\0') {
        ++i;
        LITE_COMPILER_BARRIER();
    }
    return dst + i;
}

LITE_INHEADER char *lite_stpncpy(char *dst, const char *src, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        if ((dst[i] = src[i]) == '\0') {
            lite_memset(dst + i + 1, '\0', n - i - 1);
            return dst + i;
        }
        LITE_COMPILER_BARRIER();
    }
    return dst + n;
}

LITE_INHEADER size_t lite_strlen(const char *s)
{
    size_t i = 0;
    while (s[i] != '\0') {
        ++i;
        LITE_COMPILER_BARRIER();
    }
    return i;
}

LITE_INHEADER size_t lite_strnlen(const char *s, size_t n)
{
    size_t i = 0;
    while (i < n && s[i] != '\0') {
        ++i;
        LITE_COMPILER_BARRIER();
    }
    return i;
}

LITE_INHEADER char *lite_strcat(char *dst, const char *src)
{
    lite_strcpy(dst + lite_strlen(dst), src);
    return dst;
}

LITE_INHEADER char *lite_strncat(char *dst, const char *src, size_t n)
{
    char *p = dst + lite_strlen(dst);
    size_t i = 0;
    for (; i < n; ++i) {
        char c = src[i];
        if (c == '\0') {
            break;
        }
        p[i] = c;
        LITE_COMPILER_BARRIER();
    }
    p[i] = '\0';
    return dst;
}

LITE_INHEADER void *lite_memchr(const void *p, char c, size_t n)
{
    const char *sp = p;
    const char *sp_end = sp + n;
    for (; sp != sp_end; ++sp) {
        if (*sp == c) {
            return (void *) sp;
        }
        LITE_COMPILER_BARRIER();
    }
    return NULL;
}

LITE_INHEADER void *lite_rawmemchr(const void *p, char c)
{
    for (const char *sp = p; ; ++sp) {
        if (*sp == c) {
            return (void *) sp;
        }
        LITE_COMPILER_BARRIER();
    }
}

LITE_INHEADER void *lite_memrchr(const void *p, char c, size_t n)
{
    const char *sp = p;
    const char *sp_end = sp + n;
    while (sp_end != sp) {
        --sp_end;
        if (*sp_end == c) {
            return (void *) sp_end;
        }
        LITE_COMPILER_BARRIER();
    }
    return NULL;
}

LITE_INHEADER int lite_memcmp(const void *p, const void *q, size_t n)
{
    const char *sp = p;
    const char *sq = q;
    for (size_t i = 0; i < n; ++i) {
        unsigned char cp = sp[i];
        unsigned char cq = sq[i];
        if (cp != cq) {
            return cp < cq ? -1 : 1;
        }
        LITE_COMPILER_BARRIER();
    }
    return 0;
}

LITE_INHEADER int lite_strcmp(const char *p, const char *q)
{
    for (size_t i = 0; ; ++i) {
        unsigned char cp = p[i];
        if (cp == '\0') {
            return q[i] == '\0' ? 0 : -1;
        }
        unsigned char cq = q[i];
        if (cp != cq) {
            return cp < cq ? -1 : 1;
        }
        LITE_COMPILER_BARRIER();
    }
}

LITE_INHEADER int lite_strncmp(const char *p, const char *q, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        unsigned char cp = p[i];
        if (cp == '\0') {
            return q[i] == '\0' ? 0 : -1;
        }
        unsigned char cq = q[i];
        if (cp != cq) {
            return cp < cq ? -1 : 1;
        }
        LITE_COMPILER_BARRIER();
    }
    return 0;
}

LITE_INHEADER char *lite_strchr(const char *s, char c)
{
    for (;; ++s) {
        char cs = *s;
        if (cs == c) {
            return (char *) s;
        } else if (cs == '\0') {
            return NULL;
        }
        LITE_COMPILER_BARRIER();
    }
}

LITE_INHEADER char *lite_strchrnul(const char *s, char c)
{
    for (;; ++s) {
        char cs = *s;
        if (cs == c || cs == '\0') {
            return (char *) s;
        }
        LITE_COMPILER_BARRIER();
    }
}

LITE_INHEADER char *lite_strrchr(const char *s, char c)
{
    const char *last = NULL;
    for (;; ++s) {
        char cs = *s;
        if (cs == '\0') {
            return (char *) last;
        } else if (cs == c) {
            last = s;
        }
        LITE_COMPILER_BARRIER();
    }
}

LITE_INHEADER size_t lite_strcspn(const char *haystack, const char *needle)
{
    for (size_t i = 0; ; ++i) {
        if (lite_strchr(needle, haystack[i]) != NULL) {
            return i;
        }
        LITE_COMPILER_BARRIER();
    }
}

LITE_INHEADER char *lite_strpbrk(const char *haystack, const char *needle)
{
    for (;; ++haystack) {
        char c = *haystack;
        if (c == '\0') {
            return NULL;
        } else if (lite_strchr(needle, c) != NULL) {
            return (char *) haystack;
        }
        LITE_COMPILER_BARRIER();
    }
}

LITE_INHEADER size_t lite_strspn(const char *haystack, const char *needle)
{
    for (size_t i = 0;; ++i) {
        char c = haystack[i];
        if (c == '\0' || lite_strchr(needle, c) == NULL) {
            return i;
        }
        LITE_COMPILER_BARRIER();
    }
}

LITE_INHEADER bool lite_strstartswith(const char *s, const char *prefix)
{
    for (size_t i = 0; ; ++i) {
        char c = prefix[i];
        if (c == '\0') {
            return true;
        }
        if (c != s[i]) {
            return false;
        }
        LITE_COMPILER_BARRIER();
    }
}

LITE_INHEADER char *lite_strstr(const char *haystack, const char *needle)
{
    char needle0 = *needle;
    if (needle0 == '\0') {
        return (char *) haystack;
    }
    for (;; ++haystack) {
        char c = *haystack;
        if (c == '\0') {
            return NULL;
        }
        if (c == needle0 && lite_strstartswith(haystack + 1, needle + 1)) {
            return (char *) haystack;
        }
        LITE_COMPILER_BARRIER();
    }
}

LITE_INHEADER void *lite_memmem(const void *haystack, size_t nhaystack, const void *needle, size_t nneedle)
{
    if (nneedle > nhaystack) {
        return NULL;
    }
    size_t maxpos = nhaystack - nneedle;
    for (size_t i = 0; i <= maxpos; ++i) {
        const char *s = ((const char *) haystack) + i;
        if (lite_memcmp(s, needle, nneedle) == 0) {
            return (void *) s;
        }
        LITE_COMPILER_BARRIER();
    }
    return NULL;
}

LITE_INHEADER char *lite_strtok_r(char *s, const char *delim, char **saveptr)
{
    if (!s) {
        s = *saveptr;
    }

    // Skip any delimiters.
    s += lite_strspn(s, delim);

    // Skip to the next delimiter, or to the end of the string.
    char *p = s + lite_strcspn(s, delim);
    if (*p == '\0') {
        // No next delimiter, '*p' points to the end of the string.
        if (p == s) {
            return NULL;
        } else {
            *saveptr = p;
            return s;
        }
    } else {
        // '*p' points to the next delimiter, not the end of the string.
        *p = '\0';
        *saveptr = p + 1;
        return s;
    }
}
