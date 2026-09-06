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
#ifndef Uci__Type__EOB_EmitterIdentityType_h
#define Uci__Type__EOB_EmitterIdentityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EmitterIdentityType_h)
# include "uci/type/EmitterIdentityType.h"
#endif

#if !defined(Uci__Type__EOB_WeaponSystemType_h)
# include "uci/type/EOB_WeaponSystemType.h"
#endif

#if !defined(Uci__Type__EOB_CED_NameType_h)
# include "uci/type/EOB_CED_NameType.h"
#endif

#if !defined(Uci__Type__EOB_EmitterSurrogateKeyType_h)
# include "uci/type/EOB_EmitterSurrogateKeyType.h"
#endif

#if !defined(Uci__Type__EOB_CodeWordType_h)
# include "uci/type/EOB_CodeWordType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EOB_EmitterIdentityType sequence accessor class */
      class EOB_EmitterIdentityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EOB_EmitterIdentityType()
         { }

         /** Returns this accessor's type constant, i.e. EOB_EmitterIdentityType
           *
           * @return This accessor's type constant, i.e. EOB_EmitterIdentityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eOB_EmitterIdentityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EOB_EmitterIdentityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Emitter.
           *
           * @return The acecssor that provides access to the complex content that is identified by Emitter.
           */
         virtual const uci::type::EmitterIdentityType& getEmitter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Emitter.
           *
           * @return The acecssor that provides access to the complex content that is identified by Emitter.
           */
         virtual uci::type::EmitterIdentityType& getEmitter()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Emitter to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Emitter
           */
         virtual void setEmitter(const uci::type::EmitterIdentityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Emitter exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Emitter is emabled or not
           */
         virtual bool hasEmitter() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Emitter
           *
           * @param type = uci::type::accessorType::emitterIdentityType This Accessor's accessor type
           */
         virtual void enableEmitter(uci::base::accessorType::AccessorType type = uci::type::accessorType::emitterIdentityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Emitter */
         virtual void clearEmitter()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the WeaponSystem.
           *
           * @return The value of the string data type identified by WeaponSystem.
           */
         virtual const uci::type::EOB_WeaponSystemType& getWeaponSystem() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the WeaponSystem.
           *
           * @return The value of the string data type identified by WeaponSystem.
           */
         virtual uci::type::EOB_WeaponSystemType& getWeaponSystem()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the WeaponSystem to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setWeaponSystem(const uci::type::EOB_WeaponSystemType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the WeaponSystem to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setWeaponSystem(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the WeaponSystem to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setWeaponSystem(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WeaponSystem exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WeaponSystem is emabled or not
           */
         virtual bool hasWeaponSystem() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WeaponSystem
           *
           * @param type = uci::type::accessorType::eOB_WeaponSystemType This Accessor's accessor type
           */
         virtual void enableWeaponSystem(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_WeaponSystemType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WeaponSystem */
         virtual void clearWeaponSystem()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the CED_Name.
           *
           * @return The value of the string data type identified by CED_Name.
           */
         virtual const uci::type::EOB_CED_NameType& getCED_Name() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the CED_Name.
           *
           * @return The value of the string data type identified by CED_Name.
           */
         virtual uci::type::EOB_CED_NameType& getCED_Name()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CED_Name to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setCED_Name(const uci::type::EOB_CED_NameType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CED_Name to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCED_Name(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CED_Name to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCED_Name(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CED_Name exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CED_Name is emabled or not
           */
         virtual bool hasCED_Name() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CED_Name
           *
           * @param type = uci::type::accessorType::eOB_CED_NameType This Accessor's accessor type
           */
         virtual void enableCED_Name(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_CED_NameType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CED_Name */
         virtual void clearCED_Name()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the EmitterSK.
           *
           * @return The value of the string data type identified by EmitterSK.
           */
         virtual const uci::type::EOB_EmitterSurrogateKeyType& getEmitterSK() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the EmitterSK.
           *
           * @return The value of the string data type identified by EmitterSK.
           */
         virtual uci::type::EOB_EmitterSurrogateKeyType& getEmitterSK()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the EmitterSK to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setEmitterSK(const uci::type::EOB_EmitterSurrogateKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the EmitterSK to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setEmitterSK(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the EmitterSK to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setEmitterSK(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EmitterSK exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EmitterSK is emabled or not
           */
         virtual bool hasEmitterSK() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EmitterSK
           *
           * @param type = uci::type::accessorType::eOB_EmitterSurrogateKeyType This Accessor's accessor type
           */
         virtual void enableEmitterSK(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_EmitterSurrogateKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EmitterSK */
         virtual void clearEmitterSK()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the CodeWord.
           *
           * @return The value of the string data type identified by CodeWord.
           */
         virtual const uci::type::EOB_CodeWordType& getCodeWord() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the CodeWord.
           *
           * @return The value of the string data type identified by CodeWord.
           */
         virtual uci::type::EOB_CodeWordType& getCodeWord()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CodeWord to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setCodeWord(const uci::type::EOB_CodeWordType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CodeWord to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCodeWord(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CodeWord to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCodeWord(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CodeWord exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CodeWord is emabled or not
           */
         virtual bool hasCodeWord() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CodeWord
           *
           * @param type = uci::type::accessorType::eOB_CodeWordType This Accessor's accessor type
           */
         virtual void enableCodeWord(uci::base::accessorType::AccessorType type = uci::type::accessorType::eOB_CodeWordType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CodeWord */
         virtual void clearCodeWord()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EOB_EmitterIdentityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EOB_EmitterIdentityType to copy from
           */
         EOB_EmitterIdentityType(const EOB_EmitterIdentityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EOB_EmitterIdentityType to the contents of the
           * EOB_EmitterIdentityType on the right hand side (rhs) of the assignment operator.EOB_EmitterIdentityType [only
           * available to derived classes]
           *
           * @param rhs The EOB_EmitterIdentityType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::EOB_EmitterIdentityType
           * @return A constant reference to this EOB_EmitterIdentityType.
           */
         const EOB_EmitterIdentityType& operator=(const EOB_EmitterIdentityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EOB_EmitterIdentityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EOB_EmitterIdentityType_h

