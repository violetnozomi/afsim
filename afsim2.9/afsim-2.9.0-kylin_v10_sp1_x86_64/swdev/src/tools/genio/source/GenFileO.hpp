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

/// \file  GenFileO.hpp
/// \brief GenFileO
/// \class GenFileO GenFileO.hpp
/// \brief Provides write access to a file.
///
/// This class inherits from GenO.  It provides a way to write data
/// to a file of a given endianness.

#ifndef GEN_FILE_O_HPP
#define GEN_FILE_O_HPP

#include "genio_export.h"

#include "GenBuf.hpp"
#include "GenFile.hpp"
#include "GenO.hpp"

class GENIO_EXPORT GenFileO : public GenFile, public GenO
{
public:
   /// Open a file in write mode.
   /// \param aFile     A full path or relative path file name.
   /// \param aBufType Defines endianness of buffer, possible values are:
   /// <table border="0" cellspacing="0" cellpadding="0"><tr><td width="35"></td><td>
   ///                       GenBuf::BigEndian        </td></tr>
   ///     <tr><td></td><td> GenBuf::LittleEndian     </td></tr>
   ///     <tr><td></td><td> GenBuf::LittleEndianVax  </td></tr>
   ///     <tr><td></td><td> GenBuf::Native           </td></tr>
   /// </table>
   GenFileO(const char* aFile, GenBuf::BufType aBufType);

   /// Destructor
   ~GenFileO() override;

   /// Send any buffered output.
   /// \note The Put() methods will send the buffer only when it becomes full.
   /// \retval >=0 Number of bytes written.
   /// \retval <0  Error (such as file not open for write)
   int Send() override;

   /// Method to write directly to a file.
   unsigned long WriteDirect(void* aBufferPtr, unsigned long aBytesToWrite);

protected:
   int HandlePutError(int aStatus, unsigned long aVariableSize) override;

private:
   // Should not use the default constructor
   GenFileO();

   unsigned long WriteBuffer();
};

#endif
