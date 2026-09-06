// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef ANNOTATIONPLUGIN_HPP
#define ANNOTATIONPLUGIN_HPP

#include <memory>
#include <utility>

class QMenu;
#include <QSettings>
#include <QString>

#include "VaViewer.hpp"
#include "WkfAnnotationDisplayInterface.hpp"
#include "WkfObserver.hpp"
#include "WkfPOI_VisibilityBrowser.hpp"
#include "WkfPlugin.hpp"
#include "WkfPointOfInterestPropertiesWidget.hpp"
#include "WkfRingPropertiesWidget.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"

namespace wkf
{
class Action;
class Bullseye;
class Environment;
class PointOfInterest;
} // namespace wkf

namespace Annotation
{
class BullseyeWidget;

class WKF_COMMON_EXPORT Plugin : public wkf::Plugin
{
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId, const QFlags<Roles>& aRoles = eNO_ROLE);
   ~Plugin() override = default;

   QList<wkf::PrefWidget*>     GetPreferencesWidgets() const override;
   virtual QList<wkf::Action*> GetActions() const override;

   void BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr) override;

   virtual void BuildViewerContextMenu(QMenu* aMenu, vespa::VaViewer* aViewerPtr) override;

   void           ResetOptionStates() override;
   void           SetPlatformOptionState(int aOptionId, bool aState, wkf::Platform* aPlatformPtr) override;
   Qt::CheckState GetPlatformOptionState(int aOptionId, const wkf::Platform* aPlatformPtr) const override;

   void DeleteCurrentlySelectedAnnotations();

protected:
   virtual void                DecoratePreset(const QString& aName, const QString& aPath);
   virtual bool                DecorateText(const QString& aName);
   virtual bool                DecorateImage(const QString& aImage);
   virtual void                ClearDecoration(const QString& aName);
   virtual void                ClearRangeRing(wkf::Entity* aEntityPtr, unsigned int aUniqueId);
   AnnotationDisplayInterface* mDisplayInterface;
   bool                        mEnableFeatures{true};

private:
   int mRangeRingOption{0};
   int mDecorationOption;
   int mBodyFrameOption;
   int mVelocityOption;
   int mEarthOption;
   int mSunOption;
   int mMoonOption;

   void                    AttachmentAddedHandler(vespa::VaAttachment* aAttachment);
   vespa::VaCallbackHolder mCallbacks;

   QPointer<POI_VisibilityWidget>         mPOI_VisibilityWidget;
   std::unordered_map<unsigned int, bool> mRingVisibility;
   std::unordered_map<unsigned int, bool> mDecorationVisibility;
};
} // namespace Annotation
#endif
