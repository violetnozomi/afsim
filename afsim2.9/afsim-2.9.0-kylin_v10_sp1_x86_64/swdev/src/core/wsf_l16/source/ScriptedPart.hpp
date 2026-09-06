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
#ifndef L16_SCRIPTEDPART_HPP
#define L16_SCRIPTEDPART_HPP

#include <map>

#include "ComputerPart.hpp"
class UtScript;
class WsfScenario;

namespace WsfL16
{
class ScriptedPart : public ComputerPart
{
public:
   ScriptedPart(WsfScenario& aScenario);
   ScriptedPart(const ScriptedPart& aSrc);
   ~ScriptedPart() override;
   ComputerPart* Clone() const override;

   bool Initialize(double aSimTime, ComputerProcessor& aComputer) override;

   bool ProcessInput(UtInput& aInput) override;

   //! Handle an incoming message
   void HandleReceive(double aSimTime, const Messages::InitialBase& aMessage) override;

   //! This method is called when the a slot is open
   void HandleSend(double aSimTime) override;

private:
   bool DecodeLabel(const std::string& aLabelStr, int& aLabel, int& aSubLabel);

   typedef std::pair<int, int> MessageLabel;

   UtScript* mOnSendScript;

   std::map<MessageLabel, UtScript*> mReceiveScripts;
};
} // namespace WsfL16

#endif
