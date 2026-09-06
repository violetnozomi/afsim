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

#ifndef WSFXIO_SCRIPTSERIALIZE_HPP
#define WSFXIO_SCRIPTSERIALIZE_HPP

#include "wsf_export.h"

class PakI;
class PakO;
namespace ut
{
namespace script
{
class Data;
}
} // namespace ut
void WSF_EXPORT Load(PakI& aAr, ut::script::Data& aScriptData);
void WSF_EXPORT Save(PakO& aAr, ut::script::Data& aScriptData);

// This code implements serialization for script objects
// Currently supports a limited subset of application objects
class WSF_EXPORT WsfXIO_ScriptSerialize
{
public:
   static void Initialize();
};

//}
#endif
