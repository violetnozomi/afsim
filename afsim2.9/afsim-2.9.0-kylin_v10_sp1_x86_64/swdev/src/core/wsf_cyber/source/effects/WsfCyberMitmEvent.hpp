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

#ifndef WSFCYBERMITMEVENT_HPP
#define WSFCYBERMITMEVENT_HPP

#include <functional>
#include <memory>

#include "WsfCyberMitmLayer.hpp"
#include "WsfCyberMitmMessage.hpp"
#include "WsfEvent.hpp"
#include "WsfMessage.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace cyber
{

// Class for MITM Delay Events.
class MITM_DelayEvent : public WsfEvent
{
public:
   MITM_DelayEvent(double aSimTime, std::unique_ptr<MitmMessage> aMessage, comm::Comm* aXmtrPtr = nullptr)
      : WsfEvent(aSimTime)
      , mMessage(std::move(aMessage))
      , mXmtrPtr(aXmtrPtr)
   {
   }

   EventDisposition Execute() override
   {
      // Ensure the target still exists in the framework.
      auto platformPtr = GetSimulation()->GetPlatformByName(mMessage->GetDelayParameters().mPlatformName);
      if (platformPtr)
      {
         // Ensure the comm still exists.
         auto commPtr = platformPtr->GetComponent<comm::Comm>(mMessage->GetDelayParameters().mCommName);
         if (commPtr)
         {
            // The MITM layer indicated is removed if the effect ends. Query its existence to see if
            // the effect is still ongoing.
            auto layerPtr = commPtr->GetProtocolStack().GetLayerByID(mMessage->GetDelayParameters().mMITM_LayerID);
            if (layerPtr)
            {
               // The MITM layer still exists. Let that layer handle the message delay.
               MITM_Layer::ResolveDelay(GetTime(), *layerPtr->GetCommLayerImp(), std::move(mMessage), mXmtrPtr);
            }
            else
            {
               // The MITM layer no longer exists. Attempt finding another layer that was closest to the MITM
               // layer at the time of the delayed transmission.
               for (auto layerID : mMessage->GetDelayParameters().mNextLayerIDs)
               {
                  layerPtr = commPtr->GetProtocolStack().GetLayerByID(layerID);
                  if (layerPtr)
                  {
                     MITM_Layer::ResolveDelay(GetTime(), *layerPtr->GetCommLayerImp(), std::move(mMessage), mXmtrPtr);
                     break;
                  }
               }
            }
         }
      }

      return cDELETE;
   }

   ~MITM_DelayEvent() override = default;

protected:
   std::unique_ptr<MitmMessage> mMessage;
   comm::Comm*                  mXmtrPtr{nullptr};
};

} // namespace cyber
} // namespace wsf
#endif
