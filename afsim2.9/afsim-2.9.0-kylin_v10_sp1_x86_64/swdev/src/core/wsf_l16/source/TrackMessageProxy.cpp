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
#include "TrackMessageProxy.hpp"

#include "DisEntityId.hpp"
#include "Interface.hpp"
#include "Message3_2.hpp"
#include "Message3_3.hpp"
#include "Message3_4.hpp"
#include "Message3_5.hpp"
#include "Message3_6.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtVec3.hpp"

namespace WsfL16
{
using namespace Messages;

namespace Detail
{

template<typename MSG>
struct TrackMsgAccessorBase
{
   typedef typename MSG::Extension0Type    EXT0;
   typedef typename MSG::Extension1Type    EXT1;
   typedef typename MSG::Continuation1Type CONT1;
   MSG*                                    mMsg;
   EXT0*                                   mExt0;
   EXT1*                                   mExt1;
   CONT1*                                  mCont1;

   TrackMsgAccessorBase(MSG* aMsgPtr)
      : mMsg(aMsgPtr)
      , mExt0(nullptr)
      , mExt1(nullptr)
      , mCont1(nullptr)
   {
      FindWordT(*mMsg, mExt0);
      FindWordT(*mMsg, mExt1);
      FindWordT(*mMsg, mCont1);
   }

   bool GetLocationLL(double& aLat, double& aLon) const
   {
      if (mExt0)
      {
         aLat = mExt0->Latitude;
         aLon = mExt0->Longitude;
         return true;
      }
      return false;
   }

   void SetLocationLL(double aLat, double aLon)
   {
      if (mExt0)
      {
         mExt0->Latitude  = aLat;
         mExt0->Longitude = aLon;
      }
   }
};

template<typename MSG>
struct TrackMsgAccessor;

template<>
struct TrackMsgAccessor<J3_2::Initial> : public TrackMessageProxy, public TrackMsgAccessorBase<J3_2::Initial>
{
   TrackMsgAccessor(J3_2::Initial* aMsg)
      : TrackMsgAccessorBase<J3_2::Initial>(aMsg)
   {
   }

   bool GetLocationLLA(double& aLat, double& aLon, double& aAlt) const override
   {
      aAlt = mMsg->Altitude;
      return GetLocationLL(aLat, aLon);
   }
   double GetSpeed() const override
   {
      if (mExt0 == nullptr)
      {
         return 0;
      }
      return mExt0->Speed;
   }
   double GetCourse() const override
   {
      if (mExt0 == nullptr)
      {
         return 0;
      }
      return mExt0->Course;
   }
   int         GetTrackNumber() const override { return (int)mMsg->TrackNumber.GetRawData(); }
   std::string GetStringTrackNumber() const override { return mMsg->TrackNumber; }
   unsigned    GetSourceTrackNumber() const override { return mMsg->GetHeader().mSourceTrackNumber; }
   void        GetTrackDisEntityId(WsfL16::Interface* aL16_InterfacePtr, DisEntityId& aEntityId) const override
   {
      if (mExt1)
      {
         aEntityId.Set(mExt1->DisSite, mExt1->DisApplication, mExt1->DisEntityId);
      }
   }
   double GetTrackQuality() const override { return (double)mMsg->TrackQuality.GetRawData(); }
   double GetTrackQualityErrorDistance95CEP() const override { return mMsg->TrackQuality.GetDistance95CEP(); }
   int    GetIdentity() const override { return mMsg->Identity; }

   int GetSpecificType() const override
   {
      if (mCont1)
      {
         if (mCont1->IsAirSpecificType)
         {
            return mCont1->AirSpecificType;
         }
      }
      return 0;
   }

   void SetLocationLLA(double aLat, double aLon, double aAlt) override
   {
      SetLocationLL(aLat, aLon);
      mMsg->Altitude = aAlt;
   }
   Environment GetEnvironment() const override { return cAIR; }
};

template<>
struct TrackMsgAccessor<J3_3::Initial> : public TrackMessageProxy, public TrackMsgAccessorBase<J3_3::Initial>
{
   TrackMsgAccessor(J3_3::Initial* aMsg)
      : TrackMsgAccessorBase<J3_3::Initial>(aMsg)
   {
   }

   bool GetLocationLLA(double& aLat, double& aLon, double& aAlt) const override
   {
      aAlt = 0;
      return GetLocationLL(aLat, aLon);
   }
   double GetSpeed() const override
   {
      if (mExt0 == nullptr)
      {
         return 0;
      }
      return mExt0->Speed;
   }
   double GetCourse() const override
   {
      if (mExt0 == nullptr)
      {
         return 0;
      }
      return mExt0->Course;
   }
   int         GetTrackNumber() const override { return (int)mMsg->TrackNumber.GetRawData(); }
   std::string GetStringTrackNumber() const override { return mMsg->TrackNumber; }
   unsigned    GetSourceTrackNumber() const override { return mMsg->GetHeader().mSourceTrackNumber; }
   void        GetTrackDisEntityId(WsfL16::Interface* aL16_InterfacePtr, DisEntityId& aEntityId) const override
   {
      if (mExt1)
      {
         aEntityId.Set(mExt1->DisSite, mExt1->DisApplication, mExt1->DisEntityId);
      }
   }
   double GetTrackQuality() const override { return (double)mMsg->TrackQuality.GetRawData(); }
   double GetTrackQualityErrorDistance95CEP() const override { return mMsg->TrackQuality.GetDistance95CEP(); }
   int    GetIdentity() const override { return mMsg->Identity; }

   int GetSpecificType() const override
   {
      if (mCont1)
      {
         if (mCont1->IsAirSpecificType)
         {
            return mCont1->SurfacePlatform;
         }
      }
      return 0;
   }

   void        SetLocationLLA(double aLat, double aLon, double aAlt) override { SetLocationLL(aLat, aLon); }
   Environment GetEnvironment() const override { return cSURFACE; }
};

template<>
struct TrackMsgAccessor<J3_4::Initial> : public TrackMessageProxy, public TrackMsgAccessorBase<J3_4::Initial>
{
   TrackMsgAccessor(J3_4::Initial* aMsg)
      : TrackMsgAccessorBase<J3_4::Initial>(aMsg)
   {
   }

   bool GetLocationLLA(double& aLat, double& aLon, double& aAlt) const override
   {
      aAlt = 0;
      return GetLocationLL(aLat, aLon);
   }
   double GetSpeed() const override
   {
      if (mExt0 == nullptr)
      {
         return 0;
      }
      return mExt0->Speed;
   }
   double GetCourse() const override
   {
      if (mExt0 == nullptr)
      {
         return 0;
      }
      return mExt0->Course;
   }
   int         GetTrackNumber() const override { return (int)mMsg->TrackNumber.GetRawData(); }
   std::string GetStringTrackNumber() const override { return mMsg->TrackNumber; }
   unsigned    GetSourceTrackNumber() const override { return mMsg->GetHeader().mSourceTrackNumber; }
   void        GetTrackDisEntityId(WsfL16::Interface* aL16_InterfacePtr, DisEntityId& aEntityId) const override
   {
      if (mExt1)
      {
         aEntityId.Set(mExt1->DisSite, mExt1->DisApplication, mExt1->DisEntityId);
      }
   }
   double GetTrackQuality() const override
   {
      // Track Quality is not applicable to subsurface tracks
      return 7.5;
   }
   double GetTrackQualityErrorDistance95CEP() const override { return 0; }
   int    GetIdentity() const override { return mMsg->Identity; }

   int GetSpecificType() const override { return mMsg->SubsurfacePlatform; }

   void        SetLocationLLA(double aLat, double aLon, double /*aAlt*/) override { SetLocationLL(aLat, aLon); }
   Environment GetEnvironment() const override { return cSUBSURFACE; }
};

template<>
struct TrackMsgAccessor<J3_5::Initial> : public TrackMessageProxy, public TrackMsgAccessorBase<J3_5::Initial>
{
   TrackMsgAccessor(J3_5::Initial* aMsg)
      : TrackMsgAccessorBase<J3_5::Initial>(aMsg)
   {
   }
   bool GetLocationLLA(double& aLat, double& aLon, double& aAlt) const override
   {
      aAlt = mMsg->Altitude;
      return GetLocationLL(aLat, aLon);
   }
   double GetSpeed() const override
   {
      if (mExt0 == nullptr)
      {
         return 0;
      }
      return mExt0->Speed;
   }
   double GetCourse() const override
   {
      if (mExt0 == nullptr)
      {
         return 0;
      }
      return mExt0->Course;
   }
   int         GetTrackNumber() const override { return (int)mMsg->TrackNumber.GetRawData(); }
   std::string GetStringTrackNumber() const override { return mMsg->TrackNumber; }
   unsigned    GetSourceTrackNumber() const override { return mMsg->GetHeader().mSourceTrackNumber; }
   void        GetTrackDisEntityId(WsfL16::Interface* aL16_InterfacePtr, DisEntityId& aEntityId) const override
   {
      if (mExt1)
      {
         aEntityId.Set(mExt1->DisSite, mExt1->DisApplication, mExt1->DisEntityId);
      }
   }
   double GetTrackQuality() const override { return (double)mMsg->TrackQuality.GetRawData(); }
   double GetTrackQualityErrorDistance95CEP() const override { return mMsg->TrackQuality.GetDistance95CEP(); }
   int    GetIdentity() const override { return mMsg->Identity; }

   int GetSpecificType() const override
   {
      if (mCont1)
      {
         if (mCont1->IsLandSpecificType)
         {
            return mCont1->LandSpecificType;
         }
      }
      return 0;
   }

   void        SetLocationLLA(double aLat, double aLon, double aAlt) override { SetLocationLL(aLat, aLon); }
   Environment GetEnvironment() const override { return cLAND; }
};


template<>
struct TrackMsgAccessor<J3_6::Initial> : public TrackMessageProxy, public TrackMsgAccessorBase<J3_6::Initial>
{
   TrackMsgAccessor(J3_6::Initial* aMsg)
      : TrackMsgAccessorBase<J3_6::Initial>(aMsg)
   {
   }
   bool GetLocationLLA(double& aLat, double& aLon, double& aAlt) const override
   {
      if (!mExt0 || !mExt1 || mExt0->WcsX.IsNoStatement())
      {
         return false;
      }
      double ECEF[3] = {mExt0->WcsX, mExt0->WcsY, mExt1->WcsZ};
      UtEllipsoidalEarth::ConvertECEFToLLA(ECEF, aLat, aLon, aAlt);
      return true;
   }
   bool GetVelocityWCS(double aWCS[3]) const override
   {
      if (!mExt0 || !mExt1 || mExt0->WcsVelX.IsNoStatement())
      {
         return false;
      }
      aWCS[0] = mExt0->WcsVelX;
      aWCS[1] = mExt1->WcsVelY;
      aWCS[2] = mExt1->WcsVelZ;
      return true;
   }
   double GetSpeed() const override
   {
      double WCS[3];
      if (GetVelocityWCS(WCS))
      {
         return UtVec3d::Magnitude(WCS);
      }
      return 0.0;
   }
   double      GetCourse() const override { return 0; }
   int         GetTrackNumber() const override { return (int)mMsg->TrackNumber.GetRawData(); }
   std::string GetStringTrackNumber() const override { return mMsg->TrackNumber; }
   unsigned    GetSourceTrackNumber() const override { return mMsg->GetHeader().mSourceTrackNumber; }
   void        GetTrackDisEntityId(WsfL16::Interface* aL16_InterfacePtr, DisEntityId& aEntityId) const override
   {
      if (aL16_InterfacePtr->IncludeJ3xTrackDisEntityId())
      {
         J3_6::Extension2* ext = (J3_6::Extension2*)mMsg->GetExtension(2);
         if (ext)
         {
            aEntityId.Set(ext->DisSite, ext->DisApplication, ext->DisEntityId);
         }
      }
   }
   double GetTrackQuality() const override
   {
      return 0.0; // this doesn't match the other track quality enocdings!
   }
   double GetTrackQualityErrorDistance95CEP() const override { return mMsg->TrackQuality.GetDistance95CEP(); }
   int    GetIdentity() const override { return mMsg->Identity; }

   int GetSpecificType() const override
   {
      // not yet implemented in message
      return 0;
   }

   void SetLocationLLA(double aLat, double aLon, double aAlt) override
   {
      double ECEF[3];
      UtEllipsoidalEarth::ConvertLLAToECEF(aLat, aLon, aAlt, ECEF);
      mExt0->WcsX = ECEF[0];
      mExt0->WcsY = ECEF[1];
      mExt1->WcsZ = ECEF[2];
   }
   Environment GetEnvironment() const override { return cSPACE; }
};
} // namespace Detail
} // namespace WsfL16

namespace WsfL16
{

// static
TrackMessageProxy* TrackMessageProxy::Create(Messages::InitialBase* aMsg)
{
   using namespace WsfL16::Detail;

   TrackMessageProxy* i = nullptr;
   int                lbl, sublbl;
   aMsg->GetLabel(lbl, sublbl);
   if (lbl == 3)
   {
      switch (sublbl)
      {
      case 2:
         i = new TrackMsgAccessor<J3_2::Initial>(static_cast<J3_2::Initial*>(aMsg));
         break;
      case 3:
         i = new TrackMsgAccessor<J3_3::Initial>(static_cast<J3_3::Initial*>(aMsg));
         break;
      case 4:
         i = new TrackMsgAccessor<J3_4::Initial>(static_cast<J3_4::Initial*>(aMsg));
         break;
      case 5:
         i = new TrackMsgAccessor<J3_5::Initial>(static_cast<J3_5::Initial*>(aMsg));
         break;
      case 6:
         i = new TrackMsgAccessor<J3_6::Initial>(static_cast<J3_6::Initial*>(aMsg));
         break;
      }
   }
   return i;
}

// static
const TrackMessageProxy* TrackMessageProxy::Create(const Messages::InitialBase* aMsg)
{
   return Create(const_cast<Messages::InitialBase*>(aMsg));
}

} // namespace WsfL16
