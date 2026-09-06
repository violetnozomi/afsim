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
#ifndef Uci__Type__GatewayLink16ConfigurationIdentityType_h
#define Uci__Type__GatewayLink16ConfigurationIdentityType_h 1

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

      /** Contains identity applicability indicators for a Link 16 filter. */
      class GatewayLink16ConfigurationIdentityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~GatewayLink16ConfigurationIdentityType()
         { }

         /** Returns this accessor's type constant, i.e. GatewayLink16ConfigurationIdentityType
           *
           * @return This accessor's type constant, i.e. GatewayLink16ConfigurationIdentityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::gatewayLink16ConfigurationIdentityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const GatewayLink16ConfigurationIdentityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Pending.
           *
           * @return The value of the simple primitive data type identified by Pending.
           */
         virtual xs::Boolean getPending() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Pending.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPending(xs::Boolean value)
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


         /** Returns the value of the SimplePrimitive data type that is identified by the AssumedFriend.
           *
           * @return The value of the simple primitive data type identified by AssumedFriend.
           */
         virtual xs::Boolean getAssumedFriend() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AssumedFriend.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAssumedFriend(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Friend.
           *
           * @return The value of the simple primitive data type identified by Friend.
           */
         virtual xs::Boolean getFriend() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Friend.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFriend(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Neutral.
           *
           * @return The value of the simple primitive data type identified by Neutral.
           */
         virtual xs::Boolean getNeutral() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Neutral.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNeutral(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Suspect.
           *
           * @return The value of the simple primitive data type identified by Suspect.
           */
         virtual xs::Boolean getSuspect() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Suspect.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSuspect(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hostile.
           *
           * @return The value of the simple primitive data type identified by Hostile.
           */
         virtual xs::Boolean getHostile() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hostile.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHostile(xs::Boolean value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         GatewayLink16ConfigurationIdentityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The GatewayLink16ConfigurationIdentityType to copy from
           */
         GatewayLink16ConfigurationIdentityType(const GatewayLink16ConfigurationIdentityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this GatewayLink16ConfigurationIdentityType to the contents of the
           * GatewayLink16ConfigurationIdentityType on the right hand side (rhs) of the assignment
           * operator.GatewayLink16ConfigurationIdentityType [only available to derived classes]
           *
           * @param rhs The GatewayLink16ConfigurationIdentityType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::GatewayLink16ConfigurationIdentityType
           * @return A constant reference to this GatewayLink16ConfigurationIdentityType.
           */
         const GatewayLink16ConfigurationIdentityType& operator=(const GatewayLink16ConfigurationIdentityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // GatewayLink16ConfigurationIdentityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__GatewayLink16ConfigurationIdentityType_h

