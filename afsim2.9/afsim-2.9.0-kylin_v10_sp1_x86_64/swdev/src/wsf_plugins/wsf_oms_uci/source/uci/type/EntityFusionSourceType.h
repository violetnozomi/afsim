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
#ifndef Uci__Type__EntityFusionSourceType_h
#define Uci__Type__EntityFusionSourceType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EntityID_Type_h)
# include "uci/type/EntityID_Type.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__IdentityConfidenceType_h)
# include "uci/type/IdentityConfidenceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EntityFusionSourceType sequence accessor class */
      class EntityFusionSourceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EntityFusionSourceType()
         { }

         /** Returns this accessor's type constant, i.e. EntityFusionSourceType
           *
           * @return This accessor's type constant, i.e. EntityFusionSourceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entityFusionSourceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntityFusionSourceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ContributorID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ContributorID.
           */
         virtual const uci::type::EntityID_Type& getContributorID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ContributorID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ContributorID.
           */
         virtual uci::type::EntityID_Type& getContributorID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ContributorID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ContributorID
           */
         virtual void setContributorID(const uci::type::EntityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FusionContributor.
           *
           * @return The value of the simple primitive data type identified by FusionContributor.
           */
         virtual xs::Boolean getFusionContributor() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FusionContributor.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFusionContributor(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ProbabilityOfCorrectAssociation.
           *
           * @return The value of the simple primitive data type identified by ProbabilityOfCorrectAssociation.
           */
         virtual uci::type::PercentTypeValue getProbabilityOfCorrectAssociation() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ProbabilityOfCorrectAssociation.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setProbabilityOfCorrectAssociation(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ContributorCapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ContributorCapabilityID.
           */
         virtual const uci::type::CapabilityID_Type& getContributorCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ContributorCapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ContributorCapabilityID.
           */
         virtual uci::type::CapabilityID_Type& getContributorCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ContributorCapabilityID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ContributorCapabilityID
           */
         virtual void setContributorCapabilityID(const uci::type::CapabilityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ContributorCapabilityID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ContributorCapabilityID is emabled or not
           */
         virtual bool hasContributorCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ContributorCapabilityID
           *
           * @param type = uci::type::accessorType::capabilityID_Type This Accessor's accessor type
           */
         virtual void enableContributorCapabilityID(uci::base::accessorType::AccessorType type = uci::type::accessorType::capabilityID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ContributorCapabilityID */
         virtual void clearContributorCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual const uci::type::IdentityConfidenceType& getIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Identity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Identity.
           */
         virtual uci::type::IdentityConfidenceType& getIdentity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Identity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Identity
           */
         virtual void setIdentity(const uci::type::IdentityConfidenceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Identity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Identity is emabled or not
           */
         virtual bool hasIdentity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Identity
           *
           * @param type = uci::type::accessorType::identityConfidenceType This Accessor's accessor type
           */
         virtual void enableIdentity(uci::base::accessorType::AccessorType type = uci::type::accessorType::identityConfidenceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Identity */
         virtual void clearIdentity()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntityFusionSourceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntityFusionSourceType to copy from
           */
         EntityFusionSourceType(const EntityFusionSourceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntityFusionSourceType to the contents of the
           * EntityFusionSourceType on the right hand side (rhs) of the assignment operator.EntityFusionSourceType [only available
           * to derived classes]
           *
           * @param rhs The EntityFusionSourceType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::EntityFusionSourceType
           * @return A constant reference to this EntityFusionSourceType.
           */
         const EntityFusionSourceType& operator=(const EntityFusionSourceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntityFusionSourceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntityFusionSourceType_h

