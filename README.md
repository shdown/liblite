`<string.h>` optimized for N ≈ 0
===

`liblite` is a header-only C library that implements string-related functions from `<string.h>` in the dumbest way possible.

It might be of use if you mess with really small strings and need maximum performance. In those circumstances, it wins against glibc’s `<string.h>` because:
  * it is header-only, so all functions are ready to be inlined — no function call overhead;
  * the “dumbest way possible” approach means small code footprint, which makes it inline-friendly and instruction cache-friendly;
  * it does not do manual loop unrolling — no size checks overhead. Your compiler may still decide to unroll some loops automatically, though.

It does not implement:
  * functions related to [C locales](https://github.com/mpv-player/mpv/commit/1e70e82baa9193f6f027338b0fab0f5078971fbe);
  * functions related to dynamic memory allocation (`strdup`/`strndup`);
  * functions related to system-specific error/signal names (`strerror` and co., `strsignal`);
  * functions that are marked as “LEGACY” in POSIX.1-2001 and removed in POSIX.1-2008 (e.g. `index`, `rindex`, `bcmp`, `bcopy`, `bzero`);
  * functions that are not thread-safe (`strtok`; note that `strtok_r` *is* implemented).

Licence
===

`liblite` is licensed under LGPL-3.0+. See `COPYLING.LESSER.txt` for more details.
