// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__AO_CodeType_h
#define Uci__Type__AO_CodeType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AO_PRF_CodeType_h)
# include "uci/type/AO_PRF_CodeType.h"
#endif

#if !defined(Uci__Type__AO_PIM_CodeType_h)
# include "uci/type/AO_PIM_CodeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AO_CodeType sequence accessor class */
      class AO_CodeType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AO_CodeType()
         { }

         /** Returns this accessor's type constant, i.e. AO_CodeType
           *
           * @return This accessor's type constant, i.e. AO_CodeType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aO_CodeType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AO_CodeType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PRF_Code.
           *
           * @return The value of the string data type identified by PRF_Code.
           */
         virtual const uci::type::AO_PRF_CodeType& getPRF_Code() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PRF_Code.
           *
           * @return The value of the string data type identified by PRF_Code.
           */
         virtual uci::type::AO_PRF_CodeType& getPRF_Code()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PRF_Code to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setPRF_Code(const uci::type::AO_PRF_CodeType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PRF_Code to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPRF_Code(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PRF_Code to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPRF_Code(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRF_Code exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRF_Code is emabled or not
           */
         virtual bool hasPRF_Code() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRF_Code
           *
           * @param type = uci::type::accessorType::aO_PRF_CodeType This Accessor's accessor type
           */
         virtual void enablePRF_Code(uci::base::accessorType::AccessorType type = uci::type::accessorType::aO_PRF_CodeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRF_Code */
         virtual void clearPRF_Code()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PIM_Code.
           *
           * @return The value of the string data type identified by PIM_Code.
           */
         virtual const uci::type::AO_PIM_CodeType& getPIM_Code() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PIM_Code.
           *
           * @return The value of the string data type identified by PIM_Code.
           */
         virtual uci::type::AO_PIM_CodeType& getPIM_Code()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PIM_Code to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setPIM_Code(const uci::type::AO_PIM_CodeType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PIM_Code to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPIM_Code(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PIM_Code to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPIM_Code(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PIM_Code exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PIM_Code is emabled or not
           */
         virtual bool hasPIM_Code() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PIM_Code
           *
           * @param type = uci::type::accessorType::aO_PIM_CodeType This Accessor's accessor type
           */
         virtual void enablePIM_Code(uci::base::accessorType::AccessorType type = uci::type::accessorType::aO_PIM_CodeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PIM_Code */
         virtual void clearPIM_Code()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AO_CodeType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AO_CodeType to copy from
           */
         AO_CodeType(const AO_CodeType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AO_CodeType to the contents of the AO_CodeType on the right hand
           * side (rhs) of the assignment operator.AO_CodeType [only available to derived classes]
           *
           * @param rhs The AO_CodeType on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::AO_CodeType
           * @return A constant reference to this AO_CodeType.
           */
         const AO_CodeType& operator=(const AO_CodeType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AO_CodeType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AO_CodeType_h

