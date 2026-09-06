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
#ifndef Uci__Type__EA_TechniqueGroupIdentifierType_h
#define Uci__Type__EA_TechniqueGroupIdentifierType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Xs__Type__StringXmlSchemaPrimitives_h)
# include "xs/type/stringXmlSchemaPrimitives.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates an identifier corresponding to a group of EA techniques based on the organization of EA Capability. The use
        * of Nickname and MDF name is left to the external implementation to organize and identify.
        */
      class EA_TechniqueGroupIdentifierType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EA_TechniqueGroupIdentifierType()
         { }

         /** Returns this accessor's type constant, i.e. EA_TechniqueGroupIdentifierType
           *
           * @return This accessor's type constant, i.e. EA_TechniqueGroupIdentifierType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eA_TechniqueGroupIdentifierType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EA_TechniqueGroupIdentifierType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The short name assigned to the EA technique group by the external EA capability planning and designing organization.;
           * [Minimum length: 2] [Maximum length: 4] [Pattern: "[ -~]{2,4}"]
           */
         typedef xs::String Nickname;

         /** Returns the value of the string data type that is identified by the Nickname.
           *
           * @return The value of the string data type identified by Nickname.
           */
         virtual const uci::type::EA_TechniqueGroupIdentifierType::Nickname& getNickname() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Nickname.
           *
           * @return The value of the string data type identified by Nickname.
           */
         virtual uci::type::EA_TechniqueGroupIdentifierType::Nickname& getNickname()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Nickname to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setNickname(const uci::type::EA_TechniqueGroupIdentifierType::Nickname& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Nickname to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setNickname(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Nickname to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setNickname(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MDF_Name.
           *
           * @return The value of the string data type identified by MDF_Name.
           */
         virtual const uci::type::VisibleString256Type& getMDF_Name() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MDF_Name.
           *
           * @return The value of the string data type identified by MDF_Name.
           */
         virtual uci::type::VisibleString256Type& getMDF_Name()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MDF_Name to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setMDF_Name(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MDF_Name to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMDF_Name(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MDF_Name to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMDF_Name(const char* value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EA_TechniqueGroupIdentifierType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EA_TechniqueGroupIdentifierType to copy from
           */
         EA_TechniqueGroupIdentifierType(const EA_TechniqueGroupIdentifierType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EA_TechniqueGroupIdentifierType to the contents of the
           * EA_TechniqueGroupIdentifierType on the right hand side (rhs) of the assignment
           * operator.EA_TechniqueGroupIdentifierType [only available to derived classes]
           *
           * @param rhs The EA_TechniqueGroupIdentifierType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::EA_TechniqueGroupIdentifierType
           * @return A constant reference to this EA_TechniqueGroupIdentifierType.
           */
         const EA_TechniqueGroupIdentifierType& operator=(const EA_TechniqueGroupIdentifierType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EA_TechniqueGroupIdentifierType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EA_TechniqueGroupIdentifierType_h

