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

#ifndef WSFOBSERVERSUBJECTLIST_HPP
#define WSFOBSERVERSUBJECTLIST_HPP
#include <algorithm>
#include <cassert>
#include <vector>

//! Assists in managing an observer's attachments to subjects
//! When this is deleted, the observer is detached from subjects
template<typename OBSERVER, typename SUBJECT>
class WsfObserverSubjectList
{
public:
   typedef std::vector<SUBJECT*> SubjectList;

   WsfObserverSubjectList()
      : mObserverPtr(0)
   {
   }

   ~WsfObserverSubjectList() { RemoveAll(); }

   void Attach(OBSERVER* aObserver, SUBJECT* aSubject)
   {
      // only supports one observer per subject list
      assert(mObserverPtr == 0 || mObserverPtr == aObserver);
      mObserverPtr = aObserver;

      aSubject->AttachObserver(aObserver);

      typename SubjectList::iterator i = std::find(mObservedSubjects.begin(), mObservedSubjects.end(), aSubject);
      if (i == mObservedSubjects.end())
      {
         mObservedSubjects.push_back(aSubject);
      }
   }

   bool IsAttached(SUBJECT* aSubject) const
   {
      return std::find(mObservedSubjects.begin(), mObservedSubjects.end(), aSubject) != mObservedSubjects.end();
   }

   void Detatch(SUBJECT* aSubjectPtr)
   {
      // Expect call to Attach first
      assert(mObserverPtr != 0);
      if (Remove(aSubjectPtr))
      {
         aSubjectPtr->DetatchObserver(mObserverPtr);
      }
   }

   // Remove a subject without detaching.  To be used when the subject is being or has been deleted.
   bool Remove(SUBJECT* aSubjectPtr)
   {
      typename SubjectList::iterator i = std::find(mObservedSubjects.begin(), mObservedSubjects.end(), aSubjectPtr);
      if (i != mObservedSubjects.end())
      {
         mObservedSubjects.erase(i);
         return true;
      }
      return false;
   }

   void RemoveAll()
   {
      for (size_t i = 0; i < mObservedSubjects.size(); ++i)
      {
         mObservedSubjects[i]->DetachObserver(mObserverPtr);
      }
      mObservedSubjects.clear();
   }

private:
   OBSERVER* mObserverPtr;

   SubjectList mObservedSubjects;
};

#endif
