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
#ifndef Uci__Type__ServiceConfigurationParamsRequestStatusMDT_h
#define Uci__Type__ServiceConfigurationParamsRequestStatusMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ServiceConfigurationParamsRequestID_Type_h)
# include "uci/type/ServiceConfigurationParamsRequestID_Type.h"
#endif

#if !defined(Uci__Type__ProcessingStatusEnum_h)
# include "uci/type/ProcessingStatusEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ConfigurationParameterCategoryType_h)
# include "uci/type/ConfigurationParameterCategoryType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ServiceConfigurationParamsRequestStatusMDT sequence accessor class */
      class ServiceConfigurationParamsRequestStatusMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ServiceConfigurationParamsRequestStatusMDT()
         { }

         /** Returns this accessor's type constant, i.e. ServiceConfigurationParamsRequestStatusMDT
           *
           * @return This accessor's type constant, i.e. ServiceConfigurationParamsRequestStatusMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::serviceConfigurationParamsRequestStatusMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ServiceConfigurationParamsRequestStatusMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** This element allows ConfigurationParameters to be returned in the request status in response to a request. This data
           * should only be populated if the request specifies ResultsInNativeMessage = FALSE and the state of the request =
           * COMPLETED. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ConfigurationParameterCategoryType, uci::type::accessorType::configurationParameterCategoryType> ParameterCategory;

         /** Returns the accessor that provides access to the complex content that is identified by the
           * ServiceConfigurationParamsRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by
           *      ServiceConfigurationParamsRequestID.
           */
         virtual const uci::type::ServiceConfigurationParamsRequestID_Type& getServiceConfigurationParamsRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * ServiceConfigurationParamsRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by
           *      ServiceConfigurationParamsRequestID.
           */
         virtual uci::type::ServiceConfigurationParamsRequestID_Type& getServiceConfigurationParamsRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ServiceConfigurationParamsRequestID to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ServiceConfigurationParamsRequestID
           */
         virtual void setServiceConfigurationParamsRequestID(const uci::type::ServiceConfigurationParamsRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ConfigurationRequestState.
           *
           * @return The value of the enumeration identified by ConfigurationRequestState.
           */
         virtual const uci::type::ProcessingStatusEnum& getConfigurationRequestState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ConfigurationRequestState.
           *
           * @return The value of the enumeration identified by ConfigurationRequestState.
           */
         virtual uci::type::ProcessingStatusEnum& getConfigurationRequestState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConfigurationRequestState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConfigurationRequestState(const uci::type::ProcessingStatusEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ConfigurationRequestState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setConfigurationRequestState(uci::type::ProcessingStatusEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ParameterCategory.
           *
           * @return The bounded list identified by ParameterCategory.
           */
         virtual const uci::type::ServiceConfigurationParamsRequestStatusMDT::ParameterCategory& getParameterCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ParameterCategory.
           *
           * @return The bounded list identified by ParameterCategory.
           */
         virtual uci::type::ServiceConfigurationParamsRequestStatusMDT::ParameterCategory& getParameterCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ParameterCategory.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setParameterCategory(const uci::type::ServiceConfigurationParamsRequestStatusMDT::ParameterCategory& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ServiceConfigurationParamsRequestStatusMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ServiceConfigurationParamsRequestStatusMDT to copy from
           */
         ServiceConfigurationParamsRequestStatusMDT(const ServiceConfigurationParamsRequestStatusMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ServiceConfigurationParamsRequestStatusMDT to the contents of the
           * ServiceConfigurationParamsRequestStatusMDT on the right hand side (rhs) of the assignment
           * operator.ServiceConfigurationParamsRequestStatusMDT [only available to derived classes]
           *
           * @param rhs The ServiceConfigurationParamsRequestStatusMDT on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::type::ServiceConfigurationParamsRequestStatusMDT
           * @return A constant reference to this ServiceConfigurationParamsRequestStatusMDT.
           */
         const ServiceConfigurationParamsRequestStatusMDT& operator=(const ServiceConfigurationParamsRequestStatusMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ServiceConfigurationParamsRequestStatusMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ServiceConfigurationParamsRequestStatusMDT_h

