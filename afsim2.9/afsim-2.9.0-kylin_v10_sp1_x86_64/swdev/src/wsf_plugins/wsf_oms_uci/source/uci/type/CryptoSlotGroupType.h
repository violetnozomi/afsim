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
#ifndef Uci__Type__CryptoSlotGroupType_h
#define Uci__Type__CryptoSlotGroupType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CryptoSlotGroupID_Type_h)
# include "uci/type/CryptoSlotGroupID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__CryptoSlotID_Type_h)
# include "uci/type/CryptoSlotID_Type.h"
#endif

#if !defined(Uci__Type__CryptoRolloverMethodEnum_h)
# include "uci/type/CryptoRolloverMethodEnum.h"
#endif

#if !defined(Uci__Type__CryptoSlotType_h)
# include "uci/type/CryptoSlotType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CryptoSlotGroupType sequence accessor class */
      class CryptoSlotGroupType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CryptoSlotGroupType()
         { }

         /** Returns this accessor's type constant, i.e. CryptoSlotGroupType
           *
           * @return This accessor's type constant, i.e. CryptoSlotGroupType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cryptoSlotGroupType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CryptoSlotGroupType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the ID of a Capability whose use requires operation of the crypto key slot [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** Indicates which types of key rollover are available for this group of crypto keys. [Maximum occurrences: 5] */
         typedef uci::base::BoundedList<uci::type::CryptoRolloverMethodEnum, uci::type::accessorType::cryptoRolloverMethodEnum> RolloverMethod;

         /** Indicates detailed information about each crypto slot within a group of crypto key slots. This includes state of the
           * key slot, the next slot in a link list of slots (with respect to rollover, if applicable), and unique ID for the key.
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CryptoSlotType, uci::type::accessorType::cryptoSlotType> SlotInformation;

         /** Returns the accessor that provides access to the complex content that is identified by the CryptoSlotGroupID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CryptoSlotGroupID.
           */
         virtual const uci::type::CryptoSlotGroupID_Type& getCryptoSlotGroupID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CryptoSlotGroupID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CryptoSlotGroupID.
           */
         virtual uci::type::CryptoSlotGroupID_Type& getCryptoSlotGroupID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CryptoSlotGroupID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CryptoSlotGroupID
           */
         virtual void setCryptoSlotGroupID(const uci::type::CryptoSlotGroupID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::CryptoSlotGroupType::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::CryptoSlotGroupType::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::CryptoSlotGroupType::CapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the CurrentSlotID.
           *
           * @return The value of the string data type identified by CurrentSlotID.
           */
         virtual const uci::type::CryptoSlotID_Type& getCurrentSlotID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the CurrentSlotID.
           *
           * @return The value of the string data type identified by CurrentSlotID.
           */
         virtual uci::type::CryptoSlotID_Type& getCurrentSlotID()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CurrentSlotID to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setCurrentSlotID(const uci::type::CryptoSlotID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CurrentSlotID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCurrentSlotID(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CurrentSlotID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCurrentSlotID(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RolloverMethod.
           *
           * @return The bounded list identified by RolloverMethod.
           */
         virtual const uci::type::CryptoSlotGroupType::RolloverMethod& getRolloverMethod() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RolloverMethod.
           *
           * @return The bounded list identified by RolloverMethod.
           */
         virtual uci::type::CryptoSlotGroupType::RolloverMethod& getRolloverMethod()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RolloverMethod.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRolloverMethod(const uci::type::CryptoSlotGroupType::RolloverMethod& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SlotInformation.
           *
           * @return The bounded list identified by SlotInformation.
           */
         virtual const uci::type::CryptoSlotGroupType::SlotInformation& getSlotInformation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SlotInformation.
           *
           * @return The bounded list identified by SlotInformation.
           */
         virtual uci::type::CryptoSlotGroupType::SlotInformation& getSlotInformation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SlotInformation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSlotInformation(const uci::type::CryptoSlotGroupType::SlotInformation& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CryptoSlotGroupType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CryptoSlotGroupType to copy from
           */
         CryptoSlotGroupType(const CryptoSlotGroupType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CryptoSlotGroupType to the contents of the CryptoSlotGroupType on
           * the right hand side (rhs) of the assignment operator.CryptoSlotGroupType [only available to derived classes]
           *
           * @param rhs The CryptoSlotGroupType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::CryptoSlotGroupType
           * @return A constant reference to this CryptoSlotGroupType.
           */
         const CryptoSlotGroupType& operator=(const CryptoSlotGroupType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CryptoSlotGroupType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CryptoSlotGroupType_h

