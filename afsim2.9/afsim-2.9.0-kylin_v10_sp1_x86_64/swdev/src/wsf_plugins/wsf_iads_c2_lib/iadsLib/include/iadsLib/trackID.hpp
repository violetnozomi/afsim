// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/* -*-c++-*-
 *  @class  trackID
 *  @file   trackID.hpp
 *  @author Kenneth R. Sewell III
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 */

#include <iadsLib/enum.hpp>
#include <iostream>

#ifndef TRACKID_HPP
#define TRACKID_HPP

namespace il
{
    class trackID
    {
    public:

	trackID( const unsigned int &SystemUnitID = 0,
	    const unsigned int &SystemTrackNumber = 0 );
	~trackID();

	bool operator==( const trackID &id ) const;
	bool operator<( const trackID &id ) const;

	bool isValid() const
	    {
		return(( 0 != m_SystemUnitID ) && ( 0 != m_SystemTrackNumber ));
	    }

	void setSystemUnitID( const unsigned int &SystemUnitID )
	    {
		m_SystemUnitID = SystemUnitID;
	    }

	const unsigned int &getSystemUnitID() const
	    {
		return m_SystemUnitID;
	    }

	void setSystemTrackNumber( const unsigned int &SystemTrackNumber )
	    {
		m_SystemTrackNumber = SystemTrackNumber;
	    }

	const unsigned int &getSystemTrackNumber() const
	    {
		return m_SystemTrackNumber;
	    }

    protected:
	unsigned int m_SystemUnitID;
	unsigned int m_SystemTrackNumber;

    private:

    };
}

std::ostream &operator<<( std::ostream &os, const il::trackID &TrackID );

#endif
