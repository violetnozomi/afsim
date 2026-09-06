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
#ifndef Uci__Type__CommLink16AdvancedSlotNotificationType_h
#define Uci__Type__CommLink16AdvancedSlotNotificationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommLink16_AdvanceSlotNotificationModeEnum_h)
# include "uci/type/CommLink16_AdvanceSlotNotificationModeEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Contains the Advanced Slot Notification for both modes. Indicates Advanced Slot Notification (ASN) Mode Selection
        * (MS) of Mode A (1) or Mode B (0), ASN Enabled (1) or Disabled (0) and the ASN Slot Selection (0..31, the default is
        * 7).
        */
      class CommLink16AdvancedSlotNotificationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommLink16AdvancedSlotNotificationType()
         { }

         /** Returns this accessor's type constant, i.e. CommLink16AdvancedSlotNotificationType
           *
           * @return This accessor's type constant, i.e. CommLink16AdvancedSlotNotificationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commLink16AdvancedSlotNotificationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommLink16AdvancedSlotNotificationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Denotes the ASN slot which is selected. Must be between 0 and 31, default is 7.[Default: 7]; [Minimum inclusive: 0]
           * [Maximum inclusive: 31]
           */
         typedef uci::base::IntAccessor ASN_SlotSelection;
         //  The type of primitive data accessed by ASN_SlotSelection
         typedef xs::Int ASN_SlotSelectionValue;

         /** Returns the value of the enumeration that is identified by the ASN_SlotMode.
           *
           * @return The value of the enumeration identified by ASN_SlotMode.
           */
         virtual const uci::type::CommLink16_AdvanceSlotNotificationModeEnum& getASN_SlotMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ASN_SlotMode.
           *
           * @return The value of the enumeration identified by ASN_SlotMode.
           */
         virtual uci::type::CommLink16_AdvanceSlotNotificationModeEnum& getASN_SlotMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ASN_SlotMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setASN_SlotMode(const uci::type::CommLink16_AdvanceSlotNotificationModeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ASN_SlotMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setASN_SlotMode(uci::type::CommLink16_AdvanceSlotNotificationModeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ASN_SlotModeEnabled.
           *
           * @return The value of the simple primitive data type identified by ASN_SlotModeEnabled.
           */
         virtual xs::Boolean getASN_SlotModeEnabled() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ASN_SlotModeEnabled.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setASN_SlotModeEnabled(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ASN_SlotSelection.
           *
           * @return The value of the simple primitive data type identified by ASN_SlotSelection.
           */
         virtual uci::type::CommLink16AdvancedSlotNotificationType::ASN_SlotSelectionValue getASN_SlotSelection() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ASN_SlotSelection.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setASN_SlotSelection(uci::type::CommLink16AdvancedSlotNotificationType::ASN_SlotSelectionValue value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommLink16AdvancedSlotNotificationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommLink16AdvancedSlotNotificationType to copy from
           */
         CommLink16AdvancedSlotNotificationType(const CommLink16AdvancedSlotNotificationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommLink16AdvancedSlotNotificationType to the contents of the
           * CommLink16AdvancedSlotNotificationType on the right hand side (rhs) of the assignment
           * operator.CommLink16AdvancedSlotNotificationType [only available to derived classes]
           *
           * @param rhs The CommLink16AdvancedSlotNotificationType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::CommLink16AdvancedSlotNotificationType
           * @return A constant reference to this CommLink16AdvancedSlotNotificationType.
           */
         const CommLink16AdvancedSlotNotificationType& operator=(const CommLink16AdvancedSlotNotificationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommLink16AdvancedSlotNotificationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommLink16AdvancedSlotNotificationType_h

