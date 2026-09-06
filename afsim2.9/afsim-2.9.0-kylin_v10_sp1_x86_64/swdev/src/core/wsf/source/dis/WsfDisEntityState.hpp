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

#ifndef WSFDISENTITYSTATE_HPP
#define WSFDISENTITYSTATE_HPP

#include "wsf_export.h"

#include <map>
#include <string>
#include <vector>

#include "DisEntityState.hpp"
class WsfDisInterface;
class WsfDisPlatform;
class WsfPlatform;
class UtInput;
class UtInputBlock;

//! A class for maintaining DIS entity state PDUs.
//!
//! This class performs two major functions:
//! -) Contains a received PDU (derives from DisEntityState)
//! -) Contains processing methods necessary to move data between the WsfPlatform and the DisEntityState
//!    (both ways). This encapsulates much of the details of the processing of entity state data.
class WSF_EXPORT WsfDisEntityState : public DisEntityState
{
public:
   WsfDisEntityState(WsfDisInterface* aInterfacePtr);

   WsfDisEntityState(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);

   ~WsfDisEntityState() override;

   WsfDisEntityState* Clone() const override;

   int Process() override;

private:
   bool ContentsAreValid();

   //! Pointer to the WsfinterfaceP class.
   WsfDisInterface* mInterfacePtr;
};

#endif
