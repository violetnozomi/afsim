// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef TRAJECTORYPARTICIPANTS_HPP
#define TRAJECTORYPARTICIPANTS_HPP

// Qt
#include <QWidget>

#include "ReportWidget.hpp"

namespace Ui
{
class TrajectoryParticipants;
}

namespace PostProcessor
{
// Participants for Trajectory Report
class TrajectoryParticipants : public ReportWidget
{
      Q_OBJECT
   public:
      TrajectoryParticipants();
      ~TrajectoryParticipants() override;

      void WriteData(std::ostream& aStream) override;

   private:
      void RestoreDefaults() override;
      Ui::TrajectoryParticipants* mUIPtr;
};
}
#endif // !TRAJECTORYPARTICIPANTS_HPP