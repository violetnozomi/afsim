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
#ifndef Uci__Type__ServiceConfigurationParamsMDT_h
#define Uci__Type__ServiceConfigurationParamsMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
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

      /** This is the ServiceConfigurationParamsMDT sequence accessor class */
      class ServiceConfigurationParamsMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ServiceConfigurationParamsMDT()
         { }

         /** Returns this accessor's type constant, i.e. ServiceConfigurationParamsMDT
           *
           * @return This accessor's type constant, i.e. ServiceConfigurationParamsMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::serviceConfigurationParamsMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ServiceConfigurationParamsMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** This list contains the categories and associated parameters for a particular service configuration [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ConfigurationParameterCategoryType, uci::type::accessorType::configurationParameterCategoryType> Category;

         /** Returns the accessor that provides access to the complex content that is identified by the ServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ServiceID.
           */
         virtual const uci::type::ServiceID_Type& getServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ServiceID.
           */
         virtual uci::type::ServiceID_Type& getServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ServiceID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ServiceID
           */
         virtual void setServiceID(const uci::type::ServiceID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual const uci::type::SystemID_Type& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual uci::type::SystemID_Type& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemID
           */
         virtual void setSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SystemID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SystemID is emabled or not
           */
         virtual bool hasSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SystemID
           *
           * @param type = uci::type::accessorType::systemID_Type This Accessor's accessor type
           */
         virtual void enableSystemID(uci::base::accessorType::AccessorType type = uci::type::accessorType::systemID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SystemID */
         virtual void clearSystemID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Category.
           *
           * @return The bounded list identified by Category.
           */
         virtual const uci::type::ServiceConfigurationParamsMDT::Category& getCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Category.
           *
           * @return The bounded list identified by Category.
           */
         virtual uci::type::ServiceConfigurationParamsMDT::Category& getCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Category.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCategory(const uci::type::ServiceConfigurationParamsMDT::Category& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ServiceConfigurationParamsMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ServiceConfigurationParamsMDT to copy from
           */
         ServiceConfigurationParamsMDT(const ServiceConfigurationParamsMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ServiceConfigurationParamsMDT to the contents of the
           * ServiceConfigurationParamsMDT on the right hand side (rhs) of the assignment operator.ServiceConfigurationParamsMDT
           * [only available to derived classes]
           *
           * @param rhs The ServiceConfigurationParamsMDT on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::ServiceConfigurationParamsMDT
           * @return A constant reference to this ServiceConfigurationParamsMDT.
           */
         const ServiceConfigurationParamsMDT& operator=(const ServiceConfigurationParamsMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ServiceConfigurationParamsMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ServiceConfigurationParamsMDT_h

