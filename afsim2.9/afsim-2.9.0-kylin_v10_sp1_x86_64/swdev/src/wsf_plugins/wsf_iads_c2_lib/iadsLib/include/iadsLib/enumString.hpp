// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: enumString.hpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#ifndef IADSLIB_ENUM_STRING_HPP
#define IADSLIB_ENUM_STRING_HPP

#include <string>

namespace il
{
   template <typename tEnum>
   class EnumString
   {
   public:
      EnumString (const tEnum enumrep, const std::string &stringrep)
         : m_enumrep (enumrep)
         , m_stringrep (stringrep) { }
      ~EnumString() = default;
      inline operator tEnum() const { return m_enumrep; }
      inline operator std::string() const { return m_stringrep; }
      inline tEnum AsEnum() const { return m_enumrep; }
      inline std::string AsString() const { return m_stringrep; }

   protected:
      tEnum       m_enumrep;
      std::string m_stringrep;
   };
}


#endif

