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

#ifndef SCRIPT_GENERATOR_P6DOF_HPP
#define SCRIPT_GENERATOR_P6DOF_HPP

#include <memory>
#include <QDir>
#include <QProcess>
#include <QString>
#include <QTextStream>

#include "ScriptGenerator.hpp"

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

   class ScriptGeneratorP6DOF : public ScriptGenerator
   {
      public:
         ScriptGeneratorP6DOF();
         ~ScriptGeneratorP6DOF() = default;

         void CreateFolderStructure() override;

         // Creates the autopilot tuning scenario file using the specified latitude (deg),
         // longitude (deg), altitude (meters), heading (deg), and speed (m/sec).
         // Returns the path to the file created.
         const QString CreateAutopilotTestFile(double  aLatitude,
                                               double  aLongitude,
                                               double  aAltitude,
                                               double  aHeading,
                                               double  aSpeed);

         // Creates the flight test scenario file using the specified latitude (deg),
         // longitude (deg), altitude (feet), heading (deg), and speed (ft/sec).
         // Returns the path to the file created.
         const QString CreateFlightTestFile(double aLatitude,
                                            double aLongitude,
                                            double aAltitude_ft,
                                            double aHeading_deg,
                                            double aSpeed_fps);

         QString ControlsFolderPath() const;
         void CreateAutopilotG_LimitConfigFile(QString& aControlsDir);
         void CreateHardwareG_LimitsConfigFile(QString& aControlsDir);
         void CopyAutopilotSupportTables(const QString& aControlsSourceDir);

      private:

         void CreateBaseScenarioFile();
         void CreateEnvironmentFiles();
         void CreateSignaturesFile();
         void CreateMoverFile();
         void CreatePropulsionFile();
         void CreateTestFiles() override;
         void CreatePlatformFile() override;
         void CreateAdditionalFiles() override;

         void CreateAutogenHeader(Vehicle* aVehiclePtr, QTextStream& aOutStream);

         QString GenerateControlMethodString();

         void CreateControlsInputFile(QString& aControlsDir);
         void CreateFlightControlsFile(QString& aControlsDir);
         void CreateAutopilotConfigFile(QString& aControlsDir);
         void CreateHardwareAutopilotConfigFile(QString& aControlsDir);
         void CreateStubAutopilotSupportFile(QString& aControlsDir);
         void CreateAeroFile(QString& aAeroDir);
         void CreateAeroTables(QString& aAeroDir);
         void CreateAeroComponentTables(QString& aAeroDir, VehicleAeroMovable aComponent);

         // Performance tests file creation
         void CreateCommonTestFile(QString& aTestDir);
         void CreateEnvelopeCommonTestFile(QString& aTestDir);
         void CreateBallisticRangeTestFile(QString& aTestDir);
         void CreateEnduranceTestFile(QString& aTestDir);
         void CreateFlightEnvelopeSimpleTestFile(QString& aTestDir);
         void CreateFlightEnvelopeAbTestFile(QString& aTestDir);
         void CreateFlightEnvelopeMilTestFile(QString& aTestDir);
         void CreateGCapabilityTestFile(QString& aTestDir);
         void CreateLevelFlightRangeTestFile(QString& aTestDir);
         void CreateMaximumSpeedTestFile(QString& aTestDir);
         void CreateRangeTestFile(QString& aTestDir);
         void CreateTurnPerformanceTestFile(QString& aTestDir, int aAltitude_ft, bool aUseAB);
         void CreatePitchingMomentTest(QString& aTestDir, double aAltitude_ft, double aMach);
         void CreateThrustDragTest(QString& aTestDir, double aAltitude_ft);

         // Performance test scripts that are used in multiple tests
         QString GetFlyStraightLevelFullThrottleScript();
         QString GetRangeSinceStart_kmScript();
         QString GetSetMasterNoLagTestingScript();
         QString GetSetMasterFreezeAltitudeScript();
         QString GetSetMasterFreezePitchScript();
         QString GetSetMasterFreezeRollScript();
         QString GetSetMasterFreezeVerticalSpeedScript();
         QString GetSetMasterFreezeYawScript();
         QString GetSetMasterFreezeFuelBurnScript();
         QString GetSetMasterNoAlphaTestingScript();
         QString GetEnableControlsScript();
         QString GetFuelRemainingScript();
         QString GetFlightEnvelopeScripts();
         void CreateAutopilotSupportGeneratorFile(QString& aTestDir);
         void CreatePerformanceTestFiles(QString& aTestDir);

         void Create4dTableFile(const QString& aAeroDir, AeroTable4d* aTablePtr);
         void Create3dTableFile(const QString& aAeroDir, AeroTable3d* aTablePtr);
         void Create2dTableInline(QTextStream* aStreamPtr, AeroTable2d* aTablePtr);

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

         double CalcSolidPropellantMass_lbs();

         std::shared_ptr<QProcess>    mAFSIMProcessPtr = nullptr;
   };
}

#endif //SCRIPT_GENERATOR_P6DOF_HPP
