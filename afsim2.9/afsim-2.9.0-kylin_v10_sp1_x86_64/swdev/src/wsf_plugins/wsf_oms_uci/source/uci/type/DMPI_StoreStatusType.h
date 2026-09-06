// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__DMPI_StoreStatusType_h
#define Uci__Type__DMPI_StoreStatusType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DMPI_StoreStateEnum_h)
# include "uci/type/DMPI_StoreStateEnum.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DMPI_StoreStatusType sequence accessor class */
      class DMPI_StoreStatusType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DMPI_StoreStatusType()
         { }

         /** Returns this accessor's type constant, i.e. DMPI_StoreStatusType
           *
           * @return This accessor's type constant, i.e. DMPI_StoreStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dMPI_StoreStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DMPI_StoreStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the StoreState.
           *
           * @return The value of the enumeration identified by StoreState.
           */
         virtual const uci::type::DMPI_StoreStateEnum& getStoreState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the StoreState.
           *
           * @return The value of the enumeration identified by StoreState.
           */
         virtual uci::type::DMPI_StoreStateEnum& getStoreState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the StoreState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setStoreState(const uci::type::DMPI_StoreStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the StoreState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setStoreState(uci::type::DMPI_StoreStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual const uci::type::CapabilityID_Type& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual uci::type::CapabilityID_Type& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityID
           */
         virtual void setCapabilityID(const uci::type::CapabilityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CapabilityID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CapabilityID is emabled or not
           */
         virtual bool hasCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CapabilityID
           *
           * @param type = uci::type::accessorType::capabilityID_Type This Accessor's accessor type
           */
         virtual void enableCapabilityID(uci::base::accessorType::AccessorType type = uci::type::accessorType::capabilityID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CapabilityID */
         virtual void clearCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Reason.
           *
           * @return The value of the string data type identified by Reason.
           */
         virtual const uci::type::VisibleString1024Type& getReason() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Reason.
           *
           * @return The value of the string data type identified by Reason.
           */
         virtual uci::type::VisibleString1024Type& getReason()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Reason to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setReason(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Reason to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setReason(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Reason to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setReason(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Reason exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Reason is emabled or not
           */
         virtual bool hasReason() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Reason
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enableReason(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Reason */
         virtual void clearReason()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TimeOfImpact.
           *
           * @return The value of the simple primitive data type identified by TimeOfImpact.
           */
         virtual uci::type::DateTimeTypeValue getTimeOfImpact() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TimeOfImpact.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTimeOfImpact(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TimeOfImpact exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TimeOfImpact is emabled or not
           */
         virtual bool hasTimeOfImpact() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TimeOfImpact
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableTimeOfImpact(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TimeOfImpact */
         virtual void clearTimeOfImpact()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DMPI_StoreStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DMPI_StoreStatusType to copy from
           */
         DMPI_StoreStatusType(const DMPI_StoreStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DMPI_StoreStatusType to the contents of the DMPI_StoreStatusType
           * on the right hand side (rhs) of the assignment operator.DMPI_StoreStatusType [only available to derived classes]
           *
           * @param rhs The DMPI_StoreStatusType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::DMPI_StoreStatusType
           * @return A constant reference to this DMPI_StoreStatusType.
           */
         const DMPI_StoreStatusType& operator=(const DMPI_StoreStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DMPI_StoreStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DMPI_StoreStatusType_h

