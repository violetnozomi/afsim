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
#ifndef Uci__Type__COMINT_ActivityMDT_h
#define Uci__Type__COMINT_ActivityMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__COMINT_ActivityType_h)
# include "uci/type/COMINT_ActivityType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the COMINT_ActivityMDT sequence accessor class */
      class COMINT_ActivityMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~COMINT_ActivityMDT()
         { }

         /** Returns this accessor's type constant, i.e. COMINT_ActivityMDT
           *
           * @return This accessor's type constant, i.e. COMINT_ActivityMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cOMINT_ActivityMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const COMINT_ActivityMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the dynamic operational characteristics of an Activity of a Capability. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::COMINT_ActivityType, uci::type::accessorType::cOMINT_ActivityType> Activity;

         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual const uci::type::SubsystemID_Type& getSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual uci::type::SubsystemID_Type& getSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubsystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubsystemID
           */
         virtual void setSubsystemID(const uci::type::SubsystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EntityListCleared.
           *
           * @return The value of the simple primitive data type identified by EntityListCleared.
           */
         virtual uci::type::DateTimeTypeValue getEntityListCleared() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EntityListCleared.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEntityListCleared(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EntityListCleared exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EntityListCleared is emabled or not
           */
         virtual bool hasEntityListCleared() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EntityListCleared
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableEntityListCleared(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EntityListCleared */
         virtual void clearEntityListCleared()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EntityOverload.
           *
           * @return The value of the simple primitive data type identified by EntityOverload.
           */
         virtual xs::Boolean getEntityOverload() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EntityOverload.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEntityOverload(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EntityOverload exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EntityOverload is emabled or not
           */
         virtual bool hasEntityOverload() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EntityOverload
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableEntityOverload(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EntityOverload */
         virtual void clearEntityOverload()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ExcessiveAttitude.
           *
           * @return The value of the simple primitive data type identified by ExcessiveAttitude.
           */
         virtual xs::Boolean getExcessiveAttitude() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ExcessiveAttitude.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setExcessiveAttitude(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ExcessiveAttitude exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ExcessiveAttitude is emabled or not
           */
         virtual bool hasExcessiveAttitude() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ExcessiveAttitude
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableExcessiveAttitude(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ExcessiveAttitude */
         virtual void clearExcessiveAttitude()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PulseDensityOverload.
           *
           * @return The value of the simple primitive data type identified by PulseDensityOverload.
           */
         virtual xs::Boolean getPulseDensityOverload() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PulseDensityOverload.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPulseDensityOverload(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PulseDensityOverload exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PulseDensityOverload is emabled or not
           */
         virtual bool hasPulseDensityOverload() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PulseDensityOverload
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enablePulseDensityOverload(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PulseDensityOverload */
         virtual void clearPulseDensityOverload()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the LookThroughLimited.
           *
           * @return The value of the simple primitive data type identified by LookThroughLimited.
           */
         virtual xs::Boolean getLookThroughLimited() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the LookThroughLimited.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLookThroughLimited(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LookThroughLimited exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LookThroughLimited is emabled or not
           */
         virtual bool hasLookThroughLimited() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LookThroughLimited
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableLookThroughLimited(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LookThroughLimited */
         virtual void clearLookThroughLimited()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ReceiverUtilization.
           *
           * @return The value of the simple primitive data type identified by ReceiverUtilization.
           */
         virtual uci::type::PercentTypeValue getReceiverUtilization() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ReceiverUtilization.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setReceiverUtilization(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ReceiverUtilization exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReceiverUtilization is emabled or not
           */
         virtual bool hasReceiverUtilization() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReceiverUtilization
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableReceiverUtilization(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReceiverUtilization */
         virtual void clearReceiverUtilization()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Activity.
           *
           * @return The bounded list identified by Activity.
           */
         virtual const uci::type::COMINT_ActivityMDT::Activity& getActivity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Activity.
           *
           * @return The bounded list identified by Activity.
           */
         virtual uci::type::COMINT_ActivityMDT::Activity& getActivity()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Activity.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setActivity(const uci::type::COMINT_ActivityMDT::Activity& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         COMINT_ActivityMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The COMINT_ActivityMDT to copy from
           */
         COMINT_ActivityMDT(const COMINT_ActivityMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this COMINT_ActivityMDT to the contents of the COMINT_ActivityMDT on
           * the right hand side (rhs) of the assignment operator.COMINT_ActivityMDT [only available to derived classes]
           *
           * @param rhs The COMINT_ActivityMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::COMINT_ActivityMDT
           * @return A constant reference to this COMINT_ActivityMDT.
           */
         const COMINT_ActivityMDT& operator=(const COMINT_ActivityMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // COMINT_ActivityMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__COMINT_ActivityMDT_h

