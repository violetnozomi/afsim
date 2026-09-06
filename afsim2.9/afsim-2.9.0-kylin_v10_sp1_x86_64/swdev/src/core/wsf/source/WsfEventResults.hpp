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

#ifndef WSFEVENTRESULTS_HPP
#define WSFEVENTRESULTS_HPP

#include "wsf_export.h"

#include <ostream>

class WsfBehaviorTreeNode;
class WsfCallback;
#include "WsfComm.hpp"
#include "WsfEventResult.hpp"
#include "WsfEventUtils.hpp"
#include "WsfExchange.hpp"
class WsfFuelingOperation;
class WsfImage;
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
class WsfProcessor;
class WsfSensorMode;
class WsfTask;

namespace wsf
{
namespace comm
{
class Address;
class Result;
class Network;

namespace router
{
class ProtocolInterface;
class Router;
} // namespace router
} // namespace comm

namespace event
{

// ===================================================================================================
class WSF_EXPORT BehaviorTreeNodeChildren : public Result
{
public:
   static constexpr const char* cNAME = "BTREE_NODE_CHILDREN";
   BehaviorTreeNodeChildren(double aSimTime, WsfBehaviorTreeNode* aNodePtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mNodePtr(aNodePtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   WsfBehaviorTreeNode* GetBehaviorTreeNode() const { return mNodePtr; }

private:
   WsfBehaviorTreeNode* mNodePtr{nullptr};
};

// ===================================================================================================
class WSF_EXPORT BehaviorTreeNodeExec : public Result
{
public:
   static constexpr const char* cNAME = "BTREE_NODE_EXEC";
   BehaviorTreeNodeExec(double aSimTime, WsfBehaviorTreeNode* aNodePtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mNodePtr(aNodePtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   WsfBehaviorTreeNode* GetBehaviorTreeNode() const { return mNodePtr; }

private:
   WsfBehaviorTreeNode* mNodePtr{nullptr};
};

// ===================================================================================================
class WSF_EXPORT Comment : public Result
{
public:
   static constexpr const char* cNAME = "COMMENT";
   Comment(double aSimTime, WsfPlatform* aPlatformPtr, std::string aComment, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mPlatformPtr(aPlatformPtr)
      , mComment(std::move(aComment))
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfPlatform* GetPlatform() const { return mPlatformPtr; }
   const std::string& GetComment() const { return mComment; }

private:
   WsfPlatform* mPlatformPtr;
   std::string  mComment;
};

// ===================================================================================================
class WSF_EXPORT CommAddedToManager : public Result
{
public:
   static constexpr const char* cNAME = "COMM_ADDED_TO_MANAGER";
   CommAddedToManager(double aSimTime, comm::Comm* aCommPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mCommPtr(aCommPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetComm() const { return mCommPtr; }

private:
   comm::Comm* mCommPtr;
};

// ===================================================================================================
class WSF_EXPORT CommRemovedFromManager : public Result
{
public:
   static constexpr const char* cNAME = "COMM_REMOVED_FROM_MANAGER";
   CommRemovedFromManager(double aSimTime, comm::Comm* aCommPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mCommPtr(aCommPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetComm() const { return mCommPtr; }

private:
   comm::Comm* mCommPtr;
};

// ===================================================================================================
class WSF_EXPORT CommAddedToLocal : public Result
{
public:
   static constexpr const char* cNAME = "COMM_ADDED_TO_LOCAL";
   CommAddedToLocal(double                           aSimTime,
                    comm::router::Router*            aLocalRouterPtr,
                    comm::router::ProtocolInterface* aProtocolPtr,
                    comm::Address*                   aAddedAddressPtr,
                    Settings                         aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mLocalRouterPtr(aLocalRouterPtr)
      , mProtocolPtr(aProtocolPtr)
      , mAddedAddressPtr(aAddedAddressPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::router::Router*            GetLocalRouter() const { return mLocalRouterPtr; }
   const comm::router::ProtocolInterface* GetProtocol() const { return mProtocolPtr; }
   const comm::Address*                   GetAddedAddress() const { return mAddedAddressPtr; }

private:
   comm::router::Router*            mLocalRouterPtr;
   comm::router::ProtocolInterface* mProtocolPtr;
   comm::Address*                   mAddedAddressPtr;
};

// ===================================================================================================
class WSF_EXPORT CommRemovedFromLocal : public Result
{
public:
   static constexpr const char* cNAME = "COMM_REMOVED_FROM_LOCAL";
   CommRemovedFromLocal(double                           aSimTime,
                        comm::router::Router*            aLocalRouterPtr,
                        comm::router::ProtocolInterface* aProtocolPtr,
                        comm::Address*                   aRemovedAddressPtr,
                        Settings                         aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mLocalRouterPtr(aLocalRouterPtr)
      , mProtocolPtr(aProtocolPtr)
      , mRemovedAddressPtr(aRemovedAddressPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::router::Router*            GetLocalRouter() const { return mLocalRouterPtr; }
   const comm::router::ProtocolInterface* GetProtocol() const { return mProtocolPtr; }
   const comm::Address*                   GetRemovedAddress() const { return mRemovedAddressPtr; }

private:
   comm::router::Router*            mLocalRouterPtr;
   comm::router::ProtocolInterface* mProtocolPtr;
   comm::Address*                   mRemovedAddressPtr;
};

// ===================================================================================================
class WSF_EXPORT CommBroken : public Result
{
public:
   static constexpr const char* cNAME = "COMM_BROKEN";
   CommBroken(double aSimTime, comm::Comm* aCommPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mCommPtr(aCommPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetComm() const { return mCommPtr; }

private:
   comm::Comm* mCommPtr;
};

// ===================================================================================================
class WSF_EXPORT CommFrequencyChanged : public Result
{
public:
   static constexpr const char* cNAME = "COMM_FREQUENCY_CHANGED";
   CommFrequencyChanged(double aSimTime, comm::Comm* aCommPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mCommPtr(aCommPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetComm() const { return mCommPtr; }

private:
   comm::Comm* mCommPtr;
};

// ===================================================================================================
class WSF_EXPORT CommNonOperational : public Result
{
public:
   static constexpr const char* cNAME = "COMM_NON_OPERATIONAL";
   CommNonOperational(double aSimTime, comm::Comm* aCommPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mCommPtr(aCommPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetComm() const { return mCommPtr; }

private:
   comm::Comm* mCommPtr;
};

// ===================================================================================================
class WSF_EXPORT CommOperational : public Result
{
public:
   static constexpr const char* cNAME = "COMM_OPERATIONAL";
   CommOperational(double aSimTime, comm::Comm* aCommPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mCommPtr(aCommPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetComm() const { return mCommPtr; }

private:
   comm::Comm* mCommPtr;
};

// ===================================================================================================
class WSF_EXPORT CommTurnedOff : public Result
{
public:
   static constexpr const char* cNAME = "COMM_TURNED_OFF";
   CommTurnedOff(double aSimTime, comm::Comm* aCommPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mCommPtr(aCommPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetComm() const { return mCommPtr; }

private:
   comm::Comm* mCommPtr;
};

// ===================================================================================================
class WSF_EXPORT CommTurnedOn : public Result
{
public:
   static constexpr const char* cNAME = "COMM_TURNED_ON";
   CommTurnedOn(double aSimTime, comm::Comm* aCommPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mCommPtr(aCommPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetComm() const { return mCommPtr; }

private:
   comm::Comm* mCommPtr;
};

// ===================================================================================================
class WSF_EXPORT CrashedIntoGround : public Result
{
public:
   static constexpr const char* cNAME = "CRASHED_INTO_GROUND";
   CrashedIntoGround(double aSimTime, WsfPlatform* aPlatformPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mPlatformPtr(aPlatformPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfPlatform* GetPlatform() const { return mPlatformPtr; }

private:
   WsfPlatform* mPlatformPtr;
};

// ===================================================================================================
class WSF_EXPORT ExchangeCompleted : public Result
{
public:
   static constexpr const char* cNAME = "EXCHANGE_COMPLETED";
   ExchangeCompleted(double                         aSimTime,
                     const WsfExchange::Transactor& aTransactor,
                     const WsfExchange::Query&      aQuery,
                     Settings                       aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mTransactor(aTransactor)
      , mQuery(aQuery)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfExchange::Transactor& GetTransactor() const { return mTransactor; }
   const WsfExchange::Query&      GetQuery() const { return mQuery; }
   WsfSimulation&                 GetSimulation() const { return *mTransactor.GetSimulation(); }

private:
   const WsfExchange::Transactor& mTransactor;
   const WsfExchange::Query&      mQuery;
};

// ===================================================================================================
class WSF_EXPORT ExchangeNegotiated : public Result
{
public:
   static constexpr const char* cNAME = "EXCHANGE_NEGOTIATED";
   ExchangeNegotiated(double                         aSimTime,
                      const WsfExchange::Transactor& aTransactor,
                      const WsfExchange::Query&      aQuery,
                      Settings                       aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mTransactor(aTransactor)
      , mQuery(aQuery)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfExchange::Transactor& GetTransactor() const { return mTransactor; }
   const WsfExchange::Query&      GetQuery() const { return mQuery; }
   WsfSimulation&                 GetSimulation() const { return *mTransactor.GetSimulation(); }

private:
   const WsfExchange::Transactor& mTransactor;
   const WsfExchange::Query&      mQuery;
};

// ===================================================================================================
class WSF_EXPORT ExchangeQueried : public Result
{
public:
   static constexpr const char* cNAME = "EXCHANGE_QUERIED";
   ExchangeQueried(double aSimTime, const WsfExchange::Query& aQuery, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mQuery(aQuery)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfExchange::Query& GetQuery() const { return mQuery; }
   WsfSimulation&            GetSimulation() const { return *mQuery.GetSimulation(); }

private:
   const WsfExchange::Query& mQuery;
};

// ===================================================================================================
class WSF_EXPORT ExchangeRequestFailed : public Result
{
public:
   static constexpr const char* cNAME = "EXCHANGE_REQUEST_FAILED";
   ExchangeRequestFailed(double                         aSimTime,
                         const WsfExchange::Transactor& aTransactor,
                         const WsfExchange::Query&      aQuery,
                         Settings                       aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mTransactor(aTransactor)
      , mQuery(aQuery)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfExchange::Transactor& GetTransactor() const { return mTransactor; }
   const WsfExchange::Query&      GetQuery() const { return mQuery; }
   WsfSimulation&                 GetSimulation() const { return *mQuery.GetSimulation(); }

private:
   const WsfExchange::Transactor& mTransactor;
   const WsfExchange::Query&      mQuery;
};

// ===================================================================================================
class WSF_EXPORT ExecuteCallback : public Result
{
public:
   static constexpr const char* cNAME = "EXECUTE_CALLBACK";
   ExecuteCallback(double aSimTime, WsfPlatform* aPlatformPtr, const WsfCallback& aCallback, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mPlatformPtr(aPlatformPtr)
      , mCallback(aCallback)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfPlatform* GetPlatform() const { return mPlatformPtr; }
   const WsfCallback& GetCallback() const { return mCallback; }

private:
   WsfPlatform*       mPlatformPtr;
   const WsfCallback& mCallback;
};

// ===================================================================================================
class WSF_EXPORT FuelEvent : public Result
{
public:
   static constexpr const char* cNAME = "FUEL_EVENT";
   FuelEvent(double aSimTime, WsfFuel* aFuelPtr, WsfStringId aEventNameId, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mFuelPtr(aFuelPtr)
      , mEventNameId(aEventNameId)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfFuel* GetFuel() const { return mFuelPtr; }
   WsfStringId    GetEventNameId() const { return mEventNameId; }

private:
   WsfFuel*    mFuelPtr;
   WsfStringId mEventNameId;
};

// ===================================================================================================
class WSF_EXPORT ImageCreated : public Result
{
public:
   static constexpr const char* cNAME = "IMAGE_CREATED";
   ImageCreated(double aSimTime, WsfSensor* aSensorPtr, WsfImage* aImagePtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSensorPtr(aSensorPtr)
      , mImagePtr(aImagePtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSensor* GetSensor() const { return mSensorPtr; }
   const WsfImage*  GetImage() const { return mImagePtr; }

private:
   WsfSensor* mSensorPtr;
   WsfImage*  mImagePtr;
};

// ===================================================================================================
class WSF_EXPORT LinkAddedToManager : public Result
{
public:
   static constexpr const char* cNAME = "LINK_ADDED_TO_MANAGER";
   LinkAddedToManager(double      aSimTime,
                      comm::Comm* aSourceCommPtr,
                      comm::Comm* aDestinationCommPtr,
                      Settings    aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSourceCommPtr(aSourceCommPtr)
      , mDestinationCommPtr(aDestinationCommPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetSourceComm() const { return mSourceCommPtr; }
   const comm::Comm* GetDestinationComm() const { return mDestinationCommPtr; }

private:
   comm::Comm* mSourceCommPtr;
   comm::Comm* mDestinationCommPtr;
};

// ===================================================================================================
class WSF_EXPORT LinkDisabledOnManager : public Result
{
public:
   static constexpr const char* cNAME = "LINK_DISABLED_ON_MANAGER";
   LinkDisabledOnManager(double      aSimTime,
                         comm::Comm* aSourceCommPtr,
                         comm::Comm* aDestinationCommPtr,
                         Settings    aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSourceCommPtr(aSourceCommPtr)
      , mDestinationCommPtr(aDestinationCommPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetSourceComm() const { return mSourceCommPtr; }
   const comm::Comm* GetDestinationComm() const { return mDestinationCommPtr; }

private:
   comm::Comm* mSourceCommPtr;
   comm::Comm* mDestinationCommPtr;
};

// ===================================================================================================
class WSF_EXPORT LinkEnabledOnManager : public Result
{
public:
   static constexpr const char* cNAME = "LINK_ENABLED_ON_MANAGER";
   LinkEnabledOnManager(double      aSimTime,
                        comm::Comm* aSourceCommPtr,
                        comm::Comm* aDestinationCommPtr,
                        Settings    aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSourceCommPtr(aSourceCommPtr)
      , mDestinationCommPtr(aDestinationCommPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetSourceComm() const { return mSourceCommPtr; }
   const comm::Comm* GetDestinationComm() const { return mDestinationCommPtr; }

private:
   comm::Comm* mSourceCommPtr;
   comm::Comm* mDestinationCommPtr;
};

// ===================================================================================================
class WSF_EXPORT LinkRemovedFromManager : public Result
{
public:
   static constexpr const char* cNAME = "LINK_REMOVED_FROM_MANAGER";
   LinkRemovedFromManager(double      aSimTime,
                          comm::Comm* aSourceCommPtr,
                          comm::Comm* aDestinationCommPtr,
                          Settings    aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSourceCommPtr(aSourceCommPtr)
      , mDestinationCommPtr(aDestinationCommPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetSourceComm() const { return mSourceCommPtr; }
   const comm::Comm* GetDestinationComm() const { return mDestinationCommPtr; }

private:
   comm::Comm* mSourceCommPtr;
   comm::Comm* mDestinationCommPtr;
};

// ===================================================================================================
class WSF_EXPORT LinkAddedToLocal : public Result
{
public:
   static constexpr const char* cNAME = "LINK_ADDED_TO_LOCAL";
   LinkAddedToLocal(double                                aSimTime,
                    wsf::comm::router::Router*            aLocalRouterPtr,
                    wsf::comm::router::ProtocolInterface* aProtocolPtr,
                    wsf::comm::Address*                   aSourceAddressPtr,
                    wsf::comm::Address*                   aDestinationAddressPtr,
                    Settings                              aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mLocalRouterPtr(aLocalRouterPtr)
      , mProtocolPtr(aProtocolPtr)
      , mSourceAddressPtr(aSourceAddressPtr)
      , mDestinationAddressPtr(aDestinationAddressPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::router::Router*            GetLocalRouter() const { return mLocalRouterPtr; }
   const comm::router::ProtocolInterface* GetProtocol() const { return mProtocolPtr; }
   const comm::Address*                   GetSourceAddress() const { return mSourceAddressPtr; }
   const comm::Address*                   GetDestinationAddress() const { return mDestinationAddressPtr; }

private:
   comm::router::Router*            mLocalRouterPtr;
   comm::router::ProtocolInterface* mProtocolPtr;
   comm::Address*                   mSourceAddressPtr;
   comm::Address*                   mDestinationAddressPtr;
};

// ===================================================================================================
class WSF_EXPORT LinkDisabledOnLocal : public Result
{
public:
   static constexpr const char* cNAME = "LINK_DISABLED_ON_LOCAL";
   LinkDisabledOnLocal(double                                aSimTime,
                       wsf::comm::router::Router*            aLocalRouterPtr,
                       wsf::comm::router::ProtocolInterface* aProtocolPtr,
                       wsf::comm::Address*                   aSourceAddressPtr,
                       wsf::comm::Address*                   aDestinationAddressPtr,
                       Settings                              aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mLocalRouterPtr(aLocalRouterPtr)
      , mProtocolPtr(aProtocolPtr)
      , mSourceAddressPtr(aSourceAddressPtr)
      , mDestinationAddressPtr(aDestinationAddressPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::router::Router*            GetLocalRouter() const { return mLocalRouterPtr; }
   const comm::router::ProtocolInterface* GetProtocol() const { return mProtocolPtr; }
   const comm::Address*                   GetSourceAddress() const { return mSourceAddressPtr; }
   const comm::Address*                   GetDestinationAddress() const { return mDestinationAddressPtr; }

private:
   comm::router::Router*            mLocalRouterPtr;
   comm::router::ProtocolInterface* mProtocolPtr;
   comm::Address*                   mSourceAddressPtr;
   comm::Address*                   mDestinationAddressPtr;
};

// ===================================================================================================
class WSF_EXPORT LinkEnabledOnLocal : public Result
{
public:
   static constexpr const char* cNAME = "LINK_ENABLED_ON_LOCAL";
   LinkEnabledOnLocal(double                                aSimTime,
                      wsf::comm::router::Router*            aLocalRouterPtr,
                      wsf::comm::router::ProtocolInterface* aProtocolPtr,
                      wsf::comm::Address*                   aSourceAddressPtr,
                      wsf::comm::Address*                   aDestinationAddressPtr,
                      Settings                              aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mLocalRouterPtr(aLocalRouterPtr)
      , mProtocolPtr(aProtocolPtr)
      , mSourceAddressPtr(aSourceAddressPtr)
      , mDestinationAddressPtr(aDestinationAddressPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::router::Router*            GetLocalRouter() const { return mLocalRouterPtr; }
   const comm::router::ProtocolInterface* GetProtocol() const { return mProtocolPtr; }
   const comm::Address*                   GetSourceAddress() const { return mSourceAddressPtr; }
   const comm::Address*                   GetDestinationAddress() const { return mDestinationAddressPtr; }

private:
   comm::router::Router*            mLocalRouterPtr;
   comm::router::ProtocolInterface* mProtocolPtr;
   comm::Address*                   mSourceAddressPtr;
   comm::Address*                   mDestinationAddressPtr;
};

// ===================================================================================================
class WSF_EXPORT LinkRemovedFromLocal : public Result
{
public:
   static constexpr const char* cNAME = "LINK_REMOVED_FROM_LOCAL";
   LinkRemovedFromLocal(double                           aSimTime,
                        comm::router::Router*            aLocalRouterPtr,
                        comm::router::ProtocolInterface* aProtocolPtr,
                        comm::Address*                   aSourceAddressPtr,
                        comm::Address*                   aDestinationAddressPtr,
                        Settings                         aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mLocalRouterPtr(aLocalRouterPtr)
      , mProtocolPtr(aProtocolPtr)
      , mSourceAddressPtr(aSourceAddressPtr)
      , mDestinationAddressPtr(aDestinationAddressPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::router::Router*            GetLocalRouter() const { return mLocalRouterPtr; }
   const comm::router::ProtocolInterface* GetProtocol() const { return mProtocolPtr; }
   const comm::Address*                   GetSourceAddress() const { return mSourceAddressPtr; }
   const comm::Address*                   GetDestinationAddress() const { return mDestinationAddressPtr; }

private:
   comm::router::Router*            mLocalRouterPtr;
   comm::router::ProtocolInterface* mProtocolPtr;
   comm::Address*                   mSourceAddressPtr;
   comm::Address*                   mDestinationAddressPtr;
};

// ===================================================================================================
class WSF_EXPORT LocalTrackCorrelation : public Result
{
public:
   static constexpr const char* cNAME = "LOCAL_TRACK_CORRELATION";
   LocalTrackCorrelation(double            aSimTime,
                         WsfPlatform*      aPlatformPtr,
                         const WsfTrackId& aLocalTrackId,
                         const WsfTrackId& aNonLocalTrackId,
                         Settings          aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mPlatformPtr(aPlatformPtr)
      , mLocalTrackId(aLocalTrackId)
      , mNonLocalTrackId(aNonLocalTrackId)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfPlatform* GetPlatform() const { return mPlatformPtr; }
   const WsfTrackId&  GetLocalTrackId() const { return mLocalTrackId; }
   const WsfTrackId&  GetNonLocalTrackId() const { return mNonLocalTrackId; }

private:
   WsfPlatform*      mPlatformPtr;
   const WsfTrackId& mLocalTrackId;
   const WsfTrackId& mNonLocalTrackId;
};

// ===================================================================================================
class WSF_EXPORT LocalTrackDecorrelation : public Result
{
public:
   static constexpr const char* cNAME = "LOCAL_TRACK_DECORRELATION";
   LocalTrackDecorrelation(double            aSimTime,
                           WsfPlatform*      aPlatformPtr,
                           const WsfTrackId& aLocalTrackId,
                           const WsfTrackId& aNonLocalTrackId,
                           Settings          aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mPlatformPtr(aPlatformPtr)
      , mLocalTrackId(aLocalTrackId)
      , mNonLocalTrackId(aNonLocalTrackId)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfPlatform* GetPlatform() const { return mPlatformPtr; }
   const WsfTrackId&  GetLocalTrackId() const { return mLocalTrackId; }
   const WsfTrackId&  GetNonLocalTrackId() const { return mNonLocalTrackId; }

private:
   WsfPlatform*      mPlatformPtr;
   const WsfTrackId& mLocalTrackId;
   const WsfTrackId& mNonLocalTrackId;
};

// ===================================================================================================
class WSF_EXPORT LocalTrackDropped : public Result
{
public:
   static constexpr const char* cNAME = "LOCAL_TRACK_DROPPED";
   LocalTrackDropped(double               aSimTime,
                     WsfPlatform*         aPlatformPtr,
                     const WsfLocalTrack* aTrackPtr,
                     Settings             aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mPlatformPtr(aPlatformPtr)
      , mTrackPtr(aTrackPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfPlatform*   GetPlatform() const { return mPlatformPtr; }
   const WsfLocalTrack* GetTrack() const { return mTrackPtr; }

private:
   WsfPlatform*         mPlatformPtr;
   const WsfLocalTrack* mTrackPtr;
};

// ===================================================================================================
class WSF_EXPORT LocalTrackInitiated : public Result
{
public:
   static constexpr const char* cNAME = "LOCAL_TRACK_INITIATED";
   LocalTrackInitiated(double               aSimTime,
                       WsfPlatform*         aPlatformPtr,
                       const WsfLocalTrack* aTrackPtr,
                       const WsfTrack*      aSourcePtr,
                       Settings             aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mPlatformPtr(aPlatformPtr)
      , mTrackPtr(aTrackPtr)
      , mSourcePtr(aSourcePtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfPlatform*   GetPlatform() const { return mPlatformPtr; }
   const WsfLocalTrack* GetTrack() const { return mTrackPtr; }
   const WsfTrack*      GetSourceTrack() const { return mSourcePtr; }
   WsfSimulation&       GetSimulation() const { return *mPlatformPtr->GetSimulation(); }

private:
   WsfPlatform*         mPlatformPtr;
   const WsfLocalTrack* mTrackPtr;
   const WsfTrack*      mSourcePtr;
};

// ===================================================================================================
class WSF_EXPORT LocalTrackUpdated : public Result
{
public:
   static constexpr const char* cNAME = "LOCAL_TRACK_UPDATED";
   LocalTrackUpdated(double               aSimTime,
                     WsfPlatform*         aPlatformPtr,
                     const WsfLocalTrack* aTrackPtr,
                     const WsfTrack*      aSourcePtr,
                     Settings             aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mPlatformPtr(aPlatformPtr)
      , mTrackPtr(aTrackPtr)
      , mSourcePtr(aSourcePtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfPlatform*   GetPlatform() const { return mPlatformPtr; }
   const WsfLocalTrack* GetTrack() const { return mTrackPtr; }
   const WsfTrack*      GetSourceTrack() const { return mSourcePtr; }
   WsfSimulation&       GetSimulation() const { return *mPlatformPtr->GetSimulation(); }

private:
   WsfPlatform*         mPlatformPtr;
   const WsfLocalTrack* mTrackPtr;
   const WsfTrack*      mSourcePtr;
};

// ===================================================================================================
class WSF_EXPORT MessageDeliveryAttempt : public Result
{
public:
   static constexpr const char* cNAME = "MESSAGE_DELIVERY_ATTEMPT";
   MessageDeliveryAttempt(double            aSimTime,
                          comm::Comm*       aXmtrPtr,
                          comm::Comm*       aRcvrPtr,
                          const WsfMessage& aMessage,
                          comm::Result&     aResult,
                          Settings          aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mXmtrPtr(aXmtrPtr)
      , mRcvrPtr(aRcvrPtr)
      , mMessage(aMessage)
      , mResult(aResult)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetTransmitter() const { return mXmtrPtr; }
   const comm::Comm* GetReceiver() const { return mRcvrPtr; }
   const WsfMessage& GetMessage() const { return mMessage; }
   comm::Result&     GetResult() const { return mResult; }

private:
   comm::Comm*       mXmtrPtr;
   comm::Comm*       mRcvrPtr;
   const WsfMessage& mMessage;
   comm::Result&     mResult;
};

// ===================================================================================================
class WSF_EXPORT MessageDiscarded : public Result
{
public:
   static constexpr const char* cNAME = "MESSAGE_DISCARDED";
   MessageDiscarded(double             aSimTime,
                    comm::Comm*        aCommPtr,
                    const WsfMessage&  aMessage,
                    const std::string& aComment,
                    Settings           aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mCommPtr(aCommPtr)
      , mMessage(aMessage)
      , mComment(aComment)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm*  GetComm() const { return mCommPtr; }
   const WsfMessage&  GetMessage() const { return mMessage; }
   const std::string& GetComment() const { return mComment; }
   WsfSimulation&     GetSimulation() const { return *mCommPtr->GetSimulation(); }

private:
   comm::Comm*        mCommPtr;
   const WsfMessage&  mMessage;
   const std::string& mComment;
};

// ===================================================================================================
class WSF_EXPORT MessageFailedRouting : public Result
{
public:
   static constexpr const char* cNAME = "MESSAGE_FAILED_ROUTING";
   MessageFailedRouting(double            aSimTime,
                        comm::Comm*       aCommPtr,
                        WsfPlatform*      aRcvrPlatform,
                        const WsfMessage& aMessage,
                        Settings          aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mCommPtr(aCommPtr)
      , mRcvrPlatform(aRcvrPlatform)
      , mMessage(aMessage)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm*  GetComm() const { return mCommPtr; }
   const WsfPlatform* GetReceiverPlatform() const { return mRcvrPlatform; }
   const WsfMessage&  GetMessage() const { return mMessage; }

private:
   comm::Comm*       mCommPtr;
   WsfPlatform*      mRcvrPlatform;
   const WsfMessage& mMessage;
};

// ===================================================================================================
class WSF_EXPORT MessageHop : public Result
{
public:
   static constexpr const char* cNAME = "MESSAGE_HOP";
   MessageHop(double            aSimTime,
              wsf::comm::Comm*  aRcvrPtr,
              wsf::comm::Comm*  aDstPtr,
              const WsfMessage& aMessage,
              Settings          aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mRcvrPtr(aRcvrPtr)
      , mDstPtr(aDstPtr)
      , mMessage(aMessage)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetReceiverComm() const { return mRcvrPtr; }
   const comm::Comm* GetDestinationComm() const { return mDstPtr; }
   const WsfMessage& GetMessage() const { return mMessage; }

private:
   comm::Comm*       mRcvrPtr;
   comm::Comm*       mDstPtr;
   const WsfMessage& mMessage;
};

// ===================================================================================================
class WSF_EXPORT MessageQueued : public Result
{
public:
   static constexpr const char* cNAME = "MESSAGE_QUEUED";
   MessageQueued(double            aSimTime,
                 comm::Comm*       aCommPtr,
                 const WsfMessage& aMessage,
                 size_t            aQueueSize,
                 Settings          aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mCommPtr(aCommPtr)
      , mMessage(aMessage)
      , mQueueSize(aQueueSize)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetComm() const { return mCommPtr; }
   const WsfMessage& GetMessage() const { return mMessage; }
   const size_t&     GetQueueSize() const { return mQueueSize; }
   WsfSimulation&    GetSimulation() const { return *mCommPtr->GetSimulation(); }

private:
   comm::Comm*       mCommPtr;
   const WsfMessage& mMessage;
   size_t            mQueueSize;
};

// ===================================================================================================
class WSF_EXPORT MessageReceived : public Result
{
public:
   static constexpr const char* cNAME = "MESSAGE_RECEIVED";
   MessageReceived(double             aSimTime,
                   comm::Comm*        aXmtrPtr,
                   comm::Comm*        aRcvrPtr,
                   const WsfMessage&  aMessage,
                   wsf::comm::Result& aResult,
                   Settings           aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mRcvrPtr(aRcvrPtr)
      , mMessage(aMessage)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetReceiverComm() const { return mRcvrPtr; }
   const WsfMessage& GetMessage() const { return mMessage; }
   WsfSimulation&    GetSimulation() const { return *mRcvrPtr->GetSimulation(); }

private:
   comm::Comm*       mRcvrPtr;
   const WsfMessage& mMessage;
};

// ===================================================================================================
class WSF_EXPORT MessageTransmitEnded : public Result
{
public:
   static constexpr const char* cNAME = "MESSAGE_TRANSMIT_ENDED";
   MessageTransmitEnded(double aSimTime, comm::Comm* aXmtrPtr, const WsfMessage& aMessage, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mXmtrPtr(aXmtrPtr)
      , mMessage(aMessage)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetTransmitterComm() const { return mXmtrPtr; }
   const WsfMessage& GetMessage() const { return mMessage; }
   WsfSimulation&    GetSimulation() const { return *mXmtrPtr->GetSimulation(); }

private:
   comm::Comm*       mXmtrPtr;
   const WsfMessage& mMessage;
};

// ===================================================================================================
class WSF_EXPORT MessageTransmitted : public Result
{
public:
   static constexpr const char* cNAME = "MESSAGE_TRANSMITTED";
   MessageTransmitted(double aSimTime, comm::Comm* aXmtrPtr, const WsfMessage& aMessage, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mXmtrPtr(aXmtrPtr)
      , mMessage(aMessage)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetTransmitterComm() const { return mXmtrPtr; }
   const WsfMessage& GetMessage() const { return mMessage; }
   WsfSimulation&    GetSimulation() const { return *mXmtrPtr->GetSimulation(); }

private:
   comm::Comm*       mXmtrPtr;
   const WsfMessage& mMessage;
};

// ===================================================================================================
class WSF_EXPORT MessageTransmittedHeartbeat : public Result
{
public:
   static constexpr const char* cNAME = "MESSAGE_TRANSMITTED_HEARTBEAT";
   MessageTransmittedHeartbeat(double            aSimTime,
                               comm::Comm*       aXmtrPtr,
                               const WsfMessage& aMessage,
                               Settings          aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mXmtrPtr(aXmtrPtr)
      , mMessage(aMessage)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetTransmitterComm() const { return mXmtrPtr; }
   const WsfMessage& GetMessage() const { return mMessage; }
   WsfSimulation&    GetSimulation() const { return *mXmtrPtr->GetSimulation(); }

private:
   comm::Comm*       mXmtrPtr;
   const WsfMessage& mMessage;
};

// ===================================================================================================
class WSF_EXPORT MessageUpdated : public Result
{
public:
   static constexpr const char* cNAME = "MESSAGE_UPDATED";
   MessageUpdated(double            aSimTime,
                  comm::Comm*       aCommPtr,
                  const WsfMessage& aOldMessage,
                  const WsfMessage& aNewMessage,
                  Settings          aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mCommPtr(aCommPtr)
      , mOldMessage(aOldMessage)
      , mNewMessage(aNewMessage)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Comm* GetComm() const { return mCommPtr; }
   const WsfMessage& GetOldMessage() const { return mOldMessage; }
   const WsfMessage& GetNewMessage() const { return mNewMessage; }
   WsfSimulation&    GetSimulation() const { return *mCommPtr->GetSimulation(); }

private:
   comm::Comm*       mCommPtr;
   const WsfMessage& mOldMessage;
   const WsfMessage& mNewMessage;
};

// ===================================================================================================
class WSF_EXPORT MoverBroken : public Result
{
public:
   static constexpr const char* cNAME = "MOVER_BROKEN";
   MoverBroken(double aSimTime, WsfMover* aMoverPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mMoverPtr(aMoverPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfMover* GetMover() const { return mMoverPtr; }

private:
   WsfMover* mMoverPtr;
};

// ===================================================================================================
class WSF_EXPORT MoverBurnedOut : public Result
{
public:
   static constexpr const char* cNAME = "MOVER_BURNED_OUT";
   MoverBurnedOut(double aSimTime, WsfMover* aMoverPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mMoverPtr(aMoverPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfMover* GetMover() const { return mMoverPtr; }

private:
   WsfMover* mMoverPtr;
};

// ===================================================================================================
class WSF_EXPORT MoverNonOperational : public Result
{
public:
   static constexpr const char* cNAME = "MOVER_NON_OPERATIONAL";
   MoverNonOperational(double aSimTime, WsfMover* aMoverPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mMoverPtr(aMoverPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfMover* GetMover() const { return mMoverPtr; }

private:
   WsfMover* mMoverPtr;
};

// ===================================================================================================
class WSF_EXPORT MoverOperational : public Result
{
public:
   static constexpr const char* cNAME = "MOVER_OPERATIONAL";
   MoverOperational(double aSimTime, WsfMover* aMoverPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mMoverPtr(aMoverPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfMover* GetMover() const { return mMoverPtr; }

private:
   WsfMover* mMoverPtr;
};

// ===================================================================================================
class WSF_EXPORT MoverStaged : public Result
{
public:
   static constexpr const char* cNAME = "MOVER_STAGED";
   MoverStaged(double aSimTime, WsfMover* aMoverPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mMoverPtr(aMoverPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfMover* GetMover() const { return mMoverPtr; }

private:
   WsfMover* mMoverPtr;
};

// ===================================================================================================
class WSF_EXPORT MoverTurnedOff : public Result
{
public:
   static constexpr const char* cNAME = "MOVER_TURNED_OFF";
   MoverTurnedOff(double aSimTime, WsfMover* aMoverPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mMoverPtr(aMoverPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfMover* GetMover() const { return mMoverPtr; }

private:
   WsfMover* mMoverPtr;
};

// ===================================================================================================
class WSF_EXPORT MoverTurnedOn : public Result
{
public:
   static constexpr const char* cNAME = "MOVER_TURNED_ON";
   MoverTurnedOn(double aSimTime, WsfMover* aMoverPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mMoverPtr(aMoverPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfMover* GetMover() const { return mMoverPtr; }

private:
   WsfMover* mMoverPtr;
};

// ===================================================================================================
class WSF_EXPORT NavigationStatusChanged : public Result
{
public:
   static constexpr const char* cNAME = "NAVIGATION_STATUS_CHANGED";
   NavigationStatusChanged(double             aSimTime,
                           WsfPlatform*       aPlatformPtr,
                           const std::string& aStatus,
                           Settings           aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mPlatformPtr(aPlatformPtr)
      , mStatus(aStatus)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfPlatform* GetPlatform() const { return mPlatformPtr; }
   const std::string& GetStatus() const { return mStatus; }

private:
   WsfPlatform*       mPlatformPtr;
   const std::string& mStatus;
};

// ===================================================================================================
class WSF_EXPORT NetworkAdded : public Result
{
public:
   static constexpr const char* cNAME = "NETWORK_ADDED";
   NetworkAdded(double aSimTime, comm::Network* aNetworkPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mNetworkPtr(aNetworkPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Network* GetNetwork() const { return mNetworkPtr; }

private:
   comm::Network* mNetworkPtr;
};

// ===================================================================================================
class WSF_EXPORT NetworkRemoved : public Result
{
public:
   static constexpr const char* cNAME = "NETWORK_REMOVED";
   NetworkRemoved(double aSimTime, comm::Network* aNetworkPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mNetworkPtr(aNetworkPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::Network* GetNetwork() const { return mNetworkPtr; }

private:
   comm::Network* mNetworkPtr;
};

// ===================================================================================================
class WSF_EXPORT OperatingLevelChanged : public Result
{
public:
   static constexpr const char* cNAME = "OPERATING_LEVEL_CHANGED";
   OperatingLevelChanged(double        aSimTime,
                         WsfProcessor* aProcessorPtr,
                         WsfStringId   aName,
                         int           aLevel,
                         Settings      aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mProcessorPtr(aProcessorPtr)
      , mName(aName)
      , mLevel(aLevel)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfProcessor* GetProcessor() const { return mProcessorPtr; }
   WsfStringId         GetName() const { return mName; }
   const int&          GetLevel() const { return mLevel; }

private:
   WsfProcessor* mProcessorPtr;
   WsfStringId   mName;
   int           mLevel;
};

// ===================================================================================================
class WSF_EXPORT PlatformAdded : public Result
{
public:
   static constexpr const char* cNAME = "PLATFORM_ADDED";
   PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mPlatformPtr(aPlatformPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfPlatform* GetPlatform() const { return mPlatformPtr; }

private:
   WsfPlatform* mPlatformPtr;
};

// ===================================================================================================
class WSF_EXPORT PlatformAddedTeamName : public Result
{
public:
   static constexpr const char* cNAME = "TEAM_NAME_DEFINITION";
   PlatformAddedTeamName(double aSimTime, WsfPlatform* aPlatformPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mPlatformPtr(aPlatformPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfPlatform* GetPlatform() const { return mPlatformPtr; }

private:
   WsfPlatform* mPlatformPtr;
};

// ===================================================================================================
class WSF_EXPORT PlatformAppearanceChanged : public Result
{
public:
   static constexpr const char* cNAME = "PLATFORM_APPEARANCE_CHANGED";
   PlatformAppearanceChanged(double                      aSimTime,
                             WsfPlatform*                aPlatformPtr,
                             WsfObserver::AppearanceType aAppearanceType,
                             Settings                    aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mPlatformPtr(aPlatformPtr)
      , mAppearanceType(aAppearanceType)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   WsfPlatform*                GetPlatform() const { return mPlatformPtr; }
   WsfObserver::AppearanceType GetAppearanceType() const { return mAppearanceType; }

private:
   WsfPlatform*                mPlatformPtr;
   WsfObserver::AppearanceType mAppearanceType;
};

// ===================================================================================================
class WSF_EXPORT PlatformBroken : public Result
{
public:
   static constexpr const char* cNAME = "PLATFORM_BROKEN";
   PlatformBroken(double aSimTime, WsfPlatform* aPlatformPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mPlatformPtr(aPlatformPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfPlatform* GetPlatform() const { return mPlatformPtr; }

private:
   WsfPlatform* mPlatformPtr;
};

// ===================================================================================================
class WSF_EXPORT PlatformCapabilityChanged : public Result
{
public:
   static constexpr const char* cNAME = "PLATFORM_CAPABILITY_CHANGED";
   PlatformCapabilityChanged(double       aSimTime,
                             WsfPlatform* aPlatformPtr,
                             WsfStringId  aCapabilityId,
                             bool         aIsCapable,
                             Settings     aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mPlatformPtr(aPlatformPtr)
      , mCapabilityId(aCapabilityId)
      , mIsCapable(aIsCapable)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfPlatform* GetPlatform() const { return mPlatformPtr; }
   WsfStringId        GetCapabilityId() const { return mCapabilityId; }
   bool               GetIsCapable() const { return mIsCapable; }

private:
   WsfPlatform* mPlatformPtr;
   WsfStringId  mCapabilityId;
   bool         mIsCapable;
};

// ===================================================================================================
class WSF_EXPORT PlatformDeleted : public Result
{
public:
   static constexpr const char* cNAME = "PLATFORM_DELETED";
   PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mPlatformPtr(aPlatformPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfPlatform* GetPlatform() const { return mPlatformPtr; }

private:
   WsfPlatform* mPlatformPtr;
};

// ===================================================================================================
class WSF_EXPORT PlatformInitialized : public Result
{
public:
   static constexpr const char* cNAME = "PLATFORM_INITIALIZED";
   PlatformInitialized(double aSimTime, WsfPlatform* aPlatformPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mPlatformPtr(aPlatformPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfPlatform* GetPlatform() const { return mPlatformPtr; }

private:
   WsfPlatform* mPlatformPtr;
};

// ===================================================================================================
class WSF_EXPORT PlatformOmitted : public Result
{
public:
   static constexpr const char* cNAME = "PLATFORM_OMITTED";
   PlatformOmitted(double aSimTime, WsfPlatform* aPlatformPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mPlatformPtr(aPlatformPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfPlatform* GetPlatform() const { return mPlatformPtr; }

private:
   WsfPlatform* mPlatformPtr;
};

// ===================================================================================================
class WSF_EXPORT ProcessorBroken : public Result
{
public:
   static constexpr const char* cNAME = "PROCESSOR_BROKEN";
   ProcessorBroken(double aSimTime, WsfProcessor* aProcessorPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mProcessorPtr(aProcessorPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfProcessor* GetProcessor() const { return mProcessorPtr; }

private:
   WsfProcessor* mProcessorPtr;
};

// ===================================================================================================
class WSF_EXPORT ProcessorNonOperational : public Result
{
public:
   static constexpr const char* cNAME = "PROCESSOR_NON_OPERATIONAL";
   ProcessorNonOperational(double aSimTime, WsfProcessor* aProcessorPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mProcessorPtr(aProcessorPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfProcessor* GetProcessor() const { return mProcessorPtr; }

private:
   WsfProcessor* mProcessorPtr;
};

// ===================================================================================================
class WSF_EXPORT ProcessorOperational : public Result
{
public:
   static constexpr const char* cNAME = "PROCESSOR_OPERATIONAL";
   ProcessorOperational(double aSimTime, WsfProcessor* aProcessorPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mProcessorPtr(aProcessorPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfProcessor* GetProcessor() const { return mProcessorPtr; }

private:
   WsfProcessor* mProcessorPtr;
};

// ===================================================================================================
class WSF_EXPORT ProcessorTurnedOff : public Result
{
public:
   static constexpr const char* cNAME = "PROCESSOR_TURNED_OFF";
   ProcessorTurnedOff(double aSimTime, WsfProcessor* aProcessorPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mProcessorPtr(aProcessorPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfProcessor* GetProcessor() const { return mProcessorPtr; }

private:
   WsfProcessor* mProcessorPtr;
};

// ===================================================================================================
class WSF_EXPORT ProcessorTurnedOn : public Result
{
public:
   static constexpr const char* cNAME = "PROCESSOR_TURNED_ON";
   ProcessorTurnedOn(double aSimTime, WsfProcessor* aProcessorPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mProcessorPtr(aProcessorPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfProcessor* GetProcessor() const { return mProcessorPtr; }

private:
   WsfProcessor* mProcessorPtr;
};

// ===================================================================================================
class WSF_EXPORT RouterBroken : public Result
{
public:
   static constexpr const char* cNAME = "ROUTER_BROKEN";
   RouterBroken(double aSimTime, wsf::comm::router::Router* aRouterPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mRouterPtr(aRouterPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::router::Router* GetRouter() const { return mRouterPtr; }

private:
   wsf::comm::router::Router* mRouterPtr;
};

// ===================================================================================================
class WSF_EXPORT RouterNonOperational : public Result
{
public:
   static constexpr const char* cNAME = "ROUTER_NON_OPERATIONAL";
   RouterNonOperational(double aSimTime, wsf::comm::router::Router* aRouterPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mRouterPtr(aRouterPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::router::Router* GetRouter() const { return mRouterPtr; }

private:
   wsf::comm::router::Router* mRouterPtr;
};


// ===================================================================================================
class WSF_EXPORT RouterOperational : public Result
{
public:
   static constexpr const char* cNAME = "ROUTER_OPERATIONAL";
   RouterOperational(double aSimTime, wsf::comm::router::Router* aRouterPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mRouterPtr(aRouterPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::router::Router* GetRouter() const { return mRouterPtr; }

private:
   wsf::comm::router::Router* mRouterPtr;
};


// ===================================================================================================
class WSF_EXPORT RouterTurnedOff : public Result
{
public:
   static constexpr const char* cNAME = "ROUTER_TURNED_OFF";
   RouterTurnedOff(double aSimTime, wsf::comm::router::Router* aRouterPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mRouterPtr(aRouterPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::router::Router* GetRouter() const { return mRouterPtr; }

private:
   wsf::comm::router::Router* mRouterPtr;
};


// ===================================================================================================
class WSF_EXPORT RouterTurnedOn : public Result
{
public:
   static constexpr const char* cNAME = "ROUTER_TURNED_ON";
   RouterTurnedOn(double aSimTime, wsf::comm::router::Router* aRouterPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mRouterPtr(aRouterPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const comm::router::Router* GetRouter() const { return mRouterPtr; }

private:
   wsf::comm::router::Router* mRouterPtr;
};

// ===================================================================================================
class WSF_EXPORT SensorBroken : public Result
{
public:
   static constexpr const char* cNAME = "SENSOR_BROKEN";
   SensorBroken(double aSimTime, WsfSensor* aSensorPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSensorPtr(aSensorPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSensor* GetSensor() const { return mSensorPtr; }

private:
   WsfSensor* mSensorPtr;
};

// ===================================================================================================
class WSF_EXPORT SensorDetectionAttempt : public Result
{
public:
   static constexpr const char* cNAME = "SENSOR_DETECTION_ATTEMPT";
   SensorDetectionAttempt(double           aSimTime,
                          WsfSensor*       aSensorPtr,
                          WsfPlatform*     aTargetPtr,
                          WsfSensorResult& aResult,
                          Settings         aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSensorPtr(aSensorPtr)
      , mTargetPtr(aTargetPtr)
      , mResult(aResult)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSensor*   GetSensor() const { return mSensorPtr; }
   const WsfPlatform* GetTarget() const { return mTargetPtr; }
   WsfSensorResult&   GetSensorResult() const { return mResult; }

private:
   WsfSensor*       mSensorPtr;
   WsfPlatform*     mTargetPtr;
   WsfSensorResult& mResult;
};

// ===================================================================================================
class WSF_EXPORT SensorDetectionChanged : public Result
{
public:
   static constexpr const char* cNAME = "SENSOR_DETECTION_CHANGED";
   SensorDetectionChanged(double           aSimTime,
                          WsfSensor*       aSensorPtr,
                          size_t           aTargetIndex,
                          WsfSensorResult& aResult,
                          Settings         aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSensorPtr(aSensorPtr)
      , mTargetIndex(aTargetIndex)
      , mResult(aResult)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSensor* GetSensor() const { return mSensorPtr; }
   size_t           GetTargetIndex() const { return mTargetIndex; }
   WsfSensorResult& GetSensorResult() const { return mResult; }
   WsfSimulation&   GetSimulation() const { return *mSensorPtr->GetSimulation(); }

private:
   WsfSensor*       mSensorPtr;
   size_t           mTargetIndex;
   WsfSensorResult& mResult;
};

// ===================================================================================================
class WSF_EXPORT SensorFrequencyChanged : public Result
{
public:
   static constexpr const char* cNAME = "SENSOR_FREQUENCY_CHANGED";
   SensorFrequencyChanged(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSensorPtr(aSensorPtr)
      , mModePtr(aModePtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSensor*     GetSensor() const { return mSensorPtr; }
   const WsfSensorMode* GetSensorMode() const { return mModePtr; }

private:
   WsfSensor*     mSensorPtr;
   WsfSensorMode* mModePtr;
};

// ===================================================================================================
class WSF_EXPORT SensorModeActivated : public Result
{
public:
   static constexpr const char* cNAME = "SENSOR_MODE_ACTIVATED";
   SensorModeActivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSensorPtr(aSensorPtr)
      , mModePtr(aModePtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSensor*     GetSensor() const { return mSensorPtr; }
   const WsfSensorMode* GetSensorMode() const { return mModePtr; }

private:
   WsfSensor*     mSensorPtr;
   WsfSensorMode* mModePtr;
};

// ===================================================================================================
class WSF_EXPORT SensorModeDeactivated : public Result
{
public:
   static constexpr const char* cNAME = "SENSOR_MODE_DEACTIVATED";
   SensorModeDeactivated(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSensorPtr(aSensorPtr)
      , mModePtr(aModePtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSensor*     GetSensor() const { return mSensorPtr; }
   const WsfSensorMode* GetSensorMode() const { return mModePtr; }

private:
   WsfSensor*     mSensorPtr;
   WsfSensorMode* mModePtr;
};

// ===================================================================================================
class WSF_EXPORT SensorNonOperational : public Result
{
public:
   static constexpr const char* cNAME = "SENSOR_NON_OPERATIONAL";
   SensorNonOperational(double aSimTime, WsfSensor* aSensorPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSensorPtr(aSensorPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSensor* GetSensor() const { return mSensorPtr; }

private:
   WsfSensor* mSensorPtr;
};

// ===================================================================================================
class WSF_EXPORT SensorOperational : public Result
{
public:
   static constexpr const char* cNAME = "SENSOR_OPERATIONAL";
   SensorOperational(double aSimTime, WsfSensor* aSensorPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSensorPtr(aSensorPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSensor* GetSensor() const { return mSensorPtr; }

private:
   WsfSensor* mSensorPtr;
};

// ===================================================================================================
class WSF_EXPORT SensorRequestCanceled : public Result
{
public:
   static constexpr const char* cNAME = "SENSOR_REQUEST_CANCELED";
   SensorRequestCanceled(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSensorPtr(aSensorPtr)
      , mTrackPtr(aTrackPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSensor* GetSensor() const { return mSensorPtr; }
   const WsfTrack*  GetTrack() const { return mTrackPtr; }
   WsfSimulation&   GetSimulation() const { return *mSensorPtr->GetSimulation(); }

private:
   WsfSensor*      mSensorPtr;
   const WsfTrack* mTrackPtr;
};

// ===================================================================================================
class WSF_EXPORT SensorRequestInitiated : public Result
{
public:
   static constexpr const char* cNAME = "SENSOR_REQUEST_INITIATED";
   SensorRequestInitiated(double          aSimTime,
                          WsfSensor*      aSensorPtr,
                          WsfSensorMode*  aModePtr,
                          const WsfTrack* aTrackPtr,
                          Settings        aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSensorPtr(aSensorPtr)
      , mModePtr(aModePtr)
      , mTrackPtr(aTrackPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSensor*     GetSensor() const { return mSensorPtr; }
   const WsfSensorMode* GetSensorMode() const { return mModePtr; }
   const WsfTrack*      GetTrack() const { return mTrackPtr; }
   WsfSimulation&       GetSimulation() const { return *mSensorPtr->GetSimulation(); }

private:
   WsfSensor*      mSensorPtr;
   WsfSensorMode*  mModePtr;
   const WsfTrack* mTrackPtr;
};

// ===================================================================================================
class WSF_EXPORT SensorRequestUpdated : public Result
{
public:
   static constexpr const char* cNAME = "SENSOR_REQUEST_UPDATED";
   SensorRequestUpdated(double          aSimTime,
                        WsfSensor*      aSensorPtr,
                        WsfSensorMode*  aModePtr,
                        const WsfTrack* aTrackPtr,
                        Settings        aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSensorPtr(aSensorPtr)
      , mModePtr(aModePtr)
      , mTrackPtr(aTrackPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSensor*     GetSensor() const { return mSensorPtr; }
   const WsfSensorMode* GetSensorMode() const { return mModePtr; }
   const WsfTrack*      GetTrack() const { return mTrackPtr; }
   WsfSimulation&       GetSimulation() const { return *mSensorPtr->GetSimulation(); }

private:
   WsfSensor*      mSensorPtr;
   WsfSensorMode*  mModePtr;
   const WsfTrack* mTrackPtr;
};

// ===================================================================================================
class WSF_EXPORT SensorTrackCoasted : public Result
{
public:
   static constexpr const char* cNAME = "SENSOR_TRACK_COASTED";
   SensorTrackCoasted(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSensorPtr(aSensorPtr)
      , mTrackPtr(aTrackPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSensor* GetSensor() const { return mSensorPtr; }
   const WsfTrack*  GetTrack() const { return mTrackPtr; }
   WsfSimulation&   GetSimulation() const { return *mSensorPtr->GetSimulation(); }

private:
   WsfSensor*      mSensorPtr;
   const WsfTrack* mTrackPtr;
};

// ===================================================================================================
class WSF_EXPORT SensorTrackDropped : public Result
{
public:
   static constexpr const char* cNAME = "SENSOR_TRACK_DROPPED";
   SensorTrackDropped(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSensorPtr(aSensorPtr)
      , mTrackPtr(aTrackPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSensor* GetSensor() const { return mSensorPtr; }
   const WsfTrack*  GetTrack() const { return mTrackPtr; }
   WsfSimulation&   GetSimulation() const { return *mSensorPtr->GetSimulation(); }

private:
   WsfSensor*      mSensorPtr;
   const WsfTrack* mTrackPtr;
};

// ===================================================================================================
class WSF_EXPORT SensorTrackInitiated : public Result
{
public:
   static constexpr const char* cNAME = "SENSOR_TRACK_INITIATED";
   SensorTrackInitiated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSensorPtr(aSensorPtr)
      , mTrackPtr(aTrackPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSensor* GetSensor() const { return mSensorPtr; }
   const WsfTrack*  GetTrack() const { return mTrackPtr; }
   WsfSimulation&   GetSimulation() const { return *mSensorPtr->GetSimulation(); }

private:
   WsfSensor*      mSensorPtr;
   const WsfTrack* mTrackPtr;
};

// ===================================================================================================
class WSF_EXPORT SensorTrackUpdated : public Result
{
public:
   static constexpr const char* cNAME = "SENSOR_TRACK_UPDATED";
   SensorTrackUpdated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSensorPtr(aSensorPtr)
      , mTrackPtr(aTrackPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSensor* GetSensor() const { return mSensorPtr; }
   const WsfTrack*  GetTrack() const { return mTrackPtr; }
   WsfSimulation&   GetSimulation() const { return *mSensorPtr->GetSimulation(); }

private:
   WsfSensor*      mSensorPtr;
   const WsfTrack* mTrackPtr;
};

// ===================================================================================================
class WSF_EXPORT SensorTurnedOff : public Result
{
public:
   static constexpr const char* cNAME = "SENSOR_TURNED_OFF";
   SensorTurnedOff(double aSimTime, WsfSensor* aSensorPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSensorPtr(aSensorPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSensor* GetSensor() const { return mSensorPtr; }

private:
   WsfSensor* mSensorPtr;
};

// ===================================================================================================
class WSF_EXPORT SensorTurnedOn : public Result
{
public:
   static constexpr const char* cNAME = "SENSOR_TURNED_ON";
   SensorTurnedOn(double aSimTime, WsfSensor* aSensorPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSensorPtr(aSensorPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSensor* GetSensor() const { return mSensorPtr; }

private:
   WsfSensor* mSensorPtr;
};

// ===================================================================================================
class WSF_EXPORT SimulationComplete : public Result
{
public:
   static constexpr const char* cNAME = "SIMULATION_COMPLETE";
   SimulationComplete(double aSimTime, WsfSimulation& aSimulation, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSimulation(aSimulation)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSimulation& GetSimulation() const { return mSimulation; }

private:
   const WsfSimulation& mSimulation;
};

// ===================================================================================================
class WSF_EXPORT SimulationStarting : public Result
{
public:
   static constexpr const char* cNAME = "SIMULATION_STARTING";
   SimulationStarting(double aSimTime, WsfSimulation& aSimulation, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mSimulation(aSimulation)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfSimulation& GetSimulation() const { return mSimulation; }

private:
   const WsfSimulation& mSimulation;
};

// ===================================================================================================
class WSF_EXPORT StateEntry : public Result
{
public:
   static constexpr const char* cNAME = "STATE_ENTRY";
   StateEntry(double          aSimTime,
              WsfProcessor*   aProcessorPtr,
              const WsfTrack* aTrackPtr,
              WsfStringId     aStateNameId,
              Settings        aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mProcessorPtr(aProcessorPtr)
      , mTrackPtr(aTrackPtr)
      , mStateNameId(aStateNameId)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfProcessor* GetProcessor() const { return mProcessorPtr; }
   const WsfTrack*     GetTrack() const { return mTrackPtr; }
   WsfStringId         GetStateNameId() const { return mStateNameId; }
   WsfSimulation&      GetSimulation() const { return *mProcessorPtr->GetSimulation(); }

private:
   WsfProcessor*   mProcessorPtr;
   const WsfTrack* mTrackPtr;
   WsfStringId     mStateNameId;
};

// ===================================================================================================
class WSF_EXPORT StateExit : public Result
{
public:
   static constexpr const char* cNAME = "STATE_EXIT";
   StateExit(double          aSimTime,
             WsfProcessor*   aProcessorPtr,
             const WsfTrack* aTrackPtr,
             WsfStringId     aStateNameId,
             Settings        aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mProcessorPtr(aProcessorPtr)
      , mTrackPtr(aTrackPtr)
      , mStateNameId(aStateNameId)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfProcessor* GetProcessor() const { return mProcessorPtr; }
   const WsfTrack*     GetTrack() const { return mTrackPtr; }
   WsfStringId         GetStateNameId() const { return mStateNameId; }
   WsfSimulation&      GetSimulation() const { return *mProcessorPtr->GetSimulation(); }

private:
   WsfProcessor*   mProcessorPtr;
   const WsfTrack* mTrackPtr;
   WsfStringId     mStateNameId;
};

// ===================================================================================================
class WSF_EXPORT TankingEvent : public Result
{
public:
   static constexpr const char* cNAME = "TANKING_EVENT";
   TankingEvent(double aSimTime, WsfFuelingOperation* aFuelingOpPtr, WsfStringId aEventNameId, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mFuelingOpPtr(aFuelingOpPtr)
      , mEventNameId(aEventNameId)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfFuelingOperation* GetFuelingOperation() const { return mFuelingOpPtr; }
   WsfStringId                GetEventNameId() const { return mEventNameId; }

private:
   WsfFuelingOperation* mFuelingOpPtr;
   WsfStringId          mEventNameId;
};

// ===================================================================================================
class WSF_EXPORT TaskAssigned : public Result
{
public:
   static constexpr const char* cNAME = "TASK_ASSIGNED";
   TaskAssigned(double aSimTime, const WsfTask* aTaskPtr, const WsfTrack* aTrackPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mTaskPtr(aTaskPtr)
      , mTrackPtr(aTrackPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfTask*  GetTask() const { return mTaskPtr; }
   const WsfTrack* GetTrack() const { return mTrackPtr; }
   WsfSimulation&  GetSimulation() const { return *mTrackPtr->GetSimulation(); }

private:
   const WsfTask*  mTaskPtr;
   const WsfTrack* mTrackPtr;
};

// ===================================================================================================
class WSF_EXPORT TaskCanceled : public Result
{
public:
   static constexpr const char* cNAME = "TASK_CANCELED";
   TaskCanceled(double aSimTime, const WsfTask* aTaskPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mTaskPtr(aTaskPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfTask* GetTask() const { return mTaskPtr; }

private:
   const WsfTask* mTaskPtr;
};

// ===================================================================================================
class WSF_EXPORT TaskCompleted : public Result
{
public:
   static constexpr const char* cNAME = "TASK_COMPLETED";
   TaskCompleted(double aSimTime, const WsfTask* aTaskPtr, WsfStringId aStatus, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mTaskPtr(aTaskPtr)
      , mStatus(aStatus)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfTask* GetTask() const { return mTaskPtr; }
   WsfStringId    GetStatus() const { return mStatus; }

private:
   const WsfTask* mTaskPtr;
   WsfStringId    mStatus;
};

} // namespace event
} // namespace wsf

#endif
