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

#ifndef WkfPointOfInterest_HPP
#define WkfPointOfInterest_HPP

#include "wkf_export.h"

#include <set>

#include "WkfEntity.hpp"

namespace wkf
{
class WKF_EXPORT PointOfInterest : public Entity
{
public:
   PointOfInterest();
   ~PointOfInterest() override = default;

   bool GetExportable() const { return mExportable; }
   void SetExportable(bool aExportable) { mExportable = aExportable; }

   void SetManagingAttachment(vespa::VaAttachment* aAttachmentPtr) { mManagingAttachment = aAttachmentPtr; }
   vespa::VaAttachment* GetManagingAttachment() const { return mManagingAttachment; }

private:
   bool                 mExportable{true};
   vespa::VaAttachment* mManagingAttachment;
};

using PointOfInterestList = std::set<wkf::PointOfInterest*>;
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::PointOfInterest)
#endif
