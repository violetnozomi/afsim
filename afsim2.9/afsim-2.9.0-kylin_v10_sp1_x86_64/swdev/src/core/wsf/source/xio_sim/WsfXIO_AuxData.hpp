// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFXIO_AUXDATA_HPP
#define WSFXIO_AUXDATA_HPP

#include "wsf_export.h"

#include <mutex>
#include <set>

#include "UtCallbackHolder.hpp"

class WsfAuxDataEnabled;
class WsfPlatform;
class WsfXIO_AuxDataPkt;
class WsfXIO_Interface;
class WsfXIO_Simulation;

namespace wsf
{
namespace xio
{
class WSF_EXPORT AuxData
{
public:
   explicit AuxData(WsfXIO_Simulation& aSimulation);

private:
   //! Called when a WsfXIO_AuxDataPkt packet is received.
   void HandleIncomingAuxData(WsfXIO_AuxDataPkt& aPkt);

   //! Creates and sends a WsfXIO_AuxDataPkt for all relevant platforms.
   void SendAllPlatformAuxData(double aSimTime);

   //! Creates and sends a WsfXIO_AuxDataPkt for the specified platform.
   void SendPlatformAuxData(double aSimTime, const WsfPlatform* aPlatform);

   //! Callback for WsfAuxDataEnabled::AuxDataAccessed.
   //! Will not be connected if the user specified `send_aux_data_updates false`.
   //! Adds aSource to mAuxDataAccessed.
   //! The next time SendAllPlatformAuxData is called, a WsfXIO_AuxDataPkt will be sent for *aSource.
   void AuxDataAccessed(const WsfAuxDataEnabled* aSource);

   //! Callback for WsfAuxDataEnabled::AuxDataDestroyed.
   //! Will not be connected if the user specified `send_aux_data_updates false`.
   //! Removes aDestroyed from mAuxDataAccessed.
   //! Required to prevent use-after-free errors.
   //! @note aDestroyed CANNOT be checked using dynamic_cast!
   void AuxDataDestroyed(const WsfAuxDataEnabled* aDestroyed);

   using AuxDataAccessedItems = std::set<const WsfAuxDataEnabled*>;

   WsfXIO_Simulation& mSimulation;
   WsfXIO_Interface*  mInterface;
   UtCallbackHolder   mCallbacks;

   std::mutex           mAuxDataAccessedMutex;
   AuxDataAccessedItems mAuxDataAccessed;
};
} // namespace xio
} // namespace wsf

#endif
