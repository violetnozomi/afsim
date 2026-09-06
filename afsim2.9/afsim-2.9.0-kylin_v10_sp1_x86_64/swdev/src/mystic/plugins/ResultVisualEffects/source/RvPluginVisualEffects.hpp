// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RVPLUGINVISUALEFFECTS_HPP
#define RVPLUGINVISUALEFFECTS_HPP

#include "DisEntityAppearance.hpp"
#include "RvPlugin.hpp"
#include "visual_effect/WkfVisualEffectsDisplayInterface.hpp"
#include "visual_effect/WkfVisualEffectsPrefObject.hpp"
#include "visual_effect/WkfVisualEffectsPrefWidget.hpp"

namespace RvVisualEffects
{
class Plugin : public rv::Plugin
{
   Q_OBJECT

public:
   struct AppearanceData
   {
      double              mTime;
      DisEntityAppearance mAppearance;
   };

   Plugin(const QString& aName, const size_t aUniqueId);
   ~Plugin() override = default;

protected:
   void                    GuiUpdate() override;
   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;

private:
   void AdvanceTimeRead(const rv::ResultData& aData) override;
   void RegularRead(const rv::ResultData& aData) override;
   void PlatformAddedRead(double aSimTime, rv::ResultDb& aData, const rv::MsgPlatformInfo& aPlatform) override;

   vespa::VaPosition GetPositionAtTime(double aTime, int aPlatformIndex, rv::ResultDb* aDatabase);

   // Adds effects that are present in new data, but absent in old data.
   void AddEffectsBasedOnData(wkf::Platform*        aSourcePlatformPtr,
                              const AppearanceData& aPrevData,
                              const AppearanceData& aNewData);

   // Removes effects that are present in old data, but absent in new data.
   void RemoveEffectsBasedOnData(wkf::Platform*        aSourcePlatformPtr,
                                 const AppearanceData& aPrevData,
                                 const AppearanceData& aNewData,
                                 bool                  aGradual);

   template<class EFFECT_TYPE>
   void CreateTrailingEffectOfType(wkf::Platform* aSourcePlatformPtr, const AppearanceData& aData);

   // managed by Qt
   wkf::VisualEffectsDisplayInterface*           mDisplayInterfacePtr;
   const wkf::VisualEffectsPrefObject*           mPrefObjectPtr;
   PluginUiPointer<wkf::VisualEffectsPrefWidget> mPrefWidgetPtr;

   std::map<unsigned int, std::vector<AppearanceData>> mAppearanceMap;
   double                                              mPrevReadTime;

   bool mVisibilityNeedsUpdate;
};
} // namespace RvVisualEffects

#endif // !RVPLUGINVISUALEFFECTS_HPP
