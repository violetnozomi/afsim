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
#ifndef WKFINTERACTIONPREFOBJECT_HPP
#define WKFINTERACTIONPREFOBJECT_HPP

#include <QColor>
#include <QMap>
class QSettings;
#include "wkf_common_export.h"

#include "WkfPrefObject.hpp"

namespace wkf
{
struct InteractionPrefData
{
   enum TooltipMode
   {
      cTT_OFF,
      cTT_SIMPLE,
      cTT_DETAILED
   };

   static constexpr const char* cDETECT     = "Detect";
   static constexpr const char* cTRACK      = "Track";
   static constexpr const char* cLOCALTRACK = "LocalTrack";
   static constexpr const char* cFIRE       = "Fire";
   static constexpr const char* cKILL       = "Kill";
   static constexpr const char* cJAM        = "Jam";
   static constexpr const char* cMESSAGE    = "Message";
   static constexpr const char* cTASK       = "Task";

   QMap<QString, QColor> mColors{{cDETECT, {225, 225, 225}},
                                 {cTRACK, {255, 255, 64}},
                                 {cLOCALTRACK, {196, 160, 64}},
                                 {cFIRE, {255, 64, 64}},
                                 {cKILL, {32, 32, 32}},
                                 {cJAM, {255, 128, 64}},
                                 {cMESSAGE, {64, 64, 255}},
                                 {cTASK, {170, 64, 255}}};
   double                mTimeout{30};
   bool                  mStackingAllowed{false};
   int                   mTooltipMode{cTT_DETAILED};
   unsigned int          mLineWidth{3};
   bool                  mArchSegments{true};
};


class WKF_COMMON_EXPORT InteractionPrefObject : public PrefObjectT<InteractionPrefData>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "Interactions";
   InteractionPrefObject(QObject* parent = nullptr);
   ~InteractionPrefObject() override = default;

   QColor GetColor(const QString& aType) const;
   void   SetColor(const QString& aType, const QColor& aColor);

   const double GetTimeout() const { return mCurrentPrefs.mTimeout; }
   void         SetTimeout(const double& aTimeout);

   const bool GetStackingAllowed() const { return mCurrentPrefs.mStackingAllowed; }
   void       SetStackingAllowed(const bool aAllowed);

   void                      SetTooltipMode(PrefDataType::TooltipMode aMode);
   PrefDataType::TooltipMode GetTooltipMode() const { return (PrefDataType::TooltipMode)mCurrentPrefs.mTooltipMode; }

   void         SetLineWidth(unsigned int aWidth);
   unsigned int GetLineWidth() const { return mCurrentPrefs.mLineWidth; }

   const bool GetArchSegments() const { return mCurrentPrefs.mArchSegments; }
   void       SetArchSegments(const bool aState);

   void RegisterInteraction(const QString& aInteraction, const QString& aGroup, const QColor& aColor);

signals:
   void ColorChanged(const QString&, const QColor&);
   void TimeoutChanged(const double&);
   void TooltipModeChanged(int);
   void LineWidthChanged(unsigned int);
   void StackingAllowedChanged(bool);
   void ArchSegmentsChanged(bool);
   void NewInteractionTypeRegistered(const QString&, const QString&, const QColor&);

private:
   void EmitAllSignals();

   void Apply() override;

   InteractionPrefData ReadSettings(QSettings& aSettings) const override;
   void                SaveSettingsP(QSettings& aSettings) const override;

   std::map<QString, QColor> mRegisteredPrefs;
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::InteractionPrefData)
#endif // WKFINTERACTIONPREFOBJECT_HPP
