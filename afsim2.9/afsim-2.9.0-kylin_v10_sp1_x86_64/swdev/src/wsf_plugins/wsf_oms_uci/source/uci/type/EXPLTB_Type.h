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
#ifndef Uci__Type__EXPLTB_Type_h
#define Uci__Type__EXPLTB_Type_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Xs__Type__StringXmlSchemaPrimitives_h)
# include "xs/type/stringXmlSchemaPrimitives.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EXPLTB_Type sequence accessor class */
      class EXPLTB_Type : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EXPLTB_Type()
         { }

         /** Returns this accessor's type constant, i.e. EXPLTB_Type
           *
           * @return This accessor's type constant, i.e. EXPLTB_Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eXPLTB_Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EXPLTB_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** [White space: preserve] [Length: 2] [Pattern: "[0-9 ]{2}"] */
         typedef xs::String SEQ_NUM;

         /** [White space: preserve] [Length: 16] [Pattern: "[A-Za-z0-9 ]{16}"] */
         typedef xs::String PRIME_BE;

         /** [White space: preserve] [Length: 13] [Pattern: "[A-Za-z0-9 ]{13}"] */
         typedef xs::String PRIME_ID;

         /** Returns the value of the string data type that is identified by the SEQ_NUM.
           *
           * @return The value of the string data type identified by SEQ_NUM.
           */
         virtual const uci::type::EXPLTB_Type::SEQ_NUM& getSEQ_NUM() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SEQ_NUM.
           *
           * @return The value of the string data type identified by SEQ_NUM.
           */
         virtual uci::type::EXPLTB_Type::SEQ_NUM& getSEQ_NUM()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SEQ_NUM to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSEQ_NUM(const uci::type::EXPLTB_Type::SEQ_NUM& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SEQ_NUM to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSEQ_NUM(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SEQ_NUM to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSEQ_NUM(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SEQ_NUM exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SEQ_NUM is emabled or not
           */
         virtual bool hasSEQ_NUM() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SEQ_NUM
           *
           * @param type = uci::type::eXPLTB_Type::accessorType::sEQ_NUM This Accessor's accessor type
           */
         virtual void enableSEQ_NUM(uci::base::accessorType::AccessorType type = uci::type::eXPLTB_Type::accessorType::sEQ_NUM)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SEQ_NUM */
         virtual void clearSEQ_NUM()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PRIME_BE.
           *
           * @return The value of the string data type identified by PRIME_BE.
           */
         virtual const uci::type::EXPLTB_Type::PRIME_BE& getPRIME_BE() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PRIME_BE.
           *
           * @return The value of the string data type identified by PRIME_BE.
           */
         virtual uci::type::EXPLTB_Type::PRIME_BE& getPRIME_BE()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PRIME_BE to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setPRIME_BE(const uci::type::EXPLTB_Type::PRIME_BE& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PRIME_BE to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPRIME_BE(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PRIME_BE to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPRIME_BE(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRIME_BE exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRIME_BE is emabled or not
           */
         virtual bool hasPRIME_BE() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRIME_BE
           *
           * @param type = uci::type::eXPLTB_Type::accessorType::pRIME_BE This Accessor's accessor type
           */
         virtual void enablePRIME_BE(uci::base::accessorType::AccessorType type = uci::type::eXPLTB_Type::accessorType::pRIME_BE)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRIME_BE */
         virtual void clearPRIME_BE()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PRIME_ID.
           *
           * @return The value of the string data type identified by PRIME_ID.
           */
         virtual const uci::type::EXPLTB_Type::PRIME_ID& getPRIME_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PRIME_ID.
           *
           * @return The value of the string data type identified by PRIME_ID.
           */
         virtual uci::type::EXPLTB_Type::PRIME_ID& getPRIME_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PRIME_ID to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setPRIME_ID(const uci::type::EXPLTB_Type::PRIME_ID& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PRIME_ID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPRIME_ID(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PRIME_ID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPRIME_ID(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRIME_ID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRIME_ID is emabled or not
           */
         virtual bool hasPRIME_ID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRIME_ID
           *
           * @param type = uci::type::eXPLTB_Type::accessorType::pRIME_ID This Accessor's accessor type
           */
         virtual void enablePRIME_ID(uci::base::accessorType::AccessorType type = uci::type::eXPLTB_Type::accessorType::pRIME_ID)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRIME_ID */
         virtual void clearPRIME_ID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EXPLTB_Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EXPLTB_Type to copy from
           */
         EXPLTB_Type(const EXPLTB_Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EXPLTB_Type to the contents of the EXPLTB_Type on the right hand
           * side (rhs) of the assignment operator.EXPLTB_Type [only available to derived classes]
           *
           * @param rhs The EXPLTB_Type on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::EXPLTB_Type
           * @return A constant reference to this EXPLTB_Type.
           */
         const EXPLTB_Type& operator=(const EXPLTB_Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EXPLTB_Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EXPLTB_Type_h

