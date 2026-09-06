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

#ifndef GEN_THREAD_CONTROL_HPP
#define GEN_THREAD_CONTROL_HPP

#include "genio_export.h"

class GENIO_EXPORT GenThreadControl
{
public:
   GenThreadControl();
   virtual ~GenThreadControl();

   // These two routines should return a positive number if successful.
   virtual int Lock();
   virtual int Unlock();
};

#endif
