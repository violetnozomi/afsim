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
#include <iostream>
#include <cstdlib>

#include <munkres.hpp>

int main( int argc, char **argv )
{
  if( 3 != argc )
    {
      std::cout << "Usage: " << argv[0] << " <numRows> <numCols>" << std::endl;
      return 0;
    }
  const uint16_t maxRows = std::atoi(argv[1]);
  const uint16_t maxCols = std::atoi(argv[2]);

  munkres m( maxRows, maxCols );
  for( uint16_t r = 0; r < maxRows; ++r )
    {
      for( uint16_t c = 0; c < maxCols; ++c )
	{
	  m.getCost( r, c ) = std::rand() % 100;
	}
    }

  bool verbose = false;

  float n = std::max(maxCols, maxRows);
  float solutionTime = (n*n*n)/640000000.0;

  std::cout << "Estimated time until solution: " << solutionTime << " seconds"
	    << std::endl;

  if( verbose )
    {
      std::cout << "Initial: " << std::endl;
      m.print();
    }

  uint32_t nextStep = 1;

  while( 0 != nextStep )
    {
      switch( nextStep )
	{
	case 1: nextStep = m.stepOne();
	  if( verbose )
	    {
	      std::cout << "After step 1: " << std::endl;
	      m.print();
	    }
	  break;
	case 2: nextStep = m.stepTwo();
	  if( verbose )
	    {
	      std::cout << "After step 2: " << std::endl;
	      m.print();
	    }
	  break;
	case 3: nextStep = m.stepThree();
	  if( verbose )
	    {
	      std::cout << "After step 3: " << std::endl;
	      m.print();
	    }
	  break;
	case 4: nextStep = m.stepFour();
	  if( verbose )
	    {
	      std::cout << "After step 4: " << std::endl;
	      m.print();
	    }
	  break;
	case 5: nextStep = m.stepFive();
	  if( verbose )
	    {
	      std::cout << "After step 5: " << std::endl;
	      m.print();
	    }
	  break;
	case 6: nextStep = m.stepSix();
	  if( verbose )
	    {
	      std::cout << "After step 6: " << std::endl;
	      m.print();
	    }
	  break;
	};
    }

  if( verbose )
    {
      std::cout << "Final: " << std::endl;
      m.print();
    }

  return 0;
}
