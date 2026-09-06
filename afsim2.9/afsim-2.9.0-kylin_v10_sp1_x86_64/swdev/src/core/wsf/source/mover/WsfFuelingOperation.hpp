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

#ifndef WSFFUELINGOPERATION_HPP
#define WSFFUELINGOPERATION_HPP

#include "wsf_export.h"

#include <string>
#include <vector>

class WsfTankedFuel;

//! WsfPlatform -to- WsfPlatform refueling operation.
//! This class models tanking operations, from the "Tanker" (supplier) perspective,
//! pushing fuel into a "Receiver" (client) fuel tank.  Transfer rate is assumed
//! constant in this base class, but may be overridden to be variable in a derived
//! class.  The periodic Update() call is normally done by the Tanker, but may be
//! called by the Receiver (if and when a voluntary disconnect is desired).  The
//! class will not concern itself with the physical proximity of the two platforms
//! transferring fuel, which must be managed elsewhere.  To conserve run time, if
//! the transfer rate is constant, there are two virtual methods (null in the base
//! class) that calculate a fuel transfer rate, one used only at construct time,
//! and another during run time.
class WSF_EXPORT WsfFuelingOperation
{
public:
   //! This boolean specifies whether fuel supply operations from wing fuel
   //! points (if any) and the centerline station (if any) are permitted
   //! simultaneously on the same aircraft.  The default is unrestricted
   //! operations, allowing the individual aircraft settings to prevail.
   static bool AllowSimultWingAndCenterlineFueling();
   static void SetAllowSimultWingAndCenterlineFueling(bool aValue);

   //! This boolean specifies whether fuel supply operations from refueling
   //! booms (if any) and hose-and-drogue (if any) are permitted simultaneously
   //! on the same aircraft.  The default is unrestricted operations, allowing
   //! the individual aircraft settings to prevail.
   static bool AllowSimultBoomAndHoseFueling();
   static void SetAllowSimultBoomAndHoseFueling(bool aValue);

   //! This enumeration describes the permitted methods of supplying fuel.
   enum MethodType
   {
      cNO_METHOD = 0, //!< No fuel may be transferred.
      cBOOM      = 1, //!< An extensible, controllable boom.
      cHOSE      = 2  //!< A flexible trailing hose and drogue.
   };

   //! This enumeration describes the preferred location to supply fuel from (if and when ambiguous).
   enum Preference
   {
      cNO_PREFERENCE = 0, //!< No station preference has been given.
      cCHOOSE_CENTER = 1, //!< When there is a choice, fuel from the centerline.
      cCHOOSE_WING   = 2  //!< When there is a choice, fuel from the wing stations.
   };

   //! This enumeration describes the permitted stations for supplying fuel.
   enum StationType
   {
      cNO_STATION = 0, //!< Does not indicate a valid fuel supply station.
      cLEFT_WING  = 1, //!< Left wing fuel supply station
      cCENTERLINE = 2, //!< Centerline fueling station
      cRIGHT_WING = 3  //!< Right wing fueling station
   };

   // static unsigned int GetUInt(StationType aStation);

   // Constructors/Destructor
   WsfFuelingOperation();
   WsfFuelingOperation(double         aSimTime,
                       WsfTankedFuel* aSupplierPtr,
                       WsfTankedFuel* aReceiverPtr,
                       MethodType     aMethod,
                       StationType    aStation,
                       double         aInitRate);
   WsfFuelingOperation(const WsfFuelingOperation& aSrc) = delete;
   virtual ~WsfFuelingOperation();

   virtual double Update(double aSimTime, bool& aDisconnected);

   //! Supplies the transfer rate of the operation in progress.
   double GetTransferRate() const { return mXferRate; }

   //! Supplies the cumulative fuel transferred during the operation in progress.
   double FuelTransferred() const { return mCumFuelXfer; }

   //! Supplies the cumulative elapsed time of the operation in progress.
   double FuelingTime() const { return mLastUpdate - mStartFuelTime; }

   //! Supplies the last Update time of the operation in progress.
   double LastUpdate() const { return mLastUpdate; }

   //! Supplies a pointer to the fuel tank supplying the fuel.
   WsfTankedFuel* SupplyTankPtr() const { return mSupplierPtr; }

   //! Supplies a pointer to the fuel tank receiving the fuel.
   WsfTankedFuel* ReceiveTankPtr() const { return mReceiverPtr; }

   //! Supplies the enumeration describing the fuel supply method (cHOSE, cBOOM).
   MethodType GetMethod() const { return mMethod; }

   std::string GetMethodName() const;
   std::string GetStationName() const;

   struct SupplyPoint
   {
      SupplyPoint() = default;

      SupplyPoint(MethodType& aMethod, StationType& aStation, double aRate = 0.0)
         : mMethod(aMethod)
         , mStation(aStation)
         , mOpPtr(nullptr)
         , mConstRate(aRate)
      {
      }

      std::string GetMethodName() const;
      std::string GetStationName() const;

      MethodType           mMethod{cNO_METHOD};
      StationType          mStation{cNO_STATION};
      WsfFuelingOperation* mOpPtr{nullptr};
      double               mConstRate{0.0};
   };

   using SupplyPoints         = std::vector<SupplyPoint>;
   using SupplyPointIter      = std::vector<SupplyPoint>::iterator;
   using SupplyPointConstIter = std::vector<SupplyPoint>::const_iterator;

protected:
   virtual double CalcInitXferRate(double aSimTime) { return mXferRate; }
   virtual double CalcRunXferRate(double aSimTime) { return mXferRate; }

private:
   void DisconnectP(double aSimTime);

   WsfTankedFuel* mSupplierPtr;
   WsfTankedFuel* mReceiverPtr;
   double         mStartFuelTime;
   double         mLastUpdate; // Will also represent mLastFuelTime after termination.
   double         mXferRate;
   double         mCumFuelXfer;
   bool           mCmdDisconnect; // This flag allows for the Receiver to terminate early.
   bool           mInternalDisc;
   MethodType     mMethod;
   StationType    mStation;
};

#endif
