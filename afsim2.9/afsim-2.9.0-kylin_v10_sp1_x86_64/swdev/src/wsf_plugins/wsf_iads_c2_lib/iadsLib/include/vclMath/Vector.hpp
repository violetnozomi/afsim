// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file Vector.hpp
 *  @author Keith Sawmiller, Booz Allen Hamilton \n
 *          Ken Sewell AFRL/RYZW \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3010 (Sawmiller)
 *          937-656-4899 x3004 (Sewell)
 *  @brief Vector Template Class.\n
 *  Classification: UNCLASSIFIED
 */

#include <ostream>
#include <cmath>
#include <vector>
#include <stdexcept>

#ifndef VCLMATH_VECTOR_HPP
#define VCLMATH_VECTOR_HPP 1

namespace vm
{
    /**
     *  \brief Vector Template Class.
     */
    template<typename T>
    class Vector
    {
    public:
	/*!
          \brief Vector constructor
          \param rows Number of Rows
        */
        Vector( const unsigned int &rows )
	    :m_ColumnVector( true )
	    {
		m_Array.resize( rows, static_cast<T>(0) );
	    }

	/*!
          \brief Copy constructor
          \param r Number of Rows
        */
        Vector( const Vector<T> &v ):
	    m_ColumnVector( true ),
	    m_Array( v.m_Array )
	    {
	    }

	virtual ~Vector(){}

        T operator[]( const int &i ) const { return m_Array[i]; }
        T &operator[]( const int &i ) { return m_Array[i]; }

	unsigned int size() const
	    {
		return m_Array.size();
	    }
        bool operator==( const Vector<T> &v ) const;

	Vector<T> operator+( const Vector<T> &v ) const;
	Vector<T> operator-( const Vector<T> &v ) const;

	Vector<T> &operator+=( const Vector<T> &v );
	Vector<T> &operator-=( const Vector<T> &v );

	Vector<T> operator+( const T &s ) const;
	Vector<T> operator-( const T &s ) const;
	Vector<T> operator*( const T &s ) const;
	Vector<T> operator/( const T &s ) const;

	Vector<T> &operator+=( const T &s );
	Vector<T> &operator-=( const T &s );
	Vector<T> &operator*=( const T &s );
	Vector<T> &operator/=( const T &s );

	virtual T norm() const;
	virtual void normalize();
	virtual T normSquared() const;
	virtual T dot( const Vector<T> &v ) const;
	virtual Vector<T> cross( const Vector<T> &v ) const;

	std::ostream &operator<< ( std::ostream &os ) const;

	bool isColumnVector() const
	    {
		return m_ColumnVector;
	    }

	void transpose()
	    {
		m_ColumnVector = !m_ColumnVector;
	    }

    protected:
	bool m_ColumnVector;
	std::vector< T > m_Array;

    private:
    };

    template<typename T>
    std::ostream &Vector<T>::operator<<( std::ostream &os ) const
    {
	for( unsigned int i = 0; i < m_Array.size(); ++i )
	{
	    os << m_Array[i] << " ";
	}
	return os;
    }

    template<typename T>
    bool Vector<T>::operator==( const Vector<T> &v ) const
    {
	return( m_Array == v.m_Array );
    }

    template<typename T>
    Vector<T> Vector<T>::operator+( const Vector<T> &v ) const
    {
	if( m_Array.size() == v.m_Array.size() )
	{
	    Vector<T> newVec( *this );
	    newVec += v;

	    return v;
	}
	else
	{
	    throw std::range_error("Vector dimensions do not match.");
	}
    }

    template<typename T>
    Vector<T> Vector<T>::operator-( const Vector<T> &v ) const
    {
	if( m_Array.size() == v.m_Array.size() )
	{
	    Vector<T> newVec( *this );
	    newVec -= v;

	    return newVec;
	}
	else
	{
	    throw std::range_error("Vector dimensions do not match.");
	}
    }

    template<typename T>
    Vector<T> &Vector<T>::operator+=( const Vector<T> &v )
    {
	if( m_Array.size() == v.m_Array.size() )
	{
	    for( unsigned int i = 0; i < m_Array.size(); ++i )
	    {
		m_Array[i] += v.m_Array[i];
	    }

	    return *this;
	}
	else
	{
	    throw std::range_error("Vector dimensions do not match.");
	}
    }

    template<typename T>
    Vector<T> &Vector<T>::operator-=( const Vector<T> &v )
    {
	if( m_Array.size() == v.m_Array.size() )
	{
	    for( unsigned int i = 0; i < m_Array.size(); ++i )
	    {
		m_Array[i] -= v.m_Array[i];
	    }

	    return *this;
	}
	else
	{
	    throw std::range_error("Vector dimensions do not match.");
	}
    }

    template<typename T>
    Vector<T> Vector<T>::operator+( const T &s ) const
    {
	Vector<T> newVec( *this );
	newVec += s;

	return newVec;
    }

    template<typename T>
    Vector<T> Vector<T>::operator-( const T &s ) const
    {
	Vector<T> newVec( *this );
	newVec -= s;

	return newVec;
    }

    template<typename T>
    Vector<T> Vector<T>::operator*( const T &s ) const
    {
	Vector<T> newVec( *this );
	newVec *= s;

	return newVec;
    }

    template<typename T>
    Vector<T> Vector<T>::operator/( const T &s ) const
    {
	Vector<T> newVec( *this );
	newVec /= s;

	return newVec;
    }

    template<typename T>
    Vector<T> &Vector<T>::operator+=( const T &s )
    {
	for( unsigned int i = 0; i < m_Array.size(); ++i )
	{
	    m_Array[i] += s;
	}

	return *this;
    }

    template<typename T>
    Vector<T> &Vector<T>::operator-=( const T &s )
    {
	for( unsigned int i = 0; i < m_Array.size(); ++i )
	{
	    m_Array[i] -= s;
	}

	return *this;
    }

    template<typename T>
    Vector<T> &Vector<T>::operator*=( const T &s )
    {
	for( unsigned int i = 0; i < m_Array.size(); ++i )
	{
	    m_Array[i] *= s;
	}

	return *this;
    }

    template<typename T>
    Vector<T> &Vector<T>::operator/=( const T &s )
    {
	for( unsigned int i = 0; i < m_Array.size(); ++i )
	{
	    m_Array[i] /= s;
	}

	return *this;
    }

    template<typename T>
    T Vector<T>::norm() const
    {
	return static_cast<T>( std::sqrt( normSquared() ) );
    }

    template<typename T>
    void Vector<T>::normalize()
    {
	(*this) /= norm();

	//if( std::isnan( m_Array[0] ) )
    if ( m_Array.size() == 0 )
	{
	    throw std::range_error("Attempt to normalize vector of length 0.");
	}
    }

    template<typename T>
    T Vector<T>::normSquared() const
    {
	T sum( static_cast<T>(0) );
	for( unsigned int i = 0; i < m_Array.size(); ++i )
	{
	    sum += ( m_Array[i] * m_Array[i] );
	}
	return sum;
    }

    template<typename T>
    T Vector<T>::dot( const Vector<T> &v ) const
    {
	if( m_Array.size() == v.m_Array.size() )
	{
	    T sum( static_cast<T>(0) );
	    for( unsigned int i = 0; i < m_Array.size(); ++i )
	    {
		sum += ( m_Array[i] * v.m_Array[i] );
	    }
	    return sum;
	}
	else
	{
	    throw std::range_error("Vector dimensions do not match.");
	}
    }

    template<typename T>
    Vector<T> Vector<T>::cross( const Vector<T> &v ) const
    {
	if( ( 3 == m_Array.size() )
	    && 3 == v.m_Array.size() )
	{
	    Vector<T> newVec( 3 );
	    newVec.m_Array[0] = (this->m_Array[1] * v.m_Array[2])
		- (this->m_Array[2] * v.m_Array[1]);
	    newVec.m_Array[1] = (this->m_Array[0] * v.m_Array[2])
		- (this->m_Array[2] * v.m_Array[0]);
	    newVec.m_Array[2] = (this->m_Array[0] * v.m_Array[1])
		- (this->m_Array[1] * v.m_Array[0]);

	    return newVec;
	}
	else
	{
	    throw std::range_error(
		"Vectors must be size 3 to calculate cross product."
		);
	}
    }
}

template<typename T>
std::ostream &operator<<( std::ostream &os,
			  const vm::Vector<T> &v )
{
    v.operator<<( os );
    return os;
}

#endif
