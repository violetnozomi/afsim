// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef GEN_MEM_IO_HPP
#define GEN_MEM_IO_HPP

#include "genio_export.h"

/// \file  GenMemIO.hpp
/// \brief GenMemIO
/// \class GenMemIO GenMemIO.hpp
/// \brief Provides memory access to a buffer of a users choosing.
///
/// This class inherits from GenIO.  It provides a way to write and read data
/// into and out of a buffer of a given endianness.

#include "GenBufIO.hpp"
#include "GenIO.hpp"
#include "GenMem.hpp"

class GENIO_EXPORT GenMemIO : public GenIO, public GenMem
{
public:
   /// This constructor builds a buffer for the user.
   /// \param aBufType Defines endianness of buffer, possible values are:
   /// <table border="0" cellspacing="0" cellpadding="0"><tr><td width="35"></td><td>
   ///                       GenBuf::BigEndian        </td></tr>
   ///     <tr><td></td><td> GenBuf::LittleEndian     </td></tr>
   ///     <tr><td></td><td> GenBuf::LittleEndianVax  </td></tr>
   ///     <tr><td></td><td> GenBuf::Native           </td></tr>
   /// </table>
   /// \param aSize Buffer size in bytes.
   GenMemIO(GenBuf::BufType aBufType, unsigned long aSize);

   /// This constructor takes a user provided buffer.
   /// \param aUserBuffer     A pointer to a user buffer.  This class does not take ownership of the buffer.
   /// \param aSizeInBytes    Buffer size in bytes.
   /// \param aBufType        Defines endianness of buffer, acceptable values are:
   /// <table border="0" cellspacing="0" cellpadding="0"><tr><td width="35"></td><td>
   ///                       GenBuf::BigEndian        </td></tr>
   ///     <tr><td></td><td> GenBuf::LittleEndian     </td></tr>
   ///     <tr><td></td><td> GenBuf::LittleEndianVax  </td></tr>
   ///     <tr><td></td><td> GenBuf::Native           </td></tr>
   /// </table>
   /// \param aNumValidBytes  Number of current valid bytes in the buffer.
   GenMemIO(unsigned char* aUserBuffer, unsigned long aSizeInBytes, GenBuf::BufType aBufType, unsigned long aNumValidBytes = 0);

   /// Destructor
   ~GenMemIO() override;

   /// Get a pointer to the buffer.
   /// \return A pointer to the buffer.
   unsigned char* GetBuffer() const;

   /// Redefine the buffer.
   /// \param aUserBuffer     A pointer to a user buffer.  This class does not take ownership of the buffer.
   /// \param aSizeInBytes    Buffer size in bytes.
   /// \param aNumValidBytes  Number of current valid bytes in the buffer.
   void SetBuffer(unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes = 0);

   /// Receive the data.  For this class, though there is nothing really to do.
   /// However, the buffer is cleared along with any error flags with this function.
   /// \param aWaitTimeInMicroSec Not used in this class.
   /// \return Always returns a zero.
   int Receive(int aWaitTimeInMicroSec) override;

   /// Send the data.  For this class, though there is nothing really to do.
   /// However, the buffer is cleared along with any error flags with this function.
   /// \return Always returns a zero.
   int Send() override;

private:
   GenBufIO* mBuf;
};

inline unsigned char* GenMemIO::GetBuffer() const
{
   return mBuf->GetBuffer();
}

inline void GenMemIO::SetBuffer(unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes)
{
   mBuf->SetBuffer(aUserBuffer, aSizeInBytes, aNumValidBytes);
}

#endif
