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

#ifndef VAOVERLAYQT_HPP
#define VAOVERLAYQT_HPP

#include "vespatk_qt_export.h"

#include <QMenu>

#include "VaOverlay.hpp"

class QDropEvent;

namespace vespa
{
class VESPATK_QT_EXPORT VaOverlayQt : public VaOverlay
{
public:
   VaOverlayQt(const std::string& aOverlayName, OverlayType aOverlayType = cOVERLAY_WINDOW, bool aCreateUniqueName = false);
   ~VaOverlayQt() override = default;
   virtual bool BuildContextMenu(QMenu& aMenu, const QPoint& aPoint) const { return false; }
   virtual bool SupportsDropEvent() const { return false; }
   virtual bool DropEvent(QDropEvent* aEventPtr) { return false; }
};
} // namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaOverlayQt)
#endif
