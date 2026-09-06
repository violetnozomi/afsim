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

#ifndef GENO_CONVERT_HPP
#define GENO_CONVERT_HPP

#include "genio_export.h"

#include "GenIODefs.hpp"

class GENIO_EXPORT GenOConvert
{
public:
   virtual ~GenOConvert();

   // Virtual Put functions to be overridden by derived class
   virtual void PutToBuf(unsigned char* aCurPut, unsigned short aValue) const = 0;
   virtual void PutToBuf(unsigned char* aCurPut, short aValue) const          = 0;
   virtual void PutToBuf(unsigned char* aCurPut, unsigned int aValue) const   = 0;
   virtual void PutToBuf(unsigned char* aCurPut, int aValue) const            = 0;
   virtual void PutToBuf(unsigned char* aCurPut, unsigned long aValue) const  = 0;
   virtual void PutToBuf(unsigned char* aCurPut, long aValue) const           = 0;
   virtual void PutToBuf(unsigned char* aCurPut, GENIO_UINT64 aValue) const   = 0;
   virtual void PutToBuf(unsigned char* aCurPut, GENIO_INT64 aValue) const    = 0;
   virtual void PutToBuf(unsigned char* aCurPut, float aValue) const          = 0;
   virtual void PutToBuf(unsigned char* aCurPut, double aValue) const         = 0;

protected:
   // Constructor
   GenOConvert();

private:
   // No coping of this class or its derived classes allowed
   GenOConvert& operator=(const GenOConvert& aRhs);
   GenOConvert(const GenOConvert& aSrc);
};

#endif
