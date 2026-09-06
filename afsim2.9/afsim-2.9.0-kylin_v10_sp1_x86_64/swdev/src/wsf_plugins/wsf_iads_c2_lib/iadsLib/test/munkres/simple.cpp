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
#include <munkres.hpp>

int main( int argc, char **argv )
{
  munkres m( 3, 3 );
  m.getCost( 0, 0 ) = 1;
  m.getCost( 0, 1 ) = 2;
  m.getCost( 0, 2 ) = 3;
  m.getCost( 1, 0 ) = 2;
  m.getCost( 1, 1 ) = 4;
  m.getCost( 1, 2 ) = 6;
  m.getCost( 2, 0 ) = 3;
  m.getCost( 2, 1 ) = 6;
  m.getCost( 2, 2 ) = 9;

  std::cout << "Initial: " << std::endl;
  m.print();

  uint32_t nextStep = 1;

  while( 0 != nextStep )
    {
      switch( nextStep )
	{
	case 1: nextStep = m.stepOne();
	  std::cout << "After step 1: " << std::endl;
	  m.print();
	  break;
	case 2: nextStep = m.stepTwo();
	  std::cout << "After step 2: " << std::endl;
	  m.print();
	  break;
	case 3: nextStep = m.stepThree();
	  std::cout << "After step 3: " << std::endl;
	  m.print();
	  break;
	case 4: nextStep = m.stepFour();
	  std::cout << "After step 4: " << std::endl;
	  m.print();
	  break;
	case 5: nextStep = m.stepFive();
	  std::cout << "After step 5: " << std::endl;
	  m.print();
	  break;
	case 6: nextStep = m.stepSix();
	  std::cout << "After step 6: " << std::endl;
	  m.print();
	  break;
	};
    }

  return 0;
}
