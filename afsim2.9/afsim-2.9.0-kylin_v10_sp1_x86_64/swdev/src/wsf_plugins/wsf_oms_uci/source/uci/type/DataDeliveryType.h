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
#ifndef Uci__Type__DataDeliveryType_h
#define Uci__Type__DataDeliveryType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

#if !defined(Uci__Type__DataDeliveryMessageType_h)
# include "uci/type/DataDeliveryMessageType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Provides a container of message delivery indicators. Used by a Gateway to provide feedback on whether a message has
        * been transmitted and / or delivered.
        */
      class DataDeliveryType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DataDeliveryType()
         { }

         /** Returns this accessor's type constant, i.e. DataDeliveryType
           *
           * @return This accessor's type constant, i.e. DataDeliveryType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dataDeliveryType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DataDeliveryType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Provides a key value pair mechanism to represent Tactical Data Format messages transmission and or delivered [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ForeignKeyType, uci::type::accessorType::foreignKeyType> TacticalMessage;

         /** Provides a mechanism for a gateway to indicate it has transmitted and or delivered a UCI Message of a particular
           * format to the destination system ID. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DataDeliveryMessageType, uci::type::accessorType::dataDeliveryMessageType> UCIMessage;

         /** Returns the bounded list that is identified by the TacticalMessage.
           *
           * @return The bounded list identified by TacticalMessage.
           */
         virtual const uci::type::DataDeliveryType::TacticalMessage& getTacticalMessage() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TacticalMessage.
           *
           * @return The bounded list identified by TacticalMessage.
           */
         virtual uci::type::DataDeliveryType::TacticalMessage& getTacticalMessage()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TacticalMessage.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTacticalMessage(const uci::type::DataDeliveryType::TacticalMessage& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the UCIMessage.
           *
           * @return The bounded list identified by UCIMessage.
           */
         virtual const uci::type::DataDeliveryType::UCIMessage& getUCIMessage() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the UCIMessage.
           *
           * @return The bounded list identified by UCIMessage.
           */
         virtual uci::type::DataDeliveryType::UCIMessage& getUCIMessage()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the UCIMessage.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setUCIMessage(const uci::type::DataDeliveryType::UCIMessage& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DataDeliveryType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DataDeliveryType to copy from
           */
         DataDeliveryType(const DataDeliveryType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DataDeliveryType to the contents of the DataDeliveryType on the
           * right hand side (rhs) of the assignment operator.DataDeliveryType [only available to derived classes]
           *
           * @param rhs The DataDeliveryType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::DataDeliveryType
           * @return A constant reference to this DataDeliveryType.
           */
         const DataDeliveryType& operator=(const DataDeliveryType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DataDeliveryType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DataDeliveryType_h

