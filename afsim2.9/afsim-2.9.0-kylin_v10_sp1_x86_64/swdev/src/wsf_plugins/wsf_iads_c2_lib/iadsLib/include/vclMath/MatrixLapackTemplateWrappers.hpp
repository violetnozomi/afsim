// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file MatrixLapackTemplateWrappers.hpp
 *  @author Keith Sawmiller, Booz Allen Hamilton \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3010 (Sawmiller)
 *  @brief LAPACK Interface to vclMath Matrix\n
 **  Classification: UNCLASSIFIED
 */

 #ifndef VCLMATH_LAPACKWRAPPERS_HPP
#define VCLMATH_LAPACKWRAPPERS_HPP 1

    /// Matrix Inverse
    template <>
    void Matrix < double >::xgetri_ ( int& N, double& A, int& LDA, int& Pivots, double& vec_work, int& LWORK, int& INFO ) const
    {
        dgetri_ ( N,A,LDA,Pivots,vec_work,LWORK,INFO );
    }

    template <>
    void Matrix < float >::xgetri_ ( int& N, float& A, int& LDA, int& Pivots, float& vec_work, int& LWORK, int& INFO ) const
    {
        sgetri_ ( N,A,LDA,Pivots,vec_work,LWORK,INFO );
    }

    template <>
    void Matrix < std::complex<double> >::xgetri_ ( int& N, std::complex<double>& A, int& LDA, int& Pivots, std::complex<double>& vec_work, int& LWORK, int& INFO ) const
    {
        zgetri_ ( N,A,LDA,Pivots,vec_work,LWORK,INFO );
    }

    template <>
    void Matrix < std::complex<float> >::xgetri_ ( int& N, std::complex<float>& A, int& LDA, int& Pivots, std::complex<float>& vec_work, int& LWORK, int& INFO ) const
    {
        cgetri_ ( N,A,LDA,Pivots,vec_work,LWORK,INFO );
    }

    /// Matrix LU Factorization
    template <>
    void Matrix < double >::xgetrf_ ( int& M, int& N, double& A, int& LDA, int& IPIV, int& INFO ) const
    {
        dgetrf_ ( M,N,A,LDA,IPIV,INFO );
    }

    template <>
    void Matrix < float >::xgetrf_ ( int& M, int& N, float& A, int& LDA, int& IPIV, int& INFO ) const
    {
        sgetrf_ ( M,N,A,LDA,IPIV,INFO );
    }

    template <>
    void Matrix < std::complex<double> >::xgetrf_ ( int& M, int& N, std::complex<double>& A, int& LDA, int& IPIV, int& INFO ) const
    {
        zgetrf_ ( M,N,A,LDA,IPIV,INFO );
    }

    template <>
    void Matrix < std::complex<float> >::xgetrf_ ( int& M, int& N, std::complex<float>& A, int& LDA, int& IPIV, int& INFO ) const
    {
        cgetrf_ ( M,N,A,LDA,IPIV,INFO );
    }

    /// Matrix Eigendecomposition
    template <>
    void Matrix < double >::xgeevx_ (
        char& BALANC,
        char& JOBVL,
        char& JOBVR,
        char& SENSE,
        int& N,
        double& A,  // array
        int& LDA,
        double& WR, // array
        double& WI, // array
        double& VL,  // array
        int& LDVL,
        double& VR,  // array
        int& LDVR,
        int& ILO,
        int& IHI,
        double& SCALE,  // array
        double& ABNRM,
        double& RCONDE,  // array
        double& RCONDV,  // array
        double& WORK,  // array
        int& LWORK,
        int& IWORK,  // array
        int& INFO ) const
    {
        dgeevx_ (
            BALANC,    // 1
            JOBVL,     // 2
            JOBVR,     // 3
            SENSE,     // 4
            N,         // 5
            A,          // 6 array
            LDA,       // 7
            WR,         // 8 array
            WI,         // 9 array
            VL,         // 10 array
            LDVL,      // 11
            VR,         // 12 array
            LDVR,      // 13
            ILO,       // 14
            IHI,       // 15
            SCALE,      // 16 array
            ABNRM,     // 17
            RCONDE,     // 18 array
            RCONDV,     // 19 array
            WORK,  // 20 array
            LWORK,     // 21
            IWORK,      // 22 array
            INFO );    // 23
    }

    template <>
    void Matrix < float >::xgeevx_ (
        char& BALANC,
        char& JOBVL,
        char& JOBVR,
        char& SENSE,
        int& N,
        float& A,  // array
        int& LDA,
        float& WR, // array
        float& WI, // array
        float& VL,  // array
        int& LDVL,
        float& VR,  // array
        int& LDVR,
        int& ILO,
        int& IHI,
        float& SCALE,  // array
        float& ABNRM,
        float& RCONDE,  // array
        float& RCONDV,  // array
        float& WORK,  // array
        int& LWORK,
        int& IWORK,  // array
        int& INFO ) const
    {
        sgeevx_ (
            BALANC,    // 1
            JOBVL,     // 2
            JOBVR,     // 3
            SENSE,     // 4
            N,         // 5
            A,          // 6 array
            LDA,       // 7
            WR,         // 8 array
            WI,         // 9 array
            VL,         // 10 array
            LDVL,      // 11
            VR,         // 12 array
            LDVR,      // 13
            ILO,       // 14
            IHI,       // 15
            SCALE,      // 16 array
            ABNRM,     // 17
            RCONDE,     // 18 array
            RCONDV,     // 19 array
            WORK,  // 20 array
            LWORK,     // 21
            IWORK,      // 22 array
            INFO );    // 23
    }

    template <>
    template <>
    void Matrix < std::complex<double> >::complex_xgeevx_ (
        const char& BALANC,
        const char& JOBVL,
        const char& JOBVR,
        const char& SENSE,
        const int& N,
        std::complex<double>& A,  // array
        const int& LDA,
        std::complex<double>& W, // array
        const std::complex<double>& VL,  // array
        const int& LDVL,
        std::complex<double>& VR,  // array
        const int& LDVR,
        int& ILO,
        int& IHI,
        double& SCALE,  // array
        double& ABNRM,
        const double& RCONDE,  // array
        const double& RCONDV,  // array
        std::complex<double>& WORK,  // array
        int& LWORK,
        double& RWORK,  // array
        int& INFO ) const
    {
        zgeevx_ (
            BALANC,    // 1
            JOBVL,     // 2
            JOBVR,     // 3
            SENSE,     // 4
            N,         // 5
            A,          // 6 array
            LDA,       // 7
            W,         // 8 array
            VL,         // 10 array
            LDVL,      // 11
            VR,         // 12 array
            LDVR,      // 13
            ILO,       // 14
            IHI,       // 15
            SCALE,      // 16 array
            ABNRM,     // 17
            RCONDE,     // 18 array
            RCONDV,     // 19 array
            WORK,  // 20 array
            LWORK,     // 21
            RWORK,      // 22 array
            INFO );    // 23
    }

    template <>
    template <>
    void Matrix < std::complex<float> >::complex_xgeevx_ (
        const char& BALANC,
        const char& JOBVL,
        const char& JOBVR,
        const char& SENSE,
        const int& N,
        std::complex<float>& A,  // array
        const int& LDA,
        std::complex<float>& W, // array
        const std::complex<float>& VL,  // array
        const int& LDVL,
        std::complex<float>& VR,  // array
        const int& LDVR,
        int& ILO,
        int& IHI,
        float& SCALE,  // array
        float& ABNRM,
        const float& RCONDE,  // array
        const float& RCONDV,  // array
        std::complex<float>& WORK,  // array
        int& LWORK,
        float& RWORK,  // array
        int& INFO ) const
    {
        cgeevx_ (
            BALANC,    // 1
            JOBVL,     // 2
            JOBVR,     // 3
            SENSE,     // 4
            N,         // 5
            A,          // 6 array
            LDA,       // 7
            W,         // 8 array
            VL,         // 10 array
            LDVL,      // 11
            VR,         // 12 array
            LDVR,      // 13
            ILO,       // 14
            IHI,       // 15
            SCALE,      // 16 array
            ABNRM,     // 17
            RCONDE,     // 18 array
            RCONDV,     // 19 array
            WORK,  // 20 array
            LWORK,     // 21
            RWORK,      // 22 array
            INFO );    // 23
    }

    /// Matrix Singular Value Decomposition
    template <>
    template <>
    void Matrix < double >::xgesvd_ (
        char& JOBU,
        char& JOBVT,
        int& M,
        int& N,
        double& A, // array
        int& LDA,
        double& S, // array
        double& U, // array
        int& LDU,
        double& VT, // array
        int& LDVT,
        double& WORK, // array
        int& LWORK,
        int& INFO ) const
    {
        dgesvd_ (
            JOBU,
            JOBVT,
            M,
            N,
            A, // array
            LDA,
            S, // array
            U, // array
            LDU,
            VT, // array
            LDVT,
            WORK, // array
            LWORK,
            INFO );
    }

    template <>
    template <>
    void Matrix < float >::xgesvd_ (
        char& JOBU,
        char& JOBVT,
        int& M,
        int& N,
        float& A, // array
        int& LDA,
        float& S, // array
        float& U, // array
        int& LDU,
        float& VT, // array
        int& LDVT,
        float& WORK, // array
        int& LWORK,
        int& INFO ) const
    {
        sgesvd_ (
            JOBU,
            JOBVT,
            M,
            N,
            A, // array
            LDA,
            S, // array
            U, // array
            LDU,
            VT, // array
            LDVT,
            WORK, // array
            LWORK,
            INFO );
    }

    template <>
    template <>
    void Matrix < std::complex<double> >::xgesvd_ (
        char& JOBU,
        char& JOBVT,
        int& M,
        int& N,
        std::complex<double>& A, // array
        int& LDA,
        double& S, // array
        std::complex<double>& U, // array
        int& LDU,
        std::complex<double>& VT, // array
        int& LDVT,
        std::complex<double>& WORK, // array
        int& LWORK,
        int& INFO ) const
    {
        // since the complex call uses an additional RWORK array (dimensions of 5*min(rows,cols))
        // we'll create it here

        std::vector<double> RWORK;
        if ( this->m_nRows < this->m_nCols )
            RWORK.resize( 5 * this->m_nRows );
        else
            RWORK.resize( 5 * this->m_nCols );

        zgesvd_ (
            JOBU,
            JOBVT,
            M,
            N,
            A, // array
            LDA,
            S, // array
            U, // array
            LDU,
            VT, // array
            LDVT,
            WORK, // array
            LWORK,
            RWORK[0],
            INFO );
    }

    template <>
    template <>
    void Matrix < std::complex<float> >::xgesvd_ (
        char& JOBU,
        char& JOBVT,
        int& M,
        int& N,
        std::complex<float>& A, // array
        int& LDA,
        float& S, // array
        std::complex<float>& U, // array
        int& LDU,
        std::complex<float>& VT, // array
        int& LDVT,
        std::complex<float>& WORK, // array
        int& LWORK,
        int& INFO ) const
    {
        // since the complex call uses an additional RWORK array (dimensions of 5*min(rows,cols))
        // we'll create it here

        std::vector<float> RWORK;
        if ( this->m_nRows < this->m_nCols )
            RWORK.resize( 5 * this->m_nRows );
        else
            RWORK.resize( 5 * this->m_nCols );

        cgesvd_ (
            JOBU,
            JOBVT,
            M,
            N,
            A, // array
            LDA,
            S, // array
            U, // array
            LDU,
            VT, // array
            LDVT,
            WORK, // array
            LWORK,
            RWORK[0],
            INFO );
    }

    /// Wrappers to handle different call syntax for real vs complex input

    template <typename T>
    template <typename U>
    void Matrix < T >::getEigenvectorsAndValues ( Matrix< std::complex<U> > & values, Matrix< std::complex<U> > & vectors ) const
    {
        this->getEigenvectorsAndValues(values,vectors,this->m_Array[0]);
    }


    // real call
    template <typename T>
    template <typename U>
    void Matrix < T >::getEigenvectorsAndValues ( Matrix< std::complex<U> > & values, Matrix< std::complex<U> > & vectors, const U& firstValue) const
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
                    std::vector<U> eigenvectorRealData ( this->m_Array.size() );

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
                    xgeevx_ ( BALANC,JOBVL,JOBVR,SENSE,N,AMATRIX.m_Array[0],LDA,values_real_part[0],
                              values_imag_part[0],VL[0],LDVL,eigenvectorRealData[0],LDVR,ILO,IHI,scale[0],
                              ABNRM,RCONDE[0],RCONDV[0],_work[0],LWORK,IWORK[0],INFO );

                    // _work[0] now populated with optimum value
                    LWORK = static_cast<unsigned int> ( _work[0] );
                    _work.resize ( LWORK );

                    // call LAPACK routine
                    xgeevx_ ( BALANC,JOBVL,JOBVR,SENSE,N,AMATRIX.m_Array[0],LDA,values_real_part[0],
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
                        values ( _ones_index,_ones_index ).real() = values_real_part[k];
                        values ( _ones_index,_ones_index ).imag() = values_imag_part[k];

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
                                    vectors.m_Array[p].real() = eigenvectorRealData[p];
                                    vectors.m_Array[p].imag() = eigenvectorRealData[p+columnOffset];

                                    // fix next column
                                    vectors.m_Array[p+columnOffset].real() = eigenvectorRealData[p];
                                    vectors.m_Array[p+columnOffset].imag() = -eigenvectorRealData[p+columnOffset];
                                }
                            }
                            else
                            {
                                ///  NOT CONJUGATE PAIR, direct mapping
                                for ( unsigned int p ( firstElement ); p < nextFirstElement; ++p )
                                    vectors.m_Array[p] = eigenvectorRealData[p];
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
    void Matrix < T >::getEigenvectorsAndValues ( Matrix< std::complex<U> > & values, Matrix< std::complex<U> > & vectors, const std::complex<U>& firstValue) const
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
            std::vector<U> RWORK( 2.0 * this->m_nCols );

            // prep for lapack call
            int ILO ( 99 );
            int IHI ( 99 );
            U ABNRM ( 0.0 );
            int LWORK ( -1 );
            int INFO ( 99 );

            // call LAPACK routine, query mode, determine optimal LWORK size
            complex_xgeevx_ (
            'B','N','V', 'N', static_cast<int>(AMATRIX.m_nCols), AMATRIX.m_Array[0],
            static_cast<int>(AMATRIX.m_nRows), values.m_Array[0], NULL, 1, vectors.m_Array[0], static_cast<int>(vectors.m_nRows), ILO,
            IHI, scale[0], ABNRM, static_cast<U>(NULL), static_cast<U>(NULL),
            _work[0], LWORK, RWORK[0], INFO );

            // _work[0] now populated with optimum value
            LWORK = getLapackLWORK(_work[0]);

            _work.resize ( LWORK );

            // call LAPACK routine
            complex_xgeevx_ (
            'B','N','V', 'N', static_cast<int>(AMATRIX.m_nCols), AMATRIX.m_Array[0],
            static_cast<int>(AMATRIX.m_nRows), values.m_Array[0], NULL, 1, vectors.m_Array[0], static_cast<int>(vectors.m_nRows), ILO,
            IHI, scale[0], ABNRM, static_cast<U>(NULL), static_cast<U>(NULL),
            _work[0], LWORK, RWORK[0], INFO );

            // remap
            unsigned int _ones_index ( 1 );

            for ( unsigned int k ( 1 ); k < values.m_nRows; ++k )
            {
                // increment the ones-based index
                _ones_index++;

                // set the eigenvalue along the diagonal (k,k)
                values ( _ones_index,_ones_index ) = values.m_Array[k];
                values.m_Array[k] = 0.0;
            }

        }
        else
        {
            // throw exception
            throw std::runtime_error (
                "getEigenvectorsAndValues(): input array was not square!" );
        }
    }
#endif