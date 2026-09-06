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
#ifndef Uci__Type__RF_ResourceAllocationMDT_h
#define Uci__Type__RF_ResourceAllocationMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ResourceRequestID_Type_h)
# include "uci/type/ResourceRequestID_Type.h"
#endif

#if !defined(Uci__Type__RequestingFunctionType_h)
# include "uci/type/RequestingFunctionType.h"
#endif

#if !defined(Uci__Type__ResourceAllocationStateEnum_h)
# include "uci/type/ResourceAllocationStateEnum.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ResourceDefinitionType_h)
# include "uci/type/ResourceDefinitionType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RF_ResourceAllocationMDT sequence accessor class */
      class RF_ResourceAllocationMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RF_ResourceAllocationMDT()
         { }

         /** Returns this accessor's type constant, i.e. RF_ResourceAllocationMDT
           *
           * @return This accessor's type constant, i.e. RF_ResourceAllocationMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rF_ResourceAllocationMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RF_ResourceAllocationMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Describes the allocated resources and allocation limits. Only one SystemThreadIndex can be allocated per
           * RF_ResourceAllocation; two ResourceDefinitions are allowed in order to show separate TX and RX cases for the same
           * SystemThreadIndex. Required if sibling ResourceAllocationState is NEW or UPDATE. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ResourceDefinitionType, uci::type::accessorType::resourceDefinitionType> ResourceDefinitions;

         /** Returns the accessor that provides access to the complex content that is identified by the ResourceRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ResourceRequestID.
           */
         virtual const uci::type::ResourceRequestID_Type& getResourceRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ResourceRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ResourceRequestID.
           */
         virtual uci::type::ResourceRequestID_Type& getResourceRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ResourceRequestID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ResourceRequestID
           */
         virtual void setResourceRequestID(const uci::type::ResourceRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RequestingFunction.
           *
           * @return The acecssor that provides access to the complex content that is identified by RequestingFunction.
           */
         virtual const uci::type::RequestingFunctionType& getRequestingFunction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RequestingFunction.
           *
           * @return The acecssor that provides access to the complex content that is identified by RequestingFunction.
           */
         virtual uci::type::RequestingFunctionType& getRequestingFunction()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RequestingFunction to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RequestingFunction
           */
         virtual void setRequestingFunction(const uci::type::RequestingFunctionType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ResourceAllocationState.
           *
           * @return The value of the enumeration identified by ResourceAllocationState.
           */
         virtual const uci::type::ResourceAllocationStateEnum& getResourceAllocationState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ResourceAllocationState.
           *
           * @return The value of the enumeration identified by ResourceAllocationState.
           */
         virtual uci::type::ResourceAllocationStateEnum& getResourceAllocationState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ResourceAllocationState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setResourceAllocationState(const uci::type::ResourceAllocationStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ResourceAllocationState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setResourceAllocationState(uci::type::ResourceAllocationStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DurationRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by DurationRange.
           */
         virtual const uci::type::DateTimeRangeType& getDurationRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DurationRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by DurationRange.
           */
         virtual uci::type::DateTimeRangeType& getDurationRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DurationRange to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DurationRange
           */
         virtual void setDurationRange(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DurationRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DurationRange is emabled or not
           */
         virtual bool hasDurationRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DurationRange
           *
           * @param type = uci::type::accessorType::dateTimeRangeType This Accessor's accessor type
           */
         virtual void enableDurationRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DurationRange */
         virtual void clearDurationRange()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ConfigureInteroperabilityReportIndex.
           *
           * @return The value of the simple primitive data type identified by ConfigureInteroperabilityReportIndex.
           */
         virtual xs::UnsignedInt getConfigureInteroperabilityReportIndex() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ConfigureInteroperabilityReportIndex.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setConfigureInteroperabilityReportIndex(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ConfigureInteroperabilityReportIndex exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ConfigureInteroperabilityReportIndex is emabled or not
           */
         virtual bool hasConfigureInteroperabilityReportIndex() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ConfigureInteroperabilityReportIndex
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableConfigureInteroperabilityReportIndex(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ConfigureInteroperabilityReportIndex */
         virtual void clearConfigureInteroperabilityReportIndex()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ResourceDefinitions.
           *
           * @return The bounded list identified by ResourceDefinitions.
           */
         virtual const uci::type::RF_ResourceAllocationMDT::ResourceDefinitions& getResourceDefinitions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ResourceDefinitions.
           *
           * @return The bounded list identified by ResourceDefinitions.
           */
         virtual uci::type::RF_ResourceAllocationMDT::ResourceDefinitions& getResourceDefinitions()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ResourceDefinitions.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setResourceDefinitions(const uci::type::RF_ResourceAllocationMDT::ResourceDefinitions& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RF_ResourceAllocationMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RF_ResourceAllocationMDT to copy from
           */
         RF_ResourceAllocationMDT(const RF_ResourceAllocationMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RF_ResourceAllocationMDT to the contents of the
           * RF_ResourceAllocationMDT on the right hand side (rhs) of the assignment operator.RF_ResourceAllocationMDT [only
           * available to derived classes]
           *
           * @param rhs The RF_ResourceAllocationMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::RF_ResourceAllocationMDT
           * @return A constant reference to this RF_ResourceAllocationMDT.
           */
         const RF_ResourceAllocationMDT& operator=(const RF_ResourceAllocationMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RF_ResourceAllocationMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RF_ResourceAllocationMDT_h

