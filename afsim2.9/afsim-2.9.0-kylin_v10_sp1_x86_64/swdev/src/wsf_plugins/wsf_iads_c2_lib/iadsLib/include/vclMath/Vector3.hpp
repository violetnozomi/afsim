// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file Vector3.hpp
 *  @author Keith Sawmiller, Booz Allen Hamilton \n
 *          Ken Sewell AFRL/RYZW \n
 *          Kenneth.Sewell@wpafb.af.mil \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3010 (Sawmiller)
 *          937-656-4899 x3004 (Sewell)
 *  @brief 3D Vector Template Class.\n
 *  Classification: UNCLASSIFIED
 */

#ifndef VCLMATH_VECTOR3_HPP
#define VCLMATH_VECTOR3_HPP

#include <vclMath/Vector.hpp>

namespace vm
{
    /**
     *  \brief Vector 3d Template Class.
     */
    template<typename T>
    class Vector3 : public Vector<T>
    {
    public:
	/*!
          \brief Construct an object of type Vector3
        */
        Vector3():
	    Vector<T>( 3 )
	    {
	    }

        Vector3( const Vector3<T> &v ):
	    Vector<T>( 3 )
	    {
		this->m_Array = v.m_Array;
	    }

        Vector3( const T &x, const T &y, const T &z ):
	    Vector<T>( 3 )
	    {
		setX( x );
		setY( y );
		setZ( z );
	    }

        virtual ~Vector3()
	    {
	    }

	bool operator==( const Vector3<T> &v ) const;

        Vector3<T> operator+( const Vector3<T> &v ) const;
        Vector3<T> operator-( const Vector3<T> &v ) const;

        Vector3<T> &operator+=( const Vector3<T> &v );
        Vector3<T> &operator-=( const Vector3<T> &v );

        Vector3<T> operator+( const T &s ) const;
        Vector3<T> operator-( const T &s ) const;
        Vector3<T> operator*( const T &s ) const;
        Vector3<T> operator/( const T &s ) const;

        Vector3<T> &operator+=( const T &s );
        Vector3<T> &operator-=( const T &s );
        Vector3<T> &operator*=( const T &s );
        Vector3<T> &operator/=( const T &s );

	T normSquared() const;
	T dot( const Vector3<T> &v ) const;
	Vector3<T> cross( const Vector3<T> &v ) const;

	void set( const T &x, const T &y, const T &z )
	    {
		this->m_Array[0] = x;
		this->m_Array[1] = y;
		this->m_Array[2] = z;
	    }

	void get( T &x, T &y, T &z ) const
	    {
		x = this->m_Array[0];
		y = this->m_Array[1];
		z = this->m_Array[2];
	    }
	/**
	   @brief Set value of first component in vector.
	   @param x Value to be assigned to first component in vector.
	 */
	void setX( const T &x )
	    {
		this->m_Array[0] = x;
	    }

	/**
	   @brief Set value of second component in vector.
	   @param y Value to be assigned to second component in vector.
	 */
	void setY( const T &y )
	    {
		this->m_Array[1] = y;
	    }

	/**
	   @brief Set value of third component in vector.
	   @param z Value to be assigned to third component in vector.
	 */
	void setZ( const T &z )
	    {
		this->m_Array[2] = z;
	    }

	/**
	   @brief Get value of first vector component.
	   @returns A copy of the value of the first vector component.
	 */
        const T &getX() const
	    {
		return this->m_Array[0];
	    }

	/**
	   @brief Get value of second vector component.
	   @returns A copy of the value of the second vector component.
	 */
	const T &getY() const
	    {
		return this->m_Array[1];
	    }

	/**
	   @brief Get value of third vector component.
	   @returns A copy of the value of the third vector component.
	 */
	const T &getZ() const
	    {
		return this->m_Array[2];
	    }

	/**
	   @brief Get a reference to the first vector component.
	   @returns A reference to the first vector component.
	 */
	T &getX()
	    {
		return this->m_Array[0];
	    }

	/**
	   @brief Get a reference to the second vector component.
	   @returns A reference to the second vector component.
	 */
	T &getY()
	    {
		return this->m_Array[1];
	    }

	/**
	   @brief Get a reference to the third vector component.
	   @returns A reference to the third vector component.
	 */
	T &getZ()
	    {
		return this->m_Array[2];
	    }

	/**
	   @brief Set value of first component in vector.
	   @param lat Value to be assigned to first component in vector.
	 */
	void setLat( const T &lat )
	    {
		this->m_Array[0] = lat;
	    }

	/**
	   @brief Set value of second component in vector.
	   @param lon Value to be assigned to second component in vector.
	 */
	void setLon( const T &lon )
	    {
		this->m_Array[1] = lon;
	    }

	/**
	   @brief Set value of third component in vector.
	   @param alt Value to be assigned to third component in vector.
	 */
	void setAlt( const T &alt )
	    {
		this->m_Array[2] = alt;
	    }

	/**
	   @brief Get value of first vector component.
	   @returns A copy of the value of the first vector component.
	 */
        const T &getLat() const
	    {
		return this->m_Array[0];
	    }

	/**
	   @brief Get value of second vector component.
	   @returns A copy of the value of the second vector component.
	 */
        const T &getLon() const
	    {
		return this->m_Array[1];
	    }

	/**
	   @brief Get value of third vector component.
	   @returns A copy of the value of the third vector component.
	 */
	const T &getAlt() const
	    {
		return this->m_Array[2];
	    }

	/**
	   @brief Get a reference to the first vector component.
	   @returns A reference to the first vector component.
	 */
	T &getLat()
	    {
		return this->m_Array[0];
	    }

	/**
	   @brief Get a reference to the second vector component.
	   @returns A reference to the second vector component.
	 */
	T &getLon()
	    {
		return this->m_Array[1];
	    }

	/**
	   @brief Get a reference to the third vector component.
	   @returns A reference to the third vector component.
	 */
	T &getAlt()
	    {
		return this->m_Array[2];
	    }

    protected:

    private:

    };

    template<typename T>
    bool Vector3<T>::operator==( const Vector3<T> &v ) const
    {
        return( this->m_Array == v.m_Array );
    }

    template<typename T>
    Vector3<T> Vector3<T>::operator+( const Vector3<T> &v ) const
    {
	Vector3<T> newVec( *this );
	newVec += v;

	return newVec;
    }

    template<typename T>
    Vector3<T> Vector3<T>::operator-( const Vector3<T> &v ) const
    {
	Vector3<T> newVec( *this );
	newVec -= v;

	return newVec;
    }

    template<typename T>
    Vector3<T> &Vector3<T>::operator+=( const Vector3<T> &v )
    {
	this->m_Array[0] += v.m_Array[0];
	this->m_Array[1] += v.m_Array[1];
	this->m_Array[2] += v.m_Array[2];

	return *this;
    }

    template<typename T>
    Vector3<T> &Vector3<T>::operator-=( const Vector3<T> &v )
    {
	this->m_Array[0] -= v.m_Array[0];
	this->m_Array[1] -= v.m_Array[1];
	this->m_Array[2] -= v.m_Array[2];

	return *this;
    }

    template<typename T>
    Vector3<T> Vector3<T>::operator+( const T &s ) const
    {
        Vector3<T> newVec( *this );
        newVec += s;

        return newVec;
    }

    template<typename T>
    Vector3<T> Vector3<T>::operator-( const T &s ) const
    {
        Vector3<T> newVec( *this );
        newVec -= s;

        return newVec;
    }

    template<typename T>
    Vector3<T> Vector3<T>::operator*( const T &s ) const
    {
        Vector3<T> newVec( *this );
        newVec *= s;

        return newVec;
    }

    template<typename T>
    Vector3<T> Vector3<T>::operator/( const T &s ) const
    {
        Vector3<T> newVec( *this );
        newVec /= s;

        return newVec;
    }

    template<typename T>
    Vector3<T> &Vector3<T>::operator+=( const T &s )
    {
	this->m_Array[0] += s;
	this->m_Array[1] += s;
	this->m_Array[2] += s;

        return *this;
    }

    template<typename T>
    Vector3<T> &Vector3<T>::operator-=( const T &s )
    {
	this->m_Array[0] -= s;
	this->m_Array[1] -= s;
	this->m_Array[2] -= s;

        return *this;
    }

    template<typename T>
    Vector3<T> &Vector3<T>::operator*=( const T &s )
    {
	this->m_Array[0] *= s;
	this->m_Array[1] *= s;
	this->m_Array[2] *= s;

        return *this;
    }

    template<typename T>
    Vector3<T> &Vector3<T>::operator/=( const T &s )
    {
	this->m_Array[0] /= s;
	this->m_Array[1] /= s;
	this->m_Array[2] /= s;

        return *this;
    }

    template<typename T>
    T Vector3<T>::normSquared() const
    {
        T sum = this->m_Array[0] * this->m_Array[0]
	    + this->m_Array[1] * this->m_Array[1]
	    + this->m_Array[2] * this->m_Array[2];

        return sum;
    }

    template<typename T>
    T Vector3<T>::dot( const Vector3<T> &v ) const
    {
        T sum = this->m_Array[0] * v.m_Array[0]
	    + this->m_Array[1] * v.m_Array[1]
	    + this->m_Array[2] * v.m_Array[2];

        return sum;
    }

    template<typename T>
    Vector3<T> Vector3<T>::cross( const Vector3<T> &v ) const
    {
	Vector3<T> newVec(
	    (this->m_Array[1] * v.m_Array[2])
	    - (this->m_Array[2] * v.m_Array[1]),
	    -((this->m_Array[0] * v.m_Array[2])
	      - (this->m_Array[2] * v.m_Array[0])),
	    (this->m_Array[0] * v.m_Array[1])
	    - (this->m_Array[1] * v.m_Array[0])
	    );

	return newVec;
    }

}

#endif
