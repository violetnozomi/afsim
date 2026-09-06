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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef RIFData_hpp
#define RIFData_hpp

// RECORD_ID_FIELD (tag 001)
struct RIFData
{
public:
   char mType[3];    // "VTH"
   char mID[2];      // "01"
   char mFieldDelim; // 0x1E
};

#endif
