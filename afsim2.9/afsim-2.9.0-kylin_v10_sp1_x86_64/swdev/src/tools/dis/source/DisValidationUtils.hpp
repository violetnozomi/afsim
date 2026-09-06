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


#ifndef DISVALIDATIONUTILS_HPP
#define DISVALIDATIONUTILS_HPP

#include <cmath>
#include <vector>

#define ValidateScalarTuple(x) ValidateScalarArray(x, 3)

template<typename T>
inline bool ValidateScalar(const T& value)
{
   return std::isfinite(value);
}

template<typename T>
inline bool ValidateScalarArray(const T& value, unsigned int size)
{
   bool isValid = true;

   for (unsigned int inx = 0; inx < size; ++inx)
   {
      if (ValidateScalar(value[inx]) == false)
      {
         isValid = false;
         break;
      }
   }
   return isValid;
}

template<typename T>
inline bool ValidateVector(const std::vector<T>& refVec)
{
   bool isValid = true;

   typename std::vector<T>::const_iterator iter;
   typename std::vector<T>::const_iterator endIter = refVec.end();

   for (iter = refVec.begin(); iter != endIter; ++iter)
   {
      isValid = (*iter).IsValid();
      if (isValid == false)
         break;
   }

   return isValid;
}

#endif /* DISVALIDATIONUTILS_HPP */
