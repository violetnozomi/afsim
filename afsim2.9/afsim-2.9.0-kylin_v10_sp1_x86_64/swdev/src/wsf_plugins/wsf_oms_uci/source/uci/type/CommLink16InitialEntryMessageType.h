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
#ifndef Uci__Type__CommLink16InitialEntryMessageType_h
#define Uci__Type__CommLink16InitialEntryMessageType_h 1

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

      /** Indicates which Time Slot Assignments, defined in the received initial entry messages, are to be used upon passing
        * all current validity checks.
        */
      class CommLink16InitialEntryMessageType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommLink16InitialEntryMessageType()
         { }

         /** Returns this accessor's type constant, i.e. CommLink16InitialEntryMessageType
           *
           * @return This accessor's type constant, i.e. CommLink16InitialEntryMessageType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commLink16InitialEntryMessageType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommLink16InitialEntryMessageType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ControlTimeSlotInhibit.
           *
           * @return The value of the simple primitive data type identified by ControlTimeSlotInhibit.
           */
         virtual xs::Boolean getControlTimeSlotInhibit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ControlTimeSlotInhibit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setControlTimeSlotInhibit(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ControlTimeSlotInhibit exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ControlTimeSlotInhibit is emabled or not
           */
         virtual bool hasControlTimeSlotInhibit() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ControlTimeSlotInhibit
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableControlTimeSlotInhibit(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ControlTimeSlotInhibit */
         virtual void clearControlTimeSlotInhibit()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PPLIA_TimeSlotInhibit.
           *
           * @return The value of the simple primitive data type identified by PPLIA_TimeSlotInhibit.
           */
         virtual xs::Boolean getPPLIA_TimeSlotInhibit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PPLIA_TimeSlotInhibit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPPLIA_TimeSlotInhibit(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PPLIA_TimeSlotInhibit exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PPLIA_TimeSlotInhibit is emabled or not
           */
         virtual bool hasPPLIA_TimeSlotInhibit() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PPLIA_TimeSlotInhibit
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enablePPLIA_TimeSlotInhibit(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PPLIA_TimeSlotInhibit */
         virtual void clearPPLIA_TimeSlotInhibit()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PPLIB_TimeSlotInhibit.
           *
           * @return The value of the simple primitive data type identified by PPLIB_TimeSlotInhibit.
           */
         virtual xs::Boolean getPPLIB_TimeSlotInhibit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PPLIB_TimeSlotInhibit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPPLIB_TimeSlotInhibit(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PPLIB_TimeSlotInhibit exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PPLIB_TimeSlotInhibit is emabled or not
           */
         virtual bool hasPPLIB_TimeSlotInhibit() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PPLIB_TimeSlotInhibit
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enablePPLIB_TimeSlotInhibit(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PPLIB_TimeSlotInhibit */
         virtual void clearPPLIB_TimeSlotInhibit()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RTTB_TimeSlotInhibit.
           *
           * @return The value of the simple primitive data type identified by RTTB_TimeSlotInhibit.
           */
         virtual xs::Boolean getRTTB_TimeSlotInhibit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RTTB_TimeSlotInhibit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRTTB_TimeSlotInhibit(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RTTB_TimeSlotInhibit exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RTTB_TimeSlotInhibit is emabled or not
           */
         virtual bool hasRTTB_TimeSlotInhibit() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RTTB_TimeSlotInhibit
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableRTTB_TimeSlotInhibit(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RTTB_TimeSlotInhibit */
         virtual void clearRTTB_TimeSlotInhibit()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VoiceA_TimeSlotInhibit.
           *
           * @return The value of the simple primitive data type identified by VoiceA_TimeSlotInhibit.
           */
         virtual xs::Boolean getVoiceA_TimeSlotInhibit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VoiceA_TimeSlotInhibit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVoiceA_TimeSlotInhibit(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VoiceA_TimeSlotInhibit exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VoiceA_TimeSlotInhibit is emabled or not
           */
         virtual bool hasVoiceA_TimeSlotInhibit() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VoiceA_TimeSlotInhibit
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableVoiceA_TimeSlotInhibit(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VoiceA_TimeSlotInhibit */
         virtual void clearVoiceA_TimeSlotInhibit()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VoiceBTimeSlotInhibit.
           *
           * @return The value of the simple primitive data type identified by VoiceBTimeSlotInhibit.
           */
         virtual xs::Boolean getVoiceBTimeSlotInhibit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VoiceBTimeSlotInhibit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVoiceBTimeSlotInhibit(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VoiceBTimeSlotInhibit exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VoiceBTimeSlotInhibit is emabled or not
           */
         virtual bool hasVoiceBTimeSlotInhibit() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VoiceBTimeSlotInhibit
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableVoiceBTimeSlotInhibit(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VoiceBTimeSlotInhibit */
         virtual void clearVoiceBTimeSlotInhibit()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommLink16InitialEntryMessageType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommLink16InitialEntryMessageType to copy from
           */
         CommLink16InitialEntryMessageType(const CommLink16InitialEntryMessageType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommLink16InitialEntryMessageType to the contents of the
           * CommLink16InitialEntryMessageType on the right hand side (rhs) of the assignment
           * operator.CommLink16InitialEntryMessageType [only available to derived classes]
           *
           * @param rhs The CommLink16InitialEntryMessageType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::CommLink16InitialEntryMessageType
           * @return A constant reference to this CommLink16InitialEntryMessageType.
           */
         const CommLink16InitialEntryMessageType& operator=(const CommLink16InitialEntryMessageType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommLink16InitialEntryMessageType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommLink16InitialEntryMessageType_h

