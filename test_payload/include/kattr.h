#ifndef INCLUDE_ATTR_H_
#define INCLUDE_ATTR_H_

#define ATTR_SMAIN_TEXT __attribute__((section(".smaintext")))
#define ATTR_SMAIN_DATA __attribute__((section(".smaindata")))

#define ATTR_SLIB_TEXT __attribute__((section(".slibtext")))
#define ATTR_SLIB_DATA __attribute__((section(".slibdata")))

#define ATTR_SFREEZONE_TEXT __attribute__((section(".sfreezonetext")))
#define ATTR_SFREEZONE_DATA __attribute__((section(".sfreezonedata")))

#endif