// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  \file   idRecord.hpp
 *  \author Kenneth R. Sewell III \n
 *          Kenneth.Sewell@wpafb.af.mil \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 */

#include <iadsLib/idRecord.hpp>

namespace il
{

idRecord::idRecord (const int32_t &ID, const int32_t &SubID, const std::string &IDString, const std::string &SubIDString)
  : m_ID (ID)
  , m_SubID (SubID)
  , m_IDString (IDString)
  , m_SubIDString (SubIDString)
{}


bool idRecord::operator==( const idRecord &record ) const
{
  return( (m_ID == record.m_ID) && (m_SubID == record.m_SubID) );
}

bool idRecord::operator==( const int32_t &id ) const
{
  return( (id == m_ID) && (0 == m_SubID) );
}

bool idRecord::operator!=( const idRecord &record ) const
{
  return( (m_ID != record.m_ID) || (m_SubID != record.m_SubID) );
}

bool idRecord::operator<( const idRecord &record ) const
{
  if(m_ID == record.m_ID)
    {
      return(m_SubID < record.m_SubID);
    }
  else
    {
      return(m_ID < record.m_ID);
    }
}

bool idRecord::isValid() const
{
  return( (0 != m_ID) );
}

void idRecord::set (const int32_t &ID, const int32_t &SubID, const std::string &IDString, const std::string &SubIDString)
{
  m_ID = ID;
  m_SubID = SubID;
  m_IDString = IDString;
  m_SubIDString = SubIDString;
}

// get/set when used as generic ID.
void idRecord::setID (const int32_t &ID, const std::string &IDString)
{
  m_ID = ID;
  m_IDString = IDString;
}

const int32_t &idRecord::getID() const
{
  return m_ID;
}

std::string idRecord::getIDString () const
{
   return m_IDString;
}

void idRecord::setSubID (const int32_t &SubID, const std::string &SubIDString)
{
  m_SubID = SubID;
  m_SubIDString = SubIDString;
}

const int32_t &idRecord::getSubID() const
{
  return m_SubID;
}

std::string idRecord::getSubIDString () const
{
   return m_SubIDString;
}

// get/set when used as a track ID.
void idRecord::setSystemUnitID (const int32_t &ID, const std::string &IDString)
{
  m_ID = ID;
  m_IDString = IDString;
}

const int32_t &idRecord::getSystemUnitID() const
{
  return m_ID;
}

const std::string &idRecord::getSystemUnitIDString() const
{
   return m_IDString;
}

void idRecord::setSystemTrackID (const int32_t &SubID, const std::string &SubIDString)
{
  m_SubID = SubID;
  m_SubIDString = SubIDString;
}

const int32_t &idRecord::getSystemTrackID() const
{
  return m_SubID;
}

} // namespace il
