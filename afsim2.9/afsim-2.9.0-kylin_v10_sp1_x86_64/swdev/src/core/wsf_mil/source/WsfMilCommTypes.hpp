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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef WSFMILCOMMTYPES_HPP
#define WSFMILCOMMTYPES_HPP

class WsfScenario;

class WsfMilCommTypes
{
public:
   WsfMilCommTypes() = default;
   ;
   ~WsfMilCommTypes() = default;
   ;

   static void Register(WsfScenario& aScenario);
};

#endif
