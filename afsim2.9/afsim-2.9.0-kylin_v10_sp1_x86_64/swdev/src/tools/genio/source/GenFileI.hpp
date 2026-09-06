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

// GenFileI - A derived class of GenIO that does buffered input on a file.
//             Physical reads are performed only when required.

/// \file  GenFileI.hpp
/// \brief GenFileI
/// \class GenFileI GenFileI.hpp
/// \brief Provides read access to a file.
///
/// This class inherits from GenI.  It provides a way to read data
/// from a file of a given endianness.

#ifndef GEN_FILE_I_HPP
#define GEN_FILE_I_HPP

#include "genio_export.h"

#include "GenBuf.hpp"
#include "GenFile.hpp"
#include "GenI.hpp"

class GENIO_EXPORT GenFileI : public GenFile, public GenI
{
public:
   /// Open a file in a read only mode.
   /// \param aFile     A full path or relative path file name.
   /// \param aBufType Defines endianness of buffer, possible values are:
   /// <table border="0" cellspacing="0" cellpadding="0"><tr><td width="35"></td><td>
   ///                       GenBuf::BigEndian        </td></tr>
   ///     <tr><td></td><td> GenBuf::LittleEndian     </td></tr>
   ///     <tr><td></td><td> GenBuf::LittleEndianVax  </td></tr>
   ///     <tr><td></td><td> GenBuf::Native           </td></tr>
   /// </table>
   GenFileI(const char* aFile, GenBuf::BufType aBufType);

   /// Destructor
   ~GenFileI() override;

   /// Move any unread data to the front of the buffer and then refill the buffer.
   /// \note It is not necessary to call this function.  The Get()
   ///       methods will refill the buffer as necessary.
   /// \param aWaitTimeInMicroSec Not used in this class.
   /// \retval >=0 Number of bytes in the input buffer.
   /// \retval <0  Error (such as EOF).
   int Receive(int aWaitTimeInMicroSec) override;

   /// Method to read directly from a file.
   unsigned long ReadDirect(void* aBufferPtr, unsigned long aBytesToRead);

   /// Get file offset
   /// \retval is the file position of byte 0 of mBuf
   /// \retval -1 error
   long long TellBufferOffset() const;

protected:
   int HandleGetError(int aStatus, unsigned long aVariableSize) override;

private:
   // Should not use the default constructor
   GenFileI();

   unsigned long ReadBuffer();

   long long mBufferOffset;
};

#endif
