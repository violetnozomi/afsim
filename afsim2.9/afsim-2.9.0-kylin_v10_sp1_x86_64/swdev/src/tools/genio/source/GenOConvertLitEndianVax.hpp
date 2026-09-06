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

#ifndef GENO_CONVERT_LIT_ENDIAN_VAX_HPP
#define GENO_CONVERT_LIT_ENDIAN_VAX_HPP

#include "genio_export.h"

#include "GenOConvertLitEndianInt.hpp"

#if defined(sgi) && (_COMPILER_VERSION >= 720)
#pragma set woff 1681, 1682
#endif

class GENIO_EXPORT GenOConvertLitEndianVax : public GenOConvertLitEndianInt
{
public:
   // Constructor
   GenOConvertLitEndianVax();

   ~GenOConvertLitEndianVax() override;

   // Functions for moving data into a buffer from a variable.
   // The data is not assumed to be aligned in the buffer.
   void PutToBuf(unsigned char* aCurPut, float aValue) const override;
   void PutToBuf(unsigned char* aCurPut, double aValue) const override;
};

#if defined(sgi) && (_COMPILER_VERSION >= 720)
#pragma reset woff 1681, 1682
#endif

#endif
