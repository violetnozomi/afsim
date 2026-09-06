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

// GenFile - A base class for GenFileI, GenFileO, and GenFileIO.
//           Physical reads and writes are performed only when required.

/// \file  GenFile.hpp
/// \brief GenFile
/// \class GenFile GenFile.hpp
/// \brief Provides a base class for GenFileI, GenFileO, and GenFileIO.

#ifndef GEN_FILE_HPP
#define GEN_FILE_HPP

#include "genio_export.h"

#include <cstdio>
#include <memory>
#include <string>

#include "GenBuf.hpp"
#include "GenBufIO.hpp"

class GENIO_EXPORT GenFile
{
public:
   virtual ~GenFile();

   /// Is file valid?
   /// \retval 1 File is open.
   /// \retval 0 File is closed.
   int IsValid() const;

   /// Set file position
   /// \retval non zero seek is unsuccessful.
   /// \retval 0 seek is successful.
   int Seek(long long aFilePos);

   /// Get file position
   /// \retval is file position.
   /// \retval -1 error.
   long long Tell() const;

   /// Get file name
   /// \retval is file name.
   const std::string& GetFileName() const;

protected:
   // Open the file.
   //   WriteMode: 0 for read mode, != 0 for write mode.
   //   BufType  : is the numeric representation.
   GenFile(const char* aFile, int aWriteMode, GenBuf::BufType aBufType);

   GenFile() = default;

   GenBufIO* GetGenBufIO();


   std::string               mFile{""};
   std::FILE*                mFilePtr{nullptr};
   long long                 mFilePos{0};
   int                       mWriteMode{0};
   int                       mBufSize{0};
   std::unique_ptr<GenBufIO> mBuf{nullptr};

private:
   // Disallow copy and assignment
   GenFile(const GenFile& aSrc);
   GenFile& operator=(const GenFile& aRhs);
};

inline int GenFile::IsValid() const
{
   return mFilePtr != nullptr;
}

inline GenBufIO* GenFile::GetGenBufIO()
{
   return mBuf.get();
}

inline const std::string& GenFile::GetFileName() const
{
   return mFile;
}

#endif
