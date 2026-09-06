// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "stringUtils.hpp"

#include <algorithm>

bool il::equalsIgnoreCase(const char *a, const char *b)
{
   const char *pA = a;
   const char *pB = b;
   for (;;)
   {
      char cA = tolower(*pA);
      char cB = tolower(*pB);
      bool atEndA = cA == '\0';
      bool atEndB = cB == '\0';

      // if both strings are at the end, the strings are equal
      if (atEndA && atEndB)
      {
         return true;
      }

      // if just one string is at the end, the strings have
      // different lengths, so they are not equal
      if (atEndA || atEndB)
      {
         return false;
      }

      // if two characters are not equal, the strings are not equal
      if (cA != cB)
      {
         return false;
      }

      ++pA;
      ++pB;
   }
}

bool il::equalsIgnoreCase(const std::string& a, const char *b)
{
   size_t iA = 0;
   const char *pB = b;
   for (;;)
   {
      char cA = tolower(a[iA]);
      char cB = tolower(*pB);
      bool atEndA = iA == a.size();
      bool atEndB = cB == '\0';

      // if both strings are at the end, the strings are equal
      if (atEndA && atEndB)
      {
         return true;
      }

      // if just one string is at the end, the strings have
      // different lengths, so they are not equal
      if (atEndA || atEndB)
      {
         return false;
      }

      // if two characters are not equal, the strings are not equal
      if (cA != cB)
      {
         return false;
      }

      ++iA;
      ++pB;
   }
}

bool il::equalsIgnoreCase(const char *a, const std::string& b)
{
   return il::equalsIgnoreCase(b, a);
}

bool il::equalsIgnoreCase(const std::string& a, const std::string& b)
{
   if (a.size() != b.size())
   {
      return false;
   }

   size_t size = a.size();
   for (size_t i = 0; i < size; ++i)
   {
      char cA = tolower(a[i]);
      char cB = tolower(b[i]);
      if (cA != cB)
      {
         return false;
      }
   }
   return true;
}


