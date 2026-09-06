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

#ifndef WKFORBITINTERFACE_HPP
#define WKFORBITINTERFACE_HPP

#include "wkf_common_export.h"

#include <QObject>

#include "UtCalendar.hpp"
#include "UtColor.hpp"
#include "UtMemory.hpp"
#include "WkfAttachmentOrbit.hpp"
#include "WkfMoonOrbitPrefWidget.hpp"
#include "WkfOrbitPrefWidget.hpp"

namespace wkf
{

class Platform;

class WKF_COMMON_EXPORT OrbitInterface : public QObject
{
   Q_OBJECT

public:
   OrbitInterface(QObject*                  aParentPtr,
                  wkf::OrbitPrefWidget*     aOrbitPrefWidgetPtr,
                  wkf::MoonOrbitPrefWidget* aMoonOrbitPrefWidgetPtr);
   ~OrbitInterface() override = default;

   void FadeoutChanged(int aFadeout);
   void ColorModeChanged(wkf::OrbitPrefData::ColorMode aColorMode);
   void LineWidthChanged(int aLineWidth);
   void PeriodsChanged(unsigned int aPeriods);
   void TeamColorsChanged();

   UtColor               GetTeamColor(size_t aPlatformIndex) const;
   bool                  IsScenarioColorMode() const { return mColorMode == wkf::OrbitPrefData::eSCENARIO; }
   bool                  HasAttachment(unsigned int aPlatformIndex) const;
   void                  ClearAllAttachments();
   wkf::AttachmentOrbit* GetAttachment(unsigned int aPlatformId);
   wkf::AttachmentOrbit* GetOrAddAttachment(unsigned int aPlatformId);
   wkf::AttachmentOrbit* AddAttachment(wkf::Platform* aPlatformPtr);
   void                  RemoveAttachment(unsigned int aPlatformIndex);

   void AddMoonOrbit(double aSimTime = 0.0);
   void UpdateMoonOrbit(double aSimTime);
   void MoonOrbitVisibilityChanged(bool aVisible);
   void MoonOrbitColorChanged(const QColor& aColor);
   void MoonOrbitLineWidthChanged(int aLineWidth);

   void              SetStartDateTime(const UtCalendar& aEpoch) { mStartDateTime = aEpoch; }
   const UtCalendar& GetStartDateTime() const { return mStartDateTime; }

   double GetAngleForTime(double aSimTime);
   void   UpdateOrbitAngles(double aSimTime);

   //! Perform the given @p aOperation on each tracked orbit attachment.
   //!
   //! Invoke the given @p aOperation for each tracked orbit attachment. The
   //! given operation must be any object that can invoked with the following
   //! signature void(unsigned int, wkf::AttachmentOrbit*).
   //!
   //! \param aOperation - The operation invoked for each tracked orbit attachment.
   template<typename Callable>
   void ForEach(Callable aOperation)
   {
      for (auto& it : mOrbitMap)
      {
         aOperation(it.first, it.second);
      }
   }

private:
   void EntityDeletedCB(vespa::VaEntity* aEntityPtr);
   void RemoveMoonOrbit();

   wkf::OrbitPrefWidget*                         mOrbitPrefWidgetPtr;
   std::map<unsigned int, wkf::AttachmentOrbit*> mOrbitMap{};
   wkf::OrbitPrefData::ColorMode                 mColorMode{};

   wkf::MoonOrbitPrefWidget* mMoonOrbitPrefWidgetPtr;
   // the entity is an "anchor" that holds the orbit
   std::pair<std::unique_ptr<vespa::VaEntity>, wkf::AttachmentOrbit*> mMoonOrbit;

   UtCalendar              mStartDateTime{};
   vespa::VaCallbackHolder mCallbacks;
};

} // namespace wkf

#endif // WKFORBITINTERFACE_HPP
