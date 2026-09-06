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

#ifndef WSFDISEMISSION_HPP
#define WSFDISEMISSION_HPP

#include "wsf_export.h"

#include <map>

#include "DisEmission.hpp"
#include "UtCallback.hpp"

// Forward declarations
class UtInput;
class WsfArticulatedPart;
class WsfDisInterface;
class WsfEM_Xmtr;
class WsfEM_XmtrRcvr;
class WsfPlatform;
class WsfPlatformPart;
class WsfSensor;
class WsfExtEmission;

class WSF_EXPORT WsfDisEmission : public DisEmission
{
public:
   WsfDisEmission(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   WsfDisEmission()           = default;
   ~WsfDisEmission() override = default;

   int Process() override;

   using OverrideTypeAndFunctionCallback = UtCallbackListN<void(WsfPlatformPart*, DisSystem*, DisEnum16&, DisEnum8&)>;
   static OverrideTypeAndFunctionCallback OverrideTypeAndFunction;

   WsfExtEmission* GetExtEmission() const;
   // WsfDisEmission* GetDisEmission() const;

   //! @name Update methods using DIS emission PDU data.
   //@{
   void UpdateSensor(DisSystem* aSystemPtr, WsfSensor* aSensorPtr);

   //! Updates the articulated part given the DIS emission PDU location data.
   void UpdateLocation(DisSystem* aSystemPtr, WsfArticulatedPart* aArticulatedPartPtr);
   //@}

   //! Updates an EM transmitter using the DIS emission PDU beam data.
   void UpdateXmtr(DisBeam* aBeamPtr, WsfEM_Xmtr* aXmtrPtr, bool& aBeamOn);

   //! Update the system-beam to xmtr ID mapping
   void UpdateSysBeamMapping(unsigned int aSysUniqueId, unsigned int aBeamUniqueId, unsigned int aXmtrUniqueId);

private:
   void UpdateShadowPlatform(WsfPlatform* aShadowPlatformPtr);

   void UpdatePlatform(WsfPlatform* aPlatformPtr);

   void NotifySensorListeners(DisBeam* aBeamPtr, WsfEM_Xmtr* aXmtrPtr);

   //! Pointer to the DIS interface
   WsfDisInterface* mInterfacePtr{nullptr};
};

#endif
