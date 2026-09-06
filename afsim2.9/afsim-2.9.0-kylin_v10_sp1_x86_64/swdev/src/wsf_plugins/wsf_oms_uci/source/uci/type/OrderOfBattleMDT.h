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
#ifndef Uci__Type__OrderOfBattleMDT_h
#define Uci__Type__OrderOfBattleMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__OpZoneID_Type_h)
# include "uci/type/OpZoneID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__UnitType_h)
# include "uci/type/UnitType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the OrderOfBattleMDT sequence accessor class */
      class OrderOfBattleMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~OrderOfBattleMDT()
         { }

         /** Returns this accessor's type constant, i.e. OrderOfBattleMDT
           *
           * @return This accessor's type constant, i.e. OrderOfBattleMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::orderOfBattleMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const OrderOfBattleMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** The Order of Battle Unit records that were found within the OpZone associated with this Order of Battle. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::UnitType, uci::type::accessorType::unitType> Unit;

         /** Returns the accessor that provides access to the complex content that is identified by the OpZoneID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpZoneID.
           */
         virtual const uci::type::OpZoneID_Type& getOpZoneID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OpZoneID.
           *
           * @return The acecssor that provides access to the complex content that is identified by OpZoneID.
           */
         virtual uci::type::OpZoneID_Type& getOpZoneID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OpZoneID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OpZoneID
           */
         virtual void setOpZoneID(const uci::type::OpZoneID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OpZoneID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OpZoneID is emabled or not
           */
         virtual bool hasOpZoneID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OpZoneID
           *
           * @param type = uci::type::accessorType::opZoneID_Type This Accessor's accessor type
           */
         virtual void enableOpZoneID(uci::base::accessorType::AccessorType type = uci::type::accessorType::opZoneID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OpZoneID */
         virtual void clearOpZoneID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the InitialCreationTime.
           *
           * @return The value of the simple primitive data type identified by InitialCreationTime.
           */
         virtual uci::type::DateTimeTypeValue getInitialCreationTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the InitialCreationTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInitialCreationTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InitialCreationTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InitialCreationTime is emabled or not
           */
         virtual bool hasInitialCreationTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InitialCreationTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableInitialCreationTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InitialCreationTime */
         virtual void clearInitialCreationTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the LastUpdateTime.
           *
           * @return The value of the simple primitive data type identified by LastUpdateTime.
           */
         virtual uci::type::DateTimeTypeValue getLastUpdateTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the LastUpdateTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLastUpdateTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LastUpdateTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LastUpdateTime is emabled or not
           */
         virtual bool hasLastUpdateTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LastUpdateTime
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableLastUpdateTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LastUpdateTime */
         virtual void clearLastUpdateTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Unit.
           *
           * @return The bounded list identified by Unit.
           */
         virtual const uci::type::OrderOfBattleMDT::Unit& getUnit() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Unit.
           *
           * @return The bounded list identified by Unit.
           */
         virtual uci::type::OrderOfBattleMDT::Unit& getUnit()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Unit.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setUnit(const uci::type::OrderOfBattleMDT::Unit& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         OrderOfBattleMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The OrderOfBattleMDT to copy from
           */
         OrderOfBattleMDT(const OrderOfBattleMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this OrderOfBattleMDT to the contents of the OrderOfBattleMDT on the
           * right hand side (rhs) of the assignment operator.OrderOfBattleMDT [only available to derived classes]
           *
           * @param rhs The OrderOfBattleMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::OrderOfBattleMDT
           * @return A constant reference to this OrderOfBattleMDT.
           */
         const OrderOfBattleMDT& operator=(const OrderOfBattleMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // OrderOfBattleMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__OrderOfBattleMDT_h

