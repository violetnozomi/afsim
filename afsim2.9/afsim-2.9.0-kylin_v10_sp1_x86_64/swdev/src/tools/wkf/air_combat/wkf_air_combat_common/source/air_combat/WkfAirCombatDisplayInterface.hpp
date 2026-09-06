// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFAIRCOMBATDISPLAYINTERFACE_HPP
#define WKFAIRCOMBATDISPLAYINTERFACE_HPP

#include "wkf_air_combat_common_export.h"

#include <memory>

#include <QColor>
#include <QTimer>

#include "UtoRawShape.hpp"
#include "VaCallbackHolder.hpp"
#include "VaOverlay.hpp"
#include "VaViewer.hpp"
#include "WkfAirCombatOverlay.hpp"
#include "WkfAirCombatVisualizationPrefObject.hpp"
#include "WkfAttachmentVariableInteraction.hpp"
#include "WkfPlatform.hpp"
#include "air_combat/WkfAirCombatDataContainer.hpp"
#include "annotation/WkfAttachmentDecorator.hpp"

namespace wkf
{
// A class that manages the creation and updating of the different types of attachments
// associated with the Air Combat Visualization Plugin
class WKF_AIR_COMBAT_COMMON_EXPORT AirCombatDisplayInterface : public QObject
{
public:
   enum MeterMeasurement
   {
      cNONE,
      cDEFENSIVENESS,
      cFUEL,
      cGFORCE
   };

   enum AccentType
   {
      cWEAPONS,
      cFUELSTATE,
      cTRANSMISSION,
      cSIGNATURE
   };

   enum InteractionType
   {
      cBASE,
      cWEZ,
      cDETECTION
   };

   AirCombatDisplayInterface(AirCombatVisualizationPrefObject& aPrefObject);

   void Update(wkf::DataContainer& aDataContainer);

   void CreateDecoratorAttachment(wkf::Platform& aPlatform);

   void CreateLineAttachment(wkf::Platform& aPlatform, wkf::Platform& aTarget, InteractionType aType);

   void CreateOverlay(wkf::Platform& aPlatform, vespa::VaViewer& aViewer);

   void ChangeMeterMeasurementAtLevel(int aMeterLevel, MeterMeasurement aMeasurement);

   void ShowStateData(bool aShow);

   void ShowAccents(bool aShow, AccentType aType);

   bool IsPlatformDecorated(wkf::Platform& aPlatform) const;
   void RemoveAttachments(wkf::Platform& aPlatform);

   void RemoveOverlay(wkf::Platform& aSourcePlatform, vespa::VaViewer& aViewer);

   void AddManualTargetToOverlay(wkf::Platform& aSourcePlatform, wkf::Platform& aTargetPlatform);

   void RemoveManualTargetFromAllOverlays(wkf::Platform& aTargetPlatform);

   const bool GetVisibility() { return mShowVisualization; }
   void       SetVisibility(const bool aVisible);

private:
   void ViewerDestroyedCB(vespa::VaViewer* aViewerPtr);

   // A nested class that represents some form of data associated with a platform
   // that is represented by a VaAttachment
   class AttachmentDataObject
   {
   public:
      AttachmentDataObject(wkf::Platform& aPlatform, const std::string& aAttachmentName);

      virtual void Update(const AirCombatDataEngagementSummary& aData) = 0;

      void           RemoveAttachment();
      wkf::Platform* GetPlatform() const { return mPlatformPtr; }

   protected:
      wkf::Platform*                 mPlatformPtr;
      AirCombatDataEngagementSummary mCurrentData;
      unsigned int                   mAttachmentId;
   };

   // Data objects of this type are represented by a single decorator
   // consisting of circular meters and/or data accents.
   class AttachmentDecoratorObject : public AttachmentDataObject
   {
   public:
      AttachmentDecoratorObject(wkf::Platform&                         aPlatform,
                                const std::string&                     aAttachmentName,
                                wkf::AirCombatVisualizationPrefObject& aPrefObject);

      void Update(const AirCombatDataEngagementSummary& aData) override;

      void SetVisibility(const bool aVisible);
      bool IsVisible() const;

      void ChangeMeterMeasurementAtLevel(int aMeterLevel, MeterMeasurement aMeasurement);

      void ShowAccents(bool aShow, AccentType aType);

      void ShowStateData(bool aShow) { mShowStateData = aShow; }

   private:
      // An enumeration that represents the position of a given accent.
      enum AccentPosition
      {
         cTOP_LEFT,
         cTOP_RIGHT,
         cBOTTOM_LEFT,
         cBOTTOM_RIGHT
      };

      // A collection of items that make up a circular meter.
      struct CircularMeter
      {
         MeterMeasurement mType;
         QColor           mCurrentColor;
         double           mCurrentValue;
         double           mTargetValue;
         int              mLevel;
      };

      // A collection of items that make up a data accent.
      struct DataAccent
      {
         AccentPosition mPosition;
         AccentType     mType;
         std::string    mText;
         QColor         mColor;
         bool           mEnabled;
      };

      void UpdateMeter(CircularMeter& aMeter);

      void DrawMeter(QImage& aImage, CircularMeter& aMeter, int aLevel);

      void UpdateAccent(DataAccent& aAccent);

      void DrawAccent(QImage& aImage, DataAccent& aAccent);

      void DrawStateData(QImage& aImage);

      std::map<int, CircularMeter> mMeterMap; // Maps meter levels to meter data
      std::vector<DataAccent>      mAccents;  // List of current data accents

      int  mOutermostMeterLevel;
      int  mMeterDiameter;
      int  mLevelSpacing;
      int  mAccentOffset;
      int  mAccentDiameter;
      bool mShowStateData;
   };

   // Data objects of this type are represented by interaction lines
   class AttachmentInteractionObject : public AttachmentDataObject
   {
   public:
      AttachmentInteractionObject(wkf::Platform&     aPlatform,
                                  wkf::Platform&     aTargetPlatform,
                                  InteractionType    aType,
                                  const std::string& aAttachmentName = "No Data");

      void Update(const AirCombatDataEngagementSummary& aData) override;

      void SetVisibility(const bool aVisible);
      void ToggleVisibility();
      bool IsVisible() const;

      InteractionType GetType() { return mInteractionType; }

      bool IsBlinking() const { return mBlinking; }

      void SetColor(const UtoColor& aColor);

      wkf::Platform* GetTargetPlatform() const { return mTargetPlatformPtr; }

   private:
      bool            mBlinking;
      wkf::Platform*  mTargetPlatformPtr;
      InteractionType mInteractionType;
   };

   wkf::AirCombatVisualizationPrefObject* mPrefObjectPtr;
   vespa::VaCallbackHolder                mCallbacks;
   QTimer*                                mBlinkTimer;

   std::map<Platform*, std::unique_ptr<AttachmentDecoratorObject>>                mDecoratorMap;
   std::map<Platform*, std::vector<std::unique_ptr<AttachmentInteractionObject>>> mInteractionMap;
   std::map<Platform*, AirCombatOverlay*>                                         mOverlayMap;
   bool                                                                           mShowVisualization{false};
};
} // namespace wkf

#endif // !WKFAirCombatDISPLAYINTERFACE_HPP
