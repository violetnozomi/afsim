// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__GatewayLink16ConfigurationCircularSectorType_h
#define Uci__Type__GatewayLink16ConfigurationCircularSectorType_h 1

#if !defined(Uci__Type__GatewayLink16ConfigurationClosedAreaType_h)
# include "uci/type/GatewayLink16ConfigurationClosedAreaType.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

#if !defined(Uci__Type__AngleType_h)
# include "uci/type/AngleType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Describes a circular sector Link 16 filter geographic area */
      class GatewayLink16ConfigurationCircularSectorType : public virtual uci::type::GatewayLink16ConfigurationClosedAreaType {
      public:

         /** The destructor */
         virtual ~GatewayLink16ConfigurationCircularSectorType()
         { }

         /** Returns this accessor's type constant, i.e. GatewayLink16ConfigurationCircularSectorType
           *
           * @return This accessor's type constant, i.e. GatewayLink16ConfigurationCircularSectorType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::gatewayLink16ConfigurationCircularSectorType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const GatewayLink16ConfigurationCircularSectorType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Radius.
           *
           * @return The value of the simple primitive data type identified by Radius.
           */
         virtual uci::type::DistanceTypeValue getRadius() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Radius.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRadius(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the StartBearing.
           *
           * @return The value of the simple primitive data type identified by StartBearing.
           */
         virtual uci::type::AngleTypeValue getStartBearing() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the StartBearing.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setStartBearing(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the StopBearing.
           *
           * @return The value of the simple primitive data type identified by StopBearing.
           */
         virtual uci::type::AngleTypeValue getStopBearing() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the StopBearing.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setStopBearing(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         GatewayLink16ConfigurationCircularSectorType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The GatewayLink16ConfigurationCircularSectorType to copy from
           */
         GatewayLink16ConfigurationCircularSectorType(const GatewayLink16ConfigurationCircularSectorType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this GatewayLink16ConfigurationCircularSectorType to the contents of
           * the GatewayLink16ConfigurationCircularSectorType on the right hand side (rhs) of the assignment
           * operator.GatewayLink16ConfigurationCircularSectorType [only available to derived classes]
           *
           * @param rhs The GatewayLink16ConfigurationCircularSectorType on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::type::GatewayLink16ConfigurationCircularSectorType
           * @return A constant reference to this GatewayLink16ConfigurationCircularSectorType.
           */
         const GatewayLink16ConfigurationCircularSectorType& operator=(const GatewayLink16ConfigurationCircularSectorType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // GatewayLink16ConfigurationCircularSectorType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__GatewayLink16ConfigurationCircularSectorType_h

