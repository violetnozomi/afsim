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
#ifndef Uci__Type__StrikeCapabilityStatusType_h
#define Uci__Type__StrikeCapabilityStatusType_h 1

#if !defined(Uci__Type__CapabilityStatusType_h)
# include "uci/type/CapabilityStatusType.h"
#endif

#if !defined(Uci__Type__StoreStatusType_h)
# include "uci/type/StoreStatusType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /**  Indicates the high level availability/state of a Store Capability instance. */
      class StrikeCapabilityStatusType : public virtual uci::type::CapabilityStatusType {
      public:

         /** The destructor */
         virtual ~StrikeCapabilityStatusType()
         { }

         /** Returns this accessor's type constant, i.e. StrikeCapabilityStatusType
           *
           * @return This accessor's type constant, i.e. StrikeCapabilityStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::strikeCapabilityStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const StrikeCapabilityStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the StoreStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by StoreStatus.
           */
         virtual const uci::type::StoreStatusType& getStoreStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the StoreStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by StoreStatus.
           */
         virtual uci::type::StoreStatusType& getStoreStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the StoreStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by StoreStatus
           */
         virtual void setStoreStatus(const uci::type::StoreStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by StoreStatus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by StoreStatus is emabled or not
           */
         virtual bool hasStoreStatus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by StoreStatus
           *
           * @param type = uci::type::accessorType::storeStatusType This Accessor's accessor type
           */
         virtual void enableStoreStatus(uci::base::accessorType::AccessorType type = uci::type::accessorType::storeStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by StoreStatus */
         virtual void clearStoreStatus()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         StrikeCapabilityStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The StrikeCapabilityStatusType to copy from
           */
         StrikeCapabilityStatusType(const StrikeCapabilityStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this StrikeCapabilityStatusType to the contents of the
           * StrikeCapabilityStatusType on the right hand side (rhs) of the assignment operator.StrikeCapabilityStatusType [only
           * available to derived classes]
           *
           * @param rhs The StrikeCapabilityStatusType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::StrikeCapabilityStatusType
           * @return A constant reference to this StrikeCapabilityStatusType.
           */
         const StrikeCapabilityStatusType& operator=(const StrikeCapabilityStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // StrikeCapabilityStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__StrikeCapabilityStatusType_h

