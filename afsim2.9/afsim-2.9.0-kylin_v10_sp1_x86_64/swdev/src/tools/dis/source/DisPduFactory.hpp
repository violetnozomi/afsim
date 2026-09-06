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

// This base factory class creates the various Dis Pdu classes.  It may be
// derived from so that you can create your own Dis Pdu classes.

#ifndef DISPDUFACTORY_HPP
#define DISPDUFACTORY_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisPdu.hpp"
#include "GenIFactory.hpp"

class GenI;

class DIS_EXPORT DisPduFactory : public GenIFactory<DisPdu>
{
public:
   DisPduFactory();
   ~DisPduFactory() override;

   unsigned int GetTotalFilteredPduCount() { return mFilteredPduCount; }
   void         IncrementFilteredPduCount();
   void         ResetFilteredPduCounts() { mFilteredPduCount = 0; }

protected:
   // Creates the default DIS classes
   // This method reads enough of the input stream to
   // determine the type of PDU being read.  It then creates the proper PDU.
   // Once the proper PDU is generated,
   // it is populated with the data from the input stream.
   // The caller owns the returned PDU and is responsible for its destruction.
   DisPdu* Create(GenI& aGenI) override;

   // User overrides this method to create their own DIS classes
   virtual DisPdu* UserCreate(DisPdu& aDisPdu, GenI& aGenI);

   // Used for error messages
   void LogError(long aStartPos);

private:
   DisPdu       mLastGoodDisPdu;
   DisEntityId  mLastGoodDisEntityId;
   unsigned int mLastGoodDisEntityIdValid;
   unsigned int mFilteredPduCount;
};

#endif
