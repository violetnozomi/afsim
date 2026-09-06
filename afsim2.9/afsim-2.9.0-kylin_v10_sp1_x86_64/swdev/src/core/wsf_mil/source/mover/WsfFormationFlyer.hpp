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

#ifndef WSFFORMATIONFLYER_HPP
#define WSFFORMATIONFLYER_HPP

#include "wsf_mil_export.h"

#include "WsfFollower.hpp"


//! A specialized mover that will cause an aircraft to try to fly formation flight
//! with another aircraft, at some offset relative to the lead.  See parent class.

class WSF_MIL_EXPORT WsfFormationFlyer : public WsfFollower
{
public:
   WsfFormationFlyer(WsfScenario& aScenario);
   ~WsfFormationFlyer() override = default;

   //! @name Common framework methods
   //@{
   WsfMover*   Clone() const override;
   bool        Initialize(double aSimTime) override;
   bool        ProcessInput(UtInput& aInput) override;
   void        Update(double aSimTime) override;
   const char* GetScriptClassName() const override;
   //@}

   WsfSpatialDomain GetSpatialDomain() const override { return WSF_SPATIAL_DOMAIN_AIR; }

   //! @name Formation data methods
   //! These methods are used to set lead aircraft and offset information.
   //@{
   // void SetLeadAircraft(int aLeadNameId);
   void         SetLeadAircraft(WsfPlatform* aPlatformPtr);
   WsfPlatform* GetLeadAircraft() const { return mLeadPlatPtr; }

   void SetOffsetFromLeadECS(const double aOffsetFromLeadECS[3]); // ECS frame [meters]
   void GetOffsetFromLeadECS(double aOffsetFromLeadECS[3]) const; // ECS frame [meters]

   void SetFormationRollsWithLead(bool aValue) { mFormationRolls = aValue; }
   bool GetFormationRollsWithLead() const { return mFormationRolls; }
   //@}

   //! Used to switch sides when formation turns.
   void SwitchSides() { mNegateLateralOffset = !mNegateLateralOffset; }

protected:
   // Declare default copy constructor (for Clone())
   WsfFormationFlyer(const WsfFormationFlyer& aSrc);
   WsfFormationFlyer& operator=(const WsfFormationFlyer&) = delete;

   void UpdateLeadInfo(double aSimTime);
   void UpdateLeadPosAndVel(double aSimTime);

private:
   //! @brief Used to output data when debugging.
   void OutputDiagnostic(double aSimTime, WsfPlatform* aLeadPlatPtr);
   void PlatformDiagnostic(WsfPlatform* aPlatformPtr);


   //! The ID of the lead platform in the formation. Defaults to 0.
   WsfStringId mLeadNameId;

   //! The platform index of the lead platform. Defaults to 0.
   size_t mLeadIndex;

   //! A pointer to the lead platform. Defaults to 0.
   WsfPlatform* mLeadPlatPtr;

   //! Offset from the lead platform in the ECS frame (nose, rt. wing, down) [meters]
   double mOffsetFromLeadECS[3];

   //! If "true", formation rolls with the lead aircraft,
   //! else, the formation stays flat in the NED frame. Defaults to false.
   bool mFormationRolls;

   //! Flag used to change the sign of the lateral offset. Defaults to false.
   bool mNegateLateralOffset;

   //! If "true" the follower platform is initially place at the supplied
   //! offset from the lead platform. Defaults to false.
   bool mInitializeAtOffset;
};

#endif
