// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file Matrix.hpp
 *  @author Keith Sawmiller, Booz Allen Hamilton \n
 *          Ken Sewell AFRL/RYZW \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3010 (Sawmiller)
 *          937-656-4899 x3004 (Sewell)
 *  @brief Two Dimensional Matrix Template Base Class.\n
 *  Classification: UNCLASSIFIED
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <exception>
#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include <complex>
#include <vclMath/Vector3.hpp>
#include <vclMath/Vector4.hpp>
//#include <vclMath/ComplexMatrix.hpp>


#ifndef VCLMATH_COREMATRIX_HPP
#define VCLMATH_COREMATRIX_HPP 1

#ifdef VCLMATH_LAPACK_ENABLED
#include <vclMath/MatrixLapackInterface.hpp>
#endif

#define MACRO_COUT std::cout << __FILE__ << "@" << __LINE__ << "\t"

/**
 *  \brief Namespace to encapsulate vclMath routines
 */
namespace vm
{
    //template <typename T> class ComplexMatrix;

    /**
     *  \brief Two Dimensional Matrix Template Base Class.
     */
    template <typename T > class Matrix
    {

        public:
            /*!
              \brief Construct an object of type Matrix
              \param r Number of Rows
              \param c Number of Columns
            */
            Matrix ( const unsigned int &r, const unsigned int &c );

            /*!
              \brief Construct an object of type Matrix, initialize with std::vector data
              \param inputVector initialization data
              \param rows Number of Rows (used to reshape)
              \param cols Number of Rows (used to reshape)
            */
            Matrix ( const std::vector<T> &inputVector,
                     const unsigned int &rows,
                     const unsigned int &cols );

            /*!
              \brief Construct an object of type Matrix
            */
            Matrix ();

            /*!
              \brief Construct an object of type Matrix (Copy Constructor)
              \param inObj Matrix to be copied
            */
            Matrix ( const Matrix < T > &inObj );

            Matrix ( const Matrix < std::complex<T> > &inMatrix ) : Matrix ( inMatrix.m_nRows, inMatrix.m_nCols )
            {

            }

            /*!
              \brief Destroy an object of type Matrix
            */
            virtual ~ Matrix ();

            /*!
              \brief Get the number of rows defined for this Matrix object
              \return r number of rows
            */
            unsigned int getNumRows () const;

            /*!
              \brief Get the number of columns defined for this Matrix object
              \return r number of columns
            */
            unsigned int getNumColumns () const;

            /*!
              \brief resize
              \param r New row size
              \param c New column size
            */
            void resize ( const unsigned int &r, const unsigned int &c );

            /*!
            \brief Resizes the matrix while retaining the current data
            \param r New row size
            \param c New column size
            */
            void copy_resize ( const unsigned int &r, const unsigned int &c );

            /*!
              \brief Populate the matrix with data from a file
              \param filename File containing values
            */
            void populateFromFile ( const std::string & filename );

            /*!
              \brief Populate a Matrix object with an array of data
              \param U One-Dimensional input array of size (Rows*Columns)
              of the target Matrix object
            */
            void populate ( T[] );

            /*!
              \brief Fill a Matrix object with uniform random values between
              (-1.0 and 1.0) x scalar
              \param scalar
            */
            inline void fillWithUniformRandom ( const T & scalar );

            /*!
              \brief Fill a Matrix object with value of scalar.
              \param scalar
            */
            void fill ( const T &scalar );

            /*!
              \brief Fill a Matrix object with ones
            */
            void ones ();

            /*!
              \brief Set a square Matrix to be an Identity matrix
              all zeros except 1's on the diagonal)
            */
            void makeIdentity ();

            /*!
              \brief Display the Matrix object to stdout
            */
            void display () const;

            /*!
              \brief Display the Matrix object to stdout w/ additional text
            */
            void display ( const std::string &text ) const;

            /*!
              \brief Display the Matrix object to stdout w/ additional text
            */
            void displayMatlabForm ( const std::string &text ) const;

            std::ostream & operator<< ( std::ostream & os ) const;

            /*!
              \brief Get a specific row of a Matrix object
              \param  r       The row of interest
              \return Matrix Returns a Matrix object equal to the
              row of interest
            */
            Matrix < T > getRow ( const unsigned int &r ) const;

            /*! \brief Set a specific row of a Matrix object
              \param r The row of interest
              \param Row Matrix row containing the values to be inserted
            */
            void setRow ( const unsigned int &r, const Matrix < T > &Row );

            /*!
              \brief Get a specific column of a Matrix object
              \param c The column of interest
              \return Matrix Returns a Matrix object equal to the
              column of interest
            */
            Matrix < T > getColumn ( const unsigned int &c );

            /*! \brief Set a specific column of a Matrix object
             *  \param c The column of interest
             \param Column Matrix column containing the values to be inserted
            */
            void setColumn ( const unsigned int &c, const Matrix < T > &Column );

//(AAP)
            bool getIsSquare ();

//(AAP)
            /*! \brief Compute the transpose of a matrix
             *  \return Matrix Transposed Matrix object
             */
            Matrix < T > transpose () const;

            /*! \brief Compute the inverse of a square matrix using Gaussian
              Elimination
              *  \return Matrix Inverse Matrix object
              */
            Matrix < T > inverse () const;

            /*! \brief Compute the determinant of a square matrix
             *  \return T Determinant of the matrix
             */
            T determinant () const;

            /*! \brief Concatenate horizontally
             *  \param A Matrix to be concatenated to the right
             *  \return  Concatenated matrix
             */
            Matrix < T > horzcat ( const Matrix < T > &A ) const;

            /*! \brief Concatenate vertically
             *  \param A Matrix to be concatenated to the bottom
             *  \return  Concatenated matrix
             */
            Matrix < T > vertcat ( const Matrix < T > &A ) const;

            /*! \brief Compute the sum of this Matrix with another of the same size
             *  \param A Matrix to be added
             *  \return  Matrix Sum
             */
            Matrix < T > operator+ ( const Matrix < T > &A ) const;

            /*! \brief Sum another same size Matrix with this one.
             *  \param A Matrix to be added
             *  \return  Matrix Sum
             */
            Matrix < T > &operator+= ( const Matrix < T > &A );

            /*! \brief Compute the difference of this Matrix with another of the same size
             *  \param A Matrix to be subtracted
             *  \return  Matrix Difference
             */
            Matrix < T > operator- ( const Matrix < T > &A ) const;

            /*! \brief Compute the matrix product of this Matrix with another of the same size (this oject is on left)
             *  \param RHM Matrix to be multiplied
             *  \return  Matrix Product
             */
            Matrix < T > operator* ( const Matrix <T> &RHM ) const;

            //======================================================================

            /*! \brief Compute the matrix product of this Matrix with a vector
             *  \param RHM Matrix to be multiplied
             *  \return  Matrix Product
             */
            Matrix < T > multVector3x3 ( const Matrix <T> &RHM ) const;

            template <typename U>
            /*! \brief Compute the matrix product of this Matrix with a vector
             *  \param RHM Vector3 to be multiplied
             *  \return  Matrix Product
             */
            Vector3 <U> multVector3x3 ( const Vector3<U> &RHM ) const;

            /*! \brief Compute the matrix product of this Matrix with a vector
             *  \param RHM Matrix to be multiplied
             *  \return  Matrix Product
             */
            Matrix < T > multVector4x4 ( const Matrix <T> &RHM ) const;

            template <typename U>
            /*! \brief Compute the matrix product of this Matrix with a vector
             *  \param RHM Matrix to be multiplied
             *  \return  Matrix Product
             */
            Vector4<U> multVector4x4 ( const Vector4<U> &RHM ) const;

            /*! \brief Compute the matrix product of this Matrix with another
             *  \param RHM Matrix to be multiplied
             *  \return  Matrix Product
             */
            Matrix < T > multMatrix3x3 ( const Matrix <T> &RHM ) const;

            /*! \brief Compute the matrix product of this Matrix with another
             *  \param RHM Matrix to be multiplied
             *  \return  Matrix Product
             */
            Matrix < T > multMatrix4x4 ( const Matrix <T> &RHM ) const;

            //======================================================================

            /*! \brief Compute the scalar product of this Matrix with a scalar
             *  \param scalar Scalar to be multiplied
             *  \return  Matrix Product
             */
            Matrix < T > operator* ( const T & scalar ) const;

            /*! \brief Compute the scalar quotient of this Matrix with a scalar
             *  \param scalar Scalar to be divided
             *  \return  Matrix Quotient
             */
            Matrix < T > operator/ ( const T & scalar ) const;

            /*! \brief Compute the matrix power
             *  \param x integer power
             *  \return  Matrix Result
             */
            Matrix < T > operator^ ( const int &x ) const;

            /*! \brief Compute the matrix exponential Y = e^(At) via Taylor series
             *  \param x integer number of Taylor series terms
             *  \param t exponential term
             *  \return  Matrix Result
             */
            Matrix < T > expm ( const unsigned int &x, const T &t );

            /*! \brief Provides (i,j) access to Matrix arrays
              \param r row index
              \param c column index
            */
            const T &operator() ( const unsigned int &r,
                                  const unsigned int &c ) const
            {
                //return this->m_Vector[this->m_nCols * ( r - 1 ) + c - 1];
                return this->m_Vector[this->m_nRows * ( c - 1 ) + r - 1];
            }


            /*!
              \brief Provides (i,j) assignment access to Matrix arrays
              \param  r       Row
              \param  c       Column
              \return Pointer to element (Row,Column)
            */
            T &operator() ( const unsigned int &r,
                            const unsigned int &c )
            {
                if ( ( m_nCols >= c ) && ( m_nRows >= r ) )
                {
                    return m_Vector[ m_nRows * ( c - 1 ) + r - 1];
                }
                else
                {
                    std::cerr << "Assignment attempt: ("
                    << r << "," << c << "), array size = ("
                    << m_nRows << "," << m_nCols << ")"
                    << std::endl;

                    throw std::out_of_range (
                        "Attempting to index beyond array size!" );
                }
            }
#ifndef WIN32
            __attribute__ ( ( deprecated ) )
#endif
            /*!
              \brief Provides (i,j,value) access to Matrix arrays for assignment (deprecated)
              \param r int row index
              \param c int column index
              \param u T value to be stored in the (r,c) position
            */
            void operator() ( const unsigned int &r,
                              const unsigned int &c,
                              const T & u );


            /*! \brief Overloaded = operator
              \param A Pointer to Matrix to be copied into
            */
            void operator= ( const Matrix < T > &A );

            /*!
              \brief Extract a sub-matrix
              \param rowBegin upper left corner i
              \param colBegin upper left corner j
              \param rowEnd lower right corner i
              \param colEnd lower right corner j
            */
            Matrix < T > subMatrix ( const unsigned int &rowBegin,
                                     const unsigned int &colBegin,
                                     const unsigned int &rowEnd,
                                     const unsigned int &colEnd );

            /*!
              \brief Sets the objects output precision when using the display()
              method to the screen
              \param p Number of decimal places to display
            */
            void setDisplayPrecision ( const unsigned int &p );

            /*! \brief Compute the normalize column vector
             *  \return Matrix  normalized column vector of the same size
             */
            void normalizeColumns ();

            /*! \brief Return the diagonal as a column vector
             *  \return Matrix  column vector of diagonal elements
             */
            Matrix < T > diagVector () const;

            Matrix < T > perElementPower ( const T &x ) const;
            Matrix < T > perElementAdd ( const Matrix<T> &x ) const;
            Matrix < T > perElementSub ( const Matrix<T> &x ) const;
            Matrix < T > perElementMul ( const Matrix<T> &x ) const;
            Matrix < T > perElementDiv ( const Matrix<T> &x ) const;

            /// =================================== LAPACK INTERFACE FUNCTIONS

            template <typename U>
            void getEigenvectorsAndValues ( Matrix< std::complex<U> > & values, Matrix< std::complex<U> > & vectors ) const;

            template <typename P>
            void getSingularValueDecomposition ( Matrix<T> & U, Matrix<P> & S, Matrix<T> & VT ) const
            {
                /// real matrices will always generate real SVD's.
                /// no need to worry about complex values for the real case (unlike eigendecomposition)
                std::vector<P> S_Vector;
                this->getSingularValueDecomposition ( U, S_Vector, VT );

                S.resize ( this->m_nRows, this->m_nCols );

                // reconfigure S to place along diagonal
                for ( unsigned int k ( 1 ); k <= S_Vector.size(); ++k )
                    S ( k,k ) = S_Vector[k-1];
            }

            template <typename P>
            void getSingularValueDecomposition ( Matrix<T> & U, std::vector<P> & S, Matrix<T> & VT ) const
            {
                /// real matrices will always generate real SVD's.
                /// no need to worry about complex values for the real case (unlike eigendecomposition)

                // local copy of input matrix
                vm::Matrix<T> A ( *this );

                U.resize ( A.m_nRows, A.m_nRows );
                if ( A.m_nCols < A.m_nRows )
                  S.resize ( A.m_nCols );
                else
                  S.resize ( A.m_nRows );
                VT.resize ( A.m_nCols, A.m_nCols );

                std::vector<T> WORK ( 1 );

                char JOBU ( 'A' ),  // compute (A)ll of the columns of U
                JOBVT ( 'A' ); // compute (A)ll of the columns of V'

                int M ( A.m_nRows ),
                N ( A.m_nCols ),
                LDA ( A.m_nRows ),
                LDU ( A.m_nRows ),
                LDVT ( A.m_nCols ),
                LWORK ( -1 ),
                INFO ( 99 );

                // call lapack routine for workspace query
                xgesvd_ ( JOBU, JOBVT, M, N, A.m_Vector[0],
                          LDA, S[0], U.m_Vector[0],
                          LDU, VT.m_Vector[0], LDVT, WORK[0],
                          LWORK, INFO );

                // capture LWORK
                LWORK = getLapackLWORK ( WORK[0] );
                WORK.resize ( LWORK );

                // run lapack routine
                xgesvd_ ( JOBU, JOBVT, M, N, A.m_Vector[0],
                          LDA, S[0], U.m_Vector[0],
                          LDU, VT.m_Vector[0], LDVT, WORK[0],
                          LWORK, INFO );
            }



            /// 1-D Array Container
            std::vector < T > m_Vector;




        protected:

            /// Inlined subscript to 0 based index on Matrix objects
            unsigned int sub2ind ( const unsigned int &i,
                                   const unsigned int &j ) const
            {
                //return this->m_nCols * ( i - 1 ) + j - 1;
                //return this->m_nCols * ( i - 1 ) + j - 1;
                return this->m_nRows * ( j - 1 ) + i - 1;
            }

            /// Inlined subscript to 0 based index on m_Vector objects
            unsigned int sub2ind ( const unsigned int &i,
                                   const unsigned int &j,
                                   const unsigned int &nRows ) const
            {
                return nRows * ( j - 1 ) + i - 1;
            }

            void computeLUFactorization ( Matrix<T> & A, std::vector<int> & IPIV ) const;

            void xgetri_ ( int& N, T& A, int& LDA, int& Pivots, T& vec_work, int& LWORK, int& INFO ) const;

            void xgetrf_ ( int& M, int& N, T& A, int& LDA, int& IPIV, int& INFO ) const;

            void xgeevx_ ( char& BALANC, char& JOBVL, char& JOBVR, char& SENSE,
                           int& N, T& A, int& LDA, T& WR, T& WI, T& VL, int& LDVL,
                           T& VR, int& LDVR, int& ILO, int& IHI, T& SCALE, T& ABNRM,
                           T& RCONDE, T& RCONDV, T& WORK, int& LWORK, int& IWORK, int& INFO ) const;

            template <typename P>
            void lgeevx_ ( char& BALANC, char& JOBVL, char& JOBVR, char& SENSE,
                           int& N, P& A, int& LDA, P& WR, P& WI, P& VL, int& LDVL,
                           P& VR, int& LDVR, int& ILO, int& IHI, P& SCALE, P& ABNRM,
                           P& RCONDE, P& RCONDV, P& WORK, int& LWORK, int& IWORK, int& INFO ) const;

            template <typename P>
            void complex_xgeevx_ (
                const char& BALANC,
                const char& JOBVL,
                const char& JOBVR,
                const char& SENSE,
                const int& N,
                T& A,  // array
                const int& LDA,
                T& W, // array
                const T& VL,  // array
                const int& LDVL,
                T& VR,  // array
                const int& LDVR,
                int& ILO,
                int& IHI,
                P& SCALE,  // array
                P& ABNRM,
                const P& RCONDE,  // array
                const P& RCONDV,  // array
                T& WORK,  // array
                int& LWORK,
                P& RWORK,  // array
                int& INFO ) const;

            template <typename P>
            void xgesvd_ ( char& JOBU, char& JOBVT, int& M, int& N, T& A,
                           int& LDA, P& S, T& U, int& LDU, T& VT,
                           int& LDVT, T& WORK, int& LWORK, int& INFO ) const;


        protected:
            /// Number of Rows in Matrix.
            unsigned int m_nRows;

            /// Number of Cols in Matrix.
            unsigned int m_nCols;

            /// Defines the precision of a matrix when displayed to stdout.
            unsigned int m_displayPrecision;

            /// TRUE if matrix is square.
            bool m_isSquare;

            template < typename U >
            unsigned int getLapackLWORK ( const U& workValue ) const
            {
                return static_cast<unsigned int> ( workValue );
            }

            template < typename U >
            unsigned int getLapackLWORK ( const std::complex<U>& workValue ) const
            {
                return static_cast<unsigned int> ( workValue.real() );
            }

            // these are needed because of nested templating Matrix< std::complex < double > > etc.
            template <typename U>
            void getEigenvectorsAndValues ( Matrix< std::complex<U> > & values, Matrix< std::complex<U> > & vectors, const U& firstValue ) const;

            template <typename U>
            void getEigenvectorsAndValues ( Matrix< std::complex<U> > & values, Matrix< std::complex<U> > & vectors, const std::complex<U>& firstValue ) const;

            template <typename U>
            Matrix<T> transposeOrHermitian ( const Matrix<T>&, const U& firstValue ) const;

            template <typename U>
            Matrix<T> transposeOrHermitian ( const Matrix<T>&, const std::complex<U>& firstValue ) const;

            friend class Matrix< std::complex<long double> >;
            friend class Matrix< long double >;
            friend class Matrix< std::complex<double> >;
            friend class Matrix< double >;
            friend class Matrix< std::complex<float> >;
            friend class Matrix< float >;

    };


    template < typename T >
    Matrix < T >::Matrix ( const unsigned int &r, const unsigned int &c ) :
            m_nRows ( r ),
            m_nCols ( c ),
            m_displayPrecision ( 8 ),
            m_isSquare ( r == c )
    {
        m_Vector.resize ( r * c, static_cast < T > ( 0.0 ) );
    }

    template < typename T>
    Matrix <T> :: Matrix ( const std::vector<T> &inputVector,
                           const unsigned int &rows,
                           const unsigned int &cols )
    {
        if ( ( rows * cols ) == inputVector.size() )    // size ok
        {
            m_nRows = rows;
            m_nCols = cols;
            m_isSquare = ( rows == cols );

            m_Vector.resize ( rows * cols, static_cast < T > ( 0.0 ) );
            m_displayPrecision = 8;
            m_Vector = inputVector;
        }
        else
        {
            std::cerr << "Matrix Size: "
            << rows << "x" << cols
            << ", but vector is "
            << inputVector.size() << std::endl;

            throw std::runtime_error ( "Input vector has the wrong size." );
        }
    }

    template < typename T > Matrix < T >::Matrix () :
            m_nRows ( 0 ),
            m_nCols ( 0 ),
            m_displayPrecision ( 8 ),
            m_isSquare ( false )
    {
    }

    template<typename T>
    Matrix<T>::Matrix ( const Matrix<T> &inObj ) :
            m_Vector ( inObj.m_Vector ),
            m_nRows ( inObj.m_nRows ),
            m_nCols ( inObj.m_nCols ),
            m_displayPrecision ( 8 ),
            m_isSquare ( inObj.m_isSquare )
    {
    }

    /*
      DECONSTRUCTOR
    */
    template < typename T > Matrix < T >::~Matrix ()
    {
    }


    template < typename T >
    void Matrix < T >::resize ( const unsigned int &r,
                                const unsigned int &c )
    {
        if ( ( r == 0 ) || ( c == 0 ) )
        {
            std::cout << "Matrix (resize): "
            << "Zero passed in for size. " << "Aborting!" << std::endl;
            abort ( );
        }
        this->m_nRows = r;
        this->m_nCols = c;
        if ( r == c )
            this->m_isSquare = true;
        else
            this->m_isSquare = false;

        this->m_Vector.clear ();
        this->m_Vector.resize ( r * c, static_cast < T > ( 0.0 ) );
        this->m_displayPrecision = 8;
    }

    template < typename T >
    void Matrix < T >::copy_resize ( const unsigned int &r,
                                const unsigned int &c )
    {
        if ( ( r == 0 ) || ( c == 0 ) )
        {
            std::cout << "Matrix (resize): "
            << "Zero passed in for size. " << "Aborting!" << std::endl;
            abort ( );
        }

        // Handle Matrix Shrinking
        if ( r <= m_nRows && c <= m_nCols )
        {
            *this = subMatrix( 1,1,r,c );
            return;
        }

        // Easy case
        if ( m_nRows == r )
        {
            this->m_Vector.resize ( r * c, static_cast < T > ( 0.0 ) );
        }
        // Hard case with intensive copying
        else
        {
            std::vector< T > NewArray ( r * c, static_cast < T > ( 0.0 ) );

            unsigned int newIndex ( 0 );
            for ( unsigned int oldIndex ( 0 ); oldIndex < m_nRows*m_nCols; ++oldIndex )
            {
                newIndex = static_cast<unsigned int> ( ( std::floor( static_cast<float>(oldIndex)
					 / static_cast<float>(m_nRows) ) * r )
		    + std::fmod( static_cast<float>(oldIndex),
				 static_cast<float>(m_nRows) ) );

                // New Array is most likely smaller than the old array
                if ( newIndex >= NewArray.size() )
                    break;

                NewArray[ newIndex ] = m_Vector[ oldIndex ];
            }

            m_Vector = NewArray;
        }

        this->m_nRows = r;
        this->m_nCols = c;

        if ( r == c )
            this->m_isSquare = true;
        else
            this->m_isSquare = false;

        this->m_displayPrecision = 8;
    }

    template < typename T >
    void Matrix < T >::populateFromFile ( const std::string & filename )
    {
        std::ifstream infile ( filename.c_str () );
        char _buffer[120];
        char *_fieldBuffer;
        char *_strtod_ptr;
        T _temp;
        // to track which values have been set
        unsigned int _row = 1, _col = 1;
        bool _done = false;

        if ( !infile.good () )
        {
            std::cout << "Matrix (PopulateFromFile): "
            << "Error locating file \"" << filename
            << "\" Aborting!" << std::endl;
            abort ( );
        }

        while ( !_done )
        {
            infile.getline ( _buffer, 120 );
            // check for comments
            if ( std::strstr ( _buffer, "#" ) == NULL )
            {
                // read first value & populate
                _fieldBuffer = std::strtok ( _buffer, " " );
                _temp = static_cast < T > ( strtod ( _fieldBuffer, &_strtod_ptr ) );
                this->m_Vector[ this->sub2ind ( _row,_col ) ] = _temp;

                //finish parsing this line (filling columns for this row)
                for ( unsigned int k = 2; k <= this->m_nCols; ++k )
                {
                    _col++;
                    _fieldBuffer = std::strtok ( NULL, " " );
                    _temp =
                        static_cast < T > ( strtod ( _fieldBuffer, &_strtod_ptr ) );
                    this->m_Vector[ this->sub2ind ( _row,_col ) ] = _temp;
                }
                // done with this row, reset columns to 1
                _col = 1;

                if ( _row == this->m_nRows )
                {
                    infile.close ();
                    _done = true;
                }
                ++_row;
            }
        }
    }

    template < typename T >
    void Matrix < T >::fillWithUniformRandom ( const T & scalar )
    {
        for ( typename std::vector < T >::iterator i = m_Vector.begin ();
                i != m_Vector.end (); ++i )
        {
            ( *i ) = scalar * ( static_cast < T > ( rand () )
                                / static_cast < T > ( RAND_MAX ) );
        }
    }

    template <>
    inline void Matrix < std::complex<double> >::fillWithUniformRandom ( const std::complex<double> & scalar )
    {
        for ( unsigned int k ( 0 ); k < this->m_Vector.size(); ++k )
        {
#ifdef WIN32
            this->m_Vector[k].real( scalar.real() * rand() / static_cast<double>( RAND_MAX ) );
            this->m_Vector[k].imag( scalar.real() * rand() / static_cast<double>( RAND_MAX ) );
#else
#if ( __GNUC__ > 4 || ( __GNUC__ >= 4 && __GNUC_MINOR__ >= 4 ) )
            this->m_Vector[k].real( scalar.real() * rand() / static_cast<double>( RAND_MAX ) );
            this->m_Vector[k].imag( scalar.real() * rand() / static_cast<double>( RAND_MAX ) );
#else
#ifndef __GXX_EXPERIMENTAL_CXX0X__
            this->m_Vector[k].real() = scalar.real() * rand() / static_cast<double>( RAND_MAX );
	    this->m_Vector[k].imag() = scalar.real() * rand() / static_cast<double>( RAND_MAX );
#else
            this->m_Vector[k].real() = scalar.real() * rand() / static_cast<double>( RAND_MAX );
	    this->m_Vector[k].imag() = scalar.real() * rand() / static_cast<double>( RAND_MAX );
#endif
#endif
#endif
        }
    }

    template <>
    inline void Matrix < std::complex<float> >::fillWithUniformRandom ( const std::complex<float> & scalar )
    {
      for ( unsigned int k ( 0 ); k < this->m_Vector.size(); ++k )
        {
#ifdef WIN32
	    this->m_Vector[k].real( scalar.real() * static_cast<float>(rand()) / static_cast<float>(RAND_MAX) );
	    this->m_Vector[k].imag( scalar.real() * static_cast<float>(rand()) / static_cast<float>(RAND_MAX) );
#else
#if ( __GNUC__ > 4 || ( __GNUC__ >= 4 && __GNUC_MINOR__ >= 4 ) )
	    this->m_Vector[k].real( scalar.real() * static_cast<float>(rand()) / static_cast<float>(RAND_MAX) );
	    this->m_Vector[k].imag( scalar.real() * static_cast<float>(rand()) / static_cast<float>(RAND_MAX) );
#else
#ifndef __GXX_EXPERIMENTAL_CXX0X__
	    this->m_Vector[k].real() = scalar.real() * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	    this->m_Vector[k].imag() = scalar.real() * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
#else
            this->m_Vector[k].real() = scalar.real() * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            this->m_Vector[k].imag() = scalar.real() * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
#endif
#endif
#endif
        }
    }

    template < typename T > void Matrix < T >::fill ( const T &scalar )
    {
        m_Vector.assign ( m_Vector.size (), scalar );
    }

    template < typename T > void Matrix < T >::ones ()
    {
        fill ( static_cast < T > ( 1 ) );
    }

    template < typename T > void Matrix < T >::makeIdentity ()
    {
        if ( m_isSquare )
        {
            // clear all elements (set to zero)
            fill ( 0 );

            for ( unsigned int index = 0; index < m_Vector.size();
                    index += ( m_nCols+1 ) )
            {
                // set diagonal to 1
                this->m_Vector[ index ] = static_cast < T > ( 1 );
            }
        }
        else
        {
            throw std::runtime_error (
                "makeIdentity(): input array was not square!" );
        }
    }

    template < typename T > void Matrix < T >::populate ( T U[] )
    {
        for ( unsigned int index = 0; index < m_Vector.size(); ++index )
        {
            m_Vector[index] = U[index];
        }
    }

    template < typename T > unsigned int Matrix < T >::getNumColumns () const
    {
        return m_nCols;
    }

    template < typename T > unsigned int Matrix < T >::getNumRows () const
    {
        return m_nRows;

    }

    template < typename T > void Matrix < T >::display () const
    {
        for ( unsigned int row = 1; row <= this->getNumRows (); ++row )
        {
            for ( unsigned int col = 1; col <= this->getNumColumns (); ++col )
            {
                std::cout << std::setw ( this->m_displayPrecision )
                << ( *this ) ( row, col ) << "\t\t";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    template < typename T > void Matrix < T >::display (
        const std::string &text ) const
    {
        std::cout << std::endl << text << ":" << std::endl;
        this->display();
    }

    template < typename T > std::ostream & Matrix < T >::operator<< ( std::ostream & os ) const
    {
        for ( unsigned int row = 1; row <= this->getNumRows (); ++row )
        {
            for ( unsigned int col = 1; col <= this->getNumColumns (); ++col )
            {
                os << "\t" << ( *this ) ( row, col );
            }
            os << std::endl;
        }
        return os;
    }

    template < typename T > void Matrix < T >::displayMatlabForm (
        const std::string &text ) const
    {
        std::cout << std::endl << text << ":" << std::endl;
        std::cout << "[";
        for ( unsigned int row = 1; row <= this->getNumRows (); ++row )
        {
            for ( unsigned int col = 1; col <= this->getNumColumns (); ++col )
            {
                std::cout << std::setw ( this->m_displayPrecision )
                << ( *this ) ( row, col );
                if ( ( col == this->getNumColumns() ) && ( row != this->getNumRows() ) )
                {
                    std::cout << ";" << std::endl;
                }
                else if ( col != this->getNumColumns() )
                {
                    std::cout << ",";
                }
            }
        }
        std::cout << "]\n";
    }

    template < typename T >
    void Matrix < T >::setDisplayPrecision ( const unsigned int &p )
    {
        this->m_displayPrecision = p;
    }

    template < typename T >
    Matrix < T > Matrix < T >::transpose () const
    {
        return this->transposeOrHermitian ( *this,this->m_Vector[0] );
    }

    /* OPERATOR OVERLOADING */
    template < typename T >
    Matrix < T > Matrix < T >::operator+ ( const Matrix < T >
                                           &A ) const
    {
        unsigned int r = m_nRows;
        unsigned int c = m_nCols;
        // check for same size
        if ( ( r == A.m_nRows ) && ( c == A.m_nCols ) )
        {
            Matrix < T > sum ( r, c );
            for ( unsigned int row = 1; row <= r; row++ )
            {
                for ( unsigned int col = 1; col <= c; col++ )
                {
                    //sum ( row, col ) = ( *this ) ( row, col ) + A ( row, col );
                    sum.m_Vector[sum.sub2ind ( row,col ) ] =
                        m_Vector[sub2ind ( row,col ) ]
                        + A.m_Vector[A.sub2ind ( row,col ) ];
                }
            }
            return sum;
        }
        else
        {
            std::cout << "Matrix ( (+) ) Illegal attempt to "
            << "add dissimmilar arrays!" << std::endl;
            std::cout << this->m_nRows << "x" << this->m_nCols << " - "
            << A.m_nRows << "x" << A.m_nCols << std::endl;
            return A;
        }
    }

    template < typename T >
    Matrix < T > Matrix < T >::horzcat ( const Matrix < T >
                                         &A ) const
    {
        unsigned int r = m_nRows;
        unsigned int c = m_nCols;
        // check for same size rows
        if ( ( r == A.m_nRows ) )
        {
            Matrix < T > _concatArray ( r, c + A.m_nCols );
            for ( unsigned int row = 1; row <= r; row++ )
            {
                for ( unsigned int col = 1; col <= c; col++ )
                {
                    //sum ( row, col ) = ( *this ) ( row, col ) + A ( row, col );
                    _concatArray.m_Vector[_concatArray.sub2ind ( row,col ) ] = this->m_Vector[this->sub2ind ( row,col ) ];
                }
                for ( unsigned int col = 1; col <= A.m_nCols; col++ )
                {
                    //sum ( row, col ) = ( *this ) ( row, col ) + A ( row, col );
                    _concatArray.m_Vector[_concatArray.sub2ind ( row,col+c ) ] = A.m_Vector[A.sub2ind ( row,col ) ];
                }

            }
            return _concatArray;
        }
        else
        {
            std::cout << "Matrix ( concat ) Illegal attempt to "
            << "concatenate dissimmilar arrays!" << std::endl;
            std::cout << this->m_nRows << "x" << this->m_nCols << " - "
            << A.m_nRows << "x" << A.m_nCols << std::endl;
            return A;
        }
    }

    template < typename T >
    Matrix < T > Matrix < T >::vertcat ( const Matrix < T >
                                         &A ) const
    {
        unsigned int r = this->m_nRows;
        unsigned int c = this->m_nCols;
        // check for same size rows
        if ( ( c == A.m_nCols ) )
        {
            Matrix < T > _concatArray ( r + A.m_nRows, c );
            for ( unsigned int col = 1; col <= c; col++ )
            {
                for ( unsigned int row = 1; row <= r; row++ )
                {
                    //sum ( row, col ) = ( *this ) ( row, col ) + A ( row, col );
                    _concatArray.m_Vector[_concatArray.sub2ind ( row,col ) ] = this->m_Vector[this->sub2ind ( row,col ) ];
                }
                for ( unsigned int row = 1; row <= A.m_nRows; row++ )
                {
                    //sum ( row, col ) = ( *this ) ( row, col ) + A ( row, col );
                    _concatArray.m_Vector[_concatArray.sub2ind ( row+r,col ) ] = A.m_Vector[A.sub2ind ( row,col ) ];
                }

            }
            return _concatArray;
        }
        else
        {
            std::cout << "Matrix ( concat ) Illegal attempt to "
            << "concatenate dissimmilar arrays!" << std::endl;
            std::cout << this->m_nRows << "x" << this->m_nCols << " - "
            << A.m_nRows << "x" << A.m_nCols << std::endl;
            return A;
        }
    }

    template < typename T >
    Matrix < T > &Matrix < T >::operator+= ( const Matrix < T > &A )
    {
        unsigned int r = this->m_nRows;
        unsigned int c = this->m_nCols;
        // check for same size
        if ( ( r == A.m_nRows ) && ( c == A.m_nCols ) )
        {
            Matrix < T > sum ( r, c );
            for ( unsigned int row = 1; row <= r; row++ )
            {
                for ( unsigned int col = 1; col <= c; col++ )
                {
                    //( ( *this ) ( row, col ) ) += A ( row, col );
                    this->m_Vector[this->sub2ind ( row,col ) ] += A.m_Vector[ A.sub2ind ( row,col ) ];
                }
            }
        }
        else
        {
            std::cout << "Matrix ( (+=) ) Illegal attempt to "
            << "add dissimmilar arrays!" << std::endl;
            std::cout << this->m_nRows << "x" << this->m_nCols << " - "
            << A.m_nRows << "x" << A.m_nCols << std::endl;
        }
        return *this;
    }

    template < typename T >
    Matrix < T > Matrix < T >::operator- ( const Matrix < T >
                                           &A ) const
    {
        unsigned int r = this->m_nRows;
        unsigned int c = this->m_nCols;
        // check for same size
        if ( ( r == A.m_nRows ) && ( c == A.m_nCols ) )
        {
            Matrix < T > sum ( r, c );
            for ( unsigned int row = 1; row <= r; ++row )
            {
                for ( unsigned int col = 1; col <= c; ++col )
                {
                    sum.m_Vector[ this->sub2ind ( row,col ) ] = this->m_Vector[ this->sub2ind ( row,col ) ] - A.m_Vector[ this->sub2ind ( row,col ) ];
                }
            }
            return sum;
        }
        else
        {
            std::cout << m_nRows << "x" << m_nCols << " - "
            << A.m_nRows << "x" << A.m_nCols << std::endl;
            throw std::runtime_error (
                "Illegal attempt to subtract dissimmilar arrays!"
            );
        }
    }

    // =================================== OVERLOADED MATRIX/VECTOR MULTIPLIES

    template < typename T >
    Matrix < T > Matrix < T >::multVector3x3 ( const Matrix < T > &RHM ) const
    {
        // Check For Dimensions
        if ( this->m_nCols == RHM.m_nRows )
        {
            Matrix <T> _R ( this->m_nRows, RHM.m_nCols );
            /*
            _R ( 1,1 ) = ( *this ) ( 1,1 ) * RHM ( 1,1 ) + ( *this ) ( 1,2 ) * RHM ( 2,1 ) + ( *this ) ( 1,3 ) * RHM ( 3,1 );
            _R ( 2,1 ) = ( *this ) ( 2,1 ) * RHM ( 1,1 ) + ( *this ) ( 2,2 ) * RHM ( 2,1 ) + ( *this ) ( 2,3 ) * RHM ( 3,1 );
            _R ( 3,1 ) = ( *this ) ( 3,1 ) * RHM ( 1,1 ) + ( *this ) ( 3,2 ) * RHM ( 2,1 ) + ( *this ) ( 3,3 ) * RHM ( 3,1 );
            */
            /// column major
            _R.m_Vector[0] = this->m_Vector[0] * RHM.m_Vector[0] + this->m_Vector[3] * RHM.m_Vector[1] + this->m_Vector[6] * RHM.m_Vector[2];
            _R.m_Vector[1] = this->m_Vector[1] * RHM.m_Vector[0] + this->m_Vector[4] * RHM.m_Vector[1] + this->m_Vector[7] * RHM.m_Vector[2];
            _R.m_Vector[2] = this->m_Vector[2] * RHM.m_Vector[0] + this->m_Vector[5] * RHM.m_Vector[1] + this->m_Vector[8] * RHM.m_Vector[2];
            return _R;
        }
        else
        {
            std::cout << m_nRows << "x" << m_nCols
            << " * " << RHM.m_nRows << "x" << RHM.m_nCols
            << std::endl;

            // can't multiply these
            throw std::runtime_error (
                "Illegal attempt to multiply non-conforming arrays!"
            );
        }
    }

    template < typename T >
    template< typename U >
    Vector3 <U> Matrix < T >::multVector3x3 ( const Vector3 <U> &RHM ) const
    {
        Vector3<U> _vr;

        if ( RHM.isColumnVector() == true )
        {
            //perform M * v
            _vr[0] = this->m_Vector[0] * RHM[0] + this->m_Vector[3] * RHM[1] + this->m_Vector[6] * RHM[2];
            _vr[1] = this->m_Vector[1] * RHM[0] + this->m_Vector[4] * RHM[1] + this->m_Vector[7] * RHM[2];
            _vr[2] = this->m_Vector[2] * RHM[0] + this->m_Vector[5] * RHM[1] + this->m_Vector[8] * RHM[2];
        }
        else
        {
            //perform v * M
            _vr[0] = this->m_Vector[0] * RHM[0] + this->m_Vector[1] * RHM[1] + this->m_Vector[2] * RHM[2];
            _vr[1] = this->m_Vector[3] * RHM[0] + this->m_Vector[4] * RHM[1] + this->m_Vector[5] * RHM[2];
            _vr[2] = this->m_Vector[6] * RHM[0] + this->m_Vector[7] * RHM[1] + this->m_Vector[8] * RHM[2];
        }

        return _vr;

    }

	template < typename T >
    Matrix < T > Matrix < T >::multMatrix3x3 ( const Matrix < T > &RHM ) const
    {
        // Check For Dimensions
        if ( 3 == this->m_nCols
                && 3 == this->m_nRows
                && 3 == RHM.m_nCols
                && 3 == RHM.m_nRows
           )
        {
            Matrix <T> _R ( this->m_nRows, RHM.m_nCols );

            _R.m_Vector[0] =   RHM.m_Vector[0] * this->m_Vector[0]
                              + RHM.m_Vector[1] * this->m_Vector[3]
                              + RHM.m_Vector[2] * this->m_Vector[6];
            _R.m_Vector[1] =   RHM.m_Vector[0] * this->m_Vector[1]
                              + RHM.m_Vector[1] * this->m_Vector[4]
                              + RHM.m_Vector[2] * this->m_Vector[7];
            _R.m_Vector[2] =   RHM.m_Vector[0] * this->m_Vector[2]
                              + RHM.m_Vector[1] * this->m_Vector[5]
                              + RHM.m_Vector[2] * this->m_Vector[8];

            _R.m_Vector[3] =   RHM.m_Vector[3] * this->m_Vector[0]
                              + RHM.m_Vector[4] * this->m_Vector[3]
                              + RHM.m_Vector[5] * this->m_Vector[6];
            _R.m_Vector[4] =   RHM.m_Vector[3] * this->m_Vector[1]
                              + RHM.m_Vector[4] * this->m_Vector[4]
                              + RHM.m_Vector[5] * this->m_Vector[7];
            _R.m_Vector[5] =   RHM.m_Vector[3] * this->m_Vector[2]
                              + RHM.m_Vector[4] * this->m_Vector[5]
                              + RHM.m_Vector[5] * this->m_Vector[8];

            _R.m_Vector[6] = RHM.m_Vector[6] * this->m_Vector[0]
                            + RHM.m_Vector[7] * this->m_Vector[3]
                            + RHM.m_Vector[8] * this->m_Vector[6];
            _R.m_Vector[7] = RHM.m_Vector[6] * this->m_Vector[1]
                            + RHM.m_Vector[7] * this->m_Vector[4]
                            + RHM.m_Vector[8] * this->m_Vector[7];
            _R.m_Vector[8] = RHM.m_Vector[6] * this->m_Vector[2]
                            + RHM.m_Vector[7] * this->m_Vector[5]
                            + RHM.m_Vector[8] * this->m_Vector[8];
            return _R;
        }
        else
        {
            // can't multiply these
            std::cout << m_nRows << "x" << m_nCols
            << " * " << RHM.m_nRows << "x" << RHM.m_nCols
            << std::endl;
            throw std::runtime_error (
                "Illegal attempt to multiply non-conforming arrays!"
            );
        }
    }

    template < typename T >
    Matrix < T > Matrix < T >::multVector4x4 ( const Matrix < T > &RHM ) const
    {
        // Check For Dimensions
        if ( m_nCols == RHM.m_nRows )
        {
            Matrix <T> _vr ( m_nRows, RHM.m_nCols );
            _vr.m_Vector[0] =  RHM.m_Vector[0] * this->m_Vector[0]
                              + RHM.m_Vector[1] * this->m_Vector[4]
                              + RHM.m_Vector[2] * this->m_Vector[8]
                              + RHM.m_Vector[3] * this->m_Vector[12];

            _vr.m_Vector[1] =  RHM.m_Vector[0] * this->m_Vector[1]
                              + RHM.m_Vector[1] * this->m_Vector[5]
                              + RHM.m_Vector[2] * this->m_Vector[9]
                              + RHM.m_Vector[3] * this->m_Vector[13];

            _vr.m_Vector[2] =  RHM.m_Vector[0] * this->m_Vector[2]
                              + RHM.m_Vector[1] * this->m_Vector[6]
                              + RHM.m_Vector[2] * this->m_Vector[10]
                              + RHM.m_Vector[3] * this->m_Vector[14];

            _vr.m_Vector[3] =  RHM.m_Vector[0] * this->m_Vector[3]
                              + RHM.m_Vector[1] * this->m_Vector[7]
                              + RHM.m_Vector[2] * this->m_Vector[11]
                              + RHM.m_Vector[3] * this->m_Vector[15];
            return _vr;
        }
        else
        {
            // can't multiply these
            std::cout << m_nRows << "x" << m_nCols
            << " * " << RHM.m_nRows << "x" << RHM.m_nCols
            << std::endl;
            throw std::runtime_error (
                "Illegal attempt to multiply non-conforming arrays!"
            );
        }
    }


#ifndef WIN32
    template < typename T>
    template<typename U>
    Vector4<U> Matrix < T >::multVector4x4 ( const Vector4<U> &RHM ) const
    {
        Vector4<U> _vr;

        if ( RHM.isColumnVector() == true )
        {
            //perform M * v
            _vr[0] = this->m_Vector[0]  * RHM[0] + this->m_Vector[4]  * RHM[1] + this->m_Vector[8]  * RHM[2] + this->m_Vector[12]  * RHM[3];
            _vr[1] = this->m_Vector[1]  * RHM[0] + this->m_Vector[5]  * RHM[1] + this->m_Vector[9]  * RHM[2] + this->m_Vector[13]  * RHM[3];
            _vr[2] = this->m_Vector[2]  * RHM[0] + this->m_Vector[6]  * RHM[1] + this->m_Vector[10] * RHM[2] + this->m_Vector[14]  * RHM[3];
            _vr[3] = this->m_Vector[3]  * RHM[0] + this->m_Vector[7]  * RHM[1] + this->m_Vector[11] * RHM[2] + this->m_Vector[15]  * RHM[3];
        }
        else
        {
            //perform v * M
            _vr[0] = this->m_Vector[0]  * RHM[0] + this->m_Vector[1]  * RHM[1] + this->m_Vector[2]  * RHM[2] + this->m_Vector[3]  * RHM[3];
            _vr[1] = this->m_Vector[4]  * RHM[0] + this->m_Vector[5]  * RHM[1] + this->m_Vector[6]  * RHM[2] + this->m_Vector[7]  * RHM[3];
            _vr[2] = this->m_Vector[8]  * RHM[0] + this->m_Vector[9]  * RHM[1] + this->m_Vector[10] * RHM[2] + this->m_Vector[11] * RHM[3];
            _vr[3] = this->m_Vector[12] * RHM[0] + this->m_Vector[13] * RHM[1] + this->m_Vector[14] * RHM[2] + this->m_Vector[15] * RHM[3];
        }

        return _vr;
    }
#endif

    template < typename T >
    Matrix < T > Matrix < T >::multMatrix4x4 ( const Matrix < T > &RHM ) const
    {
        // Check For Dimensions
        if ( 4 == this->m_nCols
                && 4 == this->m_nRows
                && 4 == RHM.m_nCols
                && 4 == RHM.m_nRows
           )
        {
            Matrix <T> _R ( this->m_nRows, RHM.m_nCols );
            _R.m_Vector[0] = RHM.m_Vector[0] * this->m_Vector[0]
                            + RHM.m_Vector[1] * this->m_Vector[4]
                            + RHM.m_Vector[2] * this->m_Vector[8]
                            + RHM.m_Vector[3] * this->m_Vector[12];
            _R.m_Vector[1] = RHM.m_Vector[0] * this->m_Vector[1]
                            + RHM.m_Vector[1] * this->m_Vector[5]
                            + RHM.m_Vector[2] * this->m_Vector[9]
                            + RHM.m_Vector[3] * this->m_Vector[13];
            _R.m_Vector[2] = RHM.m_Vector[0] * this->m_Vector[2]
                            + RHM.m_Vector[1] * this->m_Vector[6]
                            + RHM.m_Vector[2] * this->m_Vector[10]
                            + RHM.m_Vector[3] * this->m_Vector[14];
            _R.m_Vector[3] = RHM.m_Vector[0] * this->m_Vector[3]
                            + RHM.m_Vector[1] * this->m_Vector[7]
                            + RHM.m_Vector[2] * this->m_Vector[11]
                            + RHM.m_Vector[3] * this->m_Vector[15];

            _R.m_Vector[4] = RHM.m_Vector[4] * this->m_Vector[0]
                            + RHM.m_Vector[5] * this->m_Vector[4]
                            + RHM.m_Vector[6] * this->m_Vector[8]
                            + RHM.m_Vector[7] * this->m_Vector[12];
            _R.m_Vector[5] = RHM.m_Vector[4] * this->m_Vector[1]
                            + RHM.m_Vector[5] * this->m_Vector[5]
                            + RHM.m_Vector[6] * this->m_Vector[9]
                            + RHM.m_Vector[7] * this->m_Vector[13];
            _R.m_Vector[6] = RHM.m_Vector[4] * this->m_Vector[2]
                            + RHM.m_Vector[5] * this->m_Vector[6]
                            + RHM.m_Vector[6] * this->m_Vector[10]
                            + RHM.m_Vector[7] * this->m_Vector[14];
            _R.m_Vector[7] = RHM.m_Vector[4] * this->m_Vector[3]
                            + RHM.m_Vector[5] * this->m_Vector[7]
                            + RHM.m_Vector[6] * this->m_Vector[11]
                            + RHM.m_Vector[7] * this->m_Vector[15];

            _R.m_Vector[8] = RHM.m_Vector[8] * this->m_Vector[0]
                            + RHM.m_Vector[9] * this->m_Vector[4]
                            + RHM.m_Vector[10] * this->m_Vector[8]
                            + RHM.m_Vector[11] * this->m_Vector[12];
            _R.m_Vector[9] = RHM.m_Vector[8] * this->m_Vector[1]
                            + RHM.m_Vector[9] * this->m_Vector[5]
                            + RHM.m_Vector[10] * this->m_Vector[9]
                            + RHM.m_Vector[11] * this->m_Vector[13];
            _R.m_Vector[10] = RHM.m_Vector[8] * this->m_Vector[2]
                             + RHM.m_Vector[9] * this->m_Vector[6]
                             + RHM.m_Vector[10] * this->m_Vector[10]
                             + RHM.m_Vector[11] * this->m_Vector[14];
            _R.m_Vector[11] = RHM.m_Vector[8] * this->m_Vector[3]
                             + RHM.m_Vector[9] * this->m_Vector[7]
                             + RHM.m_Vector[10] * this->m_Vector[11]
                             + RHM.m_Vector[11] * this->m_Vector[15];

            _R.m_Vector[12] = RHM.m_Vector[12] * this->m_Vector[0]
                             + RHM.m_Vector[13] * this->m_Vector[4]
                             + RHM.m_Vector[14] * this->m_Vector[8]
                             + RHM.m_Vector[15] * this->m_Vector[12];
            _R.m_Vector[13] = RHM.m_Vector[12] * this->m_Vector[1]
                             + RHM.m_Vector[13] * this->m_Vector[5]
                             + RHM.m_Vector[14] * this->m_Vector[9]
                             + RHM.m_Vector[15] * this->m_Vector[13];
            _R.m_Vector[14] = RHM.m_Vector[12] * this->m_Vector[2]
                             + RHM.m_Vector[13] * this->m_Vector[6]
                             + RHM.m_Vector[14] * this->m_Vector[10]
                             + RHM.m_Vector[15] * this->m_Vector[14];
            _R.m_Vector[15] = RHM.m_Vector[12] * this->m_Vector[3]
                             + RHM.m_Vector[13] * this->m_Vector[7]
                             + RHM.m_Vector[14] * this->m_Vector[11]
                             + RHM.m_Vector[15] * this->m_Vector[15];
            return _R;
        }
        else
        {
            // can't multiply these
            std::cout << m_nRows << "x" << m_nCols
            << " * " << RHM.m_nRows << "x" << RHM.m_nCols
            << std::endl;

            // can't multiply these
            throw std::runtime_error (
                "Illegal attempt to multiply non-conforming arrays!"
            );
        }
    }

    // ============================================ end OVERLOADED MATRIX/VECTOR MULTIPLIES
    template < typename T >
    Matrix < T > Matrix < T >::operator* ( const Matrix < T >
                                           &RHM ) const
    {
        // Check For Dimensions
        if ( this->m_nCols == RHM.m_nRows )
        {
            Matrix < T > R ( this->m_nRows, RHM.m_nCols );
            for ( unsigned int a = 1; a <= this->m_nRows; ++a )
            {
                for ( unsigned int b = 1; b <= RHM.m_nCols; ++b )
                {
                    T temp = 0;
                    for ( unsigned int i = 1; i <= this->m_nCols; ++i )
                    {
                        //temp += ( *this ) ( a, i ) * RHM ( i, b );
                        temp += this->m_Vector[ this->sub2ind ( a,i ) ] * RHM.m_Vector[ RHM.sub2ind ( i,b ) ];
                    }
                    //R ( a, b ) = temp;
                    R.m_Vector[ R.sub2ind ( a,b ) ] = temp;
                }
            }
            return R;
        }
        else
        {
            // can't multiply these
            std::cout << m_nRows << "x" << m_nCols
            << " * " << RHM.m_nRows << "x" << RHM.m_nCols
            << std::endl;

            // can't multiply these
            throw std::runtime_error (
                "Illegal attempt to multiply non-conforming arrays!"
            );
        }
    }

    /* scalar multiply */
    template < typename T >
    Matrix < T > Matrix < T >::operator* ( const T & scalar ) const
    {
        Matrix < T > R ( this->m_nRows, this->m_nCols );
        for ( unsigned int a = 1; a <= this->m_nRows; ++a )
        {
            for ( unsigned int b = 1; b <= this->m_nCols; ++b )
            {
                R.m_Vector[ R.sub2ind ( a,b ) ] = this->m_Vector[ this->sub2ind ( a,b ) ] * scalar;
            }
        }
        return R;
    }


    /* scalar divide */
    template < typename T >
    Matrix < T > Matrix < T >::operator/ ( const T & scalar ) const
    {
        Matrix < T > R ( this->m_nRows, this->m_nCols );
        for ( unsigned int a = 1; a <= this->m_nRows; ++a )
        {
            for ( unsigned int b = 1; b <= this->m_nCols; ++b )
            {
                //R ( a, b ) = ( *this ) ( a, b ) / scalar;
                R.m_Vector[ R.sub2ind ( a,b ) ] = this->m_Vector[ this->sub2ind ( a,b ) ] / scalar;
            }
        }
        return R;
    }

    /* matrix power */
    template < typename T >
    Matrix < T > Matrix < T >::operator^ ( const int &x ) const
    {
        if ( this->m_isSquare )
        {
            Matrix < T > _temp ( *this );
            if ( x != 0 )
            {
                for ( unsigned int a = 2;
                        a <= static_cast < unsigned int > ( std::abs ( x ) ); ++a )
                {
                    _temp = _temp * ( *this );
                }
                if ( x > 0 )
                {
                    return _temp;
                }
                else
                {
                    return _temp.inverse ();
                }
            }
            else
            {
                _temp.makeIdentity();
                return _temp;
            }
        }
        else
        {
            throw std::runtime_error (
                "Matrix Power only valid for square matricies!"
            );
        }
    }

    template < typename T >
    Matrix < T > Matrix < T >::expm ( const unsigned int &x,
                                      const T &t )
    {
        if ( this->m_isSquare )
        {
            Matrix < T > _temp ( this->m_nRows, this->m_nCols );
            Matrix < T > _temp2 ( this->m_nRows, this->m_nCols );
            T _factorial;
            for ( unsigned int a = 1;a <= x; ++a )
            {
                // compute factorial term
                _factorial = 1.0;
                for ( unsigned int k = 1; k < a; ++k ) {_factorial *= k;}

                _temp += ( ( ( *this ) ^ ( a - 1 ) ) * ( pow ( fabs ( t ), ( a - 1 ) ) / _factorial ) );
            }

            if ( t > 0.0 )
            {
                return _temp;
            }
            else
            {
                return _temp.inverse ();
            }
        }
        else
        {
            throw std::runtime_error (
                "Matrix Exponential only valid for square matricies!"
            );
        }
    }


    template < typename T >
    void Matrix < T >::operator() ( const unsigned int &r,
                                    const unsigned int &c,
                                    const T &u )
    {
        // Maps traditional i,j 1 based indexing to 1-D array
        if ( ( this->m_nCols >= c ) && ( this->m_nRows >= r ) )
        {
            this->m_Vector[this->m_nRows * ( c - 1 ) + r - 1] = u;
        }
        else
        {
            std::cerr << "This array is of size: " << m_nRows
            << " x " << m_nCols << std::endl
            << "Indicies passed in (i,j): "
            << r << "," << c << std::endl;

            throw std::out_of_range (
                "Attempt to assign value beyond array size!"
            );
        }
    }

    // B = this + A
    template < typename T >
    void Matrix < T >::operator= ( const Matrix < T > &A )
    {
        //if output is not of the same size
        if ( ( A.m_nRows != this->m_nRows ) || ( A.m_nCols != this->m_nCols ) )
        {
            this->resize ( A.m_nRows, A.m_nCols );
        }

        this->m_Vector = A.m_Vector;
    }

    template < typename T >
    Matrix < T > Matrix < T >::subMatrix ( const unsigned int &rowBegin,
                                           const unsigned int &colBegin,
                                           const unsigned int &rowEnd,
                                           const unsigned int &colEnd )
                                           {
                                               Matrix < T > SubMatrix ( rowEnd - rowBegin + 1, colEnd - colBegin + 1 );
                                               for ( unsigned int r ( 1 ); r <= SubMatrix.getNumRows(); ++r )
                                               {
                                                   for ( unsigned int c ( 1 ); c <= SubMatrix.getNumColumns(); ++c )
                                                   {
                                                       SubMatrix(r,c) = this->operator()( r+rowBegin-1, c+colBegin-1 );
                                                   }
                                               }
                                               return SubMatrix;
                                           }

    template < typename T >
    Matrix < T > Matrix < T >::getRow ( const unsigned int &r ) const
    {
        Matrix < T > RowMatrix ( 1, this->m_nCols );
        for ( unsigned int k = 1; k <= this->m_nCols; ++k )
        {
            RowMatrix.m_Vector[ RowMatrix.sub2ind ( 1,k ) ] = this->m_Vector[ this->sub2ind ( r,k ) ];
        }
        return RowMatrix;
    }

    template < typename T >
    void Matrix < T >::setRow ( const unsigned int &r,
                                const Matrix < T > &Row )
    {
        for ( unsigned int k = 1; k <= this->m_nCols; ++k )
        {
            this->m_Vector[this->sub2ind ( r,k ) ] = Row.m_Vector[ Row.sub2ind ( 1,k ) ];
        }
    }

    template < typename T >
    Matrix < T > Matrix < T >::getColumn ( const unsigned int &c )
    {
        Matrix < T > ColumnMatrix ( this->m_nRows, 1 );
        for ( unsigned int k = 1; k <= this->m_nRows; ++k )
        {
            ColumnMatrix.m_Vector[ ColumnMatrix.sub2ind ( k,1 ) ] = this->m_Vector[ this->sub2ind ( k,c ) ];
        }
        return ColumnMatrix;
    }

    template < typename T >
    void Matrix < T >::setColumn ( const unsigned int &c,
                                   const Matrix < T > &Col )
    {
        for ( unsigned int k = 1; k <= this->m_nRows; ++k )
        {
            this->m_Vector[this->sub2ind ( k,c ) ] = Col.m_Vector[ Col.sub2ind ( k,1 ) ];
        }
    }

    template < typename T >
    bool Matrix < T >::getIsSquare ( )
    {
        return m_isSquare;
    }

    template < typename T >
    void Matrix < T >::normalizeColumns ()
    {
        T _norm ( 0.0 ), _normSquared ( 0.0 );
        unsigned int idx;

        for ( unsigned int k ( 1 ); k <= this->m_nCols; ++k ) // for each column
        {
            // compute vector norm
            idx = this->sub2ind ( 1,k ); // starting m_Vector location
            _normSquared = 0.0;
            for ( unsigned int m ( 0 ); m < this->m_nRows; ++m ) // for each row
            {
                _normSquared += m_Vector[ idx+m ] * m_Vector[ idx+m ];
            }
            // have norm squared, now apply it to this row
            _norm = sqrt ( _normSquared );
            for ( unsigned int m ( 0 ); m < this->m_nRows; ++m )
            {
                m_Vector[ idx+m ] /= _norm;
            }
        }
    }

    template < typename T > Matrix < T > Matrix < T >::diagVector () const
    {
        if ( m_isSquare )
        {
            Matrix < T > _output ( m_nRows, 1 );
            for ( unsigned int k = 1; k <= this->m_nRows; ++k )
            {
                _output.m_Vector[ _output.sub2ind ( k,1 ) ] =
                    m_Vector[ sub2ind ( k,k ) ];
            }
            return _output;
        }
        else
        {
            std::cout << "Matrix (DiagVector): Array must be square! "
            << "Aborting!" << std::endl;
            std::cout << "Original Matrix: " << m_nRows << " x "
            << m_nCols << std::endl;
            abort ( );
        }
    }

    template < typename T >
    Matrix < T > Matrix < T >::perElementPower ( const T &x ) const
    {
        Matrix<T> _temp ( *this );
        for ( unsigned int i = 0; i < m_Vector.size(); ++i )
        {
            _temp.m_Vector[i] = std::pow ( _temp.m_Vector[i], x );
        }

        return _temp;
    }

    template < typename T >
    Matrix < T > Matrix < T >::perElementAdd ( const Matrix<T> &x ) const
    {
        Matrix<T> _temp ( *this );
        for ( unsigned int i = 0; i < m_Vector.size(); ++i )
        {
            _temp.m_Vector[i] += x.m_Vector[i];
        }

        return _temp;
    }

    template < typename T >
    Matrix < T > Matrix < T >::perElementSub ( const Matrix<T> &x ) const
    {
        Matrix<T> _temp ( *this );
        for ( unsigned int i = 0; i < m_Vector.size(); ++i )
        {
            _temp.m_Vector[i] -= x.m_Vector[i];
        }

        return _temp;
    }

    template < typename T >
    Matrix < T > Matrix < T >::perElementMul ( const Matrix<T> &x ) const
    {
        Matrix<T> _temp ( *this );
        for ( unsigned int i = 0; i < m_Vector.size(); ++i )
        {
            _temp.m_Vector[i] *= x.m_Vector[i];
        }

        return _temp;
    }

    template < typename T >
    Matrix < T > Matrix < T >::perElementDiv ( const Matrix<T> &x ) const
    {
        Matrix<T> _temp ( *this );
        for ( unsigned int i = 0; i < m_Vector.size(); ++i )
        {
            _temp.m_Vector[i] /= x.m_Vector[i];
        }

        return _temp;
    }

    template <typename T>
    void Matrix < T >::computeLUFactorization ( Matrix<T> & A, std::vector<int> & IPIV ) const
    {
        A = *this;
        int M ( this->m_nRows );
        int N ( this->m_nCols );
        int LDA ( M );
        IPIV.resize ( N );
        int INFO;
        xgetrf_ ( M,N,A.m_Vector[0],LDA,IPIV[0],INFO );
    }

    template <typename T>
    Matrix < T > Matrix < T >::inverse () const
    {
        /// compute the LU factorization of this matrix
        vm::Matrix<T> LU;
        std::vector<int> Pivots;
        this->computeLUFactorization ( LU,Pivots );

        std::vector<T> vec_work ( 1 );

        int      N ( this->m_nCols );
        int      LDA ( this->m_nRows );
        int      LWORK ( -1 );
        int      INFO ( 99 );

        this->xgetri_ ( N,LU.m_Vector[0],LDA,Pivots[0],vec_work[0],LWORK,INFO );

        LWORK = this->getLapackLWORK ( vec_work[0] );

        vec_work.resize ( LWORK );

        this->xgetri_ ( N,LU.m_Vector[0],LDA,Pivots[0],vec_work[0],LWORK,INFO );

        return LU;
    }

    template <typename T>
    T Matrix < T >::determinant () const
    {
        if ( this->m_isSquare )
        {
            /// compute the LU factorization of this matrix
            vm::Matrix<T> LU;
            std::vector<int> Pivots;
            this->computeLUFactorization ( LU,Pivots );
            T det ( 1.0 );
            for ( unsigned int k = 1; k <=LU.m_nCols; ++k )
                det *= LU.m_Vector[ LU.sub2ind ( k,k ) ];

            return det;
        }
        else
            throw std::runtime_error ( "Non-square Matrix!" );

    }

    // transpose
    template <typename T>
    template <typename U>
    Matrix<T> Matrix<T>::transposeOrHermitian ( const Matrix<T>& thisMatrix, const U& firstValue ) const
    {
        Matrix < T > A ( m_nCols, m_nRows );
        // Compute the transpose as a new Matrix
        for ( unsigned int row = 1; row <= m_nRows; ++row )
        {
            for ( unsigned int col = 1; col <= m_nCols; ++col )
            {
                //A ( col, row ) = ( *this ) ( row, col );
                A.m_Vector[A.sub2ind ( col,row ) ] =
                    m_Vector[ sub2ind ( row,col ) ];
            }
        }
        return A;
    }

    template <typename T>
    template <typename U>
    Matrix<T> Matrix<T>::transposeOrHermitian ( const Matrix<T>& thisMatrix, const std::complex<U>& firstValue ) const
    {
        Matrix < T > A ( m_nCols, m_nRows );
        // Compute the transpose as a new Matrix
        for ( unsigned int row = 1; row <= m_nRows; ++row )
        {
            for ( unsigned int col = 1; col <= m_nCols; ++col )
            {
                //A ( col, row ) = ( *this ) ( row, col );
                A.m_Vector[A.sub2ind ( col,row ) ] =
                    conj ( m_Vector[ sub2ind ( row,col ) ] );
            }
        }
        return A;
    }

/// Wrappers to handle different call syntax for real vs complex input

    template <typename T>
    template <typename U>
    void Matrix < T >::getEigenvectorsAndValues ( Matrix< std::complex<U> > & values, Matrix< std::complex<U> > & vectors ) const
    {
        this->getEigenvectorsAndValues ( values,vectors,this->m_Vector[0] );
    }


    // real call
    template <typename T>
    template <typename U>
    void Matrix < T >::getEigenvectorsAndValues ( Matrix< std::complex<U> > & values, Matrix< std::complex<U> > & vectors, const U& firstValue ) const
    {
        if ( this->m_isSquare )
        {
            // sizing
            values.resize ( this->m_nRows, this->m_nCols );
            vectors.resize ( this->m_nRows, this->m_nCols );

            // generate a copy of this matrix
            Matrix < T > AMATRIX ( *this ); // dimensions are (LDA,N)

            // storage of eigenvalues
            std::vector<U> values_real_part ( this->m_nCols );
            std::vector<U> values_imag_part ( this->m_nCols );
            std::vector<U> scale ( this->m_nCols );
            std::vector<U> _work ( 1 ); // replace w/ query
            std::vector<U> eigenvectorRealData ( this->m_Vector.size() );

            // prep for lapack call
            char BALANC ( 'B' );
            char JOBVL ( 'N' );
            char JOBVR ( 'V' );
            char SENSE ( 'N' );

            int N ( AMATRIX.m_nCols );
            int LDA ( AMATRIX.m_nRows );
            U* VL ( NULL );  // array
            int LDVL ( 1 );
            int LDVR ( vectors.m_nRows );
            int ILO ( 99 );
            int IHI ( 99 );
            U ABNRM ( 0.0 );
            U* RCONDE ( NULL );  // array
            U* RCONDV ( NULL );  // array
            int LWORK ( -1 );
            int* IWORK ( NULL );  // array
            int INFO ( 99 );


            // call LAPACK routine, query mode, determine optimal LWORK size
            xgeevx_ ( BALANC,JOBVL,JOBVR,SENSE,N,AMATRIX.m_Vector[0],LDA,values_real_part[0],
                      values_imag_part[0],VL[0],LDVL,eigenvectorRealData[0],LDVR,ILO,IHI,scale[0],
                      ABNRM,RCONDE[0],RCONDV[0],_work[0],LWORK,IWORK[0],INFO );

            // _work[0] now populated with optimum value
            LWORK = static_cast<unsigned int> ( _work[0] );
            _work.resize ( LWORK );

            // call LAPACK routine
            xgeevx_ ( BALANC,JOBVL,JOBVR,SENSE,N,AMATRIX.m_Vector[0],LDA,values_real_part[0],
                      values_imag_part[0],VL[0],LDVL,eigenvectorRealData[0],LDVR,ILO,IHI,scale[0],
                      ABNRM,RCONDE[0],RCONDV[0],_work[0],LWORK,IWORK[0],INFO );

            // remap
            unsigned int _ones_index ( 0 );
            unsigned int firstElement, nextFirstElement;
            const unsigned int & columnOffset ( vectors.m_nRows );

            for ( unsigned int k ( 0 ); k < values.m_nRows; ++k )
            {
                // increment the ones-based index
                _ones_index++;

                // set the eigenvalue along the diagonal (k,k)
#ifndef __GXX_EXPERIMENTAL_CXX0X__
                values ( _ones_index,_ones_index ).real() = values_real_part[k];
		values ( _ones_index,_ones_index ).imag() = values_imag_part[k];
#else
#if ( __GNUC__ > 4 || ( __GNUC__ >= 4 && __GNUC_MINOR__ >= 4 ) )
                values ( _ones_index,_ones_index ).real( values_real_part[k] );
                values ( _ones_index,_ones_index ).imag( values_imag_part[k] );
#else
                values ( _ones_index,_ones_index ).real() = values_real_part[k];
		values ( _ones_index,_ones_index ).imag() = values_imag_part[k];
#endif
#endif

                // compute the first elements of this eigenvector (corresponding to column k, k+1)
                firstElement     = k*vectors.m_nRows;
                nextFirstElement = ( k+1 ) *vectors.m_nRows;

                // check if this value and the next form a conjugate pair
                if ( ( k+1 ) < values.m_nRows )
                {
                    if ( ( ( values_real_part[k] - values_real_part[k+1] +
                             values_imag_part[k] + values_imag_part[k+1] ) == 0.0 ) && ( values_imag_part[k] != 0.0 ) )
                    {
                        /// CONJUGATE PAIR
                        for ( unsigned int p ( firstElement ); p < nextFirstElement; ++p )
                        {
                            // fix this column
#ifndef __GXX_EXPERIMENTAL_CXX0X__
                            vectors.m_Vector[p].real() = eigenvectorRealData[p];
                            vectors.m_Vector[p].imag() = eigenvectorRealData[p+columnOffset];

                            // fix next column
                            vectors.m_Vector[p+columnOffset].real() = eigenvectorRealData[p];
                            vectors.m_Vector[p+columnOffset].imag() = -eigenvectorRealData[p+columnOffset];
#else
#if ( __GNUC__ > 4 || ( __GNUC__ >= 4 && __GNUC_MINOR__ >= 4 ) )
                            vectors.m_Vector[p].real( eigenvectorRealData[p] );
                            vectors.m_Vector[p].imag( eigenvectorRealData[p+columnOffset] );

                            // fix next column
                            vectors.m_Vector[p+columnOffset].real( eigenvectorRealData[p] );
                            vectors.m_Vector[p+columnOffset].imag( -eigenvectorRealData[p+columnOffset] );
#else
                            vectors.m_Vector[p].real() = eigenvectorRealData[p];
                            vectors.m_Vector[p].imag() = eigenvectorRealData[p+columnOffset];

                            // fix next column
                            vectors.m_Vector[p+columnOffset].real() = eigenvectorRealData[p];
                            vectors.m_Vector[p+columnOffset].imag() = -eigenvectorRealData[p+columnOffset];
#endif
#endif
                        }
                    }
                    else
                    {
                        ///  NOT CONJUGATE PAIR, direct mapping
                        for ( unsigned int p ( firstElement ); p < nextFirstElement; ++p )
                        {
                            vectors.m_Vector[p] = eigenvectorRealData[p];
                            //vectors.m_Vector[p+columnOffset].real() = eigenvectorRealData[p];
                            vectors.m_Vector[p+columnOffset] = eigenvectorRealData[p+columnOffset];
                        }
                    }
                }

            }

        }
        else
        {
            // throw exception
            throw std::runtime_error (
                "getEigenvectorsAndValues(): input array was not square!" );
        }
    }

    // complex call
    template <typename T>
    template <typename U>
    void Matrix < T >::getEigenvectorsAndValues ( Matrix< std::complex<U> > & values, Matrix< std::complex<U> > & vectors, const std::complex<U>& firstValue ) const
    {
        if ( this->m_isSquare )
        {
            // sizing
            values.resize ( this->m_nRows, this->m_nCols );
            vectors.resize ( this->m_nRows, this->m_nCols );

            // generate a copy of this matrix
            Matrix < std::complex<U> > AMATRIX ( *this ); // dimensions are (LDA,N)

            // storage of eigenvalues
            std::vector<U> scale ( this->m_nCols );
            std::vector< std::complex<U> > _work ( 1 ); // replace w/ query
            std::vector<U> RWORK ( 2 * this->m_nCols );

            // prep for lapack call
            int ILO ( 99 );
            int IHI ( 99 );
            U ABNRM ( 0.0 );
            int LWORK ( -1 );
            int INFO ( 99 );

            // call LAPACK routine, query mode, determine optimal LWORK size
            complex_xgeevx_ (
                'B','N','V', 'N', static_cast<int> ( AMATRIX.m_nCols ), AMATRIX.m_Vector[0],
                static_cast<int> ( AMATRIX.m_nRows ), values.m_Vector[0], 0.0, 1, vectors.m_Vector[0], static_cast<int> ( vectors.m_nRows ), ILO,
                IHI, scale[0], ABNRM, static_cast<U> ( NULL ), static_cast<U> ( NULL ),
                _work[0], LWORK, RWORK[0], INFO );

            // _work[0] now populated with optimum value
            LWORK = getLapackLWORK ( _work[0] );

            _work.resize ( LWORK );

            // call LAPACK routine
            complex_xgeevx_ (
                'B','N','V', 'N', static_cast<int> ( AMATRIX.m_nCols ), AMATRIX.m_Vector[0],
                static_cast<int> ( AMATRIX.m_nRows ), values.m_Vector[0], 0.0, 1, vectors.m_Vector[0], static_cast<int> ( vectors.m_nRows ), ILO,
                IHI, scale[0], ABNRM, static_cast<U> ( NULL ), static_cast<U> ( NULL ),
                _work[0], LWORK, RWORK[0], INFO );

            // remap
            unsigned int _ones_index ( 1 );

            for ( unsigned int k ( 1 ); k < values.m_nRows; ++k )
            {
                // increment the ones-based index
                _ones_index++;

                // set the eigenvalue along the diagonal (k,k)
                values ( _ones_index,_ones_index ) = values.m_Vector[k];
                values.m_Vector[k] = 0.0;
            }

        }
        else
        {
            // throw exception
            throw std::runtime_error (
                "getEigenvectorsAndValues(): input array was not square!" );
        }
    }
}
#endif
