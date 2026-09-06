// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKF_BM_PLATFORM_STATUS_MODEL_HPP
#define WKF_BM_PLATFORM_STATUS_MODEL_HPP

#include "wkf_common_export.h"

#include <QAbstractTableModel>

#include "WkfBM_ModelBase.hpp"
#include "WkfBM_RuleSet.hpp"

namespace wkf
{
class Platform;

namespace bm
{
class PlatformPlotDatabase;

//! Extends ModelBase to specialize behavior for platform management.
//! This class is distinct from ModelBase so that common functionality
//!    can be reused for a future Base Status display.
class WKF_COMMON_EXPORT PlatformStatusModel final : public ModelBase
{
   Q_OBJECT

public:
   explicit PlatformStatusModel(const PlatformPlotDatabase* aDatabase);

   //! Adds a platform to the given side and type.
   //! If the side or type does not exist yet, creates them.
   //! Returns the internal index of the platform.
   //! This index should be used when calling @see SetDetail().
   LeafProxy Platform(const QString& aSide, const QString& aType, const QString& aName);

   //! Wrapper for the previous overload.
   LeafProxy Platform(const wkf::Platform* aPlatform);

   //! If a platform is dead, colors its name red.
   //! If a platform has not been added yet (e.g. in rv), colors its name blue.
   void UpdatePlatformColors(double aSimTime);

protected:
   void InitializeNode(NodeProxy aNode) override;

   void InitializeLeaf(LeafProxy aLeaf) override{};

private:
   void                        UpdateTeamVisibility();
   const PlatformPlotDatabase& mDatabase;
};
} // namespace bm
} // namespace wkf

#endif
