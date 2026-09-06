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

#include "xio_sim/WsfXIO_DrawService.hpp"

#include "UtCast.hpp"
#include "WsfDraw.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio_sim/WsfXIO_Simulation.hpp"

WsfXIO_DrawService::WsfXIO_DrawService(WsfXIO_Simulation& aXIO_Simulation)
   : WsfXIO_Service(aXIO_Simulation.GetInterface())
   , mDrawCallbackPtr(nullptr)
{
   mCallbacks += mInterfacePtr->Connect(&WsfXIO_DrawService::HandleRequest, this);
   mDrawManagerPtr = WsfDraw::GetDrawManager(aXIO_Simulation.GetSimulation());
}

void WsfXIO_DrawService::HandleRequest(WsfXIO_RequestPkt& aPkt)
{
   if (aPkt.mSubscriptionType == WsfXIO_RequestPkt::cDRAW)
   {
      WsfXIO_Subscription* subscriptionPtr = CreateSubscription(aPkt);
      mDestination.Add(subscriptionPtr->GetDataConnection());
      if (mDrawCallbackPtr == nullptr)
      {
         mDrawCallbackPtr = mDrawManagerPtr->DrawUpdate.Connect(&WsfXIO_DrawService::DrawUpdate, this);
      }
   }
}

void WsfXIO_DrawService::SubscriptionCancelled(WsfXIO_Subscription* aSubscriptionPtr)
{
   mDestination.Remove(aSubscriptionPtr->GetDataConnection());
   if (mDestination.IsEmpty())
   {
      mDrawCallbackPtr.reset();
   }
}

namespace
{
void SaveVert(WsfXIO_DrawPkt::Vertex& aPkt, const WsfDraw::VertexObject& aVertex)
{
   aPkt.mPosition              = aVertex.mPosition;
   aPkt.mRelativePlatformIndex = static_cast<int32_t>(aVertex.mPlatformIndex);
   aPkt.mVertexType            = aVertex.mVertexType;
}
} // namespace

void WsfXIO_DrawService::DrawUpdate(const std::string&            aTarget,
                                    const std::string&            aLayer,
                                    const WsfDraw::DrawEventList& aDrawEvents)
{
   // Since we could be servicing non-reliable connections, make sure the packet size doesn't get too large
   // These numbers are conservative estimates
   size_t cBASE_SIZE   = 50;
   size_t cENTITY_SIZE = sizeof(WsfXIO_DrawPkt::Entity) + 20;
   size_t cREMOVE_SIZE = 25;
   size_t cSEND_SIZE   = 1200;

   size_t         currentSize = cBASE_SIZE;
   WsfXIO_DrawPkt pkt;
   for (size_t i = 0; i < aDrawEvents.size(); ++i)
   {
      const WsfDraw::DrawEvent& evt = aDrawEvents[i];
      if (evt.mDrawType != WsfDraw::cERASE)
      {
         currentSize += cENTITY_SIZE;
         WsfXIO_DrawPkt::Entity e;
         for (int c = 0; c < 4; ++c)
         {
            e.mColor[c] = (uint8_t)(255 * evt.mColor[c]);
         }
         e.mDrawType    = evt.mDrawType;
         e.mDuration    = evt.mDuration;
         e.mHeading     = evt.mHeading;
         e.mPitch       = evt.mPitch;
         e.mRoll        = evt.mRoll;
         e.mLayerName   = pkt.mDictionary.Encode(aLayer);
         e.mIconName    = pkt.mDictionary.Encode(evt.mIcon);
         e.mText        = pkt.mDictionary.Encode(evt.mText);
         e.mId          = ut::safe_cast<uint32_t, unsigned int>(evt.mID);
         e.mLineSize    = ut::safe_cast<uint8_t, int>(evt.mLineSize);
         e.mLineStyle   = ut::safe_cast<uint8_t, int>(evt.mLineStyle);
         e.mPointSize   = ut::safe_cast<uint8_t, int>(evt.mPointSize);
         e.mTextSize    = ut::safe_cast<uint8_t, int>(evt.mTextSize);
         e.mEllipseMode = evt.mEllipseMode;
         e.mAxisA       = evt.mAxisA;
         e.mAxisB       = evt.mAxisB;
         e.mAxisC       = evt.mAxisC;
         SaveVert(e.mVerts[0], evt.mVerts[0]);
         SaveVert(e.mVerts[1], evt.mVerts[1]);
         pkt.mEntities.push_back(e);
      }
      else
      {
         currentSize += cREMOVE_SIZE;
         pkt.mRemovedIds.push_back(std::make_pair(evt.mID, pkt.mDictionary.Encode(aLayer)));
      }
      if (currentSize > cSEND_SIZE && i < (aDrawEvents.size() - 1))
      {
         mDestination.Send(pkt);
         pkt.mRemovedIds.clear();
         pkt.mEntities.clear();
         pkt.mDictionary.Clear();
      }
   }
   mDestination.Send(pkt);
}
