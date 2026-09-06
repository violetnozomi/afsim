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
#ifndef Uci__Type__GatewayLink16ConfigurationAnnulusType_h
#define Uci__Type__GatewayLink16ConfigurationAnnulusType_h 1

#if !defined(Uci__Type__GatewayLink16ConfigurationClosedAreaType_h)
# include "uci/type/GatewayLink16ConfigurationClosedAreaType.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Describes an annular Link 16 filter geographic area */
      class GatewayLink16ConfigurationAnnulusType : public virtual uci::type::GatewayLink16ConfigurationClosedAreaType {
      public:

         /** The destructor */
         virtual ~GatewayLink16ConfigurationAnnulusType()
         { }

         /** Returns this accessor's type constant, i.e. GatewayLink16ConfigurationAnnulusType
           *
           * @return This accessor's type constant, i.e. GatewayLink16ConfigurationAnnulusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::gatewayLink16ConfigurationAnnulusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const GatewayLink16ConfigurationAnnulusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the InnerRadius.
           *
           * @return The value of the simple primitive data type identified by InnerRadius.
           */
         virtual uci::type::DistanceTypeValue getInnerRadius() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the InnerRadius.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInnerRadius(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the OuterRadius.
           *
           * @return The value of the simple primitive data type identified by OuterRadius.
           */
         virtual uci::type::DistanceTypeValue getOuterRadius() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the OuterRadius.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setOuterRadius(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         GatewayLink16ConfigurationAnnulusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The GatewayLink16ConfigurationAnnulusType to copy from
           */
         GatewayLink16ConfigurationAnnulusType(const GatewayLink16ConfigurationAnnulusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this GatewayLink16ConfigurationAnnulusType to the contents of the
           * GatewayLink16ConfigurationAnnulusType on the right hand side (rhs) of the assignment
           * operator.GatewayLink16ConfigurationAnnulusType [only available to derived classes]
           *
           * @param rhs The GatewayLink16ConfigurationAnnulusType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::GatewayLink16ConfigurationAnnulusType
           * @return A constant reference to this GatewayLink16ConfigurationAnnulusType.
           */
         const GatewayLink16ConfigurationAnnulusType& operator=(const GatewayLink16ConfigurationAnnulusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // GatewayLink16ConfigurationAnnulusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__GatewayLink16ConfigurationAnnulusType_h

