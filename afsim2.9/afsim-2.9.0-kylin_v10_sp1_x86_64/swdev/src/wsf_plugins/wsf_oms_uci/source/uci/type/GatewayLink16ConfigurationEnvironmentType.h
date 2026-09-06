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
#ifndef Uci__Type__GatewayLink16ConfigurationEnvironmentType_h
#define Uci__Type__GatewayLink16ConfigurationEnvironmentType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Contains environment applicability indicators for a Link 16 filter. */
      class GatewayLink16ConfigurationEnvironmentType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~GatewayLink16ConfigurationEnvironmentType()
         { }

         /** Returns this accessor's type constant, i.e. GatewayLink16ConfigurationEnvironmentType
           *
           * @return This accessor's type constant, i.e. GatewayLink16ConfigurationEnvironmentType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::gatewayLink16ConfigurationEnvironmentType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const GatewayLink16ConfigurationEnvironmentType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Unknown.
           *
           * @return The value of the simple primitive data type identified by Unknown.
           */
         virtual xs::Boolean getUnknown() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Unknown.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setUnknown(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Space.
           *
           * @return The value of the simple primitive data type identified by Space.
           */
         virtual xs::Boolean getSpace() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Space.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSpace(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Air.
           *
           * @return The value of the simple primitive data type identified by Air.
           */
         virtual xs::Boolean getAir() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Air.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAir(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Surface.
           *
           * @return The value of the simple primitive data type identified by Surface.
           */
         virtual xs::Boolean getSurface() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Surface.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSurface(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Subsurface.
           *
           * @return The value of the simple primitive data type identified by Subsurface.
           */
         virtual xs::Boolean getSubsurface() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Subsurface.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSubsurface(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Land.
           *
           * @return The value of the simple primitive data type identified by Land.
           */
         virtual xs::Boolean getLand() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Land.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLand(xs::Boolean value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         GatewayLink16ConfigurationEnvironmentType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The GatewayLink16ConfigurationEnvironmentType to copy from
           */
         GatewayLink16ConfigurationEnvironmentType(const GatewayLink16ConfigurationEnvironmentType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this GatewayLink16ConfigurationEnvironmentType to the contents of the
           * GatewayLink16ConfigurationEnvironmentType on the right hand side (rhs) of the assignment
           * operator.GatewayLink16ConfigurationEnvironmentType [only available to derived classes]
           *
           * @param rhs The GatewayLink16ConfigurationEnvironmentType on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::type::GatewayLink16ConfigurationEnvironmentType
           * @return A constant reference to this GatewayLink16ConfigurationEnvironmentType.
           */
         const GatewayLink16ConfigurationEnvironmentType& operator=(const GatewayLink16ConfigurationEnvironmentType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // GatewayLink16ConfigurationEnvironmentType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__GatewayLink16ConfigurationEnvironmentType_h

