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
#ifndef Uci__Type__AvailabilityInfoType_h
#define Uci__Type__AvailabilityInfoType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CapabilityAvailabilityEnum_h)
# include "uci/type/CapabilityAvailabilityEnum.h"
#endif

#if !defined(Uci__Type__CannotComplyType_h)
# include "uci/type/CannotComplyType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AvailabilityInfoType sequence accessor class */
      class AvailabilityInfoType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AvailabilityInfoType()
         { }

         /** Returns this accessor's type constant, i.e. AvailabilityInfoType
           *
           * @return This accessor's type constant, i.e. AvailabilityInfoType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::availabilityInfoType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AvailabilityInfoType& accessor)
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


         /** Returns the accessor that provides access to the complex content that is identified by the AvailabilityReason.
           *
           * @return The acecssor that provides access to the complex content that is identified by AvailabilityReason.
           */
         virtual const uci::type::CannotComplyType& getAvailabilityReason() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AvailabilityReason.
           *
           * @return The acecssor that provides access to the complex content that is identified by AvailabilityReason.
           */
         virtual uci::type::CannotComplyType& getAvailabilityReason()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AvailabilityReason to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AvailabilityReason
           */
         virtual void setAvailabilityReason(const uci::type::CannotComplyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AvailabilityReason exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AvailabilityReason is emabled or not
           */
         virtual bool hasAvailabilityReason() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AvailabilityReason
           *
           * @param type = uci::type::accessorType::cannotComplyType This Accessor's accessor type
           */
         virtual void enableAvailabilityReason(uci::base::accessorType::AccessorType type = uci::type::accessorType::cannotComplyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AvailabilityReason */
         virtual void clearAvailabilityReason()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NextAvailableWindow.
           *
           * @return The value of the simple primitive data type identified by NextAvailableWindow.
           */
         virtual uci::type::DateTimeTypeValue getNextAvailableWindow() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NextAvailableWindow.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNextAvailableWindow(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NextAvailableWindow exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NextAvailableWindow is emabled or not
           */
         virtual bool hasNextAvailableWindow() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NextAvailableWindow
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableNextAvailableWindow(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NextAvailableWindow */
         virtual void clearNextAvailableWindow()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AvailabilityInfoType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AvailabilityInfoType to copy from
           */
         AvailabilityInfoType(const AvailabilityInfoType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AvailabilityInfoType to the contents of the AvailabilityInfoType
           * on the right hand side (rhs) of the assignment operator.AvailabilityInfoType [only available to derived classes]
           *
           * @param rhs The AvailabilityInfoType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::AvailabilityInfoType
           * @return A constant reference to this AvailabilityInfoType.
           */
         const AvailabilityInfoType& operator=(const AvailabilityInfoType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AvailabilityInfoType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AvailabilityInfoType_h

