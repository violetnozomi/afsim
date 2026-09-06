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

#ifndef GENI_CONVERT_BIG_ENDIAN_HPP
#define GENI_CONVERT_BIG_ENDIAN_HPP

// This gets data out of a Big Endian buffer

#include "genio_export.h"

#include "GenIConvert.hpp"

class GENIO_EXPORT GenIConvertBigEndian : public GenIConvert
{
public:
   GenIConvertBigEndian();

   ~GenIConvertBigEndian() override;


   // Functions for moving data from a buffer to a variable.
   // The data is not assumed to be aligned in the buffer.

   void GetFromBuf(const unsigned char* aCurGet, unsigned short& aValue) const override;
   void GetFromBuf(const unsigned char* aCurGet, short& aValue) const override;
   void GetFromBuf(const unsigned char* aCurGet, unsigned int& aValue) const override;
   void GetFromBuf(const unsigned char* aCurGet, int& aValue) const override;
   void GetFromBuf(const unsigned char* aCurGet, unsigned long& aValue) const override;
   void GetFromBuf(const unsigned char* aCurGet, long& aValue) const override;
   void GetFromBuf(const unsigned char* aCurGet, GENIO_UINT64& aValue) const override;
   void GetFromBuf(const unsigned char* aCurGet, GENIO_INT64& aValue) const override;
   void GetFromBuf(const unsigned char* aCurGet, float& aValue) const override;
   void GetFromBuf(const unsigned char* aCurGet, double& aValue) const override;
};


#endif
