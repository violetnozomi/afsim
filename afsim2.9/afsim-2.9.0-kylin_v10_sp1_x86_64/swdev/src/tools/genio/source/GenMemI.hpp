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

/// \file  GenMemI.hpp
/// \brief GenMemI
/// \class GenMemI GenMemI.hpp
/// \brief Provides memory access to a input buffer of a users choosing.
///
/// This class inherits from GenI.  It provides a way to read data
/// from a buffer of a given endianness.


#ifndef GEN_MEM_I_HPP
#define GEN_MEM_I_HPP

#include "genio_export.h"

#include "GenBufIUser.hpp"
#include "GenI.hpp"
#include "GenMem.hpp"

class GENIO_EXPORT GenMemI : public GenI, public GenMem
{
public:
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
   GenMemI(const unsigned char* aUserBuffer,
           unsigned long        aSizeInBytes,
           GenBuf::BufType      aBufType,
           unsigned long        aNumValidBytes = 0);

   /// Destructor
   ~GenMemI() override;

   /// Get a pointer to the buffer.
   /// \return A pointer to the buffer.
   const unsigned char* GetBuffer() const;

   /// Redefine the buffer.
   /// \param aUserBuffer     A pointer to a user buffer.  This class does not take ownership of the buffer.
   /// \param aSizeInBytes    Buffer size in bytes.
   /// \param aNumValidBytes  Number of current valid bytes in the buffer.
   void SetBuffer(const unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes = 0);

   /// Receive the data.  For this class, though there is nothing really to do.
   /// However, the buffer is cleared along with any error flags with this function.
   /// \param aWaitTimeInMicroSec Not used in this class.
   /// \return Always returns a zero.
   int Receive(int aWaitTimeInMicroSec) override;

private:
   GenBufIUser* mBuf;
};

inline const unsigned char* GenMemI::GetBuffer() const
{
   return mBuf->GetBuffer();
}

inline void GenMemI::SetBuffer(const unsigned char* aUserBuffer, unsigned long aSizeInBytes, unsigned long aNumValidBytes)
{
   mBuf->SetBuffer(aUserBuffer, aSizeInBytes, aNumValidBytes);
}

#endif
