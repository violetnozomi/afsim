// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__SeparationParametersType_h
#define Uci__Type__SeparationParametersType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SeparationParametersType sequence accessor class */
      class SeparationParametersType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SeparationParametersType()
         { }

         /** Returns this accessor's type constant, i.e. SeparationParametersType
           *
           * @return This accessor's type constant, i.e. SeparationParametersType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::separationParametersType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SeparationParametersType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AltitudeDistance.
           *
           * @return The value of the simple primitive data type identified by AltitudeDistance.
           */
         virtual uci::type::DistanceTypeValue getAltitudeDistance() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AltitudeDistance.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAltitudeDistance(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the HorizontalDistance.
           *
           * @return The value of the simple primitive data type identified by HorizontalDistance.
           */
         virtual uci::type::DistanceTypeValue getHorizontalDistance() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the HorizontalDistance.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHorizontalDistance(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ForwardDistance.
           *
           * @return The value of the simple primitive data type identified by ForwardDistance.
           */
         virtual uci::type::DistanceTypeValue getForwardDistance() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ForwardDistance.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setForwardDistance(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TrailingDistance.
           *
           * @return The value of the simple primitive data type identified by TrailingDistance.
           */
         virtual uci::type::DistanceTypeValue getTrailingDistance() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TrailingDistance.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTrailingDistance(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SeparationParametersType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SeparationParametersType to copy from
           */
         SeparationParametersType(const SeparationParametersType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SeparationParametersType to the contents of the
           * SeparationParametersType on the right hand side (rhs) of the assignment operator.SeparationParametersType [only
           * available to derived classes]
           *
           * @param rhs The SeparationParametersType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SeparationParametersType
           * @return A constant reference to this SeparationParametersType.
           */
         const SeparationParametersType& operator=(const SeparationParametersType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SeparationParametersType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SeparationParametersType_h

