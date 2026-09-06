// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file MatrixLapackInterface.hpp
 *  @author Keith Sawmiller, Booz Allen Hamilton \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3010 (Sawmiller)
 *  @brief LAPACK Interface to vclMath Matrix\n
 *  Classification: UNCLASSIFIED
 */

#ifndef VCLMATH_LAPACK_HPP
#define VCLMATH_LAPACK_HPP 1
/*
*  DGEEVX computes for an N-by-N real nonsymmetric matrix A, the
*  eigenvalues and, optionally, the left and/or right eigenvectors.
*/

extern "C" void dgeevx_ (
        const char& BALANC,
        const char& JOBVL,
        const char& JOBVR,
        const char& SENSE,
        const int& N,
        double& A,  // array
        const int& LDA,
        double& WR, // array
        double& WI, // array
        const double& VL,  // array
        const int& LDVL,
        double& VR,  // array
        const int& LDVR,
        int& ILO,
        int& IHI,
        double& SCALE,  // array
        double& ABNRM,
        double& RCONDE,  // array
        double& RCONDV,  // array
        double& WORK,  // array
        int& LWORK,
        int& IWORK,  // array
        int& INFO );

extern "C" void sgeevx_ (
        const char& BALANC,
        const char& JOBVL,
        const char& JOBVR,
        const char& SENSE,
        const int& N,
        float& A,  // array
        const int& LDA,
        float& WR, // array
        float& WI, // array
        const float& VL,  // array
        const int& LDVL,
        float& VR,  // array
        const int& LDVR,
        int& ILO,
        int& IHI,
        float& SCALE,  // array
        float& ABNRM,
        float& RCONDE,  // array
        float& RCONDV,  // array
        float& WORK,  // array
        int& LWORK,
        int& IWORK,  // array
        int& INFO );

extern "C" void zgeevx_(
        const char& BALANC,
        const char& JOBVL,
        const char& JOBVR,
        const char& SENSE,
        const int& N,
        std::complex<double>& A,
        const int& LDA,
        std::complex<double>& W,
        const std::complex<double>& VL,
        const int& LDVL,
        std::complex<double>& VR,
        const int& LDVR,
        int& ILO,
        int& IHI,
        double& SCALE,
        double& ABNRM,
        const double& RCONDE,
        const double& RCONDV,
        std::complex<double>& WORK,
        int& LWORK,
        double& RWORK,
        int& INFO );

extern "C" void cgeevx_(
        const char& BALANC,
        const char& JOBVL,
        const char& JOBVR,
        const char& SENSE,
        const int& N,
        std::complex<float>& A,
        const int& LDA,
        std::complex<float>& W,
        const std::complex<float>& VL,
        const int& LDVL,
        std::complex<float>& VR,
        const int& LDVR,
        int& ILO,
        int& IHI,
        float& SCALE,
        float& ABNRM,
        const float& RCONDE,
        const float& RCONDV,
        std::complex<float>& WORK,
        int& LWORK,
        float& RWORK,
        int& INFO );

/*
*  DGETRI computes the inverse of a matrix using the LU factorization
*  computed by DGETRF.
*
*  This method inverts U and then computes inv(A) by solving the system
*  inv(A)*L = inv(U) for inv(A).
*/

extern "C" void dgetri_ (
       const int&     N,
       double&  A,    // array
       int&     LDA,
       int&     IPIV, // array
       double&  WORK, // array
       int&     LWORK,
       int&     INFO );

extern "C" void sgetri_ (
       const int&     N,
       float&   A,    // array
       int&     LDA,
       int&     IPIV, // array
       float&   WORK, // array
       int&     LWORK,
       int&     INFO );

extern "C" void zgetri_ (
       const int&     N,
       std::complex<double>&   A,    // array
       const int&     LDA,
       int&     IPIV, // array
       std::complex<double>&   WORK, // array
       int&     LWORK,
       int&     INFO );

extern "C" void cgetri_ (
       const int&     N,
       std::complex<float>&   A,    // array
       const int&     LDA,
       int&     IPIV, // array
       std::complex<float>&   WORK, // array
       int&     LWORK,
       int&     INFO );

/*
*  DGETRF computes an LU factorization of a general M-by-N matrix A
*  using partial pivoting with row interchanges.
*/
extern "C" void dgetrf_ (
       const int& M,
       const int& N,
       double& A,
       const int& LDA,
       int& IPIV,
       int& INFO );

extern "C" void sgetrf_ (
       const int& M,
       const int& N,
       float& A,
       const int& LDA,
       int& IPIV,
       int& INFO );

extern "C" void zgetrf_ (
       const int& M,
       const int& N,
       std::complex<double>& A,
       const int& LDA,
       int& IPIV,
       int& INFO );

extern "C" void cgetrf_ (
       const int& M,
       const int& N,
       std::complex<float>& A,
       const int& LDA,
       int& IPIV,
       int& INFO );

/*
*  DGESVD computes the singular value decomposition (SVD) of a real
*  M-by-N matrix A, optionally computing the left and/or right singular
*  vectors. The SVD is written
*
*       A = U * SIGMA * transpose(V)
*
*  where SIGMA is an M-by-N matrix which is zero except for its
*  min(m,n) diagonal elements, U is an M-by-M orthogonal matrix, and
*  V is an N-by-N orthogonal matrix.  The diagonal elements of SIGMA
*  are the singular values of A; they are real and non-negative, and
*  are returned in descending order.  The first min(m,n) columns of
*  U and V are the left and right singular vectors of A.
*
*  Note that the routine returns V**T, not V.
*/
extern "C" void dgesvd_ (
        const char& JOBU,
        const char& JOBVT,
        int& M,
        int& N,
        double& A, // array
        const int& LDA,
        double& S, // array
        double& U, // array
        int& LDU,
        double& VT, // array
        int& LDVT,
        double& WORK, // array
        int& LWORK,
        int& INFO );

extern "C" void sgesvd_ (
        const char& JOBU,
        const char& JOBVT,
        int& M,
        int& N,
        float& A, // array
        const int& LDA,
        float& S, // array
        float& U, // array
        int& LDU,
        float& VT, // array
        int& LDVT,
        float& WORK, // array
        int& LWORK,
        int& INFO );

extern "C" void zgesvd_ (
        const char& JOBU,
        const char& JOBVT,
        int& M,
        int& N,
        std::complex<double>& A, // array
        const int& LDA,
        double& S, // array
        std::complex<double>& U, // array
        int& LDU,
        std::complex<double>& VT, // array
        int& LDVT,
        std::complex<double>& WORK, // array
        int& LWORK,
        double& RWORK,
        int& INFO );

extern "C" void cgesvd_ (
        const char& JOBU,
        const char& JOBVT,
        int& M,
        int& N,
        std::complex<float>& A, // array
        const int& LDA,
        float& S, // array
        std::complex<float>& U, // array
        int& LDU,
        std::complex<float>& VT, // array
        int& LDVT,
        std::complex<float>& WORK, // array
        int& LWORK,
        float& RWORK,
        int& INFO );


#endif
