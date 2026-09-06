// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFFORMATIONUPDATESTATIONKEEPING_HPP
#define WSFFORMATIONUPDATESTATIONKEEPING_HPP

class WsfFormation;

class WsfFormationUpdateStationKeeping
{
public:
   static void Update(WsfFormation* aFormationPtr, double aSimTime, bool aJumpDetached = false);
};

#endif // WSFFORMATIONUPDATESTATIONKEEPING_HPP
