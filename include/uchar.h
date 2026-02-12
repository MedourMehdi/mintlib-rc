#ifndef _UCHAR_H
#define _UCHAR_H

#ifdef __MINT__
#error "uchar functions not implemented. See: https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/uchar.h.html"
#endif

__BEGIN_DECLS

#if __cplusplus < 201103L
typedef unsigned short char16_t;
typedef unsigned char32_t;
#else
#include <stdint.h> /* for char16_t and char32_t */
#endif

#define __NEED_mbstate_t
#define __NEED_size_t

#include <features.h>
#include <wchar.h> /* for mbstate_t */
#include <stddef.h> /* for size_t */

size_t c16rtomb(char *__restrict, char16_t, mbstate_t *__restrict);
size_t mbrtoc16(char16_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict);

size_t c32rtomb(char *__restrict, char32_t, mbstate_t *__restrict);
size_t mbrtoc32(char32_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict);

__END_DECLS

#endif /* !_UCHAR_H */