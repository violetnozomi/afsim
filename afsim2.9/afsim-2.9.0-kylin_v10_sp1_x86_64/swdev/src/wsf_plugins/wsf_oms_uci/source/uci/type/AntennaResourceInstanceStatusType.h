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
#ifndef Uci__Type__AntennaResourceInstanceStatusType_h
#define Uci__Type__AntennaResourceInstanceStatusType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AntennaResourceInstanceIdentifierType_h)
# include "uci/type/AntennaResourceInstanceIdentifierType.h"
#endif

#if !defined(Uci__Type__CapabilityAvailabilityEnum_h)
# include "uci/type/CapabilityAvailabilityEnum.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Status of antenna resource instance. Shared Antenna Resource Instances are not taxonomically described in UCI. It is
        * left to external actors to organize, uniquely describe and identify them.
        */
      class AntennaResourceInstanceStatusType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AntennaResourceInstanceStatusType()
         { }

         /** Returns this accessor's type constant, i.e. AntennaResourceInstanceStatusType
           *
           * @return This accessor's type constant, i.e. AntennaResourceInstanceStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::antennaResourceInstanceStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AntennaResourceInstanceStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * AntennaResourceInstanceIndex.
           *
           * @return The acecssor that provides access to the complex content that is identified by AntennaResourceInstanceIndex.
           */
         virtual const uci::type::AntennaResourceInstanceIdentifierType& getAntennaResourceInstanceIndex() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * AntennaResourceInstanceIndex.
           *
           * @return The acecssor that provides access to the complex content that is identified by AntennaResourceInstanceIndex.
           */
         virtual uci::type::AntennaResourceInstanceIdentifierType& getAntennaResourceInstanceIndex()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AntennaResourceInstanceIndex to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AntennaResourceInstanceIndex
           */
         virtual void setAntennaResourceInstanceIndex(const uci::type::AntennaResourceInstanceIdentifierType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Availability.
           *
           * @return The value of the enumeration identified by Availability.
           */
         virtual const uci::type::CapabilityAvailabilityEnum& getAvailability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Availability.
           *
           * @return The value of the enumeration identified by Availability.
           */
         virtual uci::type::CapabilityAvailabilityEnum& getAvailability()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Availability.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAvailability(const uci::type::CapabilityAvailabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Availability.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAvailability(uci::type::CapabilityAvailabilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PercentAccess.
           *
           * @return The value of the simple primitive data type identified by PercentAccess.
           */
         virtual uci::type::PercentTypeValue getPercentAccess() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PercentAccess.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPercentAccess(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PercentAccess exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PercentAccess is emabled or not
           */
         virtual bool hasPercentAccess() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PercentAccess
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enablePercentAccess(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PercentAccess */
         virtual void clearPercentAccess()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AntennaResourceInstanceStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AntennaResourceInstanceStatusType to copy from
           */
         AntennaResourceInstanceStatusType(const AntennaResourceInstanceStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AntennaResourceInstanceStatusType to the contents of the
           * AntennaResourceInstanceStatusType on the right hand side (rhs) of the assignment
           * operator.AntennaResourceInstanceStatusType [only available to derived classes]
           *
           * @param rhs The AntennaResourceInstanceStatusType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::AntennaResourceInstanceStatusType
           * @return A constant reference to this AntennaResourceInstanceStatusType.
           */
         const AntennaResourceInstanceStatusType& operator=(const AntennaResourceInstanceStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AntennaResourceInstanceStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AntennaResourceInstanceStatusType_h

