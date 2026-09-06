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

#ifndef VAATTACHMENTQT_HPP
#define VAATTACHMENTQT_HPP

#include "vespatk_qt_export.h"

#include <QMenu>

#include "VaAttachment.hpp"

namespace vespa
{
class VESPATK_QT_EXPORT VaAttachmentQt : public VaAttachment
{
public:
   VaAttachmentQt(VaEntity& aParent, VaViewer* aViewerPtr, const std::string& aName);
   ~VaAttachmentQt() override = default;
   virtual bool SupportsContextMenu() const { return false; }
   virtual void BuildContextMenu(QMenu& aMenu, unsigned int aSubPartId) {}
};
} // namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaAttachmentQt)
#endif
