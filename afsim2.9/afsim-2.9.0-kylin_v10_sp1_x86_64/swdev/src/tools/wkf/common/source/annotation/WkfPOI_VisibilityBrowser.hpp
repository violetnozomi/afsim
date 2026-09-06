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

#ifndef POIVISIBILITYBROWSER
#define POIVISIBILITYBROWSER

#include "wkf_common_export.h"

#include <QDockWidget>
#include <QObject>

#include "WkfAction.hpp"
#include "ui_WkfPOI_VisibilityOptionsWidget.h"

namespace vespa
{
class VaEntity;
}

namespace Annotation
{
// This widget provides global options for showing and hiding points of interest.
// Anything attached to the POI will follow the same visibility settings as the POI; no special handling required
class WKF_COMMON_EXPORT POI_VisibilityWidget : public QDockWidget
{
   Q_OBJECT
public:
   explicit POI_VisibilityWidget(QWidget* aParent = nullptr, Qt::WindowFlags aFlags = Qt::WindowFlags());
   ~POI_VisibilityWidget() override = default;

   QList<wkf::Action*> GetActions() const;

private:
   void SetPOI_Visibility(bool aVisible);
   void SetBullseyeVisibility(bool aVisible);
   void UpdateVisibility(vespa::VaEntity* aEnt, bool aVisible);

   Ui::POI_VisibilityWidget mUI;
   wkf::Action*             mTogglePOI_Action;
   wkf::Action*             mToggleBullseyeAction;
   bool                     mPOI_Visible{true};
   bool                     mBullseyeVisible{true};
};
} // namespace Annotation

#endif
