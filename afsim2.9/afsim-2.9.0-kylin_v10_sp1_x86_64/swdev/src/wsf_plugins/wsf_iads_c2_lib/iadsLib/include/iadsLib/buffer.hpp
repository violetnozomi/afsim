// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @file   buffer.hpp
 *  @author Kenneth R. Sewell III
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 */



#ifndef IADSLIB_BUFFER_H
#define IADSLIB_BUFFER_H

#include <iadsLib/OS.hpp>
#include <string>

namespace il
{
    unsigned short bufferRead(
	const unsigned short &size,
	void *x,
	const void *buffer
	);
    unsigned short bufferWrite(
	const unsigned short &size,
	const void *x,
	void *buffer
	);


    unsigned short bufferRead(  char &x, const char *buffer );
    unsigned short bufferWrite( const char &x, char *buffer );

    unsigned short bufferRead(  unsigned char &x, const char *buffer );
    unsigned short bufferWrite( const unsigned char &x, char *buffer );

    unsigned short bufferRead(  char *x, std::size_t len, const char *buffer );
    unsigned short bufferWrite( const char *x, std::size_t len, char *buffer );


    unsigned short bufferRead(  short &x, const char *buffer );
    unsigned short bufferWrite( const short &x, char *buffer );

    unsigned short bufferRead(  unsigned short &x, const char *buffer );
    unsigned short bufferWrite( const unsigned short &x, char *buffer );



    unsigned short bufferRead(  int &x, const char *buffer );
    unsigned short bufferWrite( const int &x, char *buffer );

    unsigned short bufferRead(  unsigned int &x, const char *buffer );
    unsigned short bufferWrite( const unsigned int &x, char *buffer );



    unsigned short bufferRead(  float &x, const char *buffer );
    unsigned short bufferWrite( const float &x, char *buffer );

    unsigned short bufferRead(  double &x, const char *buffer );
    unsigned short bufferWrite( const double &x, char *buffer );

    unsigned short bufferRead(  int64_t &x, const char *buffer );
    unsigned short bufferWrite( const int64_t &x, char *buffer );

    unsigned short bufferRead(  uint64_t &x, const char *buffer );
    unsigned short bufferWrite( const uint64_t &x, char *buffer );

    unsigned short bufferRead(  bool &x, const char *buffer );
    unsigned short bufferWrite( const bool &x, char *buffer );

}
#endif

