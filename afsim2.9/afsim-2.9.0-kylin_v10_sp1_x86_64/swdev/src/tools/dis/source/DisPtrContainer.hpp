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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

// This is an internal class that should only be used within this project.
//
// This is a very special implementation of a vector-type class that is
// intended to hold only pointers...

#ifndef DISPTRCONTAINER_HPP
#define DISPTRCONTAINER_HPP

#include <algorithm>
#include <vector>

#include "DisDeleteObject.hpp"
#include "DisTypes.hpp"
#include "DisValidationUtils.hpp"

template<class T>
class DisPtrContainer
{
public:
   DisPtrContainer();
   DisPtrContainer(const DisPtrContainer& aSrc);
   ~DisPtrContainer();

   // Copy constructor and operator=
   DisPtrContainer<T>& operator=(const DisPtrContainer<T>& aRhs);

   const T*                                         operator[](unsigned int aIndex) const;
   typedef typename std::vector<T*>::const_iterator ContainerIter;

   // Get an iterator to the beginning of the vector
   typename std::vector<T*>::iterator       GetBegin();
   typename std::vector<T*>::const_iterator GetBegin() const;

   // Get an iterator to the end of the vector
   typename std::vector<T*>::iterator       GetEnd();
   typename std::vector<T*>::const_iterator GetEnd() const;

   // Add to the end.
   // WARNING: Add takes ownership of the passed object.
   void Add(T* aPtr);

   // Remove a T pointer from the container and delete the T object
   // If successful, return a positive number, if not return a zero.
   // Warning: This function only removes and deletes the first T* in the container.
   unsigned int Remove(T* aPtr);

   unsigned int Remove(unsigned int aIndex);
   // Remove all T pointers from the container and delete all T objects.
   void RemoveAll();

   // Remove T pointer from the container, but DO NOT DELETE the object.
   // If successful, returns true, if not return false.
   // Warning: This function only removes the first "aPtr" in the container.
   bool RemoveWithNoDelete(T* aPtr);

   // Remove all T pointers from the container, but DO NOT DELETE them.
   void RemoveAllWithNoDelete();

   // Get the number of T pointers in the container
   DisUint32 GetSize() const;

   // Set the capacity of the container.  This function is here only for
   // optimization.
   void Reserve(unsigned int aNum);

   // Data Validation
   bool IsValid() const;

private:
   void                     Copy(const DisPtrContainer<T>& aSrc);
   typename std::vector<T*> mList;
};


template<class T>
DisPtrContainer<T>::DisPtrContainer()
{
}

template<class T>
DisPtrContainer<T>::~DisPtrContainer()
{
   // Delete all the pointers
   std::for_each(mList.begin(), mList.end(), DisDeleteObj());

   // The destructor for "mList" will take of itself
}

// Copy Constructor
template<class T>
DisPtrContainer<T>::DisPtrContainer(const DisPtrContainer<T>& aSrc)
{
   Copy(aSrc);
}

// Operator=
template<class T>
DisPtrContainer<T>& DisPtrContainer<T>::operator=(const DisPtrContainer<T>& aRhs)
{
   // Don't let the user copy over existing object
   if (this == &aRhs)
   {
      return *this;
   }

   // Delete all the pointers and clear the vector.
   std::for_each(mList.begin(), mList.end(), DisDeleteObj());
   mList.clear();

   Copy(aRhs);

   return *this;
}
template<class T>
const T* DisPtrContainer<T>::operator[](unsigned int aIndex) const
{
   T*           dataPtr       = 0;
   unsigned int containerSize = GetSize();
   if (aIndex < containerSize)
   {
      dataPtr = mList.at(aIndex);
   }
   return dataPtr;
}

template<class T>
typename std::vector<T*>::iterator DisPtrContainer<T>::GetBegin()
{
   return mList.begin();
}

template<class T>
typename std::vector<T*>::const_iterator DisPtrContainer<T>::GetBegin() const
{
   return mList.begin();
}

template<class T>
typename std::vector<T*>::iterator DisPtrContainer<T>::GetEnd()
{
   return mList.end();
}

template<class T>
typename std::vector<T*>::const_iterator DisPtrContainer<T>::GetEnd() const
{
   return mList.end();
}

template<class T>
void DisPtrContainer<T>::Add(T* aPtr)
{
   mList.push_back(aPtr);
}

template<class T>
unsigned int DisPtrContainer<T>::Remove(T* aPtr)
{
   if (RemoveWithNoDelete(aPtr))
   {
      delete aPtr;
      return 1;
   }
   else
   {
      return 0;
   }
}

template<class T>
unsigned int DisPtrContainer<T>::Remove(unsigned int aIndex)
{
   T* dataPtr = mList[aIndex];
   return Remove(dataPtr);
}

template<class T>
void DisPtrContainer<T>::RemoveAll()
{
   // Delete all the pointers
   std::for_each(mList.begin(), mList.end(), DisDeleteObj());

   // Remove from the list
   RemoveAllWithNoDelete();
}

template<class T>
bool DisPtrContainer<T>::RemoveWithNoDelete(T* aPtr)
{
   typename std::vector<T*>::iterator newEnd(std::remove(mList.begin(), mList.end(), aPtr));
   if (newEnd != mList.end())
   {
      // aPtr was found
      mList.erase(newEnd, mList.end());
      return true;
   }
   else
   {
      // aPtr was not found
      return false;
   }
}

template<class T>
void DisPtrContainer<T>::RemoveAllWithNoDelete()
{
   // Delete all the iterators
   mList.clear();
}

template<class T>
unsigned int DisPtrContainer<T>::GetSize() const
{
   return static_cast<unsigned int>(mList.size());
}


template<class T>
void DisPtrContainer<T>::Reserve(unsigned int aNum)
{
   mList.reserve(static_cast<typename std::vector<T*>::size_type>(aNum));
}

//
// Private methods
//

// Copy the actual data
template<class T>
void DisPtrContainer<T>::Copy(const DisPtrContainer<T>& aSrc)
{
   mList.reserve(aSrc.mList.size());

   typename std::vector<T*>::const_iterator iter(aSrc.mList.begin());
   while (iter != aSrc.mList.end())
   {
      if (*iter)
      {
         T* ptr = new T(**iter);
         mList.push_back(ptr);
      }
      else
      {
         mList.push_back(nullptr);
      }
      ++iter;
   }
}

template<class T>
bool DisPtrContainer<T>::IsValid() const
{
   bool isValid = true;

   ContainerIter iter;
   ContainerIter endIter = GetEnd();
   for (iter = mList.begin(); iter != endIter && isValid; ++iter)
   {
      isValid = (**iter).IsValid();
   }
   return isValid;
}

#endif
