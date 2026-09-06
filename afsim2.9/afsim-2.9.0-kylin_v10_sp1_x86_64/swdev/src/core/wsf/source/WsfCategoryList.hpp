// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCATEGORYLIST_HPP
#define WSFCATEGORYLIST_HPP

#include "wsf_export.h"

#include <vector>

class UtInput;
#include "WsfStringId.hpp"

/** Contains the 'categories' to which an object belongs.

  An object (platform, sensor, etc.) may be a member of zero or more
  'categories'.  Categories are generally used to associate groups of objects
  for the purposes of decision making.  For instance, a particular category of
  objects may represent high value targets.

  This class simply provides the mechanism maintain the category list and to
  determine if an object is a member of a particular category.
*/

class WSF_EXPORT WsfCategoryList
{
public:
   typedef std::vector<WsfStringId> CategoryList;

   WsfCategoryList();

   //! Clears the category list.
   void Clear() { mCategories.clear(); }

   void JoinCategory(WsfStringId aCategoryId);

   bool ProcessInput(UtInput& aInput);

   bool IsCategoryMember(WsfStringId aCategoryId) const;

   bool Intersects(const WsfCategoryList& aOtherList) const;

   bool IsASubsetOf(const WsfCategoryList& aOtherList) const;

   //! Return a the actual list.
   const CategoryList& GetCategoryList() const { return mCategories; }

private:
   // Using the default copy constructor, destructor and assignment operator.

   // The set of categories to which the associated object belongs.
   //
   // Logically this should be a 'set', but most objects will only be a member of a few categories.
   // In such a case, searching a 'vector' is faster than searching a set. Furthermore, sometimes
   // it is desirable to know the order in which categories were added. A 'set' wont work there.

   CategoryList mCategories;
};

#endif
