// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file RangeBin.hpp
 *  @author Ken Sewell AFRL/RYZW \n
 *          Kenneth.Sewell@wpafb.af.mil \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004 (Sewell)
 *  @brief Template Class to support ranged bins.\n
 *
 *  Classification: UNCLASSIFIED
 */

#include <ostream>
#include <cmath>
#include <list>
#include <iterator>
#include <stdexcept>

#ifndef VCLMATH_RANGEBIN_HPP
#define VCLMATH_RANGEBIN_HPP 1

namespace vm
{
    /**
     *  \brief Template Class to support ranged bins.
     */
    template<typename B, typename V>
    class RangeBin
    {
    public:
	/*!
          \brief RangeBin constructor
        */
        RangeBin()
	    {
	    }

	/*!
	  @brief Copy constructor
	  @param rb RangeBin to copy into new instance.
        */
        RangeBin( const RangeBin &rb ):
	    m_Range( rb.m_Range ),
	    m_Value( rb.m_Value )
	    {

	    }

	virtual ~RangeBin(){}

	/**
	   @brief Returns whether range bin is empty or not.
	   @returns True if range bin is empty, false othewise.
	 */
	bool empty() const
	    {
		return m_Range.empty();
	    }

	/**
	   @brief Set the bin ranges and values.
	   @param ranges Sorted list of bin ranges (ascending).
	   @param values List of bin values.
	 */
	void set( const std::list< B > &ranges,
		  const std::list< V > &values )
	    {
		// Number of value bins is 1 greater than the number of ranges.
		if( ( ranges.size() + 1 ) == values.size() )
		{
		    m_Range = ranges;
		    m_Value = values;
		    // Ensure ranges are in ascending order.
		    m_Range.sort();
		}
		else
		{
		    throw std::range_error("Number of values must be"
					   " equal to number of ranges + 1");
		}
	    }

	/**
	   @brief Get the bin ranges and values.
	   @param ranges Copy of the sorted list of bin ranges.
	   @param values Copy of the list of bin values.
	 */
	void get( std::list< B > &ranges,
		  std::list< V > &values ) const
	    {
		ranges = m_Range;
		values = m_Value;
	    }

	/**
	   @brief Return the value of the bin the given range falls into.
	   @param range Value to sort into bin.
	 */
	V getBinValue( const B &range ) const
	    {
		// Range list must be in ascending order.
		// Boundary conditions...
		if( range < m_Range.front() )
		{
		    return m_Value.front();
		}
		else if( range >= m_Range.back() )
		{
		    return m_Value.back();
		}


		typename std::list<B>::const_iterator
		    currentRange = m_Range.begin();
		typename std::list< V >::const_iterator
		    currentValue = m_Value.begin();

		// Ranges must be in ascending order for this to work.
		while( range >= (*currentRange) )
		{
		    ++currentRange;
		    ++currentValue;
		}

		return (*currentValue);
	    }

	/**
	   @brief Reverse the list of values for the bins.
	 */
	void reverseValues()
	    {
		m_Value.reverse();
	    }
	std::ostream &operator<< ( std::ostream &os ) const;


    protected:
	std::list< B > m_Range;
	std::list< V > m_Value;

    private:
    };

    template<typename B, typename V>
    std::ostream &RangeBin<B,V>::operator<<( std::ostream &os ) const
    {
	os << "Ranges: ";
	for( typename std::list<B>::const_iterator
		 currentRange = m_Range.begin();
	     currentRange != m_Range.end();
	     ++currentRange )
	{
	    os << (*currentRange) << " ";
	}
	os << std::endl;

	os << "Values: ";
	for( typename std::list<V>::const_iterator
		 currentValue = m_Value.begin();
	     currentValue != m_Value.end();
	     ++currentValue )
	{
	    os << (*currentValue) << " ";
	}
	os << std::endl;

	return os;
    }
}

template<typename B, typename V>
std::ostream &operator<<( std::ostream &os,
			  const vm::RangeBin<B,V> &v )
{
    v.operator<<( os );
    return os;
}

#endif
