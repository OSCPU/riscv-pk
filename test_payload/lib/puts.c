#include "mcall.h"
#include "sbi.h"

void _puts(char *str)
{
    sbi_console_putstr(str);
}
