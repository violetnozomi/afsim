// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__RF_ReportQFT_h
#define Uci__Type__RF_ReportQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeRangeBeginType_h)
# include "uci/type/DateTimeRangeBeginType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RF_ReportQFT sequence accessor class */
      class RF_ReportQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~RF_ReportQFT()
         { }

         /** Returns this accessor's type constant, i.e. RF_ReportQFT
           *
           * @return This accessor's type constant, i.e. RF_ReportQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rF_ReportQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RF_ReportQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> Frequency;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DateTimeRangeBeginType, uci::type::accessorType::dateTimeRangeBeginType> Time;

         /** Returns the bounded list that is identified by the Frequency.
           *
           * @return The bounded list identified by Frequency.
           */
         virtual const uci::type::RF_ReportQFT::Frequency& getFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Frequency.
           *
           * @return The bounded list identified by Frequency.
           */
         virtual uci::type::RF_ReportQFT::Frequency& getFrequency()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Frequency.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFrequency(const uci::type::RF_ReportQFT::Frequency& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Transmit.
           *
           * @return The value of the simple primitive data type identified by Transmit.
           */
         virtual xs::Boolean getTransmit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Transmit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTransmit(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Transmit exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Transmit is emabled or not
           */
         virtual bool hasTransmit() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Transmit
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableTransmit(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Transmit */
         virtual void clearTransmit()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Receive.
           *
           * @return The value of the simple primitive data type identified by Receive.
           */
         virtual xs::Boolean getReceive() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Receive.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setReceive(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Receive exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Receive is emabled or not
           */
         virtual bool hasReceive() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Receive
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableReceive(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Receive */
         virtual void clearReceive()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::RF_ReportQFT::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::RF_ReportQFT::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::RF_ReportQFT::CapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Time.
           *
           * @return The bounded list identified by Time.
           */
         virtual const uci::type::RF_ReportQFT::Time& getTime() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Time.
           *
           * @return The bounded list identified by Time.
           */
         virtual uci::type::RF_ReportQFT::Time& getTime()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Time.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTime(const uci::type::RF_ReportQFT::Time& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RF_ReportQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RF_ReportQFT to copy from
           */
         RF_ReportQFT(const RF_ReportQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RF_ReportQFT to the contents of the RF_ReportQFT on the right hand
           * side (rhs) of the assignment operator.RF_ReportQFT [only available to derived classes]
           *
           * @param rhs The RF_ReportQFT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::RF_ReportQFT
           * @return A constant reference to this RF_ReportQFT.
           */
         const RF_ReportQFT& operator=(const RF_ReportQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RF_ReportQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RF_ReportQFT_h

