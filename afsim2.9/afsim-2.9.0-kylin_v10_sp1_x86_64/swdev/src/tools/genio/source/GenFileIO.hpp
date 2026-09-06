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

/// \file  GenFileIO.hpp
/// \brief GenFileIO
/// \class GenFileIO GenFileIO.hpp
/// \brief Provides read/write access to a file.
///
/// This class inherits from GenIO.  It provides a way to write and read data
/// to and from a file of a given endianness.

#ifndef GEN_FILE_IO_HPP
#define GEN_FILE_IO_HPP

#include "genio_export.h"

#include "GenBuf.hpp"
#include "GenFile.hpp"
#include "GenIO.hpp"


class GENIO_EXPORT GenFileIO : public GenFile, public GenIO
{
public:
   /// Open the file.
   /// \param aFile     A full path or relative path file name.
   /// \param aBufType Defines endianness of buffer, possible values are:
   /// <table border="0" cellspacing="0" cellpadding="0"><tr><td width="35"></td><td>
   ///                       GenBuf::BigEndian        </td></tr>
   ///     <tr><td></td><td> GenBuf::LittleEndian     </td></tr>
   ///     <tr><td></td><td> GenBuf::LittleEndianVax  </td></tr>
   ///     <tr><td></td><td> GenBuf::Native           </td></tr>
   /// </table>
   /// \param aReadOnly It is expected that this argument will eventually disappear.
   ///                  One can use  GenFileI to read read only files.
   GenFileIO(const char* aFile, GenBuf::BufType aBufType, unsigned int aReadOnly = 0);

   /// Destructor
   ~GenFileIO() override;

   /// Move any unread data to the front of the buffer and then refill the buffer.
   /// \note It is not necessary to call this function.  The Get()
   ///       methods will refill the buffer as necessary.
   /// \param aWaitTimeInMicroSec Not used in this class.
   /// \retval >=0 Number of bytes in the input buffer.
   /// \retval <0  Error (such as EOF).
   int Receive(int aWaitTimeInMicroSec) override;

   /// Method to read directly from a file.
   unsigned long ReadDirect(void* aBufferPtr, unsigned long aBytesToRead);

   /// Send any buffered output.
   /// \note The Put() methods will send the buffer only when it becomes full.
   /// \retval >=0 Number of bytes written.
   /// \retval <0  Error (such as file not open for write)
   int Send() override;

   /// Method to write directly to a file.
   unsigned long WriteDirect(void* aBufferPtr, unsigned long aBytesToWrite);

protected:
   int HandleGetError(int aStatus, unsigned long aVariableSize) override;
   int HandlePutError(int aStatus, unsigned long aVariableSize) override;

private:
   // Do not use the default constructor
   GenFileIO();

   unsigned long ReadBuffer();
   unsigned long WriteBuffer();
};

#endif
