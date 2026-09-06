// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/* -*-c++-*-
 *  \class  trackID
 *  \file   trackID.cpp
 *  \author Kenneth R. Sewell III \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004 \n
 *
 *  Classification: UNCLASSIFIED
 */

#include <iadsLib/trackID.hpp>
#include <iadsLib/buffer.hpp>

namespace il
{

trackID::trackID( const unsigned int &SystemUnitID,
		  const unsigned int &SystemTrackNumber ):
    m_SystemUnitID( SystemUnitID ),
    m_SystemTrackNumber( SystemTrackNumber )
{
}

trackID::~trackID()
{
}

bool trackID::operator==( const trackID &id ) const
{
    return( (m_SystemUnitID == id.m_SystemUnitID)
	    && (m_SystemTrackNumber == id.m_SystemTrackNumber) );
}

bool trackID::operator<( const trackID &id ) const
{
    // Evaluate using System Unit ID first...
    if( m_SystemUnitID < id.m_SystemUnitID )
    {
	return true;
    }
    else if( m_SystemUnitID > id.m_SystemUnitID )
    {
	return false;
    }
    // If System Unit IDs are the same, evaluate using track number
    else
    {
	return( m_SystemTrackNumber < id.m_SystemTrackNumber );
    }
}

std::ostream &operator<<( std::ostream &os, const il::trackID &TrackID )
{
    os << TrackID.getSystemUnitID() << ":" << TrackID.getSystemTrackNumber();
    return os;
}

} // namespace il
