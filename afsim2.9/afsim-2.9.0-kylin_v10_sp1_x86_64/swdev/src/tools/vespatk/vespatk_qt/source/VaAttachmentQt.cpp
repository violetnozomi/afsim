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

#include "VaAttachmentQt.hpp"

vespa::VaAttachmentQt::VaAttachmentQt(VaEntity& aParent, VaViewer* aViewerPtr, const std::string& aName)
   : VaAttachment(aParent, aViewerPtr, aName)
{
   SetType<VaAttachmentQt>();
}
