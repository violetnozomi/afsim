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

/// \file  GenMemO.hpp
/// \brief GenMemO
/// \class GenMemO GenMemO.hpp
/// \brief Provides memory access to a buffer of a users choosing.
///
/// This class inherits from GenO.  It provides a way to write
/// into a buffer of a given endianness.

#ifndef GEN_MEM_O_HPP
#define GEN_MEM_O_HPP

#include "genio_export.h"

#include "GenBufOManaged.hpp"
#include "GenMem.hpp"
#include "GenO.hpp"

class GENIO_EXPORT GenMemO : public GenO, public GenMem
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
   GenMemO(GenBuf::BufType aBufType, unsigned long aSize);

   /// Destructor
   ~GenMemO() override;

   /// Get a pointer to the buffer.
   /// \return A pointer to the buffer.
   const unsigned char* GetBuffer() const;

   /// Send the data.  For this class, though there is nothing really to do.
   /// However, the buffer is cleared along with any error flags with this function.
   /// \return Always returns a zero.
   int Send() override;

private:
   GenBufOManaged* mBuf;
};

inline const unsigned char* GenMemO::GetBuffer() const
{
   return mBuf->GetBuffer();
}

#endif
