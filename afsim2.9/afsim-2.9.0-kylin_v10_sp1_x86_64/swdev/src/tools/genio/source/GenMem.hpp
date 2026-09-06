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

/// \file  GenMem.hpp
/// \brief GenMem
/// \class GenMem GenMem.hpp
/// \brief Provides a base class for GenMemI, GenMemO, and GenMemIO.


#ifndef GEN_MEM_HPP
#define GEN_MEM_HPP

#include "genio_export.h"

#include "GenBuf.hpp"

class GENIO_EXPORT GenMem
{
public:
   virtual ~GenMem();

   // Set the number of valid bytes.  User may need also set the
   // GetIndex and/or the PutIndex
   int  SetNumValidBytes(unsigned long aNumValidBytes);
   void Reset();

protected:
   GenMem();

   // CAUTION  CAUTION  CAUTION  CAUTION  CAUTION  CAUTION
   // The GenBuf pointer must be set before this class is used.
   // The GenBuf pointer is here as an optimization.  In a
   // more pure OO environment, the pointer would be returned from
   // a virtual function in a derived class.
   void SetGenBuf(GenBuf* aGenBuf);

private:
   GenBuf* mGenBuf;
};

inline int GenMem::SetNumValidBytes(unsigned long aNumValidBytes)
{
   return mGenBuf->SetNumValidBytes(aNumValidBytes);
}

inline void GenMem::Reset()
{
   // Clear the buffer
   mGenBuf->Reset();
}

inline void GenMem::SetGenBuf(GenBuf* aGenBuf)
{
   mGenBuf = aGenBuf;
}

#endif
