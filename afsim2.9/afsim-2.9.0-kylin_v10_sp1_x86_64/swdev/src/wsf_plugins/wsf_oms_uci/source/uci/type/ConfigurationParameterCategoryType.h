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
#ifndef Uci__Type__ConfigurationParameterCategoryType_h
#define Uci__Type__ConfigurationParameterCategoryType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__VisibleString32Type_h)
# include "uci/type/VisibleString32Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ConfigurationParameterType_h)
# include "uci/type/ConfigurationParameterType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ConfigurationParameterCategoryType sequence accessor class */
      class ConfigurationParameterCategoryType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ConfigurationParameterCategoryType()
         { }

         /** Returns this accessor's type constant, i.e. ConfigurationParameterCategoryType
           *
           * @return This accessor's type constant, i.e. ConfigurationParameterCategoryType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::configurationParameterCategoryType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ConfigurationParameterCategoryType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Name refers to the name of the parameter, value represents the current value of the parameter and default represents
           * the default value of the parameter. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ConfigurationParameterType, uci::type::accessorType::configurationParameterType> Parameter;

         /** Returns the value of the string data type that is identified by the Category.
           *
           * @return The value of the string data type identified by Category.
           */
         virtual const uci::type::VisibleString32Type& getCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Category.
           *
           * @return The value of the string data type identified by Category.
           */
         virtual uci::type::VisibleString32Type& getCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Category to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setCategory(const uci::type::VisibleString32Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Category to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCategory(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Category to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCategory(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Parameter.
           *
           * @return The bounded list identified by Parameter.
           */
         virtual const uci::type::ConfigurationParameterCategoryType::Parameter& getParameter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Parameter.
           *
           * @return The bounded list identified by Parameter.
           */
         virtual uci::type::ConfigurationParameterCategoryType::Parameter& getParameter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Parameter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setParameter(const uci::type::ConfigurationParameterCategoryType::Parameter& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ConfigurationParameterCategoryType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ConfigurationParameterCategoryType to copy from
           */
         ConfigurationParameterCategoryType(const ConfigurationParameterCategoryType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ConfigurationParameterCategoryType to the contents of the
           * ConfigurationParameterCategoryType on the right hand side (rhs) of the assignment
           * operator.ConfigurationParameterCategoryType [only available to derived classes]
           *
           * @param rhs The ConfigurationParameterCategoryType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ConfigurationParameterCategoryType
           * @return A constant reference to this ConfigurationParameterCategoryType.
           */
         const ConfigurationParameterCategoryType& operator=(const ConfigurationParameterCategoryType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ConfigurationParameterCategoryType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ConfigurationParameterCategoryType_h

