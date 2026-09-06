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
#ifndef Uci__Type__SECTGA_Type_h
#define Uci__Type__SECTGA_Type_h 1

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

      /** This is the SECTGA_Type sequence accessor class */
      class SECTGA_Type : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SECTGA_Type()
         { }

         /** Returns this accessor's type constant, i.e. SECTGA_Type
           *
           * @return This accessor's type constant, i.e. SECTGA_Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sECTGA_Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SECTGA_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** [White space: preserve] [Length: 13] [Pattern: "[A-Za-z0-9 \-_]{13}"] */
         typedef xs::String SEC_ID;

         /** [White space: preserve] [Length: 16] [Pattern: "[A-Za-z0-9 \-_]{16}"] */
         typedef xs::String SEC_BE;

         /** Returns the value of the string data type that is identified by the SEC_ID.
           *
           * @return The value of the string data type identified by SEC_ID.
           */
         virtual const uci::type::SECTGA_Type::SEC_ID& getSEC_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SEC_ID.
           *
           * @return The value of the string data type identified by SEC_ID.
           */
         virtual uci::type::SECTGA_Type::SEC_ID& getSEC_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SEC_ID to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSEC_ID(const uci::type::SECTGA_Type::SEC_ID& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SEC_ID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSEC_ID(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SEC_ID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSEC_ID(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SEC_ID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SEC_ID is emabled or not
           */
         virtual bool hasSEC_ID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SEC_ID
           *
           * @param type = uci::type::sECTGA_Type::accessorType::sEC_ID This Accessor's accessor type
           */
         virtual void enableSEC_ID(uci::base::accessorType::AccessorType type = uci::type::sECTGA_Type::accessorType::sEC_ID)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SEC_ID */
         virtual void clearSEC_ID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SEC_BE.
           *
           * @return The value of the string data type identified by SEC_BE.
           */
         virtual const uci::type::SECTGA_Type::SEC_BE& getSEC_BE() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SEC_BE.
           *
           * @return The value of the string data type identified by SEC_BE.
           */
         virtual uci::type::SECTGA_Type::SEC_BE& getSEC_BE()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SEC_BE to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSEC_BE(const uci::type::SECTGA_Type::SEC_BE& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SEC_BE to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSEC_BE(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SEC_BE to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSEC_BE(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SEC_BE exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SEC_BE is emabled or not
           */
         virtual bool hasSEC_BE() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SEC_BE
           *
           * @param type = uci::type::sECTGA_Type::accessorType::sEC_BE This Accessor's accessor type
           */
         virtual void enableSEC_BE(uci::base::accessorType::AccessorType type = uci::type::sECTGA_Type::accessorType::sEC_BE)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SEC_BE */
         virtual void clearSEC_BE()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SECTGA_Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SECTGA_Type to copy from
           */
         SECTGA_Type(const SECTGA_Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SECTGA_Type to the contents of the SECTGA_Type on the right hand
           * side (rhs) of the assignment operator.SECTGA_Type [only available to derived classes]
           *
           * @param rhs The SECTGA_Type on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::SECTGA_Type
           * @return A constant reference to this SECTGA_Type.
           */
         const SECTGA_Type& operator=(const SECTGA_Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SECTGA_Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SECTGA_Type_h

