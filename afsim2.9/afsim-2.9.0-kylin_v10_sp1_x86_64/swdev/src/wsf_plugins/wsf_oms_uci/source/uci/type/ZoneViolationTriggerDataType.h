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
#ifndef Uci__Type__ZoneViolationTriggerDataType_h
#define Uci__Type__ZoneViolationTriggerDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__OpZoneTypeEnum_h)
# include "uci/type/OpZoneTypeEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ZoneViolationTriggerDataType sequence accessor class */
      class ZoneViolationTriggerDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ZoneViolationTriggerDataType()
         { }

         /** Returns this accessor's type constant, i.e. ZoneViolationTriggerDataType
           *
           * @return This accessor's type constant, i.e. ZoneViolationTriggerDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::zoneViolationTriggerDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ZoneViolationTriggerDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the type of operational zone or region which may trigger a conflict with the current plan. See the
           * OpZoneTypeEnum enumerated value annotations for more details. [Minimum occurrences: 0] [Maximum occurrences: 22]
           */
         typedef uci::base::BoundedList<uci::type::OpZoneTypeEnum, uci::type::accessorType::opZoneTypeEnum> Category;

         /** Returns the bounded list that is identified by the Category.
           *
           * @return The bounded list identified by Category.
           */
         virtual const uci::type::ZoneViolationTriggerDataType::Category& getCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Category.
           *
           * @return The bounded list identified by Category.
           */
         virtual uci::type::ZoneViolationTriggerDataType::Category& getCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Category.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCategory(const uci::type::ZoneViolationTriggerDataType::Category& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ZoneViolationTriggerDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ZoneViolationTriggerDataType to copy from
           */
         ZoneViolationTriggerDataType(const ZoneViolationTriggerDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ZoneViolationTriggerDataType to the contents of the
           * ZoneViolationTriggerDataType on the right hand side (rhs) of the assignment operator.ZoneViolationTriggerDataType
           * [only available to derived classes]
           *
           * @param rhs The ZoneViolationTriggerDataType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::ZoneViolationTriggerDataType
           * @return A constant reference to this ZoneViolationTriggerDataType.
           */
         const ZoneViolationTriggerDataType& operator=(const ZoneViolationTriggerDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ZoneViolationTriggerDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ZoneViolationTriggerDataType_h

