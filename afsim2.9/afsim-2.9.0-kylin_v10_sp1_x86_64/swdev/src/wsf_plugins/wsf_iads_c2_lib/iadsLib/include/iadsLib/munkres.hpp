// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @class  munkres
 *  @file   munkres.hpp
 *  @author Kenneth R. Sewell III
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 ** Classification: UNCLASSIFIED
 */
#include <stdint.h>
#include <iostream>
#include <vector>
#include <cfloat>

#include "logger.hpp"

#ifndef MUNKRES_HPP
#define MUNKRES_HPP

class munkres
{
public:
  munkres( const uint16_t &numRows=3, const uint16_t &numCols=3,
	   const float &defaultValue=FLT_MAX);
  ~munkres();

  bool solve(std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger);

  void setCost(const uint16_t &row, const uint16_t &col, const float &cost);
  float &getCost(const uint16_t &row, const uint16_t &col);
  const float &getCost(const uint16_t &row, const uint16_t &col) const;

  void log(std::ostream &os) const;
  bool solved() const;

  uint32_t stepOne();
  uint32_t stepTwo();
  uint32_t stepThree();
  uint32_t stepFour(std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger);
  uint32_t stepFive(std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger);
  uint32_t stepSix();

  bool isStarInRow( const uint16_t &row, uint16_t &col ) const;

protected:
  bool isStarInRow( const uint16_t &row ) const;
  bool isStarInCol( uint16_t &row, const uint16_t &col ) const;
  bool isStarInRowCol( const uint16_t &row, const uint16_t &col ) const;
  bool isPrimeInRow( const uint16_t &row, uint16_t &col ) const;
  bool isPrimeInCol( uint16_t &row, const uint16_t &col ) const;
  bool isStarred( const uint16_t &row, const uint16_t &col ) const;
  bool isStarred( const uint32_t &index ) const;
  bool isPrimed( const uint16_t &row, const uint16_t &col ) const;

  void setStarred( const uint16_t &row, const uint16_t &col,
		   const bool &value=true );
  void setPrimed( const uint16_t &row, const uint16_t &col,
		  const bool &value=true );

  bool m_Solution;
  uint16_t m_NumRows;
  uint16_t m_NumCols;
  uint16_t m_LastPrimeRow;
  uint16_t m_LastPrimeCol;
  std::vector<bool>  m_CoveredRows;
  std::vector<bool>  m_CoveredCols;
  std::vector<float> m_Matrix;
  std::vector<bool> m_Starred;
  std::vector<bool> m_Primed;
  float m_SmallestUncoveredValue;

private:

};

#endif
