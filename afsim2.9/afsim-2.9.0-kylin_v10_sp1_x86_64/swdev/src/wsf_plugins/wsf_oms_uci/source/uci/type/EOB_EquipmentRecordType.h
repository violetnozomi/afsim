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
#ifndef Uci__Type__EOB_EquipmentRecordType_h
#define Uci__Type__EOB_EquipmentRecordType_h 1

#if !defined(Uci__Type__BaseEOB_RecordType_h)
# include "uci/type/BaseEOB_RecordType.h"
#endif

#if !defined(Uci__Type__EOB_WeaponSystemType_h)
# include "uci/type/EOB_WeaponSystemType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EOB_EmitterType_h)
# include "uci/type/EOB_EmitterType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EOB_EquipmentRecordType sequence accessor class */
      class EOB_EquipmentRecordType : public virtual uci::type::BaseEOB_RecordType {
      public:

         /** The destructor */
         virtual ~EOB_EquipmentRecordType()
         { }

         /** Returns this accessor's type constant, i.e. EOB_EquipmentRecordType
           *
           * @return This accessor's type constant, i.e. EOB_EquipmentRecordType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eOB_EquipmentRecordType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EOB_EquipmentRecordType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a modes and parametrics of a previously observed emitter associated with the EOB record. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EOB_EmitterType, uci::type::accessorType::eOB_EmitterType> Emitter;

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


         /** Returns the bounded list that is identified by the Emitter.
           *
           * @return The bounded list identified by Emitter.
           */
         virtual const uci::type::EOB_EquipmentRecordType::Emitter& getEmitter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Emitter.
           *
           * @return The bounded list identified by Emitter.
           */
         virtual uci::type::EOB_EquipmentRecordType::Emitter& getEmitter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Emitter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEmitter(const uci::type::EOB_EquipmentRecordType::Emitter& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EOB_EquipmentRecordType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EOB_EquipmentRecordType to copy from
           */
         EOB_EquipmentRecordType(const EOB_EquipmentRecordType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EOB_EquipmentRecordType to the contents of the
           * EOB_EquipmentRecordType on the right hand side (rhs) of the assignment operator.EOB_EquipmentRecordType [only
           * available to derived classes]
           *
           * @param rhs The EOB_EquipmentRecordType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::EOB_EquipmentRecordType
           * @return A constant reference to this EOB_EquipmentRecordType.
           */
         const EOB_EquipmentRecordType& operator=(const EOB_EquipmentRecordType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EOB_EquipmentRecordType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EOB_EquipmentRecordType_h

