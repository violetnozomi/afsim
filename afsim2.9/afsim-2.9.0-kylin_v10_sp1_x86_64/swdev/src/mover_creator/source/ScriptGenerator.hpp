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

#ifndef SCRIPT_GENERATOR_HPP
#define SCRIPT_GENERATOR_HPP

#include <memory>
#include <QDir>
#include <QProcess>
#include <QString>
#include <QTextStream>

namespace Designer
{
   class AeroTable2d;
   class AeroEngine;
   class Vehicle;

   // This is the base class from which ScriptGeneratorP6DOF and ScriptGeneratorGM derive
   class ScriptGenerator
   {
   public:
      ScriptGenerator();
      ~ScriptGenerator() = default;

      // Returns true if succeeds
      bool GenerateDataFiles();

      virtual void CreateFolderStructure() = 0;

      QDir GetTestDir() const { return mTestDir; }
      QDir GetVehicleDir() const { return mVehicleDir; }

   protected:

      enum class PerformanceTestType
      {
         cNO_CONTROL_NO_ENGINE, // e.g. spent stage of a missile, unguided munitions
         cNO_CONTROL_ENGINE,    // e.g. boost stage on a missile
         cBANK_TO_TURN,         // e.g. aircraft, guided drone missiles
         cSKID_TO_TURN          // e.g. intercept air missiles
      };

      virtual void CreatePlatformFile() = 0;
      virtual void CreateTestFiles() = 0;
      virtual void CreateAdditionalFiles() {}

      void CreateFolderIfDoesntExist(const QString& aPath);

      void CreateAutogenHeader(Vehicle* aVehiclePtr, QTextStream& aOutStream);

      void OutputData(QTextStream&                aOutputStream,
                      double                      aData,
                      int                         aWidth,
                      int                         aPrecision,
                      QTextStream::FieldAlignment aAlignment);

      void OutputData(QTextStream&                aOutputStream,
                      const QString&              aString,
                      int                         aWidth,
                      QTextStream::FieldAlignment aAlignment);

      PerformanceTestType GetPerformanceTestType() { return mPerformanceTestType; }

      QDir                         mVehicleDir;
      QDir                         mTestDir;
      QDir                         mOutputDir;
      QString                      mVehicleName = "";
      QString                      mVehicleType = "";
      QString                      mPlatformName = "";
      QString                      mPlatformType = "";

      std::vector<AeroEngine*>     mEngineList;
      Vehicle*                     mVehiclePtr = nullptr;

   private:
      PerformanceTestType          mPerformanceTestType;

   };
}

#endif //SCRIPT_GENERATOR_HPP
