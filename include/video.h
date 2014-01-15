#ifndef __VIDEO_H__
#define __VIDEO_H__ 1

void video_init ();
void video_clear_screen ();
void video_putchar ( char c );
void video_putstr ( const char* s );

#endif