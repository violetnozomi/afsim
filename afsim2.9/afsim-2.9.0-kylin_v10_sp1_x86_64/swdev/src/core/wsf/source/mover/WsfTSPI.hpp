// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFTSPI_HPP
#define WSFTSPI_HPP

#include "wsf_export.h"

#include <iosfwd>
#include <string>

class UtInput;

/** A Time-Space-Position-Information (TSPI) class.
   The data contained in a TSPI object are of the following form:
   \code
   <time> <latitude> <longitude> <altitude> <speed> <heading> <pitch> <roll>
   \endcode
   In the BLUEMAX formulation, the altitude is in feet, and the roll is inverted.
   Other formulations are allowed; the user configures the TSPI format via input file keywords.
*/
class WSF_EXPORT WsfTSPI
{
public:
   WsfTSPI();
   virtual ~WsfTSPI() = default;

   virtual bool ProcessInput(UtInput& aInput);

   friend WSF_EXPORT std::ostream& operator<<(std::ostream& aStream, const WsfTSPI& aTSPI);
   friend WSF_EXPORT std::istream& operator>>(std::istream& aStream, WsfTSPI& aTSPI);

   /** A single TSPI element (time, lat, heading, etc.). */
   class Element
   {
   public:
      Element() = default;
      Element(double aValue, unsigned int aValueType);
      ~Element() = default;

      /** Return the value of the element. */
      double operator()() const { return mAttribute; }
      /** Set the value of the element. */
      Element& operator=(const double aRhs)
      {
         mAttribute = aRhs;
         return *this;
      }

      /** Return the value of the element. */
      double Get() const { return mAttribute; }
      /** Set the value of the element. */
      void Set(double aAttribute) { mAttribute = aAttribute; }

      /** Set the persistent units (i.e., the units in which the value is saved. */
      void SetPersistentUnits(const std::string& aConversion);
      /** Set whether the value of the element is to be inverted (i.e., negated). */
      void SetInverted(bool aIsInverted) { mIsInverted = aIsInverted; }

      friend std::ostream& operator<<(std::ostream& aStream, const Element& aTSPI);
      friend std::istream& operator>>(std::istream& aStream, Element& aTSPI);

   private:
      void   ConvertInput();
      double GetOutputValue() const;

      double       mAttribute = 0.0;
      unsigned int mValueType = 0;
      std::string  mPersistentUnits;
      bool         mHasConversion = false;
      bool         mIsInverted    = false;
   };

   Element mTime;
   Element mLat;
   Element mLon;
   Element mAlt;
   Element mSpeed;
   Element mHeading;
   Element mPitch;
   Element mRoll;
};

#endif
