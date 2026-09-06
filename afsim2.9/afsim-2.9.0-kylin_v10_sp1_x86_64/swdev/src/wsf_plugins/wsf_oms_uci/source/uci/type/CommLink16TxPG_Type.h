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
#ifndef Uci__Type__CommLink16TxPG_Type_h
#define Uci__Type__CommLink16TxPG_Type_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

#if !defined(Uci__Type__CommLink16TxPackingLimitEnum_h)
# include "uci/type/CommLink16TxPackingLimitEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommLink16TxPG_Type sequence accessor class */
      class CommLink16TxPG_Type : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommLink16TxPG_Type()
         { }

         /** Returns this accessor's type constant, i.e. CommLink16TxPG_Type
           *
           * @return This accessor's type constant, i.e. CommLink16TxPG_Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commLink16TxPG_Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommLink16TxPG_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the priority of messages for this PG Slot. Used by the waveform to determine priority in transmit queue
           * processing. 1 is highest, 15 lowest.; [Minimum inclusive: 0] [Maximum inclusive: 15]
           */
         typedef uci::base::IntAccessor MessagePriority;
         //  The type of primitive data accessed by MessagePriority
         typedef xs::Int MessagePriorityValue;

         /** Number of messages for the PG index below that will be stored in the transmit queue.; [Minimum inclusive: 0] [Maximum
           * inclusive: 280]
           */
         typedef uci::base::IntAccessor MessageTransferStorageLimit;
         //  The type of primitive data accessed by MessageTransferStorageLimit
         typedef xs::Int MessageTransferStorageLimitValue;

         /** Indicates the PG index number for net selection. Range 0, 4 to 511 except 9, 12, 13, default 0.; [Minimum inclusive:
           * 0] [Maximum inclusive: 511]
           */
         typedef uci::base::IntAccessor PG_Index;
         //  The type of primitive data accessed by PG_Index
         typedef xs::Int PG_IndexValue;

         /** Indicates the Slot Number for this PG Slot.; [Minimum inclusive: 1] [Maximum inclusive: 10] */
         typedef uci::base::IntAccessor SlotNumber;
         //  The type of primitive data accessed by SlotNumber
         typedef xs::Int SlotNumberValue;

         /** Number of time slots the waveform will hold the message waiting for a transmit opportunity. If time is exceeded,
           * message will be deleted from the transmit queue.; [Minimum inclusive: 1] [Maximum inclusive: 16383]
           */
         typedef uci::base::IntAccessor StalenessLimit;
         //  The type of primitive data accessed by StalenessLimit
         typedef xs::Int StalenessLimitValue;

         /** Returns the value of the SimplePrimitive data type that is identified by the AutoRepromulgation.
           *
           * @return The value of the simple primitive data type identified by AutoRepromulgation.
           */
         virtual xs::Boolean getAutoRepromulgation() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AutoRepromulgation.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAutoRepromulgation(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MessagePriority.
           *
           * @return The value of the simple primitive data type identified by MessagePriority.
           */
         virtual uci::type::CommLink16TxPG_Type::MessagePriorityValue getMessagePriority() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MessagePriority.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMessagePriority(uci::type::CommLink16TxPG_Type::MessagePriorityValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MessageTransferStorageLimit.
           *
           * @return The value of the simple primitive data type identified by MessageTransferStorageLimit.
           */
         virtual uci::type::CommLink16TxPG_Type::MessageTransferStorageLimitValue getMessageTransferStorageLimit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MessageTransferStorageLimit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMessageTransferStorageLimit(uci::type::CommLink16TxPG_Type::MessageTransferStorageLimitValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PackingLimit.
           *
           * @return The value of the enumeration identified by PackingLimit.
           */
         virtual const uci::type::CommLink16TxPackingLimitEnum& getPackingLimit() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PackingLimit.
           *
           * @return The value of the enumeration identified by PackingLimit.
           */
         virtual uci::type::CommLink16TxPackingLimitEnum& getPackingLimit()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PackingLimit.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPackingLimit(const uci::type::CommLink16TxPackingLimitEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PackingLimit.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPackingLimit(uci::type::CommLink16TxPackingLimitEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PG_Index.
           *
           * @return The value of the simple primitive data type identified by PG_Index.
           */
         virtual uci::type::CommLink16TxPG_Type::PG_IndexValue getPG_Index() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PG_Index.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPG_Index(uci::type::CommLink16TxPG_Type::PG_IndexValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SlotNumber.
           *
           * @return The value of the simple primitive data type identified by SlotNumber.
           */
         virtual uci::type::CommLink16TxPG_Type::SlotNumberValue getSlotNumber() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SlotNumber.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSlotNumber(uci::type::CommLink16TxPG_Type::SlotNumberValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the StalenessLimit.
           *
           * @return The value of the simple primitive data type identified by StalenessLimit.
           */
         virtual uci::type::CommLink16TxPG_Type::StalenessLimitValue getStalenessLimit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the StalenessLimit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setStalenessLimit(uci::type::CommLink16TxPG_Type::StalenessLimitValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommLink16TxPG_Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommLink16TxPG_Type to copy from
           */
         CommLink16TxPG_Type(const CommLink16TxPG_Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommLink16TxPG_Type to the contents of the CommLink16TxPG_Type on
           * the right hand side (rhs) of the assignment operator.CommLink16TxPG_Type [only available to derived classes]
           *
           * @param rhs The CommLink16TxPG_Type on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::CommLink16TxPG_Type
           * @return A constant reference to this CommLink16TxPG_Type.
           */
         const CommLink16TxPG_Type& operator=(const CommLink16TxPG_Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommLink16TxPG_Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommLink16TxPG_Type_h

