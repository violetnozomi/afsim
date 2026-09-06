// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*
**	Classification: UNCLASSIFIED.
*/

#include <iadsLib/munkres.hpp>
#include <algorithm>
#include <cfloat>


munkres::munkres( const uint16_t &numRows, const uint16_t &numCols,
		  const float &defaultValue ):
  m_Solution( false ),
  m_NumRows( numRows ),
  m_NumCols( numCols ),
  m_CoveredRows( numRows, false ),
  m_CoveredCols( numCols, false ),
  m_Matrix( m_NumRows * m_NumCols, defaultValue ),
  m_Starred( m_NumRows * m_NumCols, false ),
  m_Primed( m_NumRows * m_NumCols, false )
{
}

munkres::~munkres()
{
}

bool munkres::solve(std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger)
{
  std::fill( m_CoveredRows.begin(), m_CoveredRows.end(), false );
  std::fill( m_CoveredCols.begin(), m_CoveredCols.end(), false );
  std::fill( m_Starred.begin(), m_Starred.end(), false );
  std::fill( m_Primed.begin(), m_Primed.end(), false );

  uint32_t nextStep = 1;
  while( 0 != nextStep )
    {
      switch( nextStep )
	{
	case 1: nextStep = stepOne(); break;
	case 2: nextStep = stepTwo(); break;
	case 3: nextStep = stepThree(); break;
	case 4: nextStep = stepFour(aGlobalLogger); break;
	case 5: nextStep = stepFive(aGlobalLogger); break;
	case 6: nextStep = stepSix(); break;
	default:
	  HCL_ERROR_LOGGER(aGlobalLogger) << "Bad step!" << std::endl;
	  return false;
	};
    }

  return m_Solution;
}

void munkres::setCost(const uint16_t &row, const uint16_t &col,
		      const float &cost)
{
  m_Matrix[(row*m_NumCols)+col] = cost;
}

float &munkres::getCost(const uint16_t &row, const uint16_t &col)
{
  return m_Matrix[(row*m_NumCols)+col];
}

const float &munkres::getCost(const uint16_t &row, const uint16_t &col) const
{
  return m_Matrix[(row*m_NumCols)+col];
}

void munkres::log(std::ostream &os) const
{
  for( uint32_t r = 0; r < m_NumRows; ++r )
    {
      for( uint32_t c = 0; c < m_NumCols; ++c )
	{
	  os << std::setw(10) << std::fixed << m_Matrix[(r * m_NumCols)+c] << " ";
	}
      os << "\t";

      for( uint32_t c = 0; c < m_NumCols; ++c )
	{
	  if( isStarred(r,c) )
	    {
	      os << "*";
	    }
	  else if( isPrimed(r,c) )
	    {
	      os << "'";
	    }
	  else
	    {
	      os << "X";
	    }

	  if( m_CoveredRows[r] || m_CoveredCols[c] )
	    {
	      os << "C ";
	    }
	  else
	    {
	      os << "  ";
	    }
	}
      os << std::endl;
    }
  os << std::endl;
}

bool munkres::solved() const
{
  return m_Solution;
}

uint32_t munkres::stepOne()
{
  uint32_t rowOffset = 0;
  for( uint32_t r = 0; r < m_NumRows; ++r )
    {
      float minCost = *std::min_element( m_Matrix.begin()+rowOffset,
					 m_Matrix.begin()+rowOffset+m_NumCols );

      for( uint32_t c = 0; c < m_NumCols; ++c )
	{
	  m_Matrix[rowOffset+c] -= minCost;
	}
      rowOffset += m_NumCols;
    }

  return 2;
}

uint32_t munkres::stepTwo()
{
  uint32_t rowOffset = 0;
  for( uint16_t r = 0; r < m_NumRows; ++r )
    {
      for( uint16_t c = 0; c < m_NumCols; ++c )
	{
	  if( !isStarred(rowOffset+c)
	      && !isStarInRowCol( r, c )
	      && (m_Matrix[rowOffset+c] <= 0.0f) )
	    {
	      m_Starred[rowOffset+c] = true;
	    }
	}
      rowOffset += m_NumCols;
    }

  return 3;
}

uint32_t munkres::stepThree()
{
  uint32_t coveredCount = 0;
  uint16_t row=0;
  for( uint16_t col = 0; col < m_NumCols; ++col )
    {
      m_CoveredCols[col] = isStarInCol( row, col );
      if( m_CoveredCols[col] )
	{
	  ++coveredCount;
	}
    }

  if( std::min( m_NumRows, m_NumCols ) == coveredCount )
    {
      m_Solution = true;
      std::fill( m_CoveredRows.begin(), m_CoveredRows.end(), false );
      std::fill( m_CoveredCols.begin(), m_CoveredCols.end(), false );
      return 0;
    }

  return 4;
}

uint32_t munkres::stepFour(std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger)
{
  m_SmallestUncoveredValue = FLT_MAX;
  // Loop through all elements of matrix
  uint32_t rowOffset = 0;
  for( uint32_t r = 0; r < m_NumRows; ++r )
    {
      for( uint32_t c = 0; c < m_NumCols; ++c )
	{
	  // If this element's row/col is uncovered
	  if( (!m_CoveredRows[r]) && (!m_CoveredCols[c]) )
	    {
	      // Keep track of the smallest uncovered value
	      m_SmallestUncoveredValue =
		std::min( m_SmallestUncoveredValue,
			  m_Matrix[rowOffset+c] );

	      // If this uncovered element is a zero
	      if( m_Matrix[rowOffset+c] <= 0.0f )
		{
		  setPrimed( r, c, true );

		  // If there is no starred zero in this row, go to step 5
		  uint16_t starredCol = 0;
		  if( !isStarInRow(r, starredCol) )
		    {
		      m_LastPrimeRow = r;
		      m_LastPrimeCol = c;
		      return 5;
		    }
		  else // Otherwise,
		    {
		      // cover this row...
		      m_CoveredRows[r] = true;
		      // ...and uncover column with starred zero
		      m_CoveredCols[starredCol] = false;
		    }
		}
	    }
	}
      rowOffset += m_NumCols;
    }

#if 0
  HCL_TRACE_LOGGER(aGlobalLogger) << "Smallest uncovered value: " << m_SmallestUncoveredValue << std::endl;
#endif
  return 6;
}

uint32_t munkres::stepFive(std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger)
{
  std::vector<uint16_t> visitedStarRows;
  std::vector<uint16_t> visitedStarCols;
  std::vector<uint16_t> visitedPrimeRows;
  std::vector<uint16_t> visitedPrimeCols;

  visitedPrimeRows.push_back( m_LastPrimeRow );
  visitedPrimeCols.push_back( m_LastPrimeCol );

  uint16_t row = m_LastPrimeRow;
  uint16_t col = m_LastPrimeCol;
  while( isStarInCol( row, col ) )
    {
      visitedStarRows.push_back( row );
      visitedStarCols.push_back( col );

      if( isPrimeInRow( row, col ) )
	{
	  visitedPrimeRows.push_back( row );
	  visitedPrimeCols.push_back( col );
	}
      else
	{
	  HCL_ERROR_LOGGER(aGlobalLogger) << "Error!  Should always be at least one primed zero." << std::endl;
	}
    }

  for( unsigned int i = 0; i < visitedStarRows.size(); ++i )
    {
      // Unstar starred zeros in the sequence.
      setStarred( visitedStarRows[i], visitedStarCols[i], false );
#if 0
      HCL_TRACE_LOGGER(aGlobalLogger) << "Unstarring " << visitedStarRows[i] << ", "
		<< visitedStarCols[i] << std::endl;
#endif
    }

  for( unsigned int i = 0; i < visitedPrimeRows.size(); ++i )
    {
      // Star primed zeros in the sequence.
      setStarred( visitedPrimeRows[i], visitedPrimeCols[i], true );
#if 0
      HCL_TRACE_LOGGER(aGlobalLogger) << "Starring " << visitedPrimeRows[i] << ", "
		<< visitedPrimeCols[i] << std::endl;
#endif
    }

  // Erase all primes
  std::fill( m_Primed.begin(), m_Primed.end(), false );
  std::fill( m_CoveredRows.begin(), m_CoveredRows.end(), false );
  std::fill( m_CoveredCols.begin(), m_CoveredCols.end(), false );

  return 3;
}

uint32_t munkres::stepSix()
{
  uint32_t offset = 0;
  for( uint32_t r = 0; r < m_NumRows; ++r )
    {
      if(m_CoveredRows[r])
	{
	  for( uint32_t c = 0; c < m_NumCols; ++c )
	    {
	      m_Matrix[offset+c] += m_SmallestUncoveredValue;
	    }
	}
      offset += m_NumCols;
    }

  for( uint32_t c = 0; c < m_NumCols; ++c )
    {
      offset = c;
      if(!m_CoveredCols[c])
	{
	  for( uint32_t r = 0; r < m_NumRows; ++r )
	    {
	      m_Matrix[offset] -= m_SmallestUncoveredValue;
	      offset += m_NumCols;
	    }
	}
    }

  return 4;
}

bool munkres::isStarInRow( const uint16_t &row, uint16_t &col ) const
{
  uint32_t rowOffset = row * m_NumCols;
  for( col = 0; col < m_NumCols; ++col )
    {
      if( m_Starred[rowOffset+col] )
	{
	  return true;
	}
    }

  return false;
}

bool munkres::isStarInRow( const uint16_t &row ) const
{
  uint32_t rowOffset = row * m_NumCols;
  for( uint32_t col = 0; col < m_NumCols; ++col )
    {
      if( m_Starred[rowOffset+col] )
	{
	  return true;
	}
    }

  return false;
}

bool munkres::isStarInCol( uint16_t &row, const uint16_t &col ) const
{
  uint32_t offset = col;
  for( row = 0; row < m_NumRows; ++row )
    {
      if( m_Starred[offset] )
	{
	  return true;
	}
      offset += m_NumCols;
    }

  return false;
}

bool munkres::isStarInRowCol( const uint16_t &row, const uint16_t &col ) const
{
  uint16_t r=0;
  return (isStarInRow( row ) || isStarInCol( r, col ));
}


bool munkres::isPrimeInRow( const uint16_t &row, uint16_t &col ) const
{
  for( col = 0; col < m_NumCols; ++col )
    {
      if( isPrimed(row, col) )
	{
	  return true;
	}
    }

  return false;
}

bool munkres::isPrimeInCol( uint16_t &row, const uint16_t &col ) const
{
  for( row = 0; row < m_NumRows; ++row )
    {
      if( isPrimed(row, col) )
	{
	  return true;
	}
    }

  return false;
}

bool munkres::isStarred( const uint16_t &row, const uint16_t &col ) const
{
  return m_Starred[(row * m_NumCols)+col];
}

bool munkres::isStarred( const uint32_t &index ) const
{
  return m_Starred[index];
}

bool munkres::isPrimed( const uint16_t &row, const uint16_t &col ) const
{
  return m_Primed[(row * m_NumCols)+col];
}

void munkres::setStarred( const uint16_t &row, const uint16_t &col,
			  const bool &value )
{
  m_Starred[(row * m_NumCols)+col] = value;
}

void munkres::setPrimed( const uint16_t &row, const uint16_t &col,
			 const bool &value )
{
  m_Primed[(row * m_NumCols)+col] = value;
}

