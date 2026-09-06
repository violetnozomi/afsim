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

// GenNullO - A derived class of GenO that does output to a null device.

#ifndef GEN_NULL_O_HPP
#define GEN_NULL_O_HPP

#include "genio_export.h"

#include "GenO.hpp"

class GenBufOManaged;

class GENIO_EXPORT GenNullO : public GenO
{
public:
   // Open the file.
   //   BufType  : is the numeric representation.
   GenNullO();

   ~GenNullO() override;

   // Always returns a -1 to say no data sent
   int Send() override;

protected:
   int HandlePutError(int aStatus, unsigned long aVariableSize) override;

private:
   GenBufOManaged* mBuf;
};

#endif
