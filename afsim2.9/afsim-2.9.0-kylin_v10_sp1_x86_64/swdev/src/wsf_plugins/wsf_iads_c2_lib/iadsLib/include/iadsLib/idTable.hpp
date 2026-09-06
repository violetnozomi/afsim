// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/* -*-c++-*-
 *  @class  idTable
 *  @file   idTable.hpp
 *  @author Kenneth R. Sewell III
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 */

#include <iadsLib/enum.hpp>
#include <map>

#ifndef IDTABLE_HPP
#define IDTABLE_HPP

namespace il
{
    class idTable
    {
    public:
	idTable();
	~idTable();

	void associate( const unsigned int &LocalID,
			const unsigned int &RemoteID );
	void disassociate( const unsigned int &RemoteID );

	unsigned int getLocalID( const unsigned int &RemoteID ) const;

	void log(std::ostream &os) const;

    protected:
	std::map< unsigned int, unsigned int > m_RemoteToLocal;

    private:

    };
}
#endif
