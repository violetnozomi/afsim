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

#ifndef WSFXIO_DEFINES_HPP
#define WSFXIO_DEFINES_HPP

#include "wsf_export.h"

// Defines some Serialize() helper functions:
// Enum(), RawData(), Array()
// This header avoids an extra packetio include path for WSF applications
#include "PakSerializeFwd.hpp"
namespace PakSerialization
{
// Should no longer be necessary

//    class WSF_EXPORT WsfXIO_StringIdSerialize
//    {
//       public:
//          WsfXIO_StringIdSerialize(int* aIdPtr)
//          : mIdPtr(aIdPtr)
//          {}
//          int*  mIdPtr;
//    };

//! Forces an integer to be interpreted as a stringID

// inline WsfXIO_StringIdSerialize StringId(int& aStringId) { return WsfXIO_StringIdSerialize(&aStringId); }
} // namespace PakSerialization
#include "xio/WsfXIO_SerializeTypes.hpp"
#endif
