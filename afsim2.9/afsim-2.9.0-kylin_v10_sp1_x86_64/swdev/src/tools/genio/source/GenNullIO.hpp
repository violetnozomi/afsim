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

// GenNullIO - A derived class of GenIO that does I/O to null device.

#ifndef GEN_NULL_IO_HPP
#define GEN_NULL_IO_HPP

#include "genio_export.h"

#include "GenIO.hpp"

class GenBufIO;

class GENIO_EXPORT GenNullIO : public GenIO
{
public:
   // Open the file.
   //   BufType  : is the numeric representation.
   GenNullIO();

   ~GenNullIO() override;

   // Always returns a -1 to say no data received
   int Receive(int aWaitTimeInMicroSec) override;

   // Always returns a -1 to say no data sent
   int Send() override;

protected:
   int HandleGetError(int aStatus, unsigned long aVariableSize) override;
   int HandlePutError(int aStatus, unsigned long aVariableSize) override;

private:
   GenBufIO* mBuf;
};

#endif
