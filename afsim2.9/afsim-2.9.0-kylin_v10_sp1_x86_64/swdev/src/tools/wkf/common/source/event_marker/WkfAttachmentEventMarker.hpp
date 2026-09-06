// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFATTACHMENTEVENTMARKER_HPP
#define WKFATTACHMENTEVENTMARKER_HPP

#include "VaAttachmentModel.hpp"

namespace wkf
{
class AttachmentEventMarker : public vespa::VaAttachmentModel
{
public:
   AttachmentEventMarker(vespa::VaEntity& aParent, vespa::VaViewer* aViewer)
      : vespa::VaAttachmentModel(aParent, aViewer, "marker")
   {
      SetType<AttachmentEventMarker>();
   }

   unsigned int GetPickMode() const override { return cPICK_NOTHING; }
   unsigned int GetHoverMode() const override { return cHOVER_ENTITY; }
};
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AttachmentEventMarker)

#endif
