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
#include "PPLI_MessageProxy.hpp"

#include "Message2_2.hpp"
#include "Message2_3.hpp"
#include "Message2_4.hpp"
#include "Message2_5.hpp"

namespace WsfL16
{
using namespace Messages;
namespace Detail
{

template<typename MSG>
struct PPLI_MsgAccessorBase
{
   typedef typename MSG::Extension0Type EXT;
   MSG*                                 mMsg;
   EXT*                                 mExt;
   PPLI_MsgAccessorBase(MSG* aMsgPtr)
      : mMsg(aMsgPtr)
      , mExt(nullptr)
   {
      FindWordT(*mMsg, mExt);
   }

   bool GetLocationLL(double& aLat, double& aLon) const
   {
      if (mExt)
      {
         aLat = mExt->Latitude;
         aLon = mExt->Longitude;
         return true;
      }
      return false;
   }

   void SetLocationLL(double aLat, double aLon)
   {
      if (mExt)
      {
         mExt->Latitude  = aLat;
         mExt->Longitude = aLon;
      }
   }
};
} // namespace Detail
} // namespace WsfL16

namespace WsfL16
{

template<typename MSG>
struct PPLI_MsgAccessor;

template<>
struct PPLI_MsgAccessor<J2_2::Initial> :
   public PPLI_MessageProxy,
   public WsfL16::Detail::PPLI_MsgAccessorBase<J2_2::Initial>
{
   PPLI_MsgAccessor(J2_2::Initial* aMsg)
      : PPLI_MsgAccessorBase<J2_2::Initial>(aMsg)
   {
   }

   bool GetLocationLLA(double& aLat, double& aLon, double& aAlt) const override
   {
      aAlt = mMsg->Altitude;
      return GetLocationLL(aLat, aLon);
   }

   double GetSpeed() const override
   {
      if (mExt == nullptr)
      {
         return 0;
      }
      return mExt->Speed;
   }

   double GetCourse() const override
   {
      if (mExt == nullptr)
      {
         return 0;
      }
      return mExt->Course;
   }

   std::string GetCallsign() const override
   {
      std::string          callsign;
      J2_2::Continuation2* cont;
      if (FindWordT(*mMsg, cont))
      {
         callsign = cont->Callsign;
      }
      return callsign;
   }

   void SetLocationLLA(double aLat, double aLon, double aAlt) override
   {
      SetLocationLL(aLat, aLon);
      mMsg->Altitude = aAlt;
   }

   Environment GetEnvironment() const override { return cAIR; }
};

template<>
struct PPLI_MsgAccessor<J2_3::Initial> :
   public PPLI_MessageProxy,
   public WsfL16::Detail::PPLI_MsgAccessorBase<J2_3::Initial>
{
   PPLI_MsgAccessor(J2_3::Initial* aMsg)
      : PPLI_MsgAccessorBase<J2_3::Initial>(aMsg)
   {
   }

   bool GetLocationLLA(double& aLat, double& aLon, double& aAlt) const override
   {
      aAlt = 0;
      return GetLocationLL(aLat, aLon);
   }

   double GetSpeed() const override
   {
      if (mExt == nullptr)
      {
         return 0;
      }
      return mExt->Speed;
   }

   double GetCourse() const override
   {
      if (mExt == nullptr)
      {
         return 0;
      }
      return mExt->Course;
   }

   std::string GetCallsign() const override
   {
      std::string          callsign;
      J2_3::Continuation2* cont;
      if (FindWordT(*mMsg, cont))
      {
         callsign = cont->Callsign;
      }
      return callsign;
   }

   void SetLocationLLA(double aLat, double aLon, double aAlt) override { SetLocationLL(aLat, aLon); }

   Environment GetEnvironment() const override { return cSURFACE; }
};

template<>
struct PPLI_MsgAccessor<J2_4::Initial> :
   public PPLI_MessageProxy,
   public WsfL16::Detail::PPLI_MsgAccessorBase<J2_4::Initial>
{
   PPLI_MsgAccessor(J2_4::Initial* aMsg)
      : PPLI_MsgAccessorBase<J2_4::Initial>(aMsg)
   {
   }

   bool GetLocationLLA(double& aLat, double& aLon, double& aAlt) const override
   {
      aAlt = -mMsg->Depth;
      return GetLocationLL(aLat, aLon);
   }
   double GetSpeed() const override
   {
      if (mExt == nullptr)
      {
         return 0;
      }
      return mExt->Speed;
   }
   double GetCourse() const override
   {
      if (mExt == nullptr)
      {
         return 0;
      }
      return mExt->Course;
   }

   std::string GetCallsign() const override
   {
      std::string          callsign;
      J2_4::Continuation2* cont;
      if (FindWordT(*mMsg, cont))
      {
         callsign = cont->Callsign;
      }
      return callsign;
   }

   void SetLocationLLA(double aLat, double aLon, double aAlt) override
   {
      SetLocationLL(aLat, aLon);
      mMsg->Depth = -aAlt;
   }
   Environment GetEnvironment() const override { return cSUBSURFACE; }
};

template<>
struct PPLI_MsgAccessor<J2_5::Initial> :
   public PPLI_MessageProxy,
   public WsfL16::Detail::PPLI_MsgAccessorBase<J2_5::Initial>
{
   PPLI_MsgAccessor(J2_5::Initial* aMsg)
      : PPLI_MsgAccessorBase<J2_5::Initial>(aMsg)
   {
   }

   bool GetLocationLLA(double& aLat, double& aLon, double& aAlt) const override
   {
      aAlt = 0;
      return GetLocationLL(aLat, aLon);
   }

   double GetSpeed() const override { return 0; }

   double GetCourse() const override { return 0; }

   std::string GetCallsign() const override
   {
      std::string          callsign;
      J2_5::Continuation1* cont;
      if (FindWordT(*mMsg, cont))
      {
         callsign = cont->Callsign;
      }
      return callsign;
   }

   void SetLocationLLA(double aLat, double aLon, double aAlt) override { SetLocationLL(aLat, aLon); }

   Environment GetEnvironment() const override { return cLAND; }
};

// static
PPLI_MessageProxy* PPLI_MessageProxy::Create(Messages::InitialBase* aMsg)
{
   using namespace WsfL16::Detail;

   PPLI_MessageProxy* i = nullptr;
   int                lbl, sublbl;
   aMsg->GetLabel(lbl, sublbl);
   if (lbl == 2)
   {
      switch (sublbl)
      {
      case 2:
         i = new PPLI_MsgAccessor<J2_2::Initial>(static_cast<J2_2::Initial*>(aMsg));
         break;
      case 3:
         i = new PPLI_MsgAccessor<J2_3::Initial>(static_cast<J2_3::Initial*>(aMsg));
         break;
      case 4:
         i = new PPLI_MsgAccessor<J2_4::Initial>(static_cast<J2_4::Initial*>(aMsg));
         break;
      case 5:
         i = new PPLI_MsgAccessor<J2_5::Initial>(static_cast<J2_5::Initial*>(aMsg));
         break;
      }
   }
   return i;
}

// static
const PPLI_MessageProxy* PPLI_MessageProxy::Create(const Messages::InitialBase* aMsg)
{
   return Create(const_cast<Messages::InitialBase*>(aMsg));
}

} // namespace WsfL16
