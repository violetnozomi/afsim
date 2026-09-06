// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKFPLATFORMHISTORYPREFOBJECT_HPP
#define WKFPLATFORMHISTORYPREFOBJECT_HPP

#include <QColor>
class QSettings;

#include "wkf_common_export.h"

#include "WkfPrefObject.hpp"

namespace wkf
{
enum TraceStateValue : int
{
   eTRACE_STATE      = 0,
   eTRACE_TEAM_COLOR = 1,
   eTRACE_NAME_HASH  = 2,
   eTRACE_BEHAVIOR   = 3
};

enum TrackableStateValue : int
{
   eHEADING      = 0,
   ePITCH        = 1,
   eROLL         = 2,
   eSPEED        = 3,
   eALTITUDE     = 4,
   eTEAM_COLOR   = 5,
   eACCELERATION = 6,
   eTRACK_NONE   = 7
};

enum InterpolationLimitsMethod : int
{
   eHARD_LIMITS = 0, // the colors are determined based on clamping state values inside a user defined range
   eCURRENT_INDIVIDUAL_LIMITS =
      1, // the colors are determined based on the current min/max values in the state buffer of the wingribbon
   eCURRENT_SCENARIO_LIMITS = 2, // the colors are determined based on min/max value in state buffers across all wingribbons
   eINDEFINITE_INDIVIDUAL_LIMITS = 3, // colors determined on min/max value of all states ever added to the wingribbon
                                      // (in the time that this LimitsMethod was set)
   eINDEFINITE_SCENARIO_LIMITS = 4,   // colors determined on min/max value of all states ever added across all
                                      // wingribbons (in the time that this LimitsMethod was set)
};

enum WingRibbonStyle : int
{
   eTEAM,
   eGREEN,
   eGRAY
};

struct TraceLinesPrefData
{
   TraceStateValue     mTraceLineColor{eTRACE_TEAM_COLOR};
   std::vector<QColor> mTraceLineColorationList{Qt::magenta, Qt::white, Qt::yellow, Qt::red, Qt::black};
   double              mTraceLineLength{30.0};
   unsigned int        mTraceLineWidth{1};
   double              mWingRibbonsLength{30.0};
   double              mWingRibbonWidthScale{1.0};
   int                 mWingRibbonDeathTransparencyPercent{50}; // 0 to 100
   WingRibbonStyle     mColorScheme{eTEAM};
   // behavior name and color
   std::map<std::string, QColor> mBehaviorTraceLineColor{};
   QColor                        mBehaviorDefaultColor{Qt::magenta};
};

class WKF_COMMON_EXPORT PlatformHistoryPrefObject : public PrefObjectT<TraceLinesPrefData>
{
   Q_OBJECT

public:
   PlatformHistoryPrefObject(QObject* parent = nullptr);

   const TraceStateValue      GetTraceLineColoration() const { return mCurrentPrefs.mTraceLineColor; }
   const std::vector<QColor>& GetTraceLineColorationList() const { return mCurrentPrefs.mTraceLineColorationList; }
   double                     GetTraceLineLength() const { return mCurrentPrefs.mTraceLineLength; }
   unsigned int               GetTraceLineWidth() const { return mCurrentPrefs.mTraceLineWidth; }
   double                     GetWingRibbonsLength() const { return mCurrentPrefs.mWingRibbonsLength; }
   const double               GetWingRibbonWidthScale() const;
   const int                  GetWingRibbonDeathTransparency() const;

   const QColor& GetWingRibbonMainMaxColor() const;
   const QColor& GetWingRibbonMainMinColor() const;
   const QColor& GetWingRibbonEdgeMaxColor() const;
   const QColor& GetWingRibbonEdgeMinColor() const;
   const QColor& GetWingRibbonMainMaxFlippedColor() const;
   const QColor& GetWingRibbonMainMinFlippedColor() const;
   const QColor& GetWingRibbonEdgeMaxFlippedColor() const;
   const QColor& GetWingRibbonEdgeMinFlippedColor() const;

   const double GetWingRibbonHardMaxEdgeLimit() const;
   const double GetWingRibbonHardMinEdgeLimit() const;
   const double GetWingRibbonHardMaxMainLimit() const;
   const double GetWingRibbonHardMinMainLimit() const;
   const double GetWingRibbonHardEdgeWidth() const;
   const double GetWingRibbonSoftEdgeWidth() const;

   const InterpolationLimitsMethod GetWingRibbonEdgeLimitType() const;
   const InterpolationLimitsMethod GetWingRibbonMainLimitType() const;
   const TrackableStateValue       GetWingRibbonEdgeStateBeingTracked() const;
   const TrackableStateValue       GetWingRibbonMainStateBeingTracked() const;

   const std::map<std::string, QColor>& GetBehaviorColorMap() const;
   const QColor&                        GetBehaviorDefaultColor() const;

signals:
   void TraceLineColorationChanged(TraceStateValue aState, const std::vector<QColor>& aTraceLineColorationList);
   void TraceLineBehaviorColorChanged(TraceStateValue aState, const std::map<std::string, QColor> aBehaviorTraceLineMap);
   void TraceLineLengthChanged(double aLength);
   void TraceLineWidthChanged(unsigned int aWidth);
   void WingRibbonsLengthChanged(double aLength);
   void WingRibbonWidthScaleFieldChanged(double aWidthScale);
   void DeathTransparencySliderChanged(int aDeathTransparency);
   void ColorSchemeChanged();

private:
   struct RibbonStyle
   {
      QColor mWingRibbonMainMaxColor;
      QColor mWingRibbonMainMinColor;
      QColor mWingRibbonEdgeMaxColor;
      QColor mWingRibbonEdgeMinColor;
      QColor mWingRibbonMainMaxFlippedColor;
      QColor mWingRibbonMainMinFlippedColor;
      QColor mWingRibbonEdgeMaxFlippedColor;
      QColor mWingRibbonEdgeMinFlippedColor;

      double mWingRibbonHardMaxEdgeLimit;
      double mWingRibbonHardMinEdgeLimit;
      double mWingRibbonHardMaxMainLimit;
      double mWingRibbonHardMinMainLimit;
      double mWingRibbonHardEdgeWidth;
      double mWingRibbonSoftEdgeWidth;

      InterpolationLimitsMethod mWingRibbonEdgeLimitType;
      InterpolationLimitsMethod mWingRibbonMainLimitType;
      TrackableStateValue       mWingRibbonEdgeStateBeingTracked;
      TrackableStateValue       mWingRibbonMainStateBeingTracked;
   };

   void Apply() override;
   void SetPreferenceDataP(const TraceLinesPrefData& aPrefData) override;

   TraceLinesPrefData ReadSettings(QSettings& aSettings) const override;
   void               SaveSettingsP(QSettings& aSettings) const override;

   void SetCurrentWingRibbonColorScheme(WingRibbonStyle aWingRibbonStyle);

   RibbonStyle mCurrentRibbonStyle;

   bool mTraceLineColorChanged{false};
   bool mTraceLineLengthChanged{false};
   bool mTraceLineWidthChanged{false};
   bool mWingRibbonsLengthChanged{false};
   bool mWingRibbonWidthScaleChanged{false};
   bool mWingRibbonDeathTransparencyPercentChanged{false};
   bool mColorSchemeChanged{false};
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::TraceLinesPrefData)
#endif
