// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WSF_SENSORS_MANAGER_FOV_IMPL
#define WSF_SENSORS_MANAGER_FOV_IMPL

#include <memory>

#include "iadsLib/sensorsManagerInterface.hpp"

namespace il
{
   class trackRecord;
   class assignmentMessage;
   class sensorRecord;
   class CueReason;
}

class WsfSensorsManagerFovImpl : public il::SensorsManagerInterface
{
   public:
      WsfSensorsManagerFovImpl();
      WsfSensorsManagerFovImpl* Clone() override;
      virtual ~WsfSensorsManagerFovImpl();

   protected:
      WsfSensorsManagerFovImpl(const WsfSensorsManagerFovImpl& src);

      bool SendSensorCueMessage(il::moeLoggingInterface& aMOELogger,
                                const double simTime,
                                const std::shared_ptr<const il::trackRecord>& masterTrack,
                                const std::shared_ptr<const il::assignmentMessage>& assignment,
                                const std::shared_ptr<il::sensorRecord>& sensor,
                                const il::cueMessage::CueReason& reason) override;

      virtual bool ProcessCurrentCueAssignment(il::moeLoggingInterface& aMOELogger,
                                               const double simTime,
                                               const std::shared_ptr<const il::trackRecord>& masterTrack,
                                               const std::shared_ptr<const il::assignmentMessage>& assignment,
                                               const std::shared_ptr<il::sensorRecord>& sensor);

      virtual bool ProcessDropCue(il::moeLoggingInterface& aMOELogger,
                                  const double simTime,
                                  const std::shared_ptr<const il::trackRecord>& masterTrack,
                                  const std::shared_ptr<const il::assignmentMessage>& assignment,
                                  const std::shared_ptr<il::sensorRecord>& sensor);

      virtual void TurnOffSensorIfNoTracks(il::moeLoggingInterface& aMOELogger,
                                           const double simTime,
                                           const std::shared_ptr<il::sensorRecord>& sensor);

   private:
      WsfSensorsManagerFovImpl& operator=(const WsfSensorsManagerFovImpl&); // = delete;
};

#endif
