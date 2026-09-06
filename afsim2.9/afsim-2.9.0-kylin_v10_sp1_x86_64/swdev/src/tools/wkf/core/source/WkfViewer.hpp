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

#ifndef WKFVIEWER_HPP
#define WKFVIEWER_HPP

#include "wkf_export.h"

#include <memory>

#include <QWidget>

#include "VaLocationReference.hpp"
#include "VaViewer.hpp"

namespace wkf
{
class AttachmentRubberband;
class PointOfInterest;

class WKF_EXPORT Viewer : public vespa::VaViewer
{
public:
   Viewer(QWidget* aParent, int aViewerType = HighlightViewer, int aViewerOptions = UseMaps);
   ~Viewer() override = default;

   AttachmentRubberband* BeginRubberband(const vespa::VaEntity&                                  aReference,
                                         const std::function<void(AttachmentRubberband&)>&       aUpdateFunc,
                                         const std::function<void(const AttachmentRubberband&)>& aFinishFunc,
                                         const QString&                                          aStatusMessage);
   void                  ClearRubberband();

   wkf::PointOfInterest* AddPointOfInterest(const vespa::VaLocationReference& aReference);

   QString GetViewLink() const;

protected:
   Viewer(const Viewer& aFrom) = delete;
   int Btn1Click(int aMouseX, int aMouseY, int aState) override;

private:
   class EventFilter : public QObject
   {
   public:
      EventFilter(Viewer* aViewerPtr)
         : QObject(nullptr)
         , mViewerPtr(aViewerPtr)
      {
      }
      ~EventFilter() override = default;

      bool eventFilter(QObject* obj, QEvent* aEvent) override;

   private:
      Viewer* mViewerPtr;
   };
   QWidget*                     mParentPtr;
   vespa::VaEntity*             mActiveRubberbandEntityPtr{nullptr};
   AttachmentRubberband*        mActiveRubberbandPtr{nullptr};
   std::unique_ptr<EventFilter> mEventFilterPtr;
};
} // namespace wkf

#endif
