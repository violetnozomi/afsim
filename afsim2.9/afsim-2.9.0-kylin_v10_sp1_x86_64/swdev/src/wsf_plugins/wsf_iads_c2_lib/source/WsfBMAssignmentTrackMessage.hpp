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
*   File: WsfBMAssignmentTrackMessage.hpp
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

#ifndef WSF_BM_ASSIGNMENT_TRACK_MESSAGE_HPP
#define WSF_BM_ASSIGNMENT_TRACK_MESSAGE_HPP

#include <WsfBMCommon.hpp>
#include <WsfMessage.hpp>
#include <WsfTrack.hpp>
#include <WsfCovariance.hpp>
#include <WsfBMGenericTypeWrapper.hpp>

#include <iadsLib/assignmentTrackMessage.hpp>

class WsfSimulation;
class WsfLocalTrack;

class WsfBMAssignmentTrackMessage : public WsfMessage, public WsfBMManagedTypeWrapper<il::baseMessage>
{
   public:
      WsfBMAssignmentTrackMessage(WsfSimulation* simPtr);
      WsfBMAssignmentTrackMessage(WsfPlatform* plat);
      WsfBMAssignmentTrackMessage(WsfPlatform* plat, const std::shared_ptr<il::baseMessage>& message);

      WsfBMAssignmentTrackMessage(const WsfBMAssignmentTrackMessage& aSrc);

      virtual ~WsfBMAssignmentTrackMessage();

      WsfBMAssignmentTrackMessage* Clone() const override;

      const char* GetScriptClassName() const override;
      virtual const char* GetBaseClassName();
      static WsfStringId GetTypeId();

   public:
      void SetTrack(WsfSimulation* sim, const WsfLocalTrack& track);
      const WsfTrack& GetTrack() const;

      void SetAssignedPlatform(const WsfPlatform* plat);
      WsfPlatform* GetAssignedPlatform(WsfSimulation* sim) const;
   public:
      inline std::shared_ptr<const il::assignmentTrackMessage> GetRawMessagePtr() const { return std::static_pointer_cast <il::assignmentTrackMessage>(m_wrapped_ptr); }

   private:
      WsfTrack m_track;

   private:
      inline std::shared_ptr<il::assignmentTrackMessage> GetMutableRawMessagePtr() { return std::static_pointer_cast <il::assignmentTrackMessage>(m_wrapped_ptr); }
};

#endif
