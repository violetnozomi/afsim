// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef SCRIPT_GENERATOR_PM6DOF_HPP
#define SCRIPT_GENERATOR_PM6DOF_HPP

#include <memory>
#include <QDir>
#include <QProcess>
#include <QString>
#include <QTextStream>

#include "ScriptGeneratorSixDOF.hpp"
#include "AeroEngine.hpp"
#include "Vehicle.hpp"

namespace Designer
{
   class ScriptGeneratorPointMassSixDOF : public ScriptGeneratorSixDOF
   {
      public:
         ScriptGeneratorPointMassSixDOF();
         ~ScriptGeneratorPointMassSixDOF() = default;

         QString GetFilenameSuffix() override { return "_pm6"; }

      protected:

         AeroEngine::ExportMoverType GetMoverType() override { return AeroEngine::ExportMoverType::cPOINT_MASS; }
         QString GetDefaultIntegratorName() override { return "STANDARD_POINT_MASS_INTEGRATOR"; }
         QString GetScriptMoverType() override { return "WSF_POINT_MASS_SIX_DOF_MOVER"; }

      private:

         void CreateMoverFile() override;
         void CreateControlsInputFile(QString& aControlsDir) override;
         void CreateFlightControlsFile(QString& aControlsDir) override;
         void CreateAeroFile(QString& aAeroDir) override;
         void CreateAeroTables(QString& aAeroDir) override;

         void CreatePerformanceTestFiles(QString& aTestDir) override;

         std::vector<AeroTableBase*> mTableVec;
   };
}

#endif // SCRIPT_GENERATOR_PM6DOF_HPP
