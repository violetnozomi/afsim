// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  \file   buffer.cpp
 *  \author Kenneth R. Sewell III
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 */

#include <stdio.h>
#include <iadsLib/buffer.hpp>
#include <cstring>

/**
    Reads <size> number of bytes from buffer(in big endian)
    into x(same endianess as machine).
    Returns bytes successfully read
*/
unsigned short il::bufferRead(
    const unsigned short &size,
    void *x,
    const void *buffer
    )
{
#if BYTE_ORDER == LITTLE_ENDIAN
    for( unsigned int i=0; i<size; ++i )
    {
        ((char *)x)[i] = ((char *)buffer)[size -1 - i];
    }
#else
    memcpy(x, buffer, size);
#endif

    return size;
}

/**
    Write <size> number of bytes from x(same endianess as machine)
    into buffer(in big endian).
    Returns bytes successfully written
*/
unsigned short il::bufferWrite(
    const unsigned short &size,
    const void *x,
    void *buffer
    )
{
#if BYTE_ORDER == LITTLE_ENDIAN
    for( unsigned int i = 0; i<size; ++i )
    {
        ((char *)buffer)[size -1 - i] = ((char *)x)[i];
    }
#else
    memcpy(buffer, x, size);
#endif

    return size;
}


/**
   Buffer read/write for chars
 */
unsigned short il::bufferRead( char &x, const char *buffer )
{
    memcpy(&x, buffer, 1);
    return 1;
}

unsigned short il::bufferWrite( const char &x, char *buffer )
{
    memcpy(buffer, &x, 1);
    return 1;
}

unsigned short il::bufferRead( unsigned char &x, const char *buffer )
{
    memcpy(&x, buffer, 1);
    return 1;
}

unsigned short il::bufferWrite( const unsigned char &x, char *buffer )
{
    memcpy(buffer, &x, 1);
    return 1;
}

/**
   Buffer read/write for fixed-length strings
*/

unsigned short il::bufferRead(  char *x, std::size_t len, const char *buffer )
{
   memcpy(x, buffer, len);
   return static_cast<unsigned short>(len);
}

unsigned short il::bufferWrite( const char *x, std::size_t len, char *buffer )
{
   memcpy(buffer, x, len);
   return static_cast<unsigned short>(len);
}

/**
   Buffer read/write for shorts
*/
unsigned short il::bufferRead( short &x, const char *buffer )
{

#if BYTE_ORDER == LITTLE_ENDIAN
    char *X = (char *)&x;

    X[0] = buffer[1];
    X[1] = buffer[0];

#else
    memcpy(&x, buffer, 2);
#endif

    return 2;
}

unsigned short il::bufferWrite( const short &x, char *buffer )
{
#if BYTE_ORDER == LITTLE_ENDIAN
    char *X = (char *)&x;

    buffer[0] = X[1];
    buffer[1] = X[0];

#else
    memcpy(buffer, &x, 2);
#endif

    return 2;
}

unsigned short il::bufferRead( unsigned short &x, const char *buffer )
{

#if BYTE_ORDER == LITTLE_ENDIAN
    char *X = (char *)&x;

    X[0] = buffer[1];
    X[1] = buffer[0];

#else
    memcpy(&x, buffer, 2);
#endif

    return 2;
}

unsigned short il::bufferWrite( const unsigned short &x, char *buffer )
{
#if BYTE_ORDER == LITTLE_ENDIAN
    char *X = (char *)&x;

    buffer[0] = X[1];
    buffer[1] = X[0];

#else
    memcpy(buffer, &x, 2);
#endif

    return 2;
}

/**
   Buffer read/write for integers
*/
unsigned short il::bufferRead( int &x, const char *buffer )
{

#if BYTE_ORDER == LITTLE_ENDIAN
    char *X = (char *)&x;

    X[0] = buffer[3];
    X[1] = buffer[2];
    X[2] = buffer[1];
    X[3] = buffer[0];

#else
    memcpy(&x, buffer, 4);
#endif

    return 4;
}

unsigned short il::bufferWrite( const int &x, char *buffer )
{
#if BYTE_ORDER == LITTLE_ENDIAN
    char *X = (char *)&x;

    buffer[0] = X[3];
    buffer[1] = X[2];
    buffer[2] = X[1];
    buffer[3] = X[0];

#else
    memcpy(buffer, &x, 4);
#endif

    return 4;
}

unsigned short il::bufferRead( unsigned int &x, const char *buffer )
{

#if BYTE_ORDER == LITTLE_ENDIAN
    char *X = (char *)&x;

    X[0] = buffer[3];
    X[1] = buffer[2];
    X[2] = buffer[1];
    X[3] = buffer[0];

#else
    memcpy(&x, buffer, 4);
#endif

    return 4;
}

unsigned short il::bufferWrite( const unsigned int &x, char *buffer )
{
#if BYTE_ORDER == LITTLE_ENDIAN
    char *X = (char *)&x;

    buffer[0] = X[3];
    buffer[1] = X[2];
    buffer[2] = X[1];
    buffer[3] = X[0];

#else
    memcpy(buffer, &x, 4);
#endif

    return 4;
}



/**
   Buffer read/write for floats
*/
unsigned short il::bufferRead( float &x, const char *buffer )
{

#if BYTE_ORDER == LITTLE_ENDIAN
    char *X = (char *)&x;

    X[0] = buffer[3];
    X[1] = buffer[2];
    X[2] = buffer[1];
    X[3] = buffer[0];

#else
    memcpy(&x, buffer, 4);
#endif

    return 4;
}

unsigned short il::bufferWrite( const float &x, char *buffer )
{
#if BYTE_ORDER == LITTLE_ENDIAN
    char *X = (char *)&x;

    buffer[0] = X[3];
    buffer[1] = X[2];
    buffer[2] = X[1];
    buffer[3] = X[0];

#else
    memcpy(buffer, &x, 4);
#endif

    return 4;
}



/**
   Buffer read/write for doubles
*/
unsigned short il::bufferRead( double &x, const char *buffer )
{
#if BYTE_ORDER == LITTLE_ENDIAN
    char *X = (char *)&x;

    X[0] = buffer[7];
    X[1] = buffer[6];
    X[2] = buffer[5];
    X[3] = buffer[4];
    X[4] = buffer[3];
    X[5] = buffer[2];
    X[6] = buffer[1];
    X[7] = buffer[0];
#else
    memcpy(&x, buffer, 8);
#endif

    return 8;
}

unsigned short il::bufferWrite( const double &x, char *buffer )
{
#if BYTE_ORDER == LITTLE_ENDIAN
    char *X = (char *)&x;

    buffer[0] = X[7];
    buffer[1] = X[6];
    buffer[2] = X[5];
    buffer[3] = X[4];
    buffer[4] = X[3];
    buffer[5] = X[2];
    buffer[6] = X[1];
    buffer[7] = X[0];
#else
    memcpy(buffer, &x, 8);
#endif

    return 8;
}

/**
   Buffer read/write for int64
*/
unsigned short il::bufferRead( int64_t &x, const char *buffer )
{
#if BYTE_ORDER == LITTLE_ENDIAN
    char *X = (char *)&x;

    X[0] = buffer[7];
    X[1] = buffer[6];
    X[2] = buffer[5];
    X[3] = buffer[4];
    X[4] = buffer[3];
    X[5] = buffer[2];
    X[6] = buffer[1];
    X[7] = buffer[0];
#else
    memcpy(&x, buffer, 8);
#endif

    return 8;
}

unsigned short il::bufferWrite( const int64_t &x, char *buffer )
{
#if BYTE_ORDER == LITTLE_ENDIAN
    char *X = (char *)&x;

    buffer[0] = X[7];
    buffer[1] = X[6];
    buffer[2] = X[5];
    buffer[3] = X[4];
    buffer[4] = X[3];
    buffer[5] = X[2];
    buffer[6] = X[1];
    buffer[7] = X[0];
#else
    memcpy(buffer, &x, 8);
#endif

    return 8;
}

/**
   Buffer read/write for uint64
*/
unsigned short il::bufferRead( uint64_t &x, const char *buffer )
{
#if BYTE_ORDER == LITTLE_ENDIAN
    char *X = (char *)&x;

    X[0] = buffer[7];
    X[1] = buffer[6];
    X[2] = buffer[5];
    X[3] = buffer[4];
    X[4] = buffer[3];
    X[5] = buffer[2];
    X[6] = buffer[1];
    X[7] = buffer[0];
#else
    memcpy(&x, buffer, 8);
#endif

    return 8;
}

unsigned short il::bufferWrite( const uint64_t &x, char *buffer )
{
#if BYTE_ORDER == LITTLE_ENDIAN
    char *X = (char *)&x;

    buffer[0] = X[7];
    buffer[1] = X[6];
    buffer[2] = X[5];
    buffer[3] = X[4];
    buffer[4] = X[3];
    buffer[5] = X[2];
    buffer[6] = X[1];
    buffer[7] = X[0];
#else
    memcpy(buffer, &x, 8);
#endif

    return 8;
}

unsigned short il::bufferRead( bool &x, const char *buffer )
{
    unsigned char temp = 0;

    memcpy(&temp, buffer, 1);

    if ( temp == 1 )
    {
       x = true;
    }
    else
    {
       x = false;
    }

    return 1;
}

unsigned short il::bufferWrite(  const bool &x, char *buffer )
{
    unsigned char temp = 0;

    if ( x == true )
    {
       temp = 1;
    }
    else
    {
       temp = 0;
    }
    memcpy( buffer, &temp, 1);
    return 1;
}














