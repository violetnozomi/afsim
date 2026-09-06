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
#ifndef Uci__Type__CommMADL_NetChangeSelectionType_h
#define Uci__Type__CommMADL_NetChangeSelectionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommMADL_NodeRangeType_h)
# include "uci/type/CommMADL_NodeRangeType.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

#if !defined(Uci__Type__CommMADL_FlightGroupType_h)
# include "uci/type/CommMADL_FlightGroupType.h"
#endif

#if !defined(Uci__Type__CommMADL_NetworkType_h)
# include "uci/type/CommMADL_NetworkType.h"
#endif

#if !defined(Uci__Type__CommMADL_NetworkLayoutType_h)
# include "uci/type/CommMADL_NetworkLayoutType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommMADL_NetChangeSelectionType sequence accessor class */
      class CommMADL_NetChangeSelectionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommMADL_NetChangeSelectionType()
         { }

         /** Returns this accessor's type constant, i.e. CommMADL_NetChangeSelectionType
           *
           * @return This accessor's type constant, i.e. CommMADL_NetChangeSelectionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commMADL_NetChangeSelectionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommMADL_NetChangeSelectionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum inclusive: -63] [Maximum inclusive: 63] */
         typedef uci::base::IntAccessor SelectionShiftValue;
         //  The type of primitive data accessed by SelectionShiftValue
         typedef xs::Int SelectionShiftValueValue;

         /** Returns the value of the SimplePrimitive data type that is identified by the SelectionStartNode.
           *
           * @return The value of the simple primitive data type identified by SelectionStartNode.
           */
         virtual uci::type::CommMADL_NodeRangeTypeValue getSelectionStartNode() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SelectionStartNode.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSelectionStartNode(uci::type::CommMADL_NodeRangeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SelectionLength.
           *
           * @return The value of the simple primitive data type identified by SelectionLength.
           */
         virtual uci::type::CommMADL_NodeRangeTypeValue getSelectionLength() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SelectionLength.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSelectionLength(uci::type::CommMADL_NodeRangeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SelectionShiftValue.
           *
           * @return The value of the simple primitive data type identified by SelectionShiftValue.
           */
         virtual uci::type::CommMADL_NetChangeSelectionType::SelectionShiftValueValue getSelectionShiftValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SelectionShiftValue.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSelectionShiftValue(uci::type::CommMADL_NetChangeSelectionType::SelectionShiftValueValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SelectionShiftValue exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SelectionShiftValue is emabled or not
           */
         virtual bool hasSelectionShiftValue() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SelectionShiftValue
           *
           * @param type = uci::type::commMADL_NetChangeSelectionType::accessorType::selectionShiftValue This Accessor's accessor
           *      type
           */
         virtual void enableSelectionShiftValue(uci::base::accessorType::AccessorType type = uci::type::commMADL_NetChangeSelectionType::accessorType::selectionShiftValue)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SelectionShiftValue */
         virtual void clearSelectionShiftValue()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SelectionFlightGroupID.
           *
           * @return The value of the string data type identified by SelectionFlightGroupID.
           */
         virtual const uci::type::CommMADL_FlightGroupType& getSelectionFlightGroupID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SelectionFlightGroupID.
           *
           * @return The value of the string data type identified by SelectionFlightGroupID.
           */
         virtual uci::type::CommMADL_FlightGroupType& getSelectionFlightGroupID()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SelectionFlightGroupID to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSelectionFlightGroupID(const uci::type::CommMADL_FlightGroupType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SelectionFlightGroupID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSelectionFlightGroupID(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SelectionFlightGroupID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSelectionFlightGroupID(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SelectionFlightGroupID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SelectionFlightGroupID is emabled or not
           */
         virtual bool hasSelectionFlightGroupID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SelectionFlightGroupID
           *
           * @param type = uci::type::accessorType::commMADL_FlightGroupType This Accessor's accessor type
           */
         virtual void enableSelectionFlightGroupID(uci::base::accessorType::AccessorType type = uci::type::accessorType::commMADL_FlightGroupType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SelectionFlightGroupID */
         virtual void clearSelectionFlightGroupID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NewNetworkID.
           *
           * @return The acecssor that provides access to the complex content that is identified by NewNetworkID.
           */
         virtual const uci::type::CommMADL_NetworkType& getNewNetworkID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NewNetworkID.
           *
           * @return The acecssor that provides access to the complex content that is identified by NewNetworkID.
           */
         virtual uci::type::CommMADL_NetworkType& getNewNetworkID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NewNetworkID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NewNetworkID
           */
         virtual void setNewNetworkID(const uci::type::CommMADL_NetworkType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NewNetworkLayout.
           *
           * @return The acecssor that provides access to the complex content that is identified by NewNetworkLayout.
           */
         virtual const uci::type::CommMADL_NetworkLayoutType& getNewNetworkLayout() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NewNetworkLayout.
           *
           * @return The acecssor that provides access to the complex content that is identified by NewNetworkLayout.
           */
         virtual uci::type::CommMADL_NetworkLayoutType& getNewNetworkLayout()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NewNetworkLayout to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NewNetworkLayout
           */
         virtual void setNewNetworkLayout(const uci::type::CommMADL_NetworkLayoutType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommMADL_NetChangeSelectionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommMADL_NetChangeSelectionType to copy from
           */
         CommMADL_NetChangeSelectionType(const CommMADL_NetChangeSelectionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommMADL_NetChangeSelectionType to the contents of the
           * CommMADL_NetChangeSelectionType on the right hand side (rhs) of the assignment
           * operator.CommMADL_NetChangeSelectionType [only available to derived classes]
           *
           * @param rhs The CommMADL_NetChangeSelectionType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::CommMADL_NetChangeSelectionType
           * @return A constant reference to this CommMADL_NetChangeSelectionType.
           */
         const CommMADL_NetChangeSelectionType& operator=(const CommMADL_NetChangeSelectionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommMADL_NetChangeSelectionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommMADL_NetChangeSelectionType_h

