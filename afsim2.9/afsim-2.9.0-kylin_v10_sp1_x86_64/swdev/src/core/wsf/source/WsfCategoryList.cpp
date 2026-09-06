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

#include "WsfCategoryList.hpp"

#include <algorithm>
#include <cassert>
#include <string>

#include "UtInput.hpp"
#include "WsfStringId.hpp"

WsfCategoryList::WsfCategoryList() {}

//! Determine if the associated object is a member of the indicated category.
//!
//!  @param aCategoryId String ID number of the category to be checked.
//!  @return 'true' if the object is a member of the indicated category or 'false' if not.
bool WsfCategoryList::IsCategoryMember(WsfStringId aCategoryId) const
{
   return (find(mCategories.begin(), mCategories.end(), aCategoryId) != mCategories.end());
}

//! Indicate the associated object is a member of the indicated category.
//!
//!  @param aCategoryId String ID number of the category of which the object is a member.
void WsfCategoryList::JoinCategory(WsfStringId aCategoryId)
{
   if (!IsCategoryMember(aCategoryId))
   {
      mCategories.push_back(aCategoryId);
   }
}

//! Process input from a generic source.
//!
//! Examine the current input command.  If it is NOT one of the commands
//! recognized by this class then it simply returns 'false'.  If it is
//! one of the commands recognized by this class then it processes the
//! command and returns 'true'.
//!
//! @param aInput a reference to a UtInput object.
//!
//! @returns 'true' if the command was one recognized by the class or 'false' if not.
//!
//! @throws UtInput::ExceptionBase (or an object derived from it) if the command was recognized
//!            but contains some sort of error.
bool WsfCategoryList::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (aInput.GetCommand() == "category")
   {
      std::string categoryName;
      aInput.ReadValue(categoryName);
      JoinCategory(categoryName);
   }
   else if (command == "clear_categories")
   {
      mCategories.clear();
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

//! Does another category list intersect with this category list?
//!
//!  @param aOtherList The other category list.
//!  @returns 'true' if ANY category of this list matches ANY category in the other list.
//!  @returns 'false' if either list is empty or if or if NO category in this list
//!  matches ANY category in the other list.
bool WsfCategoryList::Intersects(const WsfCategoryList& aOtherList) const
{
   bool                         intersects     = false;
   CategoryList::const_iterator thisIter       = mCategories.begin();
   CategoryList::const_iterator thisEndIter    = mCategories.end();
   CategoryList::const_iterator otherBeginIter = aOtherList.mCategories.begin();
   CategoryList::const_iterator otherEndIter   = aOtherList.mCategories.end();
   while (thisIter != thisEndIter)
   {
      if (find(otherBeginIter, otherEndIter, *thisIter) != otherEndIter)
      {
         intersects = true;
         break;
      }
      ++thisIter;
   }
   return intersects;
}

//! Is this category list completely contained within another category list?
//!
//!  @param aOtherList The other category list.
//!  @returns 'true' if ALL categories of this list are contained in the other list,
//!  or if this list is empty.
//!  @returns 'false' if ANY category of this list is NOT contained in the other list.
bool WsfCategoryList::IsASubsetOf(const WsfCategoryList& aOtherList) const
{
   bool                         isASubset      = true;
   CategoryList::const_iterator thisIter       = mCategories.begin();
   CategoryList::const_iterator thisEndIter    = mCategories.end();
   CategoryList::const_iterator otherBeginIter = aOtherList.mCategories.begin();
   CategoryList::const_iterator otherEndIter   = aOtherList.mCategories.end();
   while (thisIter != thisEndIter)
   {
      if (find(otherBeginIter, otherEndIter, *thisIter) == otherEndIter)
      {
         isASubset = false;
         break;
      }
      ++thisIter;
   }
   return isASubset;
}
