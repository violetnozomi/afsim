// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef GLIMITSETTINGS_HPP
#define GLIMITSETTINGS_HPP

#include <QString>

class QJsonObject;

class G_LimitSettings
{
   public:
      explicit G_LimitSettings(const QString& aName);

      double GetPitchG_LoadMax() const { return mPitchG_LoadMax; }
      double GetPitchG_LoadMin() const { return mPitchG_LoadMin; }
      double GetYawG_LoadMax() const { return mYawG_LoadMax; }
      QString GetName() const { return mName; }

      void SetPitchG_LoadMax(double aPitchG_LoadMax) { mPitchG_LoadMax = aPitchG_LoadMax; }
      void SetPitchG_LoadMin(double aPitchG_LoadMin) { mPitchG_LoadMin = aPitchG_LoadMin; }
      void SetYawG_LoadMax(double aYawG_LoadMax) { mYawG_LoadMax = aYawG_LoadMax; }

      void LoadFromFile(const QJsonObject& aFile);
      void SaveToFile(QJsonObject& aFile) const;

      bool LimitsAreValid() const;

      static QString GroupKey(const QString& aName);
      static QString PilotsGuidanceName();
      static QString HardwareAutopilotName();

      double DefaultMaxPitchG_Load() const;
      double DefaultMinPitchG_Load() const;
      double DefaultMaxYawG_Load() const;

   private:
      double  mPitchG_LoadMin;
      double  mPitchG_LoadMax;
      double  mYawG_LoadMax;
      QString mName;
      bool    mIsAircraft;
};

#endif // GLIMITSETTINGS_HPP
