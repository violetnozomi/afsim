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

#ifndef WSFPARSEAUXDATA_HPP
#define WSFPARSEAUXDATA_HPP

#include "wsf_parser_export.h"

//! Abstract class for encapsulating auxiliary data to be attached to a parse node.
class WSF_PARSER_EXPORT WsfParseAuxData
{
public:
   enum DataType
   {
      cTYPE_DATA,
      cFILE_REFERENCE,
      cDELAY_LOAD_DATA
   };
   DataType mDataType;
   WsfParseAuxData(DataType aType)
      : mDataType(aType)
   {
   }
   virtual ~WsfParseAuxData() {}
};

#endif
