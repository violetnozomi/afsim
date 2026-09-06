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

// GenIConvert - An abstract base class for the GenIConvert byte swapping routines

#ifndef GENI_CONVERT_HPP
#define GENI_CONVERT_HPP

#include "genio_export.h"

#include "GenIODefs.hpp"

class GENIO_EXPORT GenIConvert
{
public:
   virtual ~GenIConvert();

   // Virtual Get functions to be overridden by derived class
   virtual void GetFromBuf(const unsigned char* aCurGet, unsigned short& aValue) const = 0;
   virtual void GetFromBuf(const unsigned char* aCurGet, short& aValue) const          = 0;
   virtual void GetFromBuf(const unsigned char* aCurGet, unsigned int& aValue) const   = 0;
   virtual void GetFromBuf(const unsigned char* aCurGet, int& aValue) const            = 0;
   virtual void GetFromBuf(const unsigned char* aCurGet, unsigned long& aValue) const  = 0;
   virtual void GetFromBuf(const unsigned char* aCurGet, long& aValue) const           = 0;
   virtual void GetFromBuf(const unsigned char* aCurGet, GENIO_UINT64& aValue) const   = 0;
   virtual void GetFromBuf(const unsigned char* aCurGet, GENIO_INT64& aValue) const    = 0;
   virtual void GetFromBuf(const unsigned char* aCurGet, float& aValue) const          = 0;
   virtual void GetFromBuf(const unsigned char* aCurGet, double& aValue) const         = 0;

protected:
   // Constructor
   GenIConvert();

private:
   // No coping of this class or its derived classes allowed
   GenIConvert& operator=(const GenIConvert& aRhs);
   GenIConvert(const GenIConvert& aSrc);
};

#endif
