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
#ifndef L16_WEAPONSCOORDINATIONPART_HPP
#define L16_WEAPONSCOORDINATIONPART_HPP

#include "ComputerPart.hpp"
#include "InputEnumeration.hpp"

namespace WsfL16
{
//! This class is a stub for handling J9.X messages.
class WeaponsCoordinationPart : public ComputerPart
{
public:
   WeaponsCoordinationPart(WsfScenario& aScenario);
   WeaponsCoordinationPart(const WeaponsCoordinationPart& aSrc);

   ComputerPart* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   bool Initialize(double aSimTime, ComputerProcessor& aComputer) override;

   //! Handle an incoming message
   void HandleReceive(double aSimTime, const Messages::InitialBase& aMessage) override;

   //! This method is called when the a slot is open
   void HandleSend(double aSimTime) override;

private:
};
} // namespace WsfL16

#endif
