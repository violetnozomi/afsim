// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef L16_SYSTEMSTATUSPART_HPP
#define L16_SYSTEMSTATUSPART_HPP

#include "ComputerPart.hpp"
#include "InputEnumeration.hpp"

namespace WsfL16
{
class SystemStatusPart : public ComputerPart
{
public:
   SystemStatusPart(WsfScenario& aScenario);
   SystemStatusPart(const SystemStatusPart& aSrc);
   ~SystemStatusPart() override;
   ComputerPart* Clone() const override;

   bool Initialize(double aSimTime, ComputerProcessor& aComputer) override;

   void HandleReceive(double aSimTime, const Messages::InitialBase& aMessage) override;

   void HandleSend(double aSimTime) override;

   bool ProcessInput(UtInput& aInput) override;

protected:
   bool CheckWeapon(WsfStringId aWeaponId);
   int  WeaponQuantity(WsfStringId aWeaponId);

   void SendPPLI(double aSimTime);
   void SendSystemStatus(double aSimTime);
   void SendAirSystemStatus(double aSimTime);
   void SendLandPointSystemStatus(double aSimTime);
   void SendSurfaceSystemStatus(double aSimTime);

   // Hold implicitly copy constructable data
   struct Config
   {
      Config();
      WsfStringId mWeaponNameId1;
      int         mWeaponTypeId1;
      WsfStringId mWeaponNameId2;
      int         mWeaponTypeId2;
      WsfStringId mWeaponNameId3;
      int         mWeaponTypeId3;
   };

   Config mConfig;
   enum Category
   {
      cAIR,
      cSURFACE,
      cSUBSURFACE,
      cLAND,
      cUNSET
   };

   int              mCategory;
   InputEnumeration mCategoryInput;
};
} // namespace WsfL16

#endif
