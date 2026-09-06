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

#include "GenError.hpp"

unsigned int GenError::mSuppressErrorMask = 0;

// static
void GenError::SetSuppressErrorMask(unsigned int aSuppressErrorMask)
{
   mSuppressErrorMask = aSuppressErrorMask;
}

// static
unsigned int GenError::GetSuppressErrorMask()
{
   return mSuppressErrorMask;
}

// static
bool GenError::ShowError(unsigned int aErrorMask)
{
   bool showError = true;

   if (mSuppressErrorMask & aErrorMask)
   {
      showError = false;
   }
   return showError;
}
