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
#ifndef Uci__Type__EA_ActivityCommandType_h
#define Uci__Type__EA_ActivityCommandType_h 1

#if !defined(Uci__Type__ActivityCommandBaseType_h)
# include "uci/type/ActivityCommandBaseType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EA_ActivityChangeBeamCommandType_h)
# include "uci/type/EA_ActivityChangeBeamCommandType.h"
#endif

#if !defined(Uci__Type__DependentActivityType_h)
# include "uci/type/DependentActivityType.h"
#endif

#if !defined(Uci__Type__SelectAntennaType_h)
# include "uci/type/SelectAntennaType.h"
#endif

#if !defined(Uci__Type__EA_ChangeLocationAndPowerType_h)
# include "uci/type/EA_ChangeLocationAndPowerType.h"
#endif

#if !defined(Uci__Type__ProtectedAssetReturnPowerType_h)
# include "uci/type/ProtectedAssetReturnPowerType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EA_ActivityCommandType sequence accessor class */
      class EA_ActivityCommandType : public virtual uci::type::ActivityCommandBaseType {
      public:

         /** The destructor */
         virtual ~EA_ActivityCommandType()
         { }

         /** Returns this accessor's type constant, i.e. EA_ActivityCommandType
           *
           * @return This accessor's type constant, i.e. EA_ActivityCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eA_ActivityCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EA_ActivityCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a command to modify an existing Beam of an existing EA Activity. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EA_ActivityChangeBeamCommandType, uci::type::accessorType::eA_ActivityChangeBeamCommandType> ChangeBeam;

         /** The ProtectedAsset child element indicates which of the protected assets of the activity are the subject of the child
           * attribute ChangeProtectedAssetPredictedReturnPower. (In order to add or remove protected entities, a new
           * CapabilityCommandType must be utilized.) [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProtectedAssetReturnPowerType, uci::type::accessorType::protectedAssetReturnPowerType> ChangeProtectedEntityPredictedReturnPower;

         /** Returns the bounded list that is identified by the ChangeBeam.
           *
           * @return The bounded list identified by ChangeBeam.
           */
         virtual const uci::type::EA_ActivityCommandType::ChangeBeam& getChangeBeam() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ChangeBeam.
           *
           * @return The bounded list identified by ChangeBeam.
           */
         virtual uci::type::EA_ActivityCommandType::ChangeBeam& getChangeBeam()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ChangeBeam.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setChangeBeam(const uci::type::EA_ActivityCommandType::ChangeBeam& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeDependentActivity.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeDependentActivity.
           */
         virtual const uci::type::DependentActivityType& getChangeDependentActivity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeDependentActivity.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeDependentActivity.
           */
         virtual uci::type::DependentActivityType& getChangeDependentActivity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ChangeDependentActivity to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ChangeDependentActivity
           */
         virtual void setChangeDependentActivity(const uci::type::DependentActivityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ChangeDependentActivity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ChangeDependentActivity is emabled or not
           */
         virtual bool hasChangeDependentActivity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ChangeDependentActivity
           *
           * @param type = uci::type::accessorType::dependentActivityType This Accessor's accessor type
           */
         virtual void enableChangeDependentActivity(uci::base::accessorType::AccessorType type = uci::type::accessorType::dependentActivityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ChangeDependentActivity */
         virtual void clearChangeDependentActivity()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeReceiveAntenna.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeReceiveAntenna.
           */
         virtual const uci::type::SelectAntennaType& getChangeReceiveAntenna() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeReceiveAntenna.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeReceiveAntenna.
           */
         virtual uci::type::SelectAntennaType& getChangeReceiveAntenna()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ChangeReceiveAntenna to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ChangeReceiveAntenna
           */
         virtual void setChangeReceiveAntenna(const uci::type::SelectAntennaType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ChangeReceiveAntenna exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ChangeReceiveAntenna is emabled or not
           */
         virtual bool hasChangeReceiveAntenna() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ChangeReceiveAntenna
           *
           * @param type = uci::type::accessorType::selectAntennaType This Accessor's accessor type
           */
         virtual void enableChangeReceiveAntenna(uci::base::accessorType::AccessorType type = uci::type::accessorType::selectAntennaType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ChangeReceiveAntenna */
         virtual void clearChangeReceiveAntenna()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeTransmitAntenna.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeTransmitAntenna.
           */
         virtual const uci::type::SelectAntennaType& getChangeTransmitAntenna() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeTransmitAntenna.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeTransmitAntenna.
           */
         virtual uci::type::SelectAntennaType& getChangeTransmitAntenna()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ChangeTransmitAntenna to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ChangeTransmitAntenna
           */
         virtual void setChangeTransmitAntenna(const uci::type::SelectAntennaType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ChangeTransmitAntenna exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ChangeTransmitAntenna is emabled or not
           */
         virtual bool hasChangeTransmitAntenna() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ChangeTransmitAntenna
           *
           * @param type = uci::type::accessorType::selectAntennaType This Accessor's accessor type
           */
         virtual void enableChangeTransmitAntenna(uci::base::accessorType::AccessorType type = uci::type::accessorType::selectAntennaType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ChangeTransmitAntenna */
         virtual void clearChangeTransmitAntenna()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeTarget.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeTarget.
           */
         virtual const uci::type::EA_ChangeLocationAndPowerType& getChangeTarget() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeTarget.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeTarget.
           */
         virtual uci::type::EA_ChangeLocationAndPowerType& getChangeTarget()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ChangeTarget to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ChangeTarget
           */
         virtual void setChangeTarget(const uci::type::EA_ChangeLocationAndPowerType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ChangeTarget exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ChangeTarget is emabled or not
           */
         virtual bool hasChangeTarget() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ChangeTarget
           *
           * @param type = uci::type::accessorType::eA_ChangeLocationAndPowerType This Accessor's accessor type
           */
         virtual void enableChangeTarget(uci::base::accessorType::AccessorType type = uci::type::accessorType::eA_ChangeLocationAndPowerType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ChangeTarget */
         virtual void clearChangeTarget()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ChangeProtectedEntityPredictedReturnPower.
           *
           * @return The bounded list identified by ChangeProtectedEntityPredictedReturnPower.
           */
         virtual const uci::type::EA_ActivityCommandType::ChangeProtectedEntityPredictedReturnPower& getChangeProtectedEntityPredictedReturnPower() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ChangeProtectedEntityPredictedReturnPower.
           *
           * @return The bounded list identified by ChangeProtectedEntityPredictedReturnPower.
           */
         virtual uci::type::EA_ActivityCommandType::ChangeProtectedEntityPredictedReturnPower& getChangeProtectedEntityPredictedReturnPower()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ChangeProtectedEntityPredictedReturnPower.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setChangeProtectedEntityPredictedReturnPower(const uci::type::EA_ActivityCommandType::ChangeProtectedEntityPredictedReturnPower& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EA_ActivityCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EA_ActivityCommandType to copy from
           */
         EA_ActivityCommandType(const EA_ActivityCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EA_ActivityCommandType to the contents of the
           * EA_ActivityCommandType on the right hand side (rhs) of the assignment operator.EA_ActivityCommandType [only available
           * to derived classes]
           *
           * @param rhs The EA_ActivityCommandType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::EA_ActivityCommandType
           * @return A constant reference to this EA_ActivityCommandType.
           */
         const EA_ActivityCommandType& operator=(const EA_ActivityCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EA_ActivityCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EA_ActivityCommandType_h

