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
#if 1
  // Two workers, Three jobs
  munkres m( 2, 3 );
  m.getCost( 0, 0 ) = 1;
  m.getCost( 0, 1 ) = 2;
  m.getCost( 0, 2 ) = 3;
  m.getCost( 1, 0 ) = 2;
  m.getCost( 1, 1 ) = 4;
  m.getCost( 1, 2 ) = 6;
#else
  // Three workers, Two jobs
  munkres m( 3, 2 );
  m.getCost( 0, 0 ) = 1;
  m.getCost( 0, 1 ) = 2;
  m.getCost( 1, 0 ) = 2;
  m.getCost( 1, 1 ) = 4;
  m.getCost( 2, 0 ) = 3;
  m.getCost( 2, 1 ) = 6;
#endif
  std::cout << "Initial: " << std::endl;
  m.print();

  m.solve();

  std::cout << "Final: " << std::endl;
  m.print();

  return 0;
}
