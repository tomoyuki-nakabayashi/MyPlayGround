#include <stdarg.h>

int dec2asc (char *str, int dec) {
    int len = 0, len_buf;
    int buf[10];
    if (dec < 0) {
        *(str++) = '-';
        len++;
        dec = -dec;
    }
    while (1) {
        buf[len++] = dec % 10;
        if (dec < 10) break;
        dec /= 10;
    }
    len_buf = len;
    while (len) {
        *(str++) = buf[--len] + 0x30;
    }
    return len_buf;
}

int hex2asc (char *str, int dec) {
    int len = 0, len_buf;
    int buf[10];
    while (1) {
        buf[len++] = dec % 16;
        if (dec < 16) break;
        dec /= 16;
    }
    len_buf = len;
    while (len) {
        len --;
        *(str++) = (buf[len]<10)?(buf[len] + 0x30):(buf[len] - 9 + 0x60);
    }
    return len_buf;
}
 
void sprintf (char *str, char *fmt, ...) {
    va_list list;
    va_start (list, 2);
 
    while (*fmt) {
        if(*fmt=='%') {
            fmt++;
            int len = 0;
            switch(*fmt){
                case 'd':
                    len = dec2asc(str, va_arg (list, int));
                    break;
                case 'x':
                    len = hex2asc(str, va_arg (list, int));
                    break;
            }
            str += len; fmt++;
        } else {
            *(str++) = *(fmt++);
        }   
    }
    *str = 0x00;  // null-terminate
    va_end (list);
}