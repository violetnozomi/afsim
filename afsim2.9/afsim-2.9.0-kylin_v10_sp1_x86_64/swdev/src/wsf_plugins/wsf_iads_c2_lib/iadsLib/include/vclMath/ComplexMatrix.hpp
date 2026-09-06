// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file ComplexMatrix.hpp
 *  @author Keith Sawmiller, Booz Allen Hamilton \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3010 (Sawmiller)
 *  @brief Complex Matrix class \n
 **  Classification: UNCLASSIFIED
 */

 #ifndef VCLMATH_COMPLEXMATRIX_HPP
#define VCLMATH_COMPLEXMATRIX_HPP 1

#define MACRO_COUT std::cout << __FILE__ << "@" << __LINE__ << "\t"

#include <iostream>
#include <iomanip>
#include <complex>
#include <vclMath/Matrix.hpp>


/**
 *  \brief Namespace to encapsulate vclMath routines
 */
namespace vm
{
    template < typename T > class ComplexMatrix : public Matrix < std::complex < T > >
    {

        public:
            ComplexMatrix ( ) : Matrix< std::complex<T> > ()
            {
            }

            ComplexMatrix ( const unsigned int r, const unsigned int c ) :
                    Matrix< std::complex<T> > ( r,c )
            {
            }

            ComplexMatrix ( const Matrix< std::complex<T> > & inMatrix ) :
                    Matrix< std::complex<T> > ( inMatrix )
            {
            }

            ComplexMatrix < T > eigenvalues ()
            {
            }

            ComplexMatrix < T > eigenvectors ()
            {
            }

            void normalizeVectors()
            {
            }

            ComplexMatrix < T > singularValueDecomposition ( ComplexMatrix<T> *matrixU, ComplexMatrix<T> *matrixS, ComplexMatrix<T> *matrixV )
            {
            }

            ComplexMatrix < T > conjugateTranspose()
            {
            }

            void fillWithUniformRandom ( const T & scalar )
            {
                T scalar2 = scalar * static_cast < T > ( 2 );

                for ( unsigned int k ( 0 ); k < this->m_Vector.size(); ++k )
                {
                    this->m_Vector[k] = std::complex<T> (
                                           scalar2 * ( static_cast < T > ( rand () ) / static_cast < T > ( RAND_MAX ) - 0.5 ),
                                           scalar2 * ( static_cast < T > ( rand () ) / static_cast < T > ( RAND_MAX ) - 0.5 )
                                       );
                }
            }
            void displayMatlabForm ( const std::string &text ) const
            {
                std::cout << text << std::endl;
                std::cout << std::endl << "[";
                for ( unsigned int row = 1; row <= this->getNumRows (); ++row )
                {
                    for ( unsigned int col = 1; col <= this->getNumColumns (); ++col )
                    {
                        std::cout << std::setw ( this->m_displayPrecision )
                        << "complex" << this->operator() ( row, col );
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

            ComplexMatrix < T > transpose () const
            {
                ComplexMatrix < T > A ( this->m_nCols, this->m_nRows );
                // Compute the transpose as a new Matrix
                for ( unsigned int row = 1; row <= this->m_nRows; ++row )
                    for ( unsigned int col = 1; col <= this->m_nCols; ++col )
                        A.m_Vector[ A.sub2ind ( col,row ) ] = std::conj(this->m_Vector[ this->sub2ind ( row,col ) ]);

                return A;
            }

    };

}
#endif
