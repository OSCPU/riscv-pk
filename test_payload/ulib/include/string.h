#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stddasics.h>

extern void ATTR_UFREEZONE_TEXT memcpy(uint8_t *dest, const uint8_t *src, uint32_t len);
extern void ATTR_UFREEZONE_TEXT memset(void *dest, uint8_t val, uint32_t len);

extern int ATTR_UFREEZONE_TEXT strcmp(const char *str1, const char *str2);
extern char *ATTR_UFREEZONE_TEXT strcpy(char *dest, const char *src);
extern char *ATTR_UFREEZONE_TEXT strcat(char *dest, const char *src);
extern int ATTR_UFREEZONE_TEXT strlen(const char *src);

#endif /* STRING_H */
