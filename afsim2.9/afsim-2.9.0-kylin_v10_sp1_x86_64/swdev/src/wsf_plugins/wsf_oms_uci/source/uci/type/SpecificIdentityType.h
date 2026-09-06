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
#ifndef Uci__Type__SpecificIdentityType_h
#define Uci__Type__SpecificIdentityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__EnvironmentEnum_h)
# include "uci/type/EnvironmentEnum.h"
#endif

#if !defined(Uci__Type__SiteTypeEnum_h)
# include "uci/type/SiteTypeEnum.h"
#endif

#if !defined(Uci__Type__SpecificTypeModelType_h)
# include "uci/type/SpecificTypeModelType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SpecificIdentityType sequence accessor class */
      class SpecificIdentityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SpecificIdentityType()
         { }

         /** Returns this accessor's type constant, i.e. SpecificIdentityType
           *
           * @return This accessor's type constant, i.e. SpecificIdentityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::specificIdentityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SpecificIdentityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SpecificType.
           *
           * @return The value of the simple primitive data type identified by SpecificType.
           */
         virtual xs::UnsignedInt getSpecificType() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SpecificType.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSpecificType(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SpecificTypeCategory.
           *
           * @return The value of the enumeration identified by SpecificTypeCategory.
           */
         virtual const uci::type::EnvironmentEnum& getSpecificTypeCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SpecificTypeCategory.
           *
           * @return The value of the enumeration identified by SpecificTypeCategory.
           */
         virtual uci::type::EnvironmentEnum& getSpecificTypeCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SpecificTypeCategory.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSpecificTypeCategory(const uci::type::EnvironmentEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SpecificTypeCategory.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSpecificTypeCategory(uci::type::EnvironmentEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SiteType.
           *
           * @return The value of the enumeration identified by SiteType.
           */
         virtual const uci::type::SiteTypeEnum& getSiteType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SiteType.
           *
           * @return The value of the enumeration identified by SiteType.
           */
         virtual uci::type::SiteTypeEnum& getSiteType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SiteType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSiteType(const uci::type::SiteTypeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SiteType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSiteType(uci::type::SiteTypeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield SiteTypeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasSiteType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getSiteType will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::siteTypeEnum This Accessor's accessor type
           */
         virtual void enableSiteType(uci::base::accessorType::AccessorType type = uci::type::accessorType::siteTypeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearSiteType()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SpecificTypeModel.
           *
           * @return The value of the string data type identified by SpecificTypeModel.
           */
         virtual const uci::type::SpecificTypeModelType& getSpecificTypeModel() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SpecificTypeModel.
           *
           * @return The value of the string data type identified by SpecificTypeModel.
           */
         virtual uci::type::SpecificTypeModelType& getSpecificTypeModel()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SpecificTypeModel to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSpecificTypeModel(const uci::type::SpecificTypeModelType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SpecificTypeModel to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSpecificTypeModel(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SpecificTypeModel to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSpecificTypeModel(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SpecificTypeModel exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SpecificTypeModel is emabled or not
           */
         virtual bool hasSpecificTypeModel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SpecificTypeModel
           *
           * @param type = uci::type::accessorType::specificTypeModelType This Accessor's accessor type
           */
         virtual void enableSpecificTypeModel(uci::base::accessorType::AccessorType type = uci::type::accessorType::specificTypeModelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SpecificTypeModel */
         virtual void clearSpecificTypeModel()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SpecificIdentityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SpecificIdentityType to copy from
           */
         SpecificIdentityType(const SpecificIdentityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SpecificIdentityType to the contents of the SpecificIdentityType
           * on the right hand side (rhs) of the assignment operator.SpecificIdentityType [only available to derived classes]
           *
           * @param rhs The SpecificIdentityType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::SpecificIdentityType
           * @return A constant reference to this SpecificIdentityType.
           */
         const SpecificIdentityType& operator=(const SpecificIdentityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SpecificIdentityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SpecificIdentityType_h

