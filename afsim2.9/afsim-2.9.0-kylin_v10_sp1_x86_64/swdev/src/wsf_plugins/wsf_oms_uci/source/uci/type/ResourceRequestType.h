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
#ifndef Uci__Type__ResourceRequestType_h
#define Uci__Type__ResourceRequestType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DependentActivityType_h)
# include "uci/type/DependentActivityType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Type__RF_TypeEnum_h)
# include "uci/type/RF_TypeEnum.h"
#endif

#if !defined(Uci__Type__ResourceRequestPeriodType_h)
# include "uci/type/ResourceRequestPeriodType.h"
#endif

#if !defined(Uci__Type__SelectComponentResourceType_h)
# include "uci/type/SelectComponentResourceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ResourceRequestType sequence accessor class */
      class ResourceRequestType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ResourceRequestType()
         { }

         /** Returns this accessor's type constant, i.e. ResourceRequestType
           *
           * @return This accessor's type constant, i.e. ResourceRequestType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::resourceRequestType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ResourceRequestType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This indicates Activity's that this Activity is dependent on. If the dependent Activity's Resource Allocation is
           * removed or never allocated, this Activity's Resource Allocation will be removed or not fulfilled. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DependentActivityType, uci::type::accessorType::dependentActivityType> DependentActivity;

         /** Defines series of aperture access requests by the Subsystem. If the aperture access behavior is homogeneous, (such as
           * unchanging throughout a threat scan period) only one AccessPeriod is described. If the payload has different
           * behaviors such as higher duty cycle during a beam passing and less aperture access between beam passings, then two
           * AccessPeriods are described. It is assumed, if there are two AccessPeriods, that they alternate. [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ResourceRequestPeriodType, uci::type::accessorType::resourceRequestPeriodType> AccessPeriod;

         /** Returns the bounded list that is identified by the DependentActivity.
           *
           * @return The bounded list identified by DependentActivity.
           */
         virtual const uci::type::ResourceRequestType::DependentActivity& getDependentActivity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DependentActivity.
           *
           * @return The bounded list identified by DependentActivity.
           */
         virtual uci::type::ResourceRequestType::DependentActivity& getDependentActivity()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DependentActivity.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDependentActivity(const uci::type::ResourceRequestType::DependentActivity& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NumberOfGainSettings.
           *
           * @return The value of the simple primitive data type identified by NumberOfGainSettings.
           */
         virtual xs::UnsignedInt getNumberOfGainSettings() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NumberOfGainSettings.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNumberOfGainSettings(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaxPercentInterruption.
           *
           * @return The value of the simple primitive data type identified by MaxPercentInterruption.
           */
         virtual uci::type::PercentTypeValue getMaxPercentInterruption() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaxPercentInterruption.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaxPercentInterruption(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RF_Mode.
           *
           * @return The value of the enumeration identified by RF_Mode.
           */
         virtual const uci::type::RF_TypeEnum& getRF_Mode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the RF_Mode.
           *
           * @return The value of the enumeration identified by RF_Mode.
           */
         virtual uci::type::RF_TypeEnum& getRF_Mode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RF_Mode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRF_Mode(const uci::type::RF_TypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the RF_Mode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setRF_Mode(uci::type::RF_TypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AccessPeriod.
           *
           * @return The bounded list identified by AccessPeriod.
           */
         virtual const uci::type::ResourceRequestType::AccessPeriod& getAccessPeriod() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AccessPeriod.
           *
           * @return The bounded list identified by AccessPeriod.
           */
         virtual uci::type::ResourceRequestType::AccessPeriod& getAccessPeriod()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AccessPeriod.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAccessPeriod(const uci::type::ResourceRequestType::AccessPeriod& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SelectResource.
           *
           * @return The acecssor that provides access to the complex content that is identified by SelectResource.
           */
         virtual const uci::type::SelectComponentResourceType& getSelectResource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SelectResource.
           *
           * @return The acecssor that provides access to the complex content that is identified by SelectResource.
           */
         virtual uci::type::SelectComponentResourceType& getSelectResource()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SelectResource to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SelectResource
           */
         virtual void setSelectResource(const uci::type::SelectComponentResourceType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ResourceRequestType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ResourceRequestType to copy from
           */
         ResourceRequestType(const ResourceRequestType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ResourceRequestType to the contents of the ResourceRequestType on
           * the right hand side (rhs) of the assignment operator.ResourceRequestType [only available to derived classes]
           *
           * @param rhs The ResourceRequestType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::ResourceRequestType
           * @return A constant reference to this ResourceRequestType.
           */
         const ResourceRequestType& operator=(const ResourceRequestType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ResourceRequestType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ResourceRequestType_h

