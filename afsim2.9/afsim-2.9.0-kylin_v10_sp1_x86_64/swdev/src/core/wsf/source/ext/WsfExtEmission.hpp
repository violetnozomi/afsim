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

#ifndef WSFEXTEMISSION_HPP
#define WSFEXTEMISSION_HPP

#include "wsf_export.h"

#include <cstdint>
#include <map>

class UtInput;

class WsfArticulatedPart;
class WsfEM_Xmtr;
class WsfEM_XmtrRcvr;
class WsfPlatform;
class WsfPlatformPart;
class WsfScenario;
class WsfSensor;
#include "WsfStringId.hpp"

class WSF_EXPORT WsfExtEmission
{
public:
   enum
   {
      cLEVEL_ONE_DEBUG = 0x00000001, //!< General debug
      cLEVEL_TWO_DEBUG = 0x00000002  //!< Debug beam data
   };

   class WSF_EXPORT BeamKey
   {
   public:
      BeamKey() = default;
      BeamKey(WsfStringId aId, WsfStringId aMode, int aBeamNumber);
      virtual ~BeamKey() = default;

      bool operator<(const BeamKey& aRhs) const;
      bool operator==(const BeamKey& aRhs) const;
      bool operator!=(const BeamKey& aRhs) const;

      WsfStringId mId;
      WsfStringId mMode;
      int         mBeamNumber{0};
   };

   WsfExtEmission(WsfScenario& aScenario);

   bool ProcessInput(UtInput& aInput);
   void ProcessBeamFunction(UtInput& aInput);
   void ProcessBeamType(UtInput& aInput);
   void ProcessEmitterFunction(UtInput& aInput);
   void ProcessEmitterType(UtInput& aInput);
   int  GetDebugMask() { return sDebugMask; }
   void SetDebugMask(int aMask) { sDebugMask = aMask; }

   uint8_t  GetBeamFunction(WsfStringId aPartTypeId, WsfStringId aModeNameId, int aBeamNumber);
   uint16_t GetBeamType(WsfStringId aPartTypeId, WsfStringId aModeNameId, int aBeamNumber);

   uint16_t    GetEmitterType(WsfStringId aPartTypeId);
   uint8_t     GetEmitterFunction(WsfStringId aPartTypeId);
   WsfStringId GetPartTypeId(uint16_t aEmitterType);

   void UpdateBeamLimits(WsfEM_XmtrRcvr& aXmtrRcvr, double& aMinAz, double& aMaxAz, double& aMinEl, double& aMaxEl);

   WsfSensor* GetSensor(WsfPlatform* aPlatformPtr,
                        WsfStringId  aSensorTypeId,
                        unsigned int aEmitterNameDIS,
                        unsigned int aEmitterIdNumberDIS);
   WsfSensor* GetSensor(WsfStringId aSensorTypeId, unsigned int aEmitterNameDIS, unsigned int aEmitterIdNumberDIS);

   WsfSensor* AddSensor(double       aSimTime,
                        WsfPlatform* aPlatformPtr,
                        WsfStringId  aSensorTypeId,
                        unsigned int aEmitterNameDIS,
                        unsigned int aEmitterIdNumberDIS);

   WsfScenario& GetScenario() const { return *mScenarioPtr; }

protected:
   WsfScenario* mScenarioPtr{nullptr};

   //! A bit mask for turning on debug levels
   unsigned int sDebugMask{0};

   //! The following maps a beam-key (sensor-type, mode-name, beam-index) to a DIS Beam Function.
   std::map<BeamKey, uint8_t> sBeamKeyToBeamFunction;

   //! The following maps a beam-key (sensor-type, mode-name, beam-index) to a DIS Beam Parameter Index.
   std::map<BeamKey, uint16_t> sBeamKeyToBeamType;

   //! The following maps a WSF comm/sensor type to DIS emitter function.
   std::map<WsfStringId, uint8_t> sPartTypeToEmitterFunction;

   //! The following maps a WSF comm/sensor type id to DIS emitter type.
   std::map<WsfStringId, uint16_t> sPartTypeToEmitterType;
   std::map<uint16_t, WsfStringId> sEmitterTypeToPartType;
};

#endif
