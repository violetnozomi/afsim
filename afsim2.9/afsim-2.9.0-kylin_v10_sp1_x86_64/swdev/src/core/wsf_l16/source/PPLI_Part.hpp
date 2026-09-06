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
#ifndef L16_PPLI_PART_HPP
#define L16_PPLI_PART_HPP

#include "ComputerPart.hpp"
#include "InputEnumeration.hpp"
class WsfScenario;

namespace WsfL16
{
class PPLI_Part : public ComputerPart
{
public:
   PPLI_Part(WsfScenario& aScenario);
   PPLI_Part(const PPLI_Part& aSrc);

   ComputerPart* Clone() const override;

   bool Initialize(double aSimTime, ComputerProcessor& aComputer) override;
   bool ProcessInput(UtInput& aInput) override;
   //! Handle an incoming message
   void HandleReceive(double aSimTime, const Messages::InitialBase& aMessage) override;
   //! This method is called when the a slot is open
   void HandleSend(double aSimTime) override;

private:
   void SendPPLI(double aSimTime);

   void SendAirPPLI(double aSimTime, WsfPlatform& aPlatform);

   void SendSurfacePPLI(double aSimTime, WsfPlatform& aPlatform);
   void SendSubsurfacePPLI(double aSimTime, WsfPlatform& aPlatform);
   void SendLandPPLI(double aSimTime, WsfPlatform& aPlatform);
   void SendLandPointPPLI(double aSimTime, WsfPlatform& aPlatform);

   enum Category
   {
      cAIR,
      cSURFACE,
      cSUBSURFACE,
      cLAND,
      cPOINT,
      cUNSET
   };

   enum LandSubcategory
   {
      c_POINT,
      c_LAND
   };

   int              mCategory;
   InputEnumeration mCategoryInput;
   int              mLandSubcategory;
   InputEnumeration mLandSubcategoryInput;
};
} // namespace WsfL16

#endif
