// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file RandFunc.hpp
 *  @author Randall Wantz (BAH) AFRL/RYZW \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3003\n
 *  @brief Random umber utility functions.
 *  Classification: UNCLASSIFIED
*/

#include <cstdlib> // Needed for rand() and RAND_MAX

#ifndef VCLMATH_RANDFUNC_HPP
#define VCLMATH_RANDFUNC_HPP 1

namespace vm {

   /**
    @brief  Random Gaussian Noise Generator Function: Returns random Gaussian noise about
            an input mean with input standard deviation  \n
    @param mean:  value to return random error about
    @parma sigma:  one std deviation
   */
   template <typename T>
    T RandGaus( T mean, T sigma ) {

      T sf1 = 0.0833034044690608;
      T sf2 = sigma/sf1;
      T uniform_12;
      uniform_12 = 0;

      for (int i = 0; i < 12; i++) {
        uniform_12 += ((T)(rand()/(T)RAND_MAX)) - 0.5;
      }
      return mean + (sf2*uniform_12/12.0);

   }

   /**
    @brief  Random Uniform Generator Function: Returns random uniform
            number between 0 and 1\n
   */
   template <typename T>
   T RandUnif01() {
        return  ((T)(rand()/(T)RAND_MAX));
   }
}  // namespace
#endif
