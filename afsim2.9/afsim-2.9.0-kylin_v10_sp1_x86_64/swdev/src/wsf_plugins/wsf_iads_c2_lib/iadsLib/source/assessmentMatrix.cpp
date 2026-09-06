// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  \class  assessmentMatrix
 *  \file   assessmentMatrix.cpp
 *  \author Kenneth R. Sewell III \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004 \n
 *
 *  Classification: UNCLASSIFIED
 */

#include <logger.hpp>
#include <iadsLib/assessmentMatrix.hpp>

namespace il
{

assessmentMatrix::assessmentMatrix(std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
   : mGlobalLogger(aGlobalLogger)
{
}

assessmentMatrix::~assessmentMatrix()
{
}

assessmentMatrix &assessmentMatrix::operator=(const assessmentMatrix &rhs)
{
   if (this != &rhs)
   {
      clear();

      m_Assessments = rhs.m_Assessments;

      if (rhs.m_Matrix)
         m_Matrix = std::shared_ptr<munkres>(new munkres(*rhs.m_Matrix));
   }

   return *this;
}


assessmentMatrix::assessmentMatrix (const assessmentMatrix &from)
   : m_Assessments (from.m_Assessments)
{
   if (from.m_Matrix)
      m_Matrix = std::shared_ptr<munkres>(new munkres(*from.m_Matrix));
   else
      m_Matrix.reset();
}

void assessmentMatrix::clear()
{
  m_Assessments.clear();
  m_Matrix.reset();
}

void assessmentMatrix::buildMatrix( const uint16_t &numRows,
				    const uint16_t &numCols )
{
  m_Assessments.resize( numRows,
			std::vector<std::shared_ptr<assessmentRecord> >
			(numCols)
			);
  m_Matrix = std::shared_ptr<munkres>(new munkres(numRows,
						    numCols,
						    FLT_MAX));
}

void assessmentMatrix::setAssessment( const uint16_t &row,
				      const uint16_t &col,
				      std::shared_ptr<assessmentRecord>
				      assessment )
{
  if( !m_Matrix )
    {
      HCL_ERROR_LOGGER(mGlobalLogger) << "Assessment table has not been allocated!" << std::endl;
      return;
    }

  m_Assessments[row][col] = assessment;
}

void assessmentMatrix::clearAssessment( const uint16_t &row,
					const uint16_t &col )
{
  if( m_Matrix )
    {
      m_Assessments[row][col].reset();
    }
}

void assessmentMatrix::setValue( const uint16_t &row,
				 const uint16_t &col,
				 const float &value )
{
  if( !m_Matrix )
    {
      HCL_ERROR_LOGGER(mGlobalLogger) << "Assessment table has not been allocated!" << std::endl;
      return;
    }

  m_Matrix->setCost( row, col, value );
}

void assessmentMatrix::optimize()
{
  if( !m_Matrix )
    {
      HCL_ERROR_LOGGER(mGlobalLogger) << "Assessment table has not been allocated!" << std::endl;
      return;
    }

  m_Matrix->solve(mGlobalLogger);
}

void assessmentMatrix::log(std::ostream &os) const
{
  if( m_Matrix )
    {
      m_Matrix->log(os);
    }
}

uint16_t assessmentMatrix::
getAssignedColumn( const uint16_t &row ) const
{
  uint16_t col = 0;
  if( m_Matrix )
    {
      if( m_Matrix->isStarInRow( row, col ) )
	{
	  return col;
	}
    }
  else
    {
      HCL_ERROR_LOGGER(mGlobalLogger) << "Assessment table has not been allocated!" << std::endl;
    }

  return col;
}

std::shared_ptr<assessmentRecord> assessmentMatrix::
getAssignment( const uint16_t &row )
{
  if( m_Matrix )
    {
      unsigned short col;
      if( m_Matrix->isStarInRow( row, col ) )
	{
	  return m_Assessments[row][col];
	}
    }
  else
    {
      HCL_ERROR_LOGGER(mGlobalLogger) << "Assessment table has not been allocated!" << std::endl;
    }

  return std::shared_ptr<assessmentRecord>();
}

} // namespace il
