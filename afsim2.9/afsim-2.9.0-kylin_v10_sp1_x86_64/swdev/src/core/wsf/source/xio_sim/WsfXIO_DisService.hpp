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

#ifndef WSFXIO_DISSERVICE_HPP
#define WSFXIO_DISSERVICE_HPP

#include "wsf_export.h"

class WsfDisInterface;
class WsfSimulation;
class WsfXIO_DisMessagePkt;
class WsfXIO_DisPlatformInfoPkt;
#include "xio/WsfXIO_Request.hpp"
class WsfXIO_RequestDisDataPkt;
#include "xio/WsfXIO_Service.hpp"
class WsfXIO_Simulation;
class WsfXIO_Subscription;

class WSF_EXPORT WsfXIO_DisRequest : public WsfXIO_Request
{
public:
   WsfXIO_DisRequest(WsfXIO_Connection* aConnectionPtr);
   virtual void HandlePlatformInfo(WsfXIO_DisPlatformInfoPkt& aPkt) = 0;

protected:
   void Initialized() override;
};

class WSF_EXPORT WsfXIO_DisRequestManager : protected WsfXIO_RequestManager
{
public:
   WsfXIO_DisRequestManager(WsfXIO_Interface* aInterfacePtr);
   void AddRequest(WsfXIO_DisRequest* aRequestPtr);
   void RemoveRequest(WsfXIO_DisRequest* aRequestPtr);

private:
   void RequestCancelled(WsfXIO_Request* aRequestPtr) override;
   void HandleMessage(WsfXIO_DisMessagePkt& aPkt);
   void HandlePlatformInfo(WsfXIO_DisPlatformInfoPkt& aPkt);
};

class WSF_EXPORT WsfXIO_DisService : public WsfXIO_Service
{
public:
   WsfXIO_DisService(WsfXIO_Simulation& aXIO_Simulation);

private:
   void HandleRequest(WsfXIO_RequestDisDataPkt& aRequest);

   void PlatformInitialized(double aSimTime, WsfPlatform* aPlatformPtr);

   void PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr);

   void SendPlatformInfo(WsfXIO_Subscription* aSubscription, const WsfPlatform& aPlatform, bool aIsAlive) const;

   void PackPlatformInfo(const WsfPlatform& aPlatform, WsfXIO_DisPlatformInfoPkt& aPkt, bool aIsAlive) const;

   WsfSimulation&   mSimulation;
   WsfDisInterface* mDisInterfacePtr;
};

#endif
