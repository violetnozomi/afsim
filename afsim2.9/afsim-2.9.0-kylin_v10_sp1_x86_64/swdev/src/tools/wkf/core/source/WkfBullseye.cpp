// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfBullseye.hpp"

#include "VaAttachmentModel.hpp"

wkf::Bullseye::Bullseye()
   : wkf::PointOfInterest()
{
   SetType<Bullseye>();
   SetName(std::string("bullseye") + std::to_string(GetUniqueId()));

   auto* modelPtr = FindFirstAttachmentOfType<vespa::VaAttachmentModel>();
   if (modelPtr)
   {
      modelPtr->SetIcon("bullseye");
   }
}
