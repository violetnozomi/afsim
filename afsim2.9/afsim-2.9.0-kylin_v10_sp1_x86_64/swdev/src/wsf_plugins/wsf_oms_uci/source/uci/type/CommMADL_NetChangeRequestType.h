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
#ifndef Uci__Type__CommMADL_NetChangeRequestType_h
#define Uci__Type__CommMADL_NetChangeRequestType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommMADL_NetChangeOpEnum_h)
# include "uci/type/CommMADL_NetChangeOpEnum.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__CommMADL_NetChangeSelectionListType_h)
# include "uci/type/CommMADL_NetChangeSelectionListType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Utilized to perform MADL Specific tuning of a communications capability. */
      class CommMADL_NetChangeRequestType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommMADL_NetChangeRequestType()
         { }

         /** Returns this accessor's type constant, i.e. CommMADL_NetChangeRequestType
           *
           * @return This accessor's type constant, i.e. CommMADL_NetChangeRequestType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commMADL_NetChangeRequestType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommMADL_NetChangeRequestType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Specifies the extent and approach for search during acquisition on link.; [Minimum inclusive: 1] [Maximum inclusive:
           * 2]
           */
         typedef uci::base::UnsignedIntAccessor NumSelections;
         //  The type of primitive data accessed by NumSelections
         typedef xs::UnsignedInt NumSelectionsValue;

         /** Returns the value of the enumeration that is identified by the ChangeOperator.
           *
           * @return The value of the enumeration identified by ChangeOperator.
           */
         virtual const uci::type::CommMADL_NetChangeOpEnum& getChangeOperator() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ChangeOperator.
           *
           * @return The value of the enumeration identified by ChangeOperator.
           */
         virtual uci::type::CommMADL_NetChangeOpEnum& getChangeOperator()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ChangeOperator.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setChangeOperator(const uci::type::CommMADL_NetChangeOpEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ChangeOperator.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setChangeOperator(uci::type::CommMADL_NetChangeOpEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NumSelections.
           *
           * @return The value of the simple primitive data type identified by NumSelections.
           */
         virtual uci::type::CommMADL_NetChangeRequestType::NumSelectionsValue getNumSelections() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NumSelections.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNumSelections(uci::type::CommMADL_NetChangeRequestType::NumSelectionsValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NetChangeSelection.
           *
           * @return The acecssor that provides access to the complex content that is identified by NetChangeSelection.
           */
         virtual const uci::type::CommMADL_NetChangeSelectionListType& getNetChangeSelection() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NetChangeSelection.
           *
           * @return The acecssor that provides access to the complex content that is identified by NetChangeSelection.
           */
         virtual uci::type::CommMADL_NetChangeSelectionListType& getNetChangeSelection()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NetChangeSelection to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NetChangeSelection
           */
         virtual void setNetChangeSelection(const uci::type::CommMADL_NetChangeSelectionListType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommMADL_NetChangeRequestType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommMADL_NetChangeRequestType to copy from
           */
         CommMADL_NetChangeRequestType(const CommMADL_NetChangeRequestType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommMADL_NetChangeRequestType to the contents of the
           * CommMADL_NetChangeRequestType on the right hand side (rhs) of the assignment operator.CommMADL_NetChangeRequestType
           * [only available to derived classes]
           *
           * @param rhs The CommMADL_NetChangeRequestType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::CommMADL_NetChangeRequestType
           * @return A constant reference to this CommMADL_NetChangeRequestType.
           */
         const CommMADL_NetChangeRequestType& operator=(const CommMADL_NetChangeRequestType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommMADL_NetChangeRequestType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommMADL_NetChangeRequestType_h

