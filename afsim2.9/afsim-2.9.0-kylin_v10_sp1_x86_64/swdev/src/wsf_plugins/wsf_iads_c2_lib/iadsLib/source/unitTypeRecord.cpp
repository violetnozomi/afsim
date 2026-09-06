// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  \file   unitTypeRecord.hpp
 *  \author Kenneth R. Sewell III \n
 *          Kenneth.Sewell@wpafb.af.mil \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 */

#include <iadsLib/unitTypeRecord.hpp>

namespace il
{

unitTypeRecord::unitTypeRecord ()
   : m_Type (INVALID_UNIT_TYPE)
   , m_SubType (INVALID_UNIT_SUBTYPE)
   , m_TypeStr ("**type string not set **")
   , m_SubTypeStr ("**subtype string not set **")
{

}

unitTypeRecord::unitTypeRecord (const int &Type, const std::string &type_str, const int &SubType, const std::string &subtype_str)
  : m_Type (Type)
  , m_SubType (SubType)
  , m_TypeStr (type_str)
  , m_SubTypeStr (subtype_str)
{

}


bool unitTypeRecord::operator==( const unitTypeRecord &rhs ) const
{
  return (m_Type == rhs.m_Type && m_SubType == rhs.m_SubType);
}

bool unitTypeRecord::operator<( const unitTypeRecord &rhs ) const
{
  if(m_Type == rhs.m_Type)
    {
      return(m_SubType < rhs.m_SubType);
    }
  else
    {
      return(m_Type < rhs.m_Type);
    }
}

bool unitTypeRecord::isValid() const
{
  return (INVALID_UNIT_TYPE != m_Type) && (INVALID_UNIT_SUBTYPE != m_SubType);
}


void unitTypeRecord::set (const int &Type, const std::string &type_str, const int &SubType, const std::string &subtype_str)
{
   m_Type    = Type;
   m_SubType = SubType;
   m_TypeStr = type_str;
   m_SubTypeStr = subtype_str;
}

int unitTypeRecord::getType() const
{
  return m_Type;
}

int unitTypeRecord::getSubType() const
{
  return m_SubType;
}


const std::string &unitTypeRecord::getTypeStr () const
{
   return m_TypeStr;
}

const std::string &unitTypeRecord::getSubTypeStr () const
{
   return m_SubTypeStr;
}

} // namespace il
