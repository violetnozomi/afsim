// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <memory>
#include <QDir>
#include <QProcess>
#include <QString>
#include <QTextStream>

#include "AeroEngine.hpp"
#include "ScriptGenerator.hpp"

#ifndef SCRIPT_GENERATOR_GM_HPP
#define SCRIPT_GENERATOR_GM_HPP

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

   class ScriptGeneratorGM : public ScriptGenerator
   {
      public:
         ScriptGeneratorGM();
         ~ScriptGeneratorGM() = default;

         void CreateFolderStructure() override;

         private:
            void CreatePlatformFile() override;
            void CreateTestFiles() override;
            void CreateBallisticRangeTestFile(const QString& aTestDir);
			   void CreateThrustDragTestFile(const QString& aTestDir, double aAltitude_ft);
            void CreateEnduranceTestFile(const QString& aTestDir);
            void CreateRangeTestFile(const QString& aTestDir);
            void CreateLevelFlightRangeTestFile(const QString& aTestDir);
            void CreateMaximumSpeedTestFile(const QString& aTestDir);
            void OutputMover(QTextStream& aStream, double aMaxMach);
            void OutputAero(QTextStream& aStream, double& aMaxMach);
            void OutputPropulsion(QTextStream& aStream, double aMaxMach);
            AeroEngine::PropulsionParameters ComputePropulsion(double aMaxMach) const;

            static std::map<double, std::vector<double>>  CombineData(std::vector<std::vector<std::pair<double, double>>> aData);
            static std::vector<std::pair<double, double>> AverageData(const std::vector<std::vector<std::pair<double, double>>>& aData, const std::vector<double>& aWeights, double aNormalizingFactor);
            static std::vector<std::pair<double, double>> AddData(const std::vector<std::vector<std::pair<double, double>>>& aData);
   };
}
#endif