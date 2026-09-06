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
#ifndef Uci__Type__EA_TargetType_h
#define Uci__Type__EA_TargetType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__EA_EmitterDataType_h)
# include "uci/type/EA_EmitterDataType.h"
#endif

#if !defined(Uci__Type__EA_TargetPointingType_h)
# include "uci/type/EA_TargetPointingType.h"
#endif

#if !defined(Uci__Type__EA_PowerType_h)
# include "uci/type/EA_PowerType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EA_TargetType sequence accessor class */
      class EA_TargetType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EA_TargetType()
         { }

         /** Returns this accessor's type constant, i.e. EA_TargetType
           *
           * @return This accessor's type constant, i.e. EA_TargetType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eA_TargetType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EA_TargetType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmitterData.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmitterData.
           */
         virtual const uci::type::EA_EmitterDataType& getEmitterData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EmitterData.
           *
           * @return The acecssor that provides access to the complex content that is identified by EmitterData.
           */
         virtual uci::type::EA_EmitterDataType& getEmitterData()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EmitterData to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EmitterData
           */
         virtual void setEmitterData(const uci::type::EA_EmitterDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EmitterData exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EmitterData is emabled or not
           */
         virtual bool hasEmitterData() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EmitterData
           *
           * @param type = uci::type::accessorType::eA_EmitterDataType This Accessor's accessor type
           */
         virtual void enableEmitterData(uci::base::accessorType::AccessorType type = uci::type::accessorType::eA_EmitterDataType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EmitterData */
         virtual void clearEmitterData()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Pointing.
           *
           * @return The acecssor that provides access to the complex content that is identified by Pointing.
           */
         virtual const uci::type::EA_TargetPointingType& getPointing() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Pointing.
           *
           * @return The acecssor that provides access to the complex content that is identified by Pointing.
           */
         virtual uci::type::EA_TargetPointingType& getPointing()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Pointing to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Pointing
           */
         virtual void setPointing(const uci::type::EA_TargetPointingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Pointing exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Pointing is emabled or not
           */
         virtual bool hasPointing() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Pointing
           *
           * @param type = uci::type::accessorType::eA_TargetPointingType This Accessor's accessor type
           */
         virtual void enablePointing(uci::base::accessorType::AccessorType type = uci::type::accessorType::eA_TargetPointingType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Pointing */
         virtual void clearPointing()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Power.
           *
           * @return The acecssor that provides access to the complex content that is identified by Power.
           */
         virtual const uci::type::EA_PowerType& getPower() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Power.
           *
           * @return The acecssor that provides access to the complex content that is identified by Power.
           */
         virtual uci::type::EA_PowerType& getPower()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Power to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Power
           */
         virtual void setPower(const uci::type::EA_PowerType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Power exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Power is emabled or not
           */
         virtual bool hasPower() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Power
           *
           * @param type = uci::type::accessorType::eA_PowerType This Accessor's accessor type
           */
         virtual void enablePower(uci::base::accessorType::AccessorType type = uci::type::accessorType::eA_PowerType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Power */
         virtual void clearPower()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the LastSignalTimestamp.
           *
           * @return The value of the simple primitive data type identified by LastSignalTimestamp.
           */
         virtual uci::type::DateTimeTypeValue getLastSignalTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the LastSignalTimestamp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setLastSignalTimestamp(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LastSignalTimestamp exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LastSignalTimestamp is emabled or not
           */
         virtual bool hasLastSignalTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LastSignalTimestamp
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableLastSignalTimestamp(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LastSignalTimestamp */
         virtual void clearLastSignalTimestamp()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EA_TargetType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EA_TargetType to copy from
           */
         EA_TargetType(const EA_TargetType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EA_TargetType to the contents of the EA_TargetType on the right
           * hand side (rhs) of the assignment operator.EA_TargetType [only available to derived classes]
           *
           * @param rhs The EA_TargetType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::EA_TargetType
           * @return A constant reference to this EA_TargetType.
           */
         const EA_TargetType& operator=(const EA_TargetType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EA_TargetType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EA_TargetType_h

