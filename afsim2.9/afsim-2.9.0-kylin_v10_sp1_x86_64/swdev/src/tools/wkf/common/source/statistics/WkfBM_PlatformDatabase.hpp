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
#ifndef WKF_BM_PLATFORM_DATABASE_HPP
#define WKF_BM_PLATFORM_DATABASE_HPP

#include "wkf_common_export.h"

#include <limits>
#include <unordered_map>

#include <QString>
#include <QStringList>

namespace wkf
{
namespace bm
{
struct WKF_COMMON_EXPORT PlatformPlotData
{
   bool AliveAt(double aSimTime) const noexcept;

   double      mStartTime = std::numeric_limits<double>::max();
   double      mEndTime   = std::numeric_limits<double>::max();
   QString     mName;
   QStringList mTypes;
   QStringList mCategories;
   QString     mSide;
   bool        mIsWeapon = false;
};

class WKF_COMMON_EXPORT PlatformPlotDatabase
{
public:
   using Container = std::unordered_map<unsigned, PlatformPlotData>;
   using iterator  = Container::const_iterator;

   //! If aSide is empty, it is not used in the check.
   //! If aType is empty, it is not used in the check.
   //! {
   int TotalCount(double aSimTime, const QString& aSide = {}, const QString& aType = {}) const noexcept;
   int PlatformCount(double aSimTime, const QString& aSide = {}, const QString& aType = {}) const noexcept;
   int WeaponCount(double aSimTime, const QString& aSide = {}, const QString& aType = {}) const noexcept;
   //! }

   //! Returns the number of platforms on aSide with category aCategory.
   //! If aSide is empty, it is not used in the check.
   //! If aType is empty, it is not used in the check.
   //! If aCategory is empty, finds platforms with no category.
   int CategoryCount(double aSimTime, const QString& aSide, const QString& aType, const QString& aCategory) const noexcept;

   //! If platform already exists, returns a reference to its data.
   PlatformPlotData& AddPlatform(unsigned aIndex);
   //! Throws if a platform with aIndex does not exist.
   PlatformPlotData& FindPlatform(unsigned aIndex);
   //! Throws if a platform with aIndex does not exist.
   void RemovePlatform(double aSimTime, unsigned aIndex);

   QStringList GetSides() const noexcept;
   QStringList GetTypes() const noexcept;
   QStringList GetCategories() const noexcept;

   void AddSides(QStringList aSides) noexcept;
   void AddTypes(QStringList aTypes) noexcept;
   void AddCategories(QStringList aCategories) noexcept;

   void Clear() noexcept;
   bool Empty() const noexcept;
   int  EntryCount() const noexcept;

   iterator begin() const noexcept;
   iterator end() const noexcept;

private:
   Container mData;

   mutable QStringList mSides;
   mutable QStringList mTypes;
   mutable QStringList mCategories;

   mutable bool mSidesChanged      = true;
   mutable bool mTypesChanged      = true;
   mutable bool mCategoriesChanged = true;
};
} // namespace bm
} // namespace wkf

#endif
