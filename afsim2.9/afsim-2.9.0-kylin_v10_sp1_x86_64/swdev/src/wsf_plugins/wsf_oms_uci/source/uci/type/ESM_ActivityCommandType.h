// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__ESM_ActivityCommandType_h
#define Uci__Type__ESM_ActivityCommandType_h 1

#if !defined(Uci__Type__ActivityCommandBaseType_h)
# include "uci/type/ActivityCommandBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ESM_ChangeDwellType_h)
# include "uci/type/ESM_ChangeDwellType.h"
#endif

#if !defined(Uci__Type__ESM_ChangeGeolocationType_h)
# include "uci/type/ESM_ChangeGeolocationType.h"
#endif

#if !defined(Uci__Type__DependentActivityType_h)
# include "uci/type/DependentActivityType.h"
#endif

#if !defined(Uci__Type__SubCapabilityDetailsType_h)
# include "uci/type/SubCapabilityDetailsType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ESM_ActivityCommandType sequence accessor class */
      class ESM_ActivityCommandType : public virtual uci::type::ActivityCommandBaseType {
      public:

         /** The destructor */
         virtual ~ESM_ActivityCommandType()
         { }

         /** Returns this accessor's type constant, i.e. ESM_ActivityCommandType
           *
           * @return This accessor's type constant, i.e. ESM_ActivityCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eSM_ActivityCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ESM_ActivityCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Identifies the specific dwell that ESM should enable, disable, unallocate, or delete. The dwell's identity is
           * originally defined in the optional ReceiveDwell structure of the ESM_Activity message. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ESM_ChangeDwellType, uci::type::accessorType::eSM_ChangeDwellType> ESM_ChangeDwell;

         /** This indicates the details required for a SubCapability, at least one of the data sets need to be populated. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SubCapabilityDetailsType, uci::type::accessorType::subCapabilityDetailsType> ESM_ChangeSubCapabilityDetails;

         /** Returns the bounded list that is identified by the ESM_ChangeDwell.
           *
           * @return The bounded list identified by ESM_ChangeDwell.
           */
         virtual const uci::type::ESM_ActivityCommandType::ESM_ChangeDwell& getESM_ChangeDwell() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ESM_ChangeDwell.
           *
           * @return The bounded list identified by ESM_ChangeDwell.
           */
         virtual uci::type::ESM_ActivityCommandType::ESM_ChangeDwell& getESM_ChangeDwell()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ESM_ChangeDwell.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setESM_ChangeDwell(const uci::type::ESM_ActivityCommandType::ESM_ChangeDwell& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ESM_ChangeGeolocation.
           *
           * @return The acecssor that provides access to the complex content that is identified by ESM_ChangeGeolocation.
           */
         virtual const uci::type::ESM_ChangeGeolocationType& getESM_ChangeGeolocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ESM_ChangeGeolocation.
           *
           * @return The acecssor that provides access to the complex content that is identified by ESM_ChangeGeolocation.
           */
         virtual uci::type::ESM_ChangeGeolocationType& getESM_ChangeGeolocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ESM_ChangeGeolocation to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ESM_ChangeGeolocation
           */
         virtual void setESM_ChangeGeolocation(const uci::type::ESM_ChangeGeolocationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ESM_ChangeGeolocation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ESM_ChangeGeolocation is emabled or not
           */
         virtual bool hasESM_ChangeGeolocation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ESM_ChangeGeolocation
           *
           * @param type = uci::type::accessorType::eSM_ChangeGeolocationType This Accessor's accessor type
           */
         virtual void enableESM_ChangeGeolocation(uci::base::accessorType::AccessorType type = uci::type::accessorType::eSM_ChangeGeolocationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ESM_ChangeGeolocation */
         virtual void clearESM_ChangeGeolocation()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeDependentActivity.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeDependentActivity.
           */
         virtual const uci::type::DependentActivityType& getChangeDependentActivity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeDependentActivity.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeDependentActivity.
           */
         virtual uci::type::DependentActivityType& getChangeDependentActivity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ChangeDependentActivity to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ChangeDependentActivity
           */
         virtual void setChangeDependentActivity(const uci::type::DependentActivityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ChangeDependentActivity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ChangeDependentActivity is emabled or not
           */
         virtual bool hasChangeDependentActivity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ChangeDependentActivity
           *
           * @param type = uci::type::accessorType::dependentActivityType This Accessor's accessor type
           */
         virtual void enableChangeDependentActivity(uci::base::accessorType::AccessorType type = uci::type::accessorType::dependentActivityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ChangeDependentActivity */
         virtual void clearChangeDependentActivity()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ESM_ChangeSubCapabilityDetails.
           *
           * @return The bounded list identified by ESM_ChangeSubCapabilityDetails.
           */
         virtual const uci::type::ESM_ActivityCommandType::ESM_ChangeSubCapabilityDetails& getESM_ChangeSubCapabilityDetails() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ESM_ChangeSubCapabilityDetails.
           *
           * @return The bounded list identified by ESM_ChangeSubCapabilityDetails.
           */
         virtual uci::type::ESM_ActivityCommandType::ESM_ChangeSubCapabilityDetails& getESM_ChangeSubCapabilityDetails()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ESM_ChangeSubCapabilityDetails.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setESM_ChangeSubCapabilityDetails(const uci::type::ESM_ActivityCommandType::ESM_ChangeSubCapabilityDetails& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SpectralDensityReporting.
           *
           * @return The value of the simple primitive data type identified by SpectralDensityReporting.
           */
         virtual xs::Boolean getSpectralDensityReporting() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SpectralDensityReporting.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSpectralDensityReporting(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SpectralDensityReporting exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SpectralDensityReporting is emabled or not
           */
         virtual bool hasSpectralDensityReporting() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SpectralDensityReporting
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableSpectralDensityReporting(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SpectralDensityReporting */
         virtual void clearSpectralDensityReporting()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ESM_ActivityCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ESM_ActivityCommandType to copy from
           */
         ESM_ActivityCommandType(const ESM_ActivityCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ESM_ActivityCommandType to the contents of the
           * ESM_ActivityCommandType on the right hand side (rhs) of the assignment operator.ESM_ActivityCommandType [only
           * available to derived classes]
           *
           * @param rhs The ESM_ActivityCommandType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ESM_ActivityCommandType
           * @return A constant reference to this ESM_ActivityCommandType.
           */
         const ESM_ActivityCommandType& operator=(const ESM_ActivityCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ESM_ActivityCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ESM_ActivityCommandType_h

