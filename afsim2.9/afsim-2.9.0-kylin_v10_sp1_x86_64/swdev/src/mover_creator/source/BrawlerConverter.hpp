// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef BRAWLER_CONVERTER_HPP
#define BRAWLER_CONVERTER_HPP

#include <memory>
#include <QDir>
#include <QProcess>
#include <QString>
#include <QTextStream>

#include "BrawlerMover.hpp"
#include "ScriptGeneratorPointMassSixDOF.hpp"
#include "Vehicle.hpp"

namespace Designer
{
   class BrawlerMoverData : public BrawlerMover
   {
         // With no connection to WSF, this function can be empty
         void DeleteMover() override {};
   };

   class BrawlerConverter : public ScriptGeneratorPointMassSixDOF
   {
      public:
         explicit BrawlerConverter(const QString& aFileName);
         ~BrawlerConverter() = default;

         bool OutputSixDOF_Vehicle();

      private:

         void PopulateCD_Table();
         void PopulateCL_Table();

         void CreateMoverFile() override;
         void CreatePropulsionFile() override;
         void CreateControlsInputFile(QString& aControlsDir) override;
         void CreateAutopilotG_LimitConfigFile(QString& aControlsDir) override;
         void CreateFlightControlsFile(QString& aControlsDir) override;
         void CreateAeroFile(QString& aAeroDir) override;
         void CreateAeroTables(QString& aAeroDir) override;

         BrawlerMoverData mBrawlerMover;

         AeroTable4d mLiftTable              = AeroTable4d("CL", "Alpha", "Beta", "Mach");
         AeroTable4d mDragTable              = AeroTable4d("Cd", "Alpha", "Beta", "Mach");
         AeroTable2d mRollAccelerationTable  = AeroTable2d("Maximum_Roll_Acceleration",   "Mach");
         AeroTable2d mPitchAccelerationTable = AeroTable2d("Maximum_Pitch_Acceleration",  "Mach");
         AeroTable2d mYawAccelerationTable   = AeroTable2d("Maximum_Yaw_Acceleration",    "Mach");
         AeroTable2d mRollFrequencyTable     = AeroTable2d("Roll_Stabilizing_Frequency",  "Mach");
         AeroTable2d mPitchFrequencyTable    = AeroTable2d("Alpha_Stabilizing_Frequency", "Mach");
         AeroTable2d mYawFrequencyTable      = AeroTable2d("Beta_Stabilizing_Frequency",  "Mach");
         AeroTable2d mSpeedbrakeDragTable    = AeroTable2d("Speedbrake_Cd",               "Mach");

         std::vector<AeroTableBase*> mTableVec;
   };
}

#endif // BRAWLER_CONVERTER_HPP
