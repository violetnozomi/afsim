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

#include "WkfPointOfInterest.hpp"

#include "VaAttachmentModel.hpp"
#include "VaEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::PointOfInterest::PointOfInterest()
   : Entity("")
   , mManagingAttachment(nullptr)
{
   SetType<PointOfInterest>();
   SetName(std::string("point_of_interest") + std::to_string(GetUniqueId()));

   if (vaEnv.GetStandardViewer())
   {
      vespa::VaAttachmentModel* m =
         vespa::VaEnvironment::CreateAttachment<vespa::VaAttachmentModel>("model", *this, vaEnv.GetStandardViewer());
      SetDraggable(true);
      if (m)
      {
         m->SetIcon("poi");
         m->SetColor(UtColor(1.0f, 1.0f, 0.5f));
         m->SetProxy(true);
      }
   }
}
