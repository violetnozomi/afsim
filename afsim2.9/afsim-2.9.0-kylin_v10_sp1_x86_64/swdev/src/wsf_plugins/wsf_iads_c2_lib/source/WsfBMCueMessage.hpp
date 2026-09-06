// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: WsfBMCueMessage.hpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#ifndef WSF_BM_CUE_MESSAGE_HPP
#define WSF_BM_CUE_MESSAGE_HPP

#include <memory>
#include <WsfBMCommon.hpp>

#include <WsfMessage.hpp>
#include <WsfLocalTrack.hpp>

#include <iadsLib/cueMessage.hpp>
#include "WsfBMGenericTypeWrapper.hpp"

class WsfPlatform;
class WsfSimulation;
class WsfSensor;

class WsfBMCueMessage : public WsfMessage, public WsfBMManagedTypeWrapper<il::baseMessage>
{
   public:
      WsfBMCueMessage(WsfSimulation* simPtr);
      WsfBMCueMessage(WsfPlatform* plat);
      WsfBMCueMessage(WsfPlatform* plat, const std::shared_ptr<il::baseMessage>& message);

      WSF_IADS_C2_LIB_EXPORT WsfBMCueMessage(const WsfBMCueMessage& aSrc);

      virtual WSF_IADS_C2_LIB_EXPORT ~WsfBMCueMessage();

      WsfBMCueMessage* Clone() const override;

      const char* GetScriptClassName() const override;
      virtual const char* GetBaseClassName();
      static WsfStringId GetTypeId();

      void SetCueTime(const double sim_time_s);
      double GetCueTime() const;

      void SetReferenceTrackID(const WsfTrackId& track_id);
      const WsfTrackId GetReferenceTrackID() const;

      void SetLocalTrackID(const WsfTrackId& track_id);
      const WsfTrackId GetLocalTrackID() const;

      void SetInitiatingPlatform(const WsfPlatform* plat);
      const WsfPlatform* GetInitiatingPlatform(WsfSimulation* sim_ptr) const;

      void SetCuedSensor(const WsfPlatform* plat, const WsfSensor* sensor);
      const WsfPlatform* GetCuedPlatform(WsfSimulation* sim_ptr) const;
      const WsfSensor* GetCuedSensor(WsfSimulation* sim_ptr) const;

      void SetCueReason(const il::cueMessage::CueReason reason);
      il::cueMessage::CueReason GetCueReason() const;
      std::string GetCueReasonString() const;

      void Print() const;

      inline std::shared_ptr<const il::cueMessage> GetRawMessagePtr() const { return std::static_pointer_cast <il::cueMessage>(m_wrapped_ptr); }

   private:
      inline std::shared_ptr<il::cueMessage> GetMutableRawMessagePtr() { return std::static_pointer_cast <il::cueMessage>(m_wrapped_ptr); }
};

#endif
