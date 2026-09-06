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
#ifndef Uci__Type__CommunicationsEmitterIdentityType_h
#define Uci__Type__CommunicationsEmitterIdentityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__NotationType_h)
# include "uci/type/NotationType.h"
#endif

#if !defined(Uci__Type__CommModeEnum_h)
# include "uci/type/CommModeEnum.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommunicationsEmitterIdentityType sequence accessor class */
      class CommunicationsEmitterIdentityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommunicationsEmitterIdentityType()
         { }

         /** Returns this accessor's type constant, i.e. CommunicationsEmitterIdentityType
           *
           * @return This accessor's type constant, i.e. CommunicationsEmitterIdentityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::communicationsEmitterIdentityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommunicationsEmitterIdentityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the CENOT_ID.
           *
           * @return The value of the string data type identified by CENOT_ID.
           */
         virtual const uci::type::NotationType& getCENOT_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the CENOT_ID.
           *
           * @return The value of the string data type identified by CENOT_ID.
           */
         virtual uci::type::NotationType& getCENOT_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CENOT_ID to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setCENOT_ID(const uci::type::NotationType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CENOT_ID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCENOT_ID(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CENOT_ID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCENOT_ID(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CENOT_ID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CENOT_ID is emabled or not
           */
         virtual bool hasCENOT_ID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CENOT_ID
           *
           * @param type = uci::type::accessorType::notationType This Accessor's accessor type
           */
         virtual void enableCENOT_ID(uci::base::accessorType::AccessorType type = uci::type::accessorType::notationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CENOT_ID */
         virtual void clearCENOT_ID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommMode.
           *
           * @return The value of the enumeration identified by CommMode.
           */
         virtual const uci::type::CommModeEnum& getCommMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommMode.
           *
           * @return The value of the enumeration identified by CommMode.
           */
         virtual uci::type::CommModeEnum& getCommMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommMode(const uci::type::CommModeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommMode(uci::type::CommModeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield CommModeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasCommMode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getCommMode will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::commModeEnum This Accessor's accessor type
           */
         virtual void enableCommMode(uci::base::accessorType::AccessorType type = uci::type::accessorType::commModeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearCommMode()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmitterIndex.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmitterIndex.
           */
         virtual const uci::type::ForeignKeyType& getEmitterIndex() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmitterIndex.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmitterIndex.
           */
         virtual uci::type::ForeignKeyType& getEmitterIndex()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EmitterIndex to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EmitterIndex
           */
         virtual void setEmitterIndex(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EmitterIndex exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EmitterIndex is emabled or not
           */
         virtual bool hasEmitterIndex() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EmitterIndex
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableEmitterIndex(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EmitterIndex */
         virtual void clearEmitterIndex()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommunicationsEmitterIdentityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommunicationsEmitterIdentityType to copy from
           */
         CommunicationsEmitterIdentityType(const CommunicationsEmitterIdentityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommunicationsEmitterIdentityType to the contents of the
           * CommunicationsEmitterIdentityType on the right hand side (rhs) of the assignment
           * operator.CommunicationsEmitterIdentityType [only available to derived classes]
           *
           * @param rhs The CommunicationsEmitterIdentityType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::CommunicationsEmitterIdentityType
           * @return A constant reference to this CommunicationsEmitterIdentityType.
           */
         const CommunicationsEmitterIdentityType& operator=(const CommunicationsEmitterIdentityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommunicationsEmitterIdentityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommunicationsEmitterIdentityType_h

