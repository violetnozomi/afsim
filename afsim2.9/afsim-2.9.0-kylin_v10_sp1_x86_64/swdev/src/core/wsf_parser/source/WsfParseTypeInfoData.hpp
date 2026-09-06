// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFPARSETYPEINFODATA_HPP
#define WSFPARSETYPEINFODATA_HPP

#include "wsf_parser_export.h"

#include <vector>

#include "UtStringRef.hpp"
#include "WsfParseAuxData.hpp"
#include "WsfParseTypePath.hpp"

// Stores information about parsed types in the parse tree
class WSF_PARSER_EXPORT WsfParseTypeInfoData : public WsfParseAuxData
{
public:
   enum Operation
   {
      cLOAD_TYPE,
      cCREATE_TYPE,
      cDELETE_TYPE
   };

   enum Flags
   {
      cNESTED_SAVE    = 1,
      cNESTED_LOAD    = 2,
      cDUPLICATE_NAME = 4,
      cINVALID_TYPE   = 8
   };

   WsfParseTypeInfoData(Operation aOperation)
      : WsfParseAuxData(cTYPE_DATA)
      , mOperation(aOperation)
      , mFlags(0)
   {
   }

   ~WsfParseTypeInfoData() override {}

   Operation        mOperation;
   WsfParseTypePath mSaveKey;
   WsfParseTypePath mLoadKey;
   int              mFlags;
};

#endif
