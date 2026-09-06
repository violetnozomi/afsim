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
#ifndef WKFSENSORVOLUMESPREFOBJECT_HPP
#define WKFSENSORVOLUMESPREFOBJECT_HPP

#include <QColor>
class QSettings;

#include "wkf_common_export.h"

#include "WkfPrefObject.hpp"

namespace wkf
{
struct SensorModeOptions
{
   SensorModeOptions(bool          aShowFaces,
                     const QColor& aFaceColor,
                     bool          aShowEdges,
                     const QColor& aEdgeColor,
                     bool          aShowProjection,
                     const QColor& aProjColor)
      : mShowFaces(aShowFaces)
      , mFaceColor(aFaceColor)
      , mShowEdges(aShowEdges)
      , mEdgeColor(aEdgeColor)
      , mShowProjection(aShowProjection)
      , mProjColor(aProjColor)
   {
   }

   SensorModeOptions() = default;

   bool   mShowFaces{true};
   QColor mFaceColor{0, 255, 0, 68};
   bool   mShowEdges{true};
   QColor mEdgeColor{68, 255, 68, 255};
   bool   mShowProjection{true};
   QColor mProjColor{0, 255, 0, 68};
};

struct SensorVolumesPrefData
{
   SensorVolumesPrefData();

   std::map<std::pair<std::string, bool>, SensorModeOptions> mModeMap;

   enum DrawMode
   {
      cSLEW_VOLUME,
      cCUE_VOLUME,
      cSCAN_VOLUME,
      cFIELD_OF_VIEW,
      cBEAM_WIDTH,
      cCALCULATED
   };
   DrawMode mDrawMode{cCALCULATED};
};

class WKF_COMMON_EXPORT SensorVolumesPrefObject : public PrefObjectT<SensorVolumesPrefData>
{
   Q_OBJECT

public:
   SensorVolumesPrefObject(QObject* parent = nullptr);

   void                            GetModeDefinition(const std::string& aName,
                                                     bool               aWeapon,
                                                     bool&              aShowFaces,
                                                     QColor&            aFaceColor,
                                                     bool&              aShowEdges,
                                                     QColor&            aEdgeColor,
                                                     bool&              aShowProjection,
                                                     QColor&            aProjColor) const;
   SensorVolumesPrefData::DrawMode GetDrawMode() const { return mCurrentPrefs.mDrawMode; }

signals:
   void ModesReset();
   void ModeChanged(const std::string& aName,
                    bool               aWeapon,
                    bool               aShowFaces,
                    const QColor&      aFaceColor,
                    bool               aShowEdges,
                    const QColor&      aEdgeColor,
                    bool               aShowProjection,
                    const QColor&      aProjColor);
   void DrawModeChanged(const SensorVolumesPrefData::DrawMode& aMode);

private:
   void EmitAllSignals();

   void Apply() override;

   SensorVolumesPrefData ReadSettings(QSettings& aSettings) const override;
   void                  SaveSettingsP(QSettings& aSettings) const override;
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::SensorVolumesPrefData)
#endif // WKFSENSORVOLUMESPREFOBJECT_HPP
