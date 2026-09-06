// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file Vector4.hpp
 *  @author Keith Sawmiller, Booz Allen Hamilton \n
 *          Ken Sewell AFRL/RYZW \n
 *          Kenneth.Sewell@wpafb.af.mil \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3010 (Sawmiller)
 *          937-656-4899 x3004 (Sewell)
 *  @brief 4D Vector Template Class.\n
 *  Classification: UNCLASSIFIED
 */

#include <vclMath/Vector.hpp>
#include <vclMath/Vector3.hpp>

#ifndef VCLMATH_VECTOR4_HPP
#define VCLMATH_VECTOR4_HPP 1

namespace vm
{
    /**
     *  \brief Vector 4d Template Class.
     */
    template<typename T>
    class Vector4 : public Vector<T>
    {
    public:
	/*!
          \brief Construct an object of type Vector4
        */
        Vector4():
	    Vector<T>( 4 )
	    {
	    }

        Vector4( const T &x, const T &y, const T &z, const T &w ):
	    Vector<T>( 4 )
	    {
		setX( x );
		setY( y );
		setZ( z );
		setW( w );
	    }

        Vector4( const Vector3<T> &v, const T &w ):
	    Vector<T>( 4 )
	    {
		setX( v.getX() );
		setY( v.getY() );
		setZ( v.getZ() );
		setW( w );
	    }

        virtual ~Vector4()
	    {
	    }

	T normSquared() const;
	T dot( const Vector4<T> &v ) const;

        void set( const T &x, const T &y, const T &z, const T &w )
            {
                this->m_Array[0] = x;
                this->m_Array[1] = y;
                this->m_Array[2] = z;
                this->m_Array[3] = w;
            }

        void get( T &x, T &y, T &z, T &w ) const
            {
                x = this->m_Array[0];
                y = this->m_Array[1];
                z = this->m_Array[2];
                w = this->m_Array[3];
            }

	void setX( const T &x )
	    {
		this->m_Array[0] = x;
	    }

	void setY( const T &y )
	    {
		this->m_Array[1] = y;
	    }

	void setZ( const T &z )
	    {
		this->m_Array[2] = z;
	    }

	void setW( const T &w )
	    {
		this->m_Array[3] = w;
	    }

	T getX() const
	    {
		return this->m_Array[0];
	    }

	T getY() const
	    {
		return this->m_Array[1];
	    }

	T getZ() const
	    {
		return this->m_Array[2];
	    }

	T getW() const
	    {
		return this->m_Array[3];
	    }

	T &getX()
	    {
		return this->m_Array[0];
	    }

	T &getY()
	    {
		return this->m_Array[1];
	    }

	T &getZ()
	    {
		return this->m_Array[2];
	    }

	T &getW()
	    {
		return this->m_Array[3];
	    }

    protected:

    private:

    };

    template<typename T>
    T Vector4<T>::normSquared() const
    {
        T sum = this->m_Array[0] * this->m_Array[0]
	    + this->m_Array[1] * this->m_Array[1]
	    + this->m_Array[2] * this->m_Array[2]
	    + this->m_Array[3] * this->m_Array[3];

        return sum;
    }

    template<typename T>
    T Vector4<T>::dot( const Vector4<T> &v ) const
    {
        T sum = this->m_Array[0] * v.m_Array[0]
	    + this->m_Array[1] * v.m_Array[1]
	    + this->m_Array[2] * v.m_Array[2]
	    + this->m_Array[3] * v.m_Array[3];

        return sum;
    }

}

#endif
