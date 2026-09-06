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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "dis/WsfDisDraw.hpp"

#include "DisEntityId.hpp"
#include "DisOther.hpp"
#include "GenMemO.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisUtil.hpp"
#include "ext/WsfExtInterface.hpp"

const int WsfDisDraw::cVERSION = 3;

WsfDisDraw::WsfDisDraw(WsfDisInterface* aInterfacePtr)
   : mCallbackPtr(nullptr)
   , mUseDeprecatedVersion(false)
   , mInterfacePtr(aInterfacePtr)
{
   WsfDrawManager* mgrPtr = WsfDraw::GetDrawManager(WsfDisUtil::GetSimulation(mInterfacePtr));
   if (mgrPtr != nullptr)
   {
      mCallbackPtr = mgrPtr->DrawUpdate.Connect(&WsfDisDraw::UpdateDrawEvents, this);
   }
}

WsfDisDraw::WsfDisDraw(bool aUseDeprecatedVersion, WsfDisInterface* aInterfacePtr)
   : mCallbackPtr(nullptr)
   , mUseDeprecatedVersion(aUseDeprecatedVersion)
   , mInterfacePtr(aInterfacePtr)
{
   WsfDrawManager* mgrPtr = WsfDraw::GetDrawManager(WsfDisUtil::GetSimulation(mInterfacePtr));
   if (mgrPtr != nullptr)
   {
      if (mUseDeprecatedVersion)
      {
         mCallbackPtr = mgrPtr->DrawUpdate.Connect(&WsfDisDraw::UpdateDrawEventsDeprecated, this);
      }
      else
      {
         mCallbackPtr = mgrPtr->DrawUpdate.Connect(&WsfDisDraw::UpdateDrawEvents, this);
      }
   }
}

namespace
{
GenMemO& Write(WsfDisInterface* aInterfacePtr, GenMemO& aIO, const WsfDraw::VertexObject& aVertex)
{
   unsigned char vertexType = (unsigned char)aVertex.mVertexType;
   aIO << vertexType;
   if (vertexType != WsfDraw::cUNSET_VERTEX)
   {
      if (vertexType != WsfDraw::cRELATIVE_ZERO)
      {
         aIO << aVertex.mPosition[0] << aVertex.mPosition[1] << aVertex.mPosition[2];
      }
      if (vertexType != WsfDraw::cABSOLUTE_WCS)
      {
         DisEntityId entityId;
         WsfDisUtil::GetEntityIdFromIndex(aInterfacePtr, aVertex.mPlatformIndex, entityId);
         aIO << entityId;
      }
   }
   return aIO;
}
} // namespace

void WsfDisDraw::UpdateDrawEvents(const std::string& aTarget, const std::string& aLayer, const WsfDraw::DrawEventList& aEvents)
{
   double simTime                = WsfDisUtil::GetSimTime(mInterfacePtr);
   double deferredConnectionTime = WsfDisUtil::GetExtInterface(mInterfacePtr)->GetDeferredConnectionTime();
   bool   myEvent                = !aEvents.empty();
   if (myEvent && !aTarget.empty())
   {
      myEvent                       = false;
      std::string::size_type offset = aTarget.find("DIS");
      if ((offset != std::string::npos) && (offset == 0 || aTarget[offset] == ',') &&
          (offset + 3 == aTarget.length() || aTarget[offset + 3] == ','))
      {
         myEvent = true;
      }
   }
   if (myEvent)
   {
      GenMemO   data(GenBuf::BigEndian, 0);
      DisUint32 rtsHeader(0x12345678), rtsEvent(2028);
      data << rtsHeader << rtsEvent;
      data << cVERSION;
      data << (simTime - deferredConnectionTime);

      int lastEventIndex = static_cast<int>(aEvents.size()) - 1;
      for (size_t i = 0; i < aEvents.size(); ++i)
      {
         const WsfDraw::DrawEvent& event = aEvents[i];

         std::string layer = aLayer.empty() ? "default" : aLayer;
         data.PutString(layer);
         data << (DisUint16)event.mDrawType;
         data << event.mID;
         if (event.mDrawType == WsfDraw::cLINE || event.mDrawType == WsfDraw::cPOINT ||
             event.mDrawType == WsfDraw::cICON || event.mDrawType == WsfDraw::cELLIPSE ||
             event.mDrawType == WsfDraw::cELLIPSOID || event.mDrawType == WsfDraw::cQUADRILATERAL ||
             event.mDrawType == WsfDraw::cTEXT || event.mDrawType == WsfDraw::cTIMER)
         {
            unsigned char r, g, b, a;
            r = (unsigned char)std::min(255.0f, std::max(0.0f, event.mColor.GetData()[0] * 255.0f));
            g = (unsigned char)std::min(255.0f, std::max(0.0f, event.mColor.GetData()[1] * 255.0f));
            b = (unsigned char)std::min(255.0f, std::max(0.0f, event.mColor.GetData()[2] * 255.0f));
            a = (unsigned char)std::min(255.0f, std::max(0.0f, event.mColor.GetData()[3] * 255.0f));
            data << r << g << b << a;
            data << event.mDuration;
            if (event.mDrawType == WsfDraw::cLINE)
            {
               Write(mInterfacePtr, data, event.mVerts[0]);
               Write(mInterfacePtr, data, event.mVerts[1]);
               data << (DisUint8)event.mLineSize << (DisUint8)event.mLineStyle;
            }
            else if (event.mDrawType == WsfDraw::cPOINT)
            {
               Write(mInterfacePtr, data, event.mVerts[0]);
               data << (unsigned char)event.mPointSize;
            }
            else if (event.mDrawType == WsfDraw::cICON)
            {
               Write(mInterfacePtr, data, event.mVerts[0]);
               data << event.mHeading;
               data.PutString(event.mIcon);
            }
            else if (event.mDrawType == WsfDraw::cELLIPSE)
            {
               Write(mInterfacePtr, data, event.mVerts[0]);
               data << (DisUint8)event.mLineSize << (DisUint8)event.mLineStyle;
               data << (DisUint8)event.mEllipseMode;
               data << event.mHeading;
               data << event.mAxisA;
               data << event.mAxisB;
            }
            else if (event.mDrawType == WsfDraw::cELLIPSOID)
            {
               Write(mInterfacePtr, data, event.mVerts[0]);
               data << (DisUint8)event.mLineSize << (DisUint8)event.mLineStyle;
               data << (DisUint8)event.mEllipseMode;
               data << event.mHeading << event.mPitch << event.mRoll;
               data << event.mAxisA << event.mAxisB << event.mAxisC;
            }
            else if (event.mDrawType == WsfDraw::cQUADRILATERAL)
            {
               for (const auto& vert : event.mVerts)
               {
                  Write(mInterfacePtr, data, vert);
               }
            }
            else if (event.mDrawType == WsfDraw::cTEXT)
            {
               Write(mInterfacePtr, data, event.mVerts[0]);
               data << (unsigned char)event.mTextSize;
               data.PutString(event.mText);
            }
            else if (event.mDrawType == WsfDraw::cTIMER)
            {
               Write(mInterfacePtr, data, event.mVerts[0]);
               data << (unsigned char)event.mPointSize;
            }
         }
         // Maximum DIS PDU size is 2048
         if ((int)i == lastEventIndex || data.GetPutOffset() > 1500)
         {
            auto pduPtr = ut::make_unique<DisOther>();
            pduPtr->SetUserData((unsigned char*)data.GetBuffer(), (DisUint16)data.GetPutOffset());
            WsfDisUtil::PutPdu(mInterfacePtr, simTime, std::move(pduPtr));
            if ((int)i != lastEventIndex)
            {
               data.SetPutOffset(0);
               data << rtsHeader << rtsEvent;
               data << cVERSION;
               data << (simTime - deferredConnectionTime);
            }
         }
      }
   }
}

void WsfDisDraw::UpdateDrawEventsDeprecated(const std::string&            aTarget,
                                            const std::string&            aLayer,
                                            const WsfDraw::DrawEventList& aEvents)
{
   enum
   {
      cLINE_DEP,
      cPOINT_DEP,
      cICON_DEP,
      cERASE_DEP,
      cNONE_DEP
   };
   double simTime                = WsfDisUtil::GetSimTime(mInterfacePtr);
   double deferredConnectionTime = WsfDisUtil::GetExtInterface(mInterfacePtr)->GetDeferredConnectionTime();
   bool   myEvent                = !aEvents.empty();
   if (myEvent && !aTarget.empty())
   {
      myEvent                       = false;
      std::string::size_type offset = aTarget.find("DIS");
      if ((offset != std::string::npos) && (offset == 0 || aTarget[offset] == ',') &&
          (offset + 3 == aTarget.length() || aTarget[offset + 3] == ','))
      {
         myEvent = true;
      }
   }
   if (myEvent)
   {
      GenMemO   data(GenBuf::BigEndian, 0);
      DisUint32 rtsHeader(0x12345678), rtsEvent(2027);
      data << rtsHeader << rtsEvent;
      data << (simTime - deferredConnectionTime);

      int lastEventIndex = static_cast<int>(aEvents.size()) - 1;
      for (size_t i = 0; i < aEvents.size(); ++i)
      {
         const WsfDraw::DrawEvent& event = aEvents[i];

         // Map the new draw type enumeration to the deprecated enumeration.
         int drawType = event.mDrawType;
         switch (drawType)
         {
         case WsfDraw::cLINE:
            drawType = cLINE_DEP;
            break;
         case WsfDraw::cPOINT:
            drawType = cPOINT_DEP;
            break;
         case WsfDraw::cICON:
            drawType = cICON_DEP;
            break;
         case WsfDraw::cELLIPSE:
            drawType = cNONE_DEP;
            break;
         case WsfDraw::cERASE:
            drawType = cERASE_DEP;
            break;
         case WsfDraw::cNONE:
            drawType = cNONE_DEP;
            break;
         default:
            drawType = cNONE_DEP;
            break;
         }

         if (drawType == cNONE_DEP)
         {
            continue;
            ;
         }

         data << (DisUint16)drawType;
         data << event.mID;
         if (drawType == cLINE_DEP || drawType == cPOINT_DEP || drawType == cICON_DEP)
         {
            unsigned char r, g, b, a;
            r = (unsigned char)std::min(255.0f, std::max(0.0f, event.mColor.GetData()[0] * 255.0f));
            g = (unsigned char)std::min(255.0f, std::max(0.0f, event.mColor.GetData()[1] * 255.0f));
            b = (unsigned char)std::min(255.0f, std::max(0.0f, event.mColor.GetData()[2] * 255.0f));
            a = (unsigned char)std::min(255.0f, std::max(0.0f, event.mColor.GetData()[3] * 255.0f));
            data << r << g << b << a;
            data << event.mDuration;
            if (drawType == cLINE_DEP)
            {
               Write(mInterfacePtr, data, event.mVerts[0]);
               Write(mInterfacePtr, data, event.mVerts[1]);
               data << (DisUint8)event.mLineSize << (DisUint8)event.mLineStyle;
            }
            else if (drawType == cPOINT_DEP)
            {
               Write(mInterfacePtr, data, event.mVerts[0]);
               data << (unsigned char)event.mPointSize;
            }
            else if (drawType == cICON_DEP)
            {
               Write(mInterfacePtr, data, event.mVerts[0]);
               data << event.mHeading;
               data.PutString(event.mIcon);
            }
         }
         // Maximum DIS PDU size is 2048
         if ((int)i == lastEventIndex || data.GetPutOffset() > 1500)
         {
            auto pduPtr = ut::make_unique<DisOther>();
            pduPtr->SetUserData((unsigned char*)data.GetBuffer(), (DisUint16)data.GetPutOffset());
            WsfDisUtil::PutPdu(mInterfacePtr, simTime, std::move(pduPtr));
            if ((int)i != lastEventIndex)
            {
               data.SetPutOffset(0);
               data << rtsHeader << rtsEvent;
               data << (simTime - deferredConnectionTime);
            }
         }
      }
   }
}
