// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef L16_MESSAGEPACKING_HPP
#define L16_MESSAGEPACKING_HPP

#include "wsf_l16_export.h"

//! functions for packing and unpacking variable-sized integers into/out of a buffer
namespace WsfL16
{
namespace Messages
{
void WSF_L16_EXPORT Pack(char* aBuf, unsigned int aData, int start, int stop);

void WSF_L16_EXPORT Unpack(const char* aBuf, unsigned int& aData, int start, int stop);
} // namespace Messages
} // namespace WsfL16

#endif
