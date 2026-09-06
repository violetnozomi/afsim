// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef SENSORMANAGERFOVSCHEDULER_HPP
#define SENSORMANAGERFOVSCHEDULER_HPP

#include "WsfDefaultSensorScheduler.hpp"

class WsfSensorsManagerFovSensorScheduler : public WsfDefaultSensorScheduler
{
   public:
      WsfSensorsManagerFovSensorScheduler();
      WsfSensorsManagerFovSensorScheduler(const WsfDefaultSensorScheduler&);
      virtual ~WsfSensorsManagerFovSensorScheduler();

      WsfSensorScheduler* Clone() const override;

      void TurnOn(double simTime) override;
      void TurnOff(double simTime) override;

   protected:
      WsfSensorsManagerFovSensorScheduler(const WsfSensorsManagerFovSensorScheduler&);

   private:
      WsfSensorsManagerFovSensorScheduler& operator=(const WsfSensorsManagerFovSensorScheduler&) = delete;
};

#endif
