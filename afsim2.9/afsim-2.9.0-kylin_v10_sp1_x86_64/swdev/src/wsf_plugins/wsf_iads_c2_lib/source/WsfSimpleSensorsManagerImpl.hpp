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
*   File: WsfSimpleSensorsManagerImpl.hpp
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
*   Abstract: Since what differentiates the simple sensor manager from the FOV,
*     is mostly AFSIM-implementation details, we bubble the nuances between the
*     two up to the AFSIM glue level while keeping all commonality in the core
*     IADSLib.
*
*******************************************************************************/

#ifndef WSF_SIMPLE_SENSORS_MANAGER_IMPL_HPP
#define WSF_SIMPLE_SENSORS_MANAGER_IMPL_HPP

#include <WsfBMCommon.hpp>
#include <iadsLib/sensorsManagerInterface.hpp>

class WsfSimpleSensorsManagerImpl : public il::SensorsManagerInterface
{
   public:
      WsfSimpleSensorsManagerImpl();
      virtual ~WsfSimpleSensorsManagerImpl();

      WsfSimpleSensorsManagerImpl* Clone() override;


   protected:
      WsfSimpleSensorsManagerImpl(const WsfSimpleSensorsManagerImpl& from);

      // hijaack outgoing sensor manager cues for direct cueing in AFSIM
      bool SendSensorCueMessage(il::moeLoggingInterface& aMOELogger,
                                const double sim_time,
                                const std::shared_ptr<const il::trackRecord>& master_track,
                                const std::shared_ptr<const il::assignmentMessage>& assignment,
                                const std::shared_ptr<il::sensorRecord>& sensor,
                                const il::cueMessage::CueReason& reason) override;
      virtual bool ProcessCurrentCueAssignment( il::moeLoggingInterface& aMOELogger,
                                               const double                                        sim_time,
                                               const std::shared_ptr<const il::trackRecord>&       master_track,
                                               const std::shared_ptr<const il::assignmentMessage>& assignment,
                                               const std::shared_ptr<il::sensorRecord>&            sensor);
      virtual bool ProcessDropCue(il::moeLoggingInterface& aMOELogger,
                                  const double sim_time,
                                  const std::shared_ptr<const il::trackRecord>& master_track,
                                  const std::shared_ptr<const il::assignmentMessage>& assignment,
                                  const std::shared_ptr<il::sensorRecord>& sensor);
      virtual void TurnOffSensorIfNoTracks(il::moeLoggingInterface& aMOELogger,
                                           const double sim_time,
                                           const std::shared_ptr<il::sensorRecord>& sensor);

   private:
      WsfSimpleSensorsManagerImpl& operator=(const WsfSimpleSensorsManagerImpl& rhs) = delete;
};

#endif
