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

#ifndef WKFTRACKDISPLAYINTERFACE_HPP
#define WKFTRACKDISPLAYINTERFACE_HPP

#include "VaCallbackHolder.hpp"
#include "WkfTrack.hpp"
#include "visibility/WkfTrackVisibilityPrefObject.hpp"

namespace vespa
{
class VaAttachmentModel;
class VaEntity;
class VaPosition;
} // namespace vespa

namespace wkf
{
class Scenario;

class WKF_COMMON_EXPORT TrackDisplayInterface : public QObject
{
   Q_OBJECT
public:
   TrackDisplayInterface(QObject* aParent);
   ~TrackDisplayInterface() override = default;

private:
   vespa::VaAttachmentModel* GetModel(vespa::VaEntity* aEntityPtr) const;
   void                      TrackAddedHandler(Track* aTrackPtr);
   //! Sets the color used for track labels.
   void UpdateTrackLabelColor(wkf::Track* aTrack) const;
   //! Sets all custom icon image models.
   void TrackImageChanged();
   //! Sets the color used for track entities.
   void TrackColorChanged() const;
   //! Sets the color used for a single track entity.
   void UpdateTrackColor(Track& aTrack) const;
   //! Setup for mil-std custom models.
   void UpdateMilStdModels();
   //! Sets a single custom icon image model.
   //! \param aDomain The name of the domain to use. Empty string = no domain.
   //! \param aImagePath The path to the image to use.
   void UpdateTrackImage(const QString& aDomain, const QString& aImagePath);

   //! Sets the icon used for all track entities.
   void TrackIconChanged();
   //! Sets the icon used for a single track entity.
   void UpdateTrackIcon(Track& aTrack);

   void SetWedgeIcon(vespa::VaAttachmentModel& aModel);
   void SetCustomIcon(vespa::VaAttachmentModel& aModel);
   void SetDomainIcon(vespa::VaAttachmentModel& aModel, Track& aTrack);
   void SetPlatformIcon(vespa::VaAttachmentModel& aModel, Track& aTrack);
   void SetMilStdIcon(vespa::VaAttachmentModel& aModel, Track& aTrack);

   vespa::VaCallbackHolder mCallbacks;
};
} // namespace wkf

#endif
