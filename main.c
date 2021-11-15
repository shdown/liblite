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

#include "lite.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define CALL_TEST(Expr_) \
    do { \
        (Expr_); \
        fprintf(stderr, "Passed: %s.\n", #Expr_); \
    } while (0)

#define CHECK(Expr_) check_impl((Expr_), #Expr_, __FILE__, __LINE__, __func__)

static void check_impl(bool result, const char *expr, const char *file, int line, const char *func)
{
    if (!result) {
        fprintf(stderr, "CHECK(%s) failed at %s:%d (function '%s').\n", expr, file, line, func);
        abort();
    }
}

//--------------------------------------------------------------------------------------------------

static void test_lite_memcpy_simple(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_memcpy(buf + 1, "@@!", 3);
    CHECK(ret == buf + 1);
    CHECK(strcmp(buf, "f@@!bar") == 0);
}

static void test_lite_memcpy_size0(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_memcpy(buf + 1, NULL, 0);
    CHECK(ret == buf + 1);
    CHECK(strcmp(buf, "foo_bar") == 0);
}

static void test_lite_memcpy_both_null(void)
{
    char *ret = lite_memcpy(NULL, NULL, 0);
    CHECK(ret == NULL);
}

static void test_lite_memmove_no_overlap(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_memmove(buf + 1, "@@!", 3);
    CHECK(ret == buf + 1);
    CHECK(strcmp(buf, "f@@!bar") == 0);
}

static void test_lite_memmove_overlap_1(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_memmove(buf + 2, buf + 1, 3);
    CHECK(ret == buf + 2);
    CHECK(strcmp(buf, "fooo_ar") == 0);
}

static void test_lite_memmove_overlap_2(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_memmove(buf + 1, buf + 2, 3);
    CHECK(ret == buf + 1);
    CHECK(strcmp(buf, "fo_bbar") == 0);
}

static void test_lite_memmove_full_overlap(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_memmove(buf + 1, buf + 1, 5);
    CHECK(ret == buf + 1);
    CHECK(strcmp(buf, "foo_bar") == 0);
}

static void test_lite_memmove_size0(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_memmove(buf + 1, NULL, 0);
    CHECK(ret == buf + 1);
    CHECK(strcmp(buf, "foo_bar") == 0);
}

static void test_lite_memmove_both_null(void)
{
    char *ret = lite_memmove(NULL, NULL, 0);
    CHECK(ret == NULL);
}

static void test_lite_memccpy_nul_1(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_memccpy(buf + 1, "quiz", '\0', 4);
    CHECK(ret == NULL);
    CHECK(strcmp(buf, "fquizar") == 0);
}

static void test_lite_memccpy_nul_2(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_memccpy(buf + 1, "quiz\0xx", '\0', 7);
    CHECK(ret == buf + 6);
    CHECK(strcmp(buf, "fquiz") == 0);
}

static void test_lite_memccpy_c_1(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_memccpy(buf + 1, "quiz", 'w', 4);
    CHECK(ret == NULL);
    CHECK(strcmp(buf, "fquizar") == 0);
}

static void test_lite_memccpy_c_2(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_memccpy(buf + 1, "quiz", 'u', 4);
    CHECK(ret == buf + 3);
    CHECK(strcmp(buf, "fqu_bar") == 0);
}

static void test_lite_memccpy_size0(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_memccpy(buf + 1, "quiz", 'u', 0);
    CHECK(ret == NULL);
    CHECK(strcmp(buf, "foo_bar") == 0);
}

static void test_lite_memccpy_both_null(void)
{
    char *ret = lite_memccpy(NULL, NULL, 'u', 0);
    CHECK(ret == NULL);
}

static void test_lite_memset_nul(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_memset(buf + 1, '\0', 5);
    CHECK(ret == buf + 1);
    CHECK(strcmp(buf, "f") == 0);
}

static void test_lite_memset_c(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_memset(buf + 1, '~', 5);
    CHECK(ret == buf + 1);
    CHECK(strcmp(buf, "f~~~~~r") == 0);
}

static void test_lite_memset_size0(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_memset(buf + 1, '~', 0);
    CHECK(ret == buf + 1);
    CHECK(strcmp(buf, "foo_bar") == 0);
}

static void test_lite_memset_null(void)
{
    char *ret = lite_memset(NULL, '~', 0);
    CHECK(ret == NULL);
}

static void test_lite_strcpy(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_strcpy(buf + 1, "riend");
    CHECK(ret == buf + 1);
    CHECK(strcmp(buf, "friend") == 0);
}

static void test_lite_strncpy_1(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_strncpy(buf + 1, "riendship", 7);
    CHECK(ret == buf + 1);
    CHECK(memcmp(buf, "friendsh", 8) == 0);
}

static void test_lite_strncpy_2(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_strncpy(buf + 1, "ix", 7);
    CHECK(ret == buf + 1);
    CHECK(memcmp(buf, "fix\0\0\0\0\0", 8) == 0);
}

static void test_lite_strncpy_size0_null(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_strncpy(buf + 1, NULL, 0);
    CHECK(ret == buf + 1);
    CHECK(strcmp(buf, "foo_bar") == 0);
}

static void test_lite_stpcpy(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_stpcpy(buf + 1, "riend");
    CHECK(ret == buf + 6);
    CHECK(strcmp(buf, "friend") == 0);
}

static void test_lite_stpncpy_1(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_stpncpy(buf + 1, "riendship", 7);
    CHECK(ret == buf + 8);
    CHECK(memcmp(buf, "friendsh", 8) == 0);
}

static void test_lite_stpncpy_2(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_stpncpy(buf + 1, "ix", 7);
    CHECK(ret == buf + 3);
    CHECK(memcmp(buf, "fix\0\0\0\0\0", 8) == 0);
}

static void test_lite_stpncpy_size0_null(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_stpncpy(buf + 1, NULL, 0);
    CHECK(ret == buf + 1);
    CHECK(strcmp(buf, "foo_bar") == 0);
}

static void test_lite_strlen(void)
{
    CHECK(lite_strlen("") == 0);
    CHECK(lite_strlen("a") == 1);
    CHECK(lite_strlen("calculate the length of the string") == 34);
}

static void test_lite_strnlen_1(void)
{
    CHECK(lite_strnlen("", 100) == 0);
    CHECK(lite_strnlen("a", 2) == 1);
    CHECK(lite_strnlen("calculate the length of the string", 600) == 34);
}

static void test_lite_strnlen_2(void)
{
    CHECK(lite_strnlen("xyz", 3) == 3);
    CHECK(lite_strnlen("foobar", 4) == 4);
    CHECK(lite_strnlen("foobar", 5) == 5);
}

static void test_lite_strnlen_size0_null(void)
{
    CHECK(lite_strnlen(NULL, 0) == 0);
}

static void test_lite_strcat(void)
{
    char buf[10] = "foo";
    char *ret = lite_strcat(buf, "+bar");
    CHECK(ret == buf);
    CHECK(strcmp(buf, "foo+bar") == 0);
}

static void test_lite_strncat_1(void)
{
    char buf[10] = "foo";
    char *ret = lite_strncat(buf, "+bar", 40);
    CHECK(ret == buf);
    CHECK(strcmp(buf, "foo+bar") == 0);
}

static void test_lite_strncat_2(void)
{
    char buf[10] = "foo";
    char *ret = lite_strncat(buf, "+bar+quiz", 4);
    CHECK(ret == buf);
    CHECK(strcmp(buf, "foo+bar") == 0);
}

static void test_lite_strncat_3(void)
{
    char buf[10] = "foo";
    char *ret = lite_strncat(buf, "+bar", 4);
    CHECK(ret == buf);
    CHECK(strcmp(buf, "foo+bar") == 0);
}

static void test_lite_strncat_size0_null(void)
{
    char buf[10] = "foo";
    char *ret = lite_strncat(buf, NULL, 0);
    CHECK(ret == buf);
    CHECK(strcmp(buf, "foo") == 0);
}

static void test_lite_memchr_found(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_memchr(buf, 'o', 7);
    CHECK(ret == buf + 1);
}

static void test_lite_memchr_notfound(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_memchr(buf, 'x', 7);
    CHECK(ret == NULL);
}

static void test_lite_memchr_size0_null(void)
{
    char *ret = lite_memchr(NULL, 'x', 0);
    CHECK(ret == NULL);
}

static void test_lite_rawmemchr_1(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_rawmemchr(buf, '\0');
    CHECK(ret == buf + 7);
}

static void test_lite_rawmemchr_2(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_rawmemchr(buf, 'o');
    CHECK(ret == buf + 1);
}

static void test_lite_memrchr_found(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_memrchr(buf, 'o', 7);
    CHECK(ret == buf + 2);
}

static void test_lite_memrchr_notfound(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_memrchr(buf, 'x', 7);
    CHECK(ret == NULL);
}

static void test_lite_memrchr_size0_null(void)
{
    char *ret = lite_memrchr(NULL, 'x', 0);
    CHECK(ret == NULL);
}

static void test_lite_memcmp_1(void)
{
    int ret = lite_memcmp("uwu", "zoo", 4);
    CHECK(ret == -1);
}

static void test_lite_memcmp_2(void)
{
    int ret = lite_memcmp("abc", "abc", 4);
    CHECK(ret == 0);
}

static void test_lite_memcmp_3(void)
{
    int ret = lite_memcmp("foo", "bar", 4);
    CHECK(ret == 1);
}

static void test_lite_memcmp_size0(void)
{
    int ret = lite_memcmp("foo", NULL, 0);
    CHECK(ret == 0);
}

static void test_lite_memcmp_size0_both_null(void)
{
    int ret = lite_memcmp(NULL, NULL, 0);
    CHECK(ret == 0);
}

static void test_lite_strcmp(const char *p, const char *q, int expected_ret)
{
    int ret = lite_strcmp(p, q);
    CHECK(ret == expected_ret);
}

static void test_lite_strncmp(const char *p, const char *q, size_t n, int expected_ret)
{
    int ret = lite_strncmp(p, q, n);
    CHECK(ret == expected_ret);
}

static void test_lite_strchr_found(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_strchr(buf, 'o');
    CHECK(ret == buf + 1);
}

static void test_lite_strchr_notfound(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_strchr(buf, 'x');
    CHECK(ret == NULL);
}

static void test_lite_strchrnul_found(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_strchrnul(buf, 'o');
    CHECK(ret == buf + 1);
}

static void test_lite_strchrnul_notfound(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_strchrnul(buf, 'x');
    CHECK(ret == buf + 7);
}

static void test_lite_strrchr_found(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_strrchr(buf, 'o');
    CHECK(ret == buf + 2);
}

static void test_lite_strrchr_notfound(void)
{
    char buf[] = "foo_bar";
    char *ret = lite_strrchr(buf, 'x');
    CHECK(ret == NULL);
}

static void test_lite_strcspn(const char *haystack, const char *needle, size_t expected_ret)
{
    size_t ret = lite_strcspn(haystack, needle);
    CHECK(ret == expected_ret);
}

static void test_lite_strpbrk(const char *haystack, const char *needle, int expected_offset)
{
    char *ret = lite_strpbrk(haystack, needle);
    const char *expected_ret = (expected_offset < 0 ? NULL : (haystack + expected_offset));
    CHECK(ret == expected_ret);
}

static void test_lite_strspn(const char *haystack, const char *needle, size_t expected_ret)
{
    size_t ret = lite_strspn(haystack, needle);
    CHECK(ret == expected_ret);
}

static void test_lite_strstr(const char *haystack, const char *needle, int expected_offset)
{
    char *ret = lite_strstr(haystack, needle);
    const char *expected_ret = (expected_offset < 0 ? NULL : (haystack + expected_offset));
    CHECK(ret == expected_ret);
}

static void test_lite_memmem(const char *haystack, const char *needle, int expected_offset)
{
    char *ret = lite_memmem(haystack, lite_strlen(haystack), needle, lite_strlen(needle));
    const char *expected_ret = (expected_offset < 0 ? NULL : (haystack + expected_offset));
    CHECK(ret == expected_ret);
}

static void test_lite_strstartswith(const char *s, const char *prefix, bool expected_ret)
{
    bool ret = lite_strstartswith(s, prefix);
    CHECK(ret == expected_ret);
}

static void test_lite_strtok_r_simple(void)
{
    char buf[] = "~~one     two three~four ~ five  ~~ ";
    static const char *expected_tokens[] = {"one", "two", "three", "four", "five", NULL};
    size_t i = 0;
    for (char *saveptr, *s, *tmp = buf; (s = lite_strtok_r(tmp, " ~", &saveptr)) != NULL; tmp = NULL) {
        const char *expected_token = expected_tokens[i++];
        CHECK(expected_token != NULL);
        CHECK(lite_strcmp(s, expected_token) == 0);
    }
    CHECK(expected_tokens[i] == NULL);
}

//--------------------------------------------------------------------------------------------------

int main()
{
    CALL_TEST(test_lite_memcpy_simple());
    CALL_TEST(test_lite_memcpy_size0());
    CALL_TEST(test_lite_memcpy_both_null());

    CALL_TEST(test_lite_memmove_no_overlap());
    CALL_TEST(test_lite_memmove_overlap_1());
    CALL_TEST(test_lite_memmove_overlap_2());
    CALL_TEST(test_lite_memmove_full_overlap());
    CALL_TEST(test_lite_memmove_size0());
    CALL_TEST(test_lite_memmove_both_null());

    CALL_TEST(test_lite_memccpy_nul_1());
    CALL_TEST(test_lite_memccpy_nul_2());
    CALL_TEST(test_lite_memccpy_c_1());
    CALL_TEST(test_lite_memccpy_c_2());
    CALL_TEST(test_lite_memccpy_size0());
    CALL_TEST(test_lite_memccpy_both_null());

    CALL_TEST(test_lite_memset_nul());
    CALL_TEST(test_lite_memset_c());
    CALL_TEST(test_lite_memset_size0());
    CALL_TEST(test_lite_memset_null());

    CALL_TEST(test_lite_strcpy());

    CALL_TEST(test_lite_strncpy_1());
    CALL_TEST(test_lite_strncpy_2());
    CALL_TEST(test_lite_strncpy_size0_null());

    CALL_TEST(test_lite_stpcpy());

    CALL_TEST(test_lite_stpncpy_1());
    CALL_TEST(test_lite_stpncpy_2());
    CALL_TEST(test_lite_stpncpy_size0_null());

    CALL_TEST(test_lite_strlen());

    CALL_TEST(test_lite_strnlen_1());
    CALL_TEST(test_lite_strnlen_2());
    CALL_TEST(test_lite_strnlen_size0_null());

    CALL_TEST(test_lite_strcat());

    CALL_TEST(test_lite_strncat_1());
    CALL_TEST(test_lite_strncat_2());
    CALL_TEST(test_lite_strncat_3());
    CALL_TEST(test_lite_strncat_size0_null());

    CALL_TEST(test_lite_memchr_found());
    CALL_TEST(test_lite_memchr_notfound());
    CALL_TEST(test_lite_memchr_size0_null());

    CALL_TEST(test_lite_rawmemchr_1());
    CALL_TEST(test_lite_rawmemchr_2());

    CALL_TEST(test_lite_memrchr_found());
    CALL_TEST(test_lite_memrchr_notfound());
    CALL_TEST(test_lite_memrchr_size0_null());

    CALL_TEST(test_lite_memcmp_1());
    CALL_TEST(test_lite_memcmp_2());
    CALL_TEST(test_lite_memcmp_3());
    CALL_TEST(test_lite_memcmp_size0());
    CALL_TEST(test_lite_memcmp_size0_both_null());

    CALL_TEST(test_lite_strcmp("", "", 0));
    CALL_TEST(test_lite_strcmp("x", "", 1));
    CALL_TEST(test_lite_strcmp("", "x", -1));
    CALL_TEST(test_lite_strcmp("y", "y", 0));
    CALL_TEST(test_lite_strcmp("foo", "bar", 1));
    CALL_TEST(test_lite_strcmp("uwu", "zoo", -1));
    CALL_TEST(test_lite_strcmp("foobar", "foo", 1));
    CALL_TEST(test_lite_strcmp("foo", "foobar", -1));

    CALL_TEST(test_lite_strncmp("", "", 0, 0));
    CALL_TEST(test_lite_strncmp("", "", 128, 0));
    CALL_TEST(test_lite_strncmp("x", "", 0, 0));
    CALL_TEST(test_lite_strncmp("x", "", 1, 1));
    CALL_TEST(test_lite_strncmp("x", "", 128, 1));
    CALL_TEST(test_lite_strncmp("", "x", 1, -1));
    CALL_TEST(test_lite_strncmp("", "x", 128, -1));
    CALL_TEST(test_lite_strncmp("y", "y", 0, 0));
    CALL_TEST(test_lite_strncmp("y", "y", 1, 0));
    CALL_TEST(test_lite_strncmp("y", "y", 2, 0));
    CALL_TEST(test_lite_strncmp("y", "y", 128, 0));
    CALL_TEST(test_lite_strncmp("foo", "bar", 0, 0));
    CALL_TEST(test_lite_strncmp("foo", "bar", 1, 1));
    CALL_TEST(test_lite_strncmp("foo", "bar", 128, 1));
    CALL_TEST(test_lite_strncmp("faa", "fee", 1, 0));
    CALL_TEST(test_lite_strncmp("faa", "fee", 2, -1));
    CALL_TEST(test_lite_strncmp("faa", "fee", 128, -1));
    CALL_TEST(test_lite_strncmp("uwu", "zoo", 3, -1));
    CALL_TEST(test_lite_strncmp("zoo", "uwu", 4, 1));
    CALL_TEST(test_lite_strncmp("foobar", "foo", 3, 0));
    CALL_TEST(test_lite_strncmp("foobar", "foo", 4, 1));
    CALL_TEST(test_lite_strncmp("foobar", "foo", 128, 1));
    CALL_TEST(test_lite_strncmp("foo", "foobar", 3, 0));
    CALL_TEST(test_lite_strncmp("foo", "foobar", 4, -1));
    CALL_TEST(test_lite_strncmp("foo", "foobar", 128, -1));

    CALL_TEST(test_lite_strchr_found());
    CALL_TEST(test_lite_strchr_notfound());

    CALL_TEST(test_lite_strchrnul_found());
    CALL_TEST(test_lite_strchrnul_notfound());

    CALL_TEST(test_lite_strrchr_found());
    CALL_TEST(test_lite_strrchr_notfound());

    CALL_TEST(test_lite_strcspn("haystack", "ayh", 0));
    CALL_TEST(test_lite_strcspn("haystack", "pawn", 1));
    CALL_TEST(test_lite_strcspn("haystack", "beef", 8));
    CALL_TEST(test_lite_strcspn("haystack", "ccckkckkkk$J", 6));
    CALL_TEST(test_lite_strcspn("haystack", "", 8));
    CALL_TEST(test_lite_strcspn("", "", 0));

    CALL_TEST(test_lite_strpbrk("haystack", "ayh", 0));
    CALL_TEST(test_lite_strpbrk("haystack", "pawn", 1));
    CALL_TEST(test_lite_strpbrk("haystack", "beef", -1));
    CALL_TEST(test_lite_strpbrk("haystack", "ccckkckkkk$J", 6));
    CALL_TEST(test_lite_strpbrk("haystack", "", -1));
    CALL_TEST(test_lite_strpbrk("", "", -1));

    CALL_TEST(test_lite_strspn("haystack", "ayh", 3));
    CALL_TEST(test_lite_strspn("haystack", "pawn", 0));
    CALL_TEST(test_lite_strspn("haystack", "beef", 0));
    CALL_TEST(test_lite_strspn("haystack", "haystick", 8));
    CALL_TEST(test_lite_strspn("haystack", "", 0));
    CALL_TEST(test_lite_strspn("", "", 0));

    CALL_TEST(test_lite_strstr("haystack", "hay", 0));
    CALL_TEST(test_lite_strstr("haystack", "ay", 1));
    CALL_TEST(test_lite_strstr("haystack", "a", 1));
    CALL_TEST(test_lite_strstr("haystack", "sta", 3));
    CALL_TEST(test_lite_strstr("haystack", "stack", 3));
    CALL_TEST(test_lite_strstr("haystack", "stack_", -1));
    CALL_TEST(test_lite_strstr("haystack", "haystack", 0));
    CALL_TEST(test_lite_strstr("haystack", "haystick", -1));
    CALL_TEST(test_lite_strstr("hay", "haystack", -1));
    CALL_TEST(test_lite_strstr("hay", "", 0));
    CALL_TEST(test_lite_strstr("", "", 0));

    CALL_TEST(test_lite_memmem("haystack", "hay", 0));
    CALL_TEST(test_lite_memmem("haystack", "ay", 1));
    CALL_TEST(test_lite_memmem("haystack", "a", 1));
    CALL_TEST(test_lite_memmem("haystack", "sta", 3));
    CALL_TEST(test_lite_memmem("haystack", "stack", 3));
    CALL_TEST(test_lite_memmem("haystack", "stack_", -1));
    CALL_TEST(test_lite_memmem("haystack", "haystack", 0));
    CALL_TEST(test_lite_memmem("haystack", "haystick", -1));
    CALL_TEST(test_lite_memmem("hay", "haystack", -1));
    CALL_TEST(test_lite_memmem("hay", "", 0));
    CALL_TEST(test_lite_memmem("", "", 0));

    CALL_TEST(test_lite_strstartswith("foo", "bar", false));
    CALL_TEST(test_lite_strstartswith("foo", "foobar", false));
    CALL_TEST(test_lite_strstartswith("foobar", "foo", true));
    CALL_TEST(test_lite_strstartswith("foobar", "foobar", true));
    CALL_TEST(test_lite_strstartswith("", "", true));
    CALL_TEST(test_lite_strstartswith("foo", "", true));
    CALL_TEST(test_lite_strstartswith("", "foo", false));

    CALL_TEST(test_lite_strtok_r_simple());

    fprintf(stderr, "All tests passed!\n");

    return 0;
}
