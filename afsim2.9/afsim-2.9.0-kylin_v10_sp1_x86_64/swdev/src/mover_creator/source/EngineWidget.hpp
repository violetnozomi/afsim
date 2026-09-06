// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef ENGINE_WIDGET_HPP
#define ENGINE_WIDGET_HPP

#include <QWidget>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

#include <map>
#include <memory>

#include "AeroEngine.hpp"

class QSettings;

namespace Designer
{
   class EngineWidget : public QWidget
   {
         Q_OBJECT
      public:
         EngineWidget(const QString& aName, size_t aNumBaseSpecs, size_t aNumBaseSpecsOptions);
         ~EngineWidget() {};

         void LengthChanged(double aLength);
         void DiameterChanged(double aDiameter);
         void ThrustOffsetChanged(double aThrustOffset);

         std::vector<double> GetEngineSpecs() const { return mEngineSpecs; }
         std::vector<QString> GetEngineSpecsOptions() const { return mEngineSpecsOptions; }

         void LoadEngine(AeroEngine* aAeroEngine);
         void UpdateEngine(AeroEngine* aAeroEngine);

         // This performs an "audit" to check that the engine is fully
         // defined, identifying any issues that may exist. The function returns a
         // list of QStrings that detail any problems. If no problems are found,
         // the list will be empty.
         virtual QStringList Audit() const;

         virtual void PopulateLineEdits() = 0;

      signals:
         void EngineSpecsChanged();

      protected:
         QString                           mName;
         std::vector<double>               mEngineSpecs;
         std::vector<QString>              mEngineSpecsOptions;
         size_t                            mNumBaseSpecs;
         size_t                            mNumBaseSpecsOptions;

         std::shared_ptr<QDoubleValidator> mPositiveValidator;

      private:
         virtual void HandleEngineSpecEditingFinished() = 0;
   };
}
#endif // !ENGINE_WIDGET_HPP
