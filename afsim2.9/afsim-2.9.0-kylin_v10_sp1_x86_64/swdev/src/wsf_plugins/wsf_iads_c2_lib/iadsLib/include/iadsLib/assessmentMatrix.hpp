// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @class  assessmentMatrix
 *  @file   assessmentMatrix.hpp
 *  @author Kenneth R. Sewell III
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 */

#include <vector>
#include <iostream>
#include <memory>

#include <iadsLib/munkres.hpp>
#include <iadsLib/idRecord.hpp>
#include <iadsLib/assessmentRecord.hpp>
#include <stdint.h>

#ifndef IADSLIB_ASSESSMENTMATRIX_HPP
#define IADSLIB_ASSESSMENTMATRIX_HPP

namespace il
{
  class assessmentMatrix
  {
  public:
    assessmentMatrix(std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
    ~assessmentMatrix();

    // assignment and copy constructors
    assessmentMatrix &operator=(const assessmentMatrix &rhs);
    assessmentMatrix (const assessmentMatrix &from);

    void clear();

    void buildMatrix( const uint16_t &numRows, const uint16_t &numCols );

    void setAssessment( const uint16_t &row, const uint16_t &col,
			std::shared_ptr<assessmentRecord> assessment );
    void clearAssessment( const uint16_t &row, const uint16_t &col );

    void setValue( const uint16_t &row, const uint16_t &col,
		   const float &value );

    void optimize();

    void log(std::ostream &os) const;

    uint16_t getAssignedColumn( const uint16_t &row ) const;
    std::shared_ptr<assessmentRecord> getAssignment( const uint16_t &row );


  protected:
    // 2x2 array of assessments - we don't own the assessments so they will not be cloned
    // upon copying or assigning this class - a simple slice will occur.
    std::vector<std::vector<std::shared_ptr<assessmentRecord>>> m_Assessments;
    // this is owned by this class and will be cloned upon assignment/copying
    std::shared_ptr<munkres> m_Matrix;

  private:
     std::weak_ptr<logger::GlobalLogger> mGlobalLogger;
  };
}
#endif
