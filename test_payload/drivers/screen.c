#include <drivers/screen.h>
#include <arch/common.h>
#include <kio.h>
#include <os/string.h>

#define SCREEN_WIDTH    80
#define SCREEN_HEIGHT   50

int ATTR_SFREEZONE_DATA screen_cursor_x;
int ATTR_SFREEZONE_DATA screen_cursor_y;

/* screen buffer */
char ATTR_SFREEZONE_DATA new_screen[SCREEN_HEIGHT * SCREEN_WIDTH] = {0};
char ATTR_SFREEZONE_DATA old_screen[SCREEN_HEIGHT * SCREEN_WIDTH] = {0};

/* cursor position */
void ATTR_SFREEZONE_TEXT vt100_move_cursor(int x, int y)
{
    // \033[y;xH
    dasics_smaincall(SMAINCALL_DISABLE_PREEMPT, 0, 0, 0);
    printk("%c[%d;%dH", 27, y, x);
    dasics_smaincall(SMAINCALL_STORE_CURSOR, (uint64_t)x, (uint64_t)y, 0);
    dasics_smaincall(SMAINCALL_ENABLE_PREEMPT, 0, 0, 0);
}

/* clear screen */
static void ATTR_SFREEZONE_TEXT vt100_clear()
{
    // \033[2J
    printk("%c[2J", 27);
}

/* write a char */
static void ATTR_SFREEZONE_TEXT screen_write_ch(char ch)
{
    if (ch == '\n')
    {
        screen_cursor_x = 1;
        screen_cursor_y++;
    }
    else
    {
        new_screen[(screen_cursor_y - 1) * SCREEN_WIDTH + (screen_cursor_x - 1)] = ch;
        screen_cursor_x++;
    }
    dasics_smaincall(SMAINCALL_STORE_CURSOR, (uint64_t)screen_cursor_x, (uint64_t)screen_cursor_y, 0);
}

void ATTR_SLIB_TEXT init_screen(void)
{
    // vt100_hidden_cursor();
    vt100_clear();
}

void ATTR_SFREEZONE_TEXT screen_clear(void)
{
    int i, j;
    for (i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (j = 0; j < SCREEN_WIDTH; j++)
        {
            new_screen[i * SCREEN_WIDTH + j] = ' ';
        }
    }
    screen_cursor_x = 1;
    screen_cursor_y = 1;
    screen_reflush();
}

void ATTR_SFREEZONE_TEXT screen_move_cursor(int x, int y)
{
    screen_cursor_x = x;
    screen_cursor_y = y;
    dasics_smaincall(SMAINCALL_STORE_CURSOR, (uint64_t)screen_cursor_x, (uint64_t)screen_cursor_y, 0);
}

void ATTR_SFREEZONE_TEXT screen_write(char *buff)
{
    int i = 0;
    int l = kstrlen(buff);

    for (i = 0; i < l; i++)
    {
        screen_write_ch(buff[i]);
    }
}

/*
 * This function is used to print the serial port when the clock
 * interrupt is triggered. However, we need to pay attention to
 * the fact that in order to speed up printing, we only refresh
 * the characters that have been modified since this time.
 */
void ATTR_SFREEZONE_TEXT screen_reflush(void)
{
    int i, j;

    /* here to reflush screen buffer to serial port */
    for (i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (j = 0; j < SCREEN_WIDTH; j++)
        {
            /* We only print the data of the modified location. */
            if (new_screen[i * SCREEN_WIDTH + j] != old_screen[i * SCREEN_WIDTH + j])
            {
                vt100_move_cursor(j + 1, i + 1);
                dasics_smaincall(SMAINCALL_WRITE_CH, new_screen[i * SCREEN_WIDTH + j], 0, 0);
                old_screen[i * SCREEN_WIDTH + j] = new_screen[i * SCREEN_WIDTH + j];
            }
        }
    }

    /* recover cursor position */
    vt100_move_cursor(screen_cursor_x, screen_cursor_y);
}
