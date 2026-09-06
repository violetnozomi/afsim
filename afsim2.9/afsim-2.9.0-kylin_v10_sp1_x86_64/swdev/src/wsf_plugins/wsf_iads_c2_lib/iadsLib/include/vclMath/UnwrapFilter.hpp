// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file UnwrapFilter.hpp
 *  @author Keith Sawmiller AFRL/RYZW \n
 *          Keith.Sawmiller.ctr@wpafb.af.mil \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3010
 *  @brief Template Class to support unwrapping of angle measurements.\n
 *
 *  Classification: UNCLASSIFIED
 */

#ifndef VCLMATH_UNWRAPFILTER_HPP
#define VCLMATH_UNWRAPFILTER_HPP 1
#include <cmath>
#include <complex>
#include <iostream>
#include <vclMath/Constants.hpp>

namespace vm
{
    /**
     *  \brief Template Class to support unwrapping of angle measurements
     */
    template<typename T>
    class UnwrapFilter
    {
    public:
	/*!
          \brief UnwrapFilter
        */
        UnwrapFilter() : m_offset(0.0), m_currentSample(0.0), m_lastSample(0.0)
	{
	}

	~UnwrapFilter(){}

	/**
	   @brief Set the current filter sample input
	   @param _sample current angle data in radians
	   @return filtered (unwrapped) angle
	 */
	T unwrap(T _sample)
	{

		// set the last sample prior to overwriting
		this->m_lastSample = this->m_currentSample;

		this->m_currentSample = _sample;

		// convert angle to complex representation e^(jw)
		std::complex<T> _j(0,1);
		std::complex<T> _expAngle;
		_expAngle = exp( _j * this->m_currentSample );

		// convert back to angle space by ln(e^(jw)), guaranteeing angles between +/- PI
		this->m_currentSample = imag( log( _expAngle ) );

		// compute the difference between the last sample and this one
		T _angleDiff = this->m_currentSample - this->m_lastSample;

		// if the difference is larger than PI, the signal has wrapped, and a 2PI offset needs applied
		if(fabs(_angleDiff) > vm::PI)  // REW hack
		{
			if(_angleDiff > 0)
			{
				this->m_offset = this->m_offset + vm::TWOPI;
				//std::cout << "+" << std::endl;
			}
			else
			{
				this->m_offset = this->m_offset - vm::TWOPI;
				//std::cout << "-" << std::endl;
			}
		}

		// update the filter output sample
		return (this->m_currentSample - this->m_offset);

	}

    private:
	T m_offset;
	T m_currentSample;
	T m_lastSample;
    };
}
#endif
