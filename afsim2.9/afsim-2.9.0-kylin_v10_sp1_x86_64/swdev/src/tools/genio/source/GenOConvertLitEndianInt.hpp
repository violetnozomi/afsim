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

// GenOConvert - An abstract base class for the GenO byte swapping routines

#ifndef GENO_CONVERT_LIT_ENDIAN_INT_HPP
#define GENO_CONVERT_LIT_ENDIAN_INT_HPP

#include "genio_export.h"

#include "GenOConvert.hpp"

#if defined(sgi) && (_COMPILER_VERSION >= 720)
#pragma set woff 1681, 1682
#endif

class GENIO_EXPORT GenOConvertLitEndianInt : public GenOConvert
{
public:
   // Constructor
   GenOConvertLitEndianInt();

   ~GenOConvertLitEndianInt() override;

   // Functions for moving data into a buffer from a variable.
   // The data is not assumed to be aligned in the buffer.
   void PutToBuf(unsigned char* aCurPut, unsigned short aValue) const override;
   void PutToBuf(unsigned char* aCurPut, short aValue) const override;
   void PutToBuf(unsigned char* aCurPut, unsigned int aValue) const override;
   void PutToBuf(unsigned char* aCurPut, int aValue) const override;
   void PutToBuf(unsigned char* aCurPut, unsigned long aValue) const override;
   void PutToBuf(unsigned char* aCurPut, long aValue) const override;
   void PutToBuf(unsigned char* aCurPut, GENIO_UINT64 aValue) const override;
   void PutToBuf(unsigned char* aCurPut, GENIO_INT64 aValue) const override;
};

#if defined(sgi) && (_COMPILER_VERSION >= 720)
#pragma reset woff 1681, 1682
#endif

#endif
