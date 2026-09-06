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
#ifndef GENPIPEO_HPP
#define GENPIPEO_HPP

#include "genio_export.h"

#include "GenBufOManaged.hpp"
#include "GenO.hpp"
#include "GenPipe.hpp"
class GENIO_EXPORT GenPipeO : public GenO
{
public:
   // Sends any buffered output.
   // Return value: >= 0, number of bytes written.
   //                < 0, error (such as file not open for write)
   // NOTE: The Put() methods will send the buffer only when it becomes full.
   int Send() override;

   GenPipeO(GenBuf::BufType aBufType, GenPipe& aPipe);

   ~GenPipeO() override;

private:
   GenPipe         mPipe;
   GenBufOManaged* mBuffer;
};

#endif
