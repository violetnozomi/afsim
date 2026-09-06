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

#ifndef GENERROR_HPP
#define GENERROR_HPP

#include "genio_export.h"

class GENIO_EXPORT GenError
{
public:
   enum GenErrorMask
   {
      cIO_ERRORS      = 0x01,
      cCONNECT_ERRORS = 0x02
   };

   static void SetSuppressErrorMask(unsigned int aSuppressErrorMask);

   static unsigned int GetSuppressErrorMask();

   static bool ShowError(unsigned int aErrorMask);

private:
   static unsigned int mSuppressErrorMask;
};
#endif
