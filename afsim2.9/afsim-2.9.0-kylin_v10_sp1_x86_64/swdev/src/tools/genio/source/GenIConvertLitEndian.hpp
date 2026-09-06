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

#ifndef GENI_CONVERT_LIT_ENDIAN_HPP
#define GENI_CONVERT_LIT_ENDIAN_HPP

#include "genio_export.h"

// This gets data out of a little Endian buffer

#include "GenIConvertLitEndianInt.hpp"

#if defined(sgi) && (_COMPILER_VERSION >= 720)
#pragma set woff 1681, 1682
#endif

class GENIO_EXPORT GenIConvertLitEndian : public GenIConvertLitEndianInt
{
public:
   GenIConvertLitEndian();

   ~GenIConvertLitEndian() override;


   // Functions for moving data from a buffer to a variable.
   // The data is not assumed to be aligned in the buffer.

   void GetFromBuf(const unsigned char* aCurGet, float& aValue) const override;
   void GetFromBuf(const unsigned char* aCurGet, double& aValue) const override;
};

#if defined(sgi) && (_COMPILER_VERSION >= 720)
#pragma reset woff 1681, 1682
#endif

#endif
