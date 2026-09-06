// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/* -*-c++-*-
 *  \class  idTable
 *  \file   idTable.cpp
 *  \author Kenneth R. Sewell III \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004 \n
 *
 *  Classification: UNCLASSIFIED
 */

#include <iadsLib/idTable.hpp>
#include <iostream>

namespace il
{

idTable::idTable()
{
}

idTable::~idTable()
{
}

void idTable::associate( const unsigned int &LocalID,
			 const unsigned int &RemoteID )
{
    if( (NO_ID == LocalID) || (NO_ID == RemoteID ) )
    {
	return;
    }

    disassociate( LocalID );
    m_RemoteToLocal[RemoteID] = LocalID;
}

void idTable::disassociate( const unsigned int &LocalID )
{
    // Remove any previous mappings to local id.
    std::map<unsigned int, unsigned int>::iterator i = m_RemoteToLocal.begin();
    while( i != m_RemoteToLocal.end() )
    {
	if( i->second == LocalID )
	{
	    m_RemoteToLocal.erase( i++ );
	}
	else
	{
	    ++i;
	}
    }
}

unsigned int idTable::getLocalID( const unsigned int &RemoteID ) const
{
    std::map<unsigned int, unsigned int>::const_iterator
	i = m_RemoteToLocal.find( RemoteID );

    if( i == m_RemoteToLocal.end() )
    {
	return NO_ID;
    }
    else
    {
	return i->second;
    }
}

void idTable::log(std::ostream &os) const
{
    os << "Remote to local mappings:" << std::endl;
    for( std::map<unsigned int, unsigned int>::const_iterator i = m_RemoteToLocal.begin(); i != m_RemoteToLocal.end(); ++i )
	   os << i->first << "..." << i->second << std::endl;
}

} // namespace il
