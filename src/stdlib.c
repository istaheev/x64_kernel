#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <stdlib.h>

void * memchr( const void * s, int c, size_t n )
{
    const unsigned char * p = (const unsigned char *) s;
    while ( n-- )
    {
        if ( *p == (unsigned char) c )
        {
            return (void *) p;
        }
        ++p;
    }
    return NULL;
}
void * memcpy( void * s1, const void * s2, size_t n )
{
    char * dest = (char *) s1;
    const char * src = (const char *) s2;
    while ( n-- )
    {
        *dest++ = *src++;
    }
    return s1;
}

void * memmove( void * s1, const void * s2, size_t n )
{
    char * dest = (char *) s1;
    const char * src = (const char *) s2;
    if ( dest <= src )
    {
        while ( n-- )
        {
            *dest++ = *src++;
        }
    }
    else
    {
        src += n;
        dest += n;
        while ( n-- )
        {
            *--dest = *--src;
        }
    }
    return s1;
}

void * memset( void * s, int c, size_t n )
{
    unsigned char * p = (unsigned char *) s;
    while ( n-- )
    {
        *p++ = (unsigned char) c;
    }
    return s;
}

char * strchr( const char * s, int c )
{
    do
    {
        if ( *s == (char) c )
        {
            return (char *) s;
        }
    } while ( *s++ );
    return NULL;
}

size_t strlen( const char * s )
{
    size_t rc = 0;
    while ( s[rc] )
    {
        ++rc;
    }
    return rc;
}

static
int _lltostr ( long long value, char * str, int base, int padding, char fill )
{
    char* ptr;
    char* low;
    int count = 0;

    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return 0;
    }
    
    ptr = str;
    
    // Set '-' for negative decimals.
    if ( value < 0 )
    {
        *ptr++ = '-';
        value = -value;
    }
    
    // Remember where the numbers start.
    low = ptr;
    
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[value % base];
        value /= base;
        count++;
    } while ( value );
    
    if ( padding > 0 )
    {
        while ( count < padding )
        {
            *ptr++ = fill;
            count++;
        }
    }

    // Terminating the string.
    *ptr-- = '\0';
    
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    
    return count;
}

static
int _ulltostr( unsigned long long value, char* str, int base, int padding, char fill )
{
    char* ptr;
    char* low;
    int count = 0;

    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return 0;
    }
    
    // Remember where the numbers start.
    low = ptr = str;
    
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[value % base];
        value /= base;
        count++;
    } while ( value );
    
    if ( padding > 0 )
    {
        while ( count < padding )
        {
            *ptr++ = fill;
            count++;
        }
    }

    // Terminating the string.
    *ptr-- = '\0';
    
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    
    return count;
}

char* itostr( int value, char* str, int base )
{
    _lltostr( (long long)value, str, base, 0, ' ' );
    return str;
}

char* ltostr( long value, char* str, int base )
{
    _lltostr( (long long)value, str, base, 0, ' ' );
    return str;
}

char* ultostr( unsigned long value, char* str, int base )
{
    _ulltostr ( (unsigned long long)value, str, base, 0, ' ' );
    return str;
}

bool isdigit ( char c )
{
    return c >= '0' && c <= '9';
}