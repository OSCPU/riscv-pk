#ifndef _STDATTR_H_
#define _STDATTR_H_

#define ATTR_UMAIN_TEXT __attribute__((section(".umaintext")))
#define ATTR_UMAIN_DATA __attribute__((section(".umaindata")))

#define ATTR_ULIB_TEXT __attribute__((section(".ulibtext")))
#define ATTR_ULIB_DATA __attribute__((section(".ulibdata")))

#define ATTR_UFREEZONE_TEXT __attribute__((section(".ufreezonetext")))
#define ATTR_UFREEZONE_DATA __attribute__((section(".ufreezonedata")))

#endif