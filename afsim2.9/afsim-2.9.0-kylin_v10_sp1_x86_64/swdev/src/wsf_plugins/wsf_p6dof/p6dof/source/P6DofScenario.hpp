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

#ifndef P6DOFSCENARIO_HPP
#define P6DOFSCENARIO_HPP

#include "p6dof_export.h"

class P6DofAtmosphere;
class P6DofFreezeFlags;
class P6DofGravity;
class P6DofTerrain;
class P6DofTypeManager;
class P6DofWind;

class P6DOF_EXPORT P6DofScenario
{
public:
   explicit P6DofScenario(P6DofTypeManager* aTypeManager);
   virtual ~P6DofScenario();

   P6DofScenario& operator=(const P6DofScenario& other) = delete;

   P6DofScenario* Clone() const;

   P6DofTypeManager* GetTypeManager() const { return mTypeManager; }

   P6DofFreezeFlags* GetFreezeFlags() const { return mFreezeFlags; }

   P6DofAtmosphere* GetAtmosphere() const { return mAtmosphere; }
   P6DofGravity*    GetGravity() const { return mGravity; }
   P6DofTerrain*    GetTerrain() const { return mTerrain; }
   P6DofWind*       GetWind() const { return mWind; }

   void SetAtmosphere(P6DofAtmosphere* aAtmosphere) { mAtmosphere = aAtmosphere; }
   void SetGravity(P6DofGravity* aGravity) { mGravity = aGravity; }
   void SetTerrain(P6DofTerrain* aTerrain) { mTerrain = aTerrain; }
   void SetWind(P6DofWind* aWind) { mWind = aWind; }

protected:
   P6DofScenario(const P6DofScenario& aSrc); // disable copy construction

private:
   P6DofTypeManager* mTypeManager;
   P6DofFreezeFlags* mFreezeFlags;
   P6DofAtmosphere*  mAtmosphere;
   P6DofGravity*     mGravity;
   P6DofTerrain*     mTerrain;
   P6DofWind*        mWind;
};
#endif // P6DOFSCENARIO_HPP
