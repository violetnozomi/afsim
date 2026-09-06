// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
// @warning  This file was automatically generated, edit at your own risk

/**
* Unclassified               U N C L A S S I F I E D               Unclassified
*
* DEVELOPMENT:
*    This document wholly developed with USG funds.
*    For additional information, contact the AFRCO.
*
* ‒  DISTRIBUTION STATEMENT D.  Distribution authorized to the Department
*    of Defense and U.S. DoD contractors only; Critical Technology; 17 Sep 2015.
*    Other requests shall be referred to Air Force Rapid Capabilities Office,
*    Bolling AFB, Washington DC 20032-6400. 
*
* EXPORT CONTROL:
*    WARNING - ITAR CONTROLLED - US ONLY
*    This distribution contains technical data whose export is restricted by
*    the Arms Export Control Act (Title 22, U.S.C., Sec. 2751 et seq. or the
*    Export Administration Act of 1979 as amended Title 50, U.S.C., App.
*    2401-2420 et seq.), as amended. Violation of these export laws are subject
*    to severe criminal penalties.  Disseminate in accordance with provisions of DoD
*    Directive 5230.25.
*/
#ifndef Uci__Type__AltitudeRangePairType_h
#define Uci__Type__AltitudeRangePairType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AltitudeType_h)
# include "uci/type/AltitudeType.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AltitudeRangePairType sequence accessor class */
      class AltitudeRangePairType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AltitudeRangePairType()
         { }

         /** Returns this accessor's type constant, i.e. AltitudeRangePairType
           *
           * @return This accessor's type constant, i.e. AltitudeRangePairType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::altitudeRangePairType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AltitudeRangePairType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Altitude.
           *
           * @return The value of the simple primitive data type identified by Altitude.
           */
         virtual uci::type::AltitudeTypeValue getAltitude() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Altitude.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAltitude(uci::type::AltitudeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Range.
           *
           * @return The value of the simple primitive data type identified by Range.
           */
         virtual uci::type::DistanceTypeValue getRange() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Range.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRange(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AltitudeRangePairType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AltitudeRangePairType to copy from
           */
         AltitudeRangePairType(const AltitudeRangePairType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AltitudeRangePairType to the contents of the AltitudeRangePairType
           * on the right hand side (rhs) of the assignment operator.AltitudeRangePairType [only available to derived classes]
           *
           * @param rhs The AltitudeRangePairType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::AltitudeRangePairType
           * @return A constant reference to this AltitudeRangePairType.
           */
         const AltitudeRangePairType& operator=(const AltitudeRangePairType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AltitudeRangePairType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AltitudeRangePairType_h

