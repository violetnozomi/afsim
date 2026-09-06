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
#ifndef Uci__Type__EOB_SiteIdentityType_h
#define Uci__Type__EOB_SiteIdentityType_h 1

#if !defined(Uci__Type__EOB_IdentityBaseType_h)
# include "uci/type/EOB_IdentityBaseType.h"
#endif

#if !defined(Uci__Type__BasicEncyclopediaNumberType_h)
# include "uci/type/BasicEncyclopediaNumberType.h"
#endif

#if !defined(Uci__Type__EOB_O_SuffixType_h)
# include "uci/type/EOB_O_SuffixType.h"
#endif

#if !defined(Xs__Type__StringXmlSchemaPrimitives_h)
# include "xs/type/stringXmlSchemaPrimitives.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EOB_SiteIdentityType sequence accessor class */
      class EOB_SiteIdentityType : public virtual uci::type::EOB_IdentityBaseType {
      public:

         /** The destructor */
         virtual ~EOB_SiteIdentityType()
         { }

         /** Returns this accessor's type constant, i.e. EOB_SiteIdentityType
           *
           * @return This accessor's type constant, i.e. EOB_SiteIdentityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eOB_SiteIdentityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EOB_SiteIdentityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum length: 0] [Maximum length: 6] [Pattern: "[A-Za-z0-9]{0,6}"] */
         typedef xs::String Category;

         /** Returns the accessor that provides access to the complex content that is identified by the BE_Number.
           *
           * @return The acecssor that provides access to the complex content that is identified by BE_Number.
           */
         virtual const uci::type::BasicEncyclopediaNumberType& getBE_Number() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BE_Number.
           *
           * @return The acecssor that provides access to the complex content that is identified by BE_Number.
           */
         virtual uci::type::BasicEncyclopediaNumberType& getBE_Number()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the BE_Number to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by BE_Number
           */
         virtual void setBE_Number(const uci::type::BasicEncyclopediaNumberType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by BE_Number exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by BE_Number is emabled or not
           */
         virtual bool hasBE_Number() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by BE_Number
           *
           * @param type = uci::type::accessorType::basicEncyclopediaNumberType This Accessor's accessor type
           */
         virtual void enableBE_Number(uci::base::accessorType::AccessorType type = uci::type::accessorType::basicEncyclopediaNumberType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by BE_Number */
         virtual void clearBE_Number()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the O_Suffix.
           *
           * @return The value of the string data type identified by O_Suffix.
           */
         virtual const uci::type::EOB_O_SuffixType& getO_Suffix() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the O_Suffix.
           *
           * @return The value of the string data type identified by O_Suffix.
           */
         virtual uci::type::EOB_O_SuffixType& getO_Suffix()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the O_Suffix to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setO_Suffix(const uci::type::EOB_O_SuffixType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the O_Suffix to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setO_Suffix(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the O_Suffix to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setO_Suffix(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by O_Suffix exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by O_Suffix is emabled or not
           */
         virtual bool hasO_Suffix() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by O_Suffix
           *
           * @param type = uci::type::accessorType::eOB_O_SuffixType This Accessor's accessor type
           */
         virtual void enableO_Suffix(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_O_SuffixType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by O_Suffix */
         virtual void clearO_Suffix()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Category.
           *
           * @return The value of the string data type identified by Category.
           */
         virtual const uci::type::EOB_SiteIdentityType::Category& getCategory() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Category.
           *
           * @return The value of the string data type identified by Category.
           */
         virtual uci::type::EOB_SiteIdentityType::Category& getCategory()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Category to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setCategory(const uci::type::EOB_SiteIdentityType::Category& value)
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


         /** Returns whether the Element that is identified by Category exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Category is emabled or not
           */
         virtual bool hasCategory() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Category
           *
           * @param type = uci::type::eOB_SiteIdentityType::accessorType::category This Accessor's accessor type
           */
         virtual void enableCategory(uci::base::accessorType::AccessorType type = uci::type::eOB_SiteIdentityType::accessorType::category)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Category */
         virtual void clearCategory()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EOB_SiteIdentityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EOB_SiteIdentityType to copy from
           */
         EOB_SiteIdentityType(const EOB_SiteIdentityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EOB_SiteIdentityType to the contents of the EOB_SiteIdentityType
           * on the right hand side (rhs) of the assignment operator.EOB_SiteIdentityType [only available to derived classes]
           *
           * @param rhs The EOB_SiteIdentityType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::EOB_SiteIdentityType
           * @return A constant reference to this EOB_SiteIdentityType.
           */
         const EOB_SiteIdentityType& operator=(const EOB_SiteIdentityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EOB_SiteIdentityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EOB_SiteIdentityType_h

