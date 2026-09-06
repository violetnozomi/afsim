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

#include "RvSatelliteTetherDockWidget.hpp"

#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "RvSatelliteTetherAttachmentTrace.hpp"
#include "RvSpaceEventPipeClasses.hpp"
#include "VaUtils.hpp"
#include "WkfPlatform.hpp"
#include "WkfUtils.hpp"

RvSatelliteTether::DockWidget::DockWidget(QString         aObjectName,
                                          QString         aPlatformName,
                                          unsigned int    aId,
                                          QMainWindow*    aParent /*= nullptr*/,
                                          Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : wkf::SatelliteTetherDockWidget(aObjectName, aPlatformName, aId, aParent, aFlags)
{
}

wkf::AttachmentSatelliteTetherTrace* RvSatelliteTether::DockWidget::MakeTrace(wkf::Platform&   aSrc,
                                                                              vespa::VaViewer& aViewer,
                                                                              wkf::Platform&   aTgt,
                                                                              const QColor&    aColor)
{
   return vespa::make_attachment<AttachmentTrace>(aSrc, aViewer, aTgt, aColor);
}

void RvSatelliteTether::DockWidget::AddTrack(wkf::Platform* aPlatformPtr)
{
   Update(*rvEnv.GetData(), aPlatformPtr->GetName());
   AddTrackToDock(aPlatformPtr);
}

unsigned int RvSatelliteTether::DockWidget::Update(const rv::ResultData& aData, const std::string& aPlatform)
{
   unsigned int retVal = 0;
   auto         p      = aData.GetDb()->FindPlatformByName(aPlatform);
   auto         orbel  = p->FindFirstBefore<rv::MsgOrbitalElements>(aData.GetSimTime());
   unsigned int index(0);

   QColor color = wkf::util::GetHashColor(QString::fromStdString(aPlatform));
   if (orbel)
   {
      index = orbel->GetMessageIndex();
      if (orbel->orbitColorValid())
      {
         rv::Vec3f orbitColor = orbel->orbitColor();
         color                = QColor(orbitColor.x() * 255, orbitColor.y() * 255, orbitColor.z() * 255);
      }
   }
   else
   {
      mOrbitIndex[aPlatform] = 0;
   }

   if (index != mOrbitIndex[aPlatform])
   {
      auto scenarioPtr = vaEnv.GetStandardScenario();
      if (scenarioPtr)
      {
         auto platformPtr = scenarioPtr->FindPlatformByName(aPlatform);
         if (platformPtr)
         {
            UpdateColor(platformPtr, color);
         }
      }
   }

   mOrbitIndex[aPlatform] = index;
   DataUpdate();
   return retVal;
}
