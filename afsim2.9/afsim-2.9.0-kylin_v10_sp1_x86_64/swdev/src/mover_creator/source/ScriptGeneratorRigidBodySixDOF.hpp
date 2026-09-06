// ****************************************************************************
// UNCLASSIFIED//FOUO
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2020 Infoscitex, a DCS Company. All rights reserved.
//
// Distribution authorized to Department of Defense and U.S. DoD contractors only.
// You may not use this file except in compliance with the
// terms and conditions of 48 C.F.R. 252.204-7000 (Disclosure of Information),
// 48 C.F.R. 252.227-7025 (Limitations on the Use or Disclosure of Government-
// Furnished Information Marked with Restrictive Legends), and the AFSIM
// Memorandum of Understanding or Information Transfer Agreement as applicable.
// All requests for this software must be referred to the Air Force Research
// Laboratory Aerospace Systems Directorate, 2130 8th St., Wright-Patterson AFB,
// OH 45433. This software is provided "as is" without warranties of any kind.
//
// WARNING - EXPORT CONTROLLED
// This document contains technical data whose export is restricted by the
// Arms Export Control Act (Title 22, U.S.C. Sec 2751 et seq.) or the Export
// Administration Act of 1979, as amended, Title 50 U.S.C., App. 2401 et seq.
// Violations of these export laws are subject to severe criminal penalties.
// Disseminate in accordance with provisions of DoD Directive 5230.25.
// ****************************************************************************

#ifndef SCRIPT_GENERATOR_RB6DOF_HPP
#define SCRIPT_GENERATOR_RB6DOF_HPP

#include <memory>
#include <QDir>
#include <QProcess>
#include <QString>
#include <QTextStream>

#include "ScriptGeneratorSixDOF.hpp"

namespace Designer
{
   class AeroTable4d;
   class AeroTable3d;
   class AeroTable2d;
   class AeroEngine;
   class Vehicle;
   class VehicleAeroMovable;
   class VehicleGeometry;
   class VehicleControls;

   class ScriptGeneratorRigidBodySixDOF : public ScriptGeneratorSixDOF
   {
      public:
         ScriptGeneratorRigidBodySixDOF();
         ~ScriptGeneratorRigidBodySixDOF() = default;

         QString GetFilenameSuffix() override { return "_rb6"; }

      protected:

         AeroEngine::ExportMoverType GetMoverType() override { return AeroEngine::ExportMoverType::cRIGID_BODY; }
         QString GetDefaultIntegratorName() override { return "STANDARD_RIGID_BODY_INTEGRATOR"; }
         QString GetScriptMoverType() override { return "WSF_RIGID_BODY_SIX_DOF_MOVER"; }

      private:

         virtual void CreateMoverFile();
         virtual void CreatePropulsionFile();
         void CreateTestFiles() override;

         void CreateAutogenHeader(Vehicle* aVehiclePtr, QTextStream& aOutStream);

         QString GenerateControlMethodString();

         virtual void CreateControlsInputFile(QString& aControlsDir);
         virtual void CreateFlightControlsFile(QString& aControlsDir);
         void CreateAutopilotConfigFile(QString& aControlsDir);
         virtual void CreateAeroFile(QString& aAeroDir);
         virtual void CreateAeroTables(QString& aAeroDir);
         void CreateAeroComponentTables(QString& aAeroDir, VehicleAeroMovable aComponent);

         // Performance tests file creation
         void CreatePitchingMomentTest(QString& aTestDir, double aAltitude_ft, double aMach);

         virtual void CreatePerformanceTestFiles(QString& aTestDir);

         void OutputControlDataForMovableComponents(QTextStream& aOutputStream);

         void OutputConfigStabsVerticalTail(QTextStream& aOutputStream);
         void OutputConfigStabsVentralTail(QTextStream& aOutputStream);
         void OutputConfigStabsUpperLowerTails(QTextStream& aOutputStream);
         void OutputConfigVTail(QTextStream& aOutputStream);
         void OutputConfigCanardsVerticalTail(QTextStream& aOutputStream);
         void OutputConfigCanardsStabsVerticalTail(QTextStream& aOutputStream);
         void OutputConfigFlyingWing(QTextStream& aOutputStream);
         void OutputConfigStabsFlyingWing(QTextStream& aOutputStream);

         bool OutputAileronControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls);
         bool OutputSpoileronControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls);
         bool OutputSpoilerControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls);
         bool OutputDrageronControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls);
         bool OutputElevonControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls);
         bool OutputWingControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls);
         bool OutputHorizontalTailControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls);
         bool OutputCanardControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls);
         bool OutputVerticalTailControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls);
         bool OutputVentralTailControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls);
         bool OutputVTailControl(QTextStream& aOutputStream, const VehicleGeometry& geometry, VehicleControls& controls);

         void OutputConfigTailFins(QTextStream& aOutputStream);
         void OutputConfigMidFins(QTextStream& aOutputStream);
         void OutputConfigCanardFins(QTextStream& aOutputStream);

         void OutputWingControlSurfaces(QTextStream& aOutputStream);
         void OutputSpeedBrakes(QTextStream& aOutputStream);
         void OutputLandingGear(QTextStream& aOutputStream);

         // This outputs a single control surface to the specified output stream
         void OutputControlSurface(QTextStream&                aOutputStream,
                                   const QString&              aControlSurfaceName,
                                   double                      aMinAngle_deg,
                                   double                      aMaxAngle_deg,
                                   double                      aCurrentAngle_deg,
                                   double                      aMinActuatorAngle_deg,
                                   double                      aMaxActuatorAngle_deg,
                                   double                      aMaxNegativeActuatorRate_dps,
                                   double                      aMaxPositiveActuatorRate_dps,
                                   double                      aCurrentActuatorAngle_deg,
                                   bool                        aUseExpo,
                                   bool                        aUseStickBack,
                                   const std::vector<QString>& aStickBackModifierList,
                                   bool                        aUseStickRight,
                                   const std::vector<QString>& aStickRightModifierList,
                                   bool                        aUseRudderRight,
                                   const std::vector<QString>& aRudderRightModifierList);

         void OutputControlSurface(QTextStream&                aOutputStream,
                                   const QString&              aControlSurfaceName,
                                   double                      aMinAngle_deg,
                                   double                      aMaxAngle_deg,
                                   double                      aCurrentAngle_deg,
                                   double                      aMinActuatorAngle_deg,
                                   double                      aMaxActuatorAngle_deg,
                                   double                      aMaxNegativeActuatorRate_dps,
                                   double                      aMaxPositiveActuatorRate_dps,
                                   double                      aCurrentActuatorAngle_deg,
                                   bool                        aUseExpo,
                                   bool                        aUseStickBack,
                                   const std::vector<QString>& aStickBackModifierList,
                                   bool                        aUseStickRight,
                                   const std::vector<QString>& aStickRightModifierList,
                                   bool                        aUseRudderRight,
                                   const std::vector<QString>& aRudderRightModifierList,
                                   bool                        aUseSpeedBrakesOut,
                                   const std::vector<QString>& aSpeedBrakesOutModifierList);

         // This outputs an actuator data block to the specified output stream
         void OutputActuatorData(QTextStream& aOutputStream,
                                 double       aMax_positive_rate_dps,
                                 double       aMax_negative_rate_dps,
                                 double       aMax_angle_deg,
                                 double       aMin_angle_deg,
                                 double       aCurrent_angle_deg);

         // This outputs a control input block to the specified output stream
         void OutputControlInputBlock(QTextStream&                aOutputStream,
                                      const QString&              aControlInputName,
                                      const std::vector<QString>& aModifierList);

         // This outputs a single control surface to the specified output stream
         void OutputAngleMappingTable(QTextStream& aOutputStream,
                                      bool         aUseExponential,
                                      double       aMinAngle_deg,
                                      double       aMaxAngle_deg);

         // This outputs a series of pairs to the specified output stream
         void OutputPairsInAngleMappingTable(QTextStream&                                  aOutputStream,
                                             const std::vector<std::pair<double, double>>& aVectorPair);

   };
}

#endif //SCRIPT_GENERATOR_RB6DOF_HPP
