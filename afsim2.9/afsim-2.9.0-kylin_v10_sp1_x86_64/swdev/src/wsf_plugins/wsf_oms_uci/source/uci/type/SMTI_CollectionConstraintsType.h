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
#ifndef Uci__Type__SMTI_CollectionConstraintsType_h
#define Uci__Type__SMTI_CollectionConstraintsType_h 1

#if !defined(Uci__Type__CollectionConstraintsType_h)
# include "uci/type/CollectionConstraintsType.h"
#endif

#if !defined(Uci__Type__SMTI_CollectionConstraintsQualityType_h)
# include "uci/type/SMTI_CollectionConstraintsQualityType.h"
#endif

#if !defined(Uci__Type__SpeedRangeType_h)
# include "uci/type/SpeedRangeType.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

#if !defined(Uci__Type__SMTI_FalseAlarmType_h)
# include "uci/type/SMTI_FalseAlarmType.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Type__EmconConstraintType_h)
# include "uci/type/EmconConstraintType.h"
#endif

#if !defined(Uci__Type__PositionLocationUncertaintyType_h)
# include "uci/type/PositionLocationUncertaintyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Indicates a set of bounding requirements for the particular collection associated with this command. The subsystem
        * should attempt to satisfy all specified constraints.
        */
      class SMTI_CollectionConstraintsType : public virtual uci::type::CollectionConstraintsType {
      public:

         /** The destructor */
         virtual ~SMTI_CollectionConstraintsType()
         { }

         /** Returns this accessor's type constant, i.e. SMTI_CollectionConstraintsType
           *
           * @return This accessor's type constant, i.e. SMTI_CollectionConstraintsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sMTI_CollectionConstraintsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SMTI_CollectionConstraintsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Quality.
           *
           * @return The acecssor that provides access to the complex content that is identified by Quality.
           */
         virtual const uci::type::SMTI_CollectionConstraintsQualityType& getQuality() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Quality.
           *
           * @return The acecssor that provides access to the complex content that is identified by Quality.
           */
         virtual uci::type::SMTI_CollectionConstraintsQualityType& getQuality()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Quality to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Quality
           */
         virtual void setQuality(const uci::type::SMTI_CollectionConstraintsQualityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Quality exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Quality is emabled or not
           */
         virtual bool hasQuality() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Quality
           *
           * @param type = uci::type::accessorType::sMTI_CollectionConstraintsQualityType This Accessor's accessor type
           */
         virtual void enableQuality(uci::base::accessorType::AccessorType type = uci::type::accessorType::sMTI_CollectionConstraintsQualityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Quality */
         virtual void clearQuality()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TargetSpeed.
           *
           * @return The acecssor that provides access to the complex content that is identified by TargetSpeed.
           */
         virtual const uci::type::SpeedRangeType& getTargetSpeed() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TargetSpeed.
           *
           * @return The acecssor that provides access to the complex content that is identified by TargetSpeed.
           */
         virtual uci::type::SpeedRangeType& getTargetSpeed()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TargetSpeed to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TargetSpeed
           */
         virtual void setTargetSpeed(const uci::type::SpeedRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TargetSpeed exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TargetSpeed is emabled or not
           */
         virtual bool hasTargetSpeed() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TargetSpeed
           *
           * @param type = uci::type::accessorType::speedRangeType This Accessor's accessor type
           */
         virtual void enableTargetSpeed(uci::base::accessorType::AccessorType type = uci::type::accessorType::speedRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TargetSpeed */
         virtual void clearTargetSpeed()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the TargetRCS.
           *
           * @return The value of the simple primitive data type identified by TargetRCS.
           */
         virtual uci::type::DecibelTypeValue getTargetRCS() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the TargetRCS.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTargetRCS(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TargetRCS exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TargetRCS is emabled or not
           */
         virtual bool hasTargetRCS() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TargetRCS
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableTargetRCS(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TargetRCS */
         virtual void clearTargetRCS()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FalseAlarm.
           *
           * @return The acecssor that provides access to the complex content that is identified by FalseAlarm.
           */
         virtual const uci::type::SMTI_FalseAlarmType& getFalseAlarm() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FalseAlarm.
           *
           * @return The acecssor that provides access to the complex content that is identified by FalseAlarm.
           */
         virtual uci::type::SMTI_FalseAlarmType& getFalseAlarm()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FalseAlarm to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FalseAlarm
           */
         virtual void setFalseAlarm(const uci::type::SMTI_FalseAlarmType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FalseAlarm exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FalseAlarm is emabled or not
           */
         virtual bool hasFalseAlarm() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FalseAlarm
           *
           * @param type = uci::type::accessorType::sMTI_FalseAlarmType This Accessor's accessor type
           */
         virtual void enableFalseAlarm(uci::base::accessorType::AccessorType type = uci::type::accessorType::sMTI_FalseAlarmType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FalseAlarm */
         virtual void clearFalseAlarm()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ProbabilityOfDetection.
           *
           * @return The value of the simple primitive data type identified by ProbabilityOfDetection.
           */
         virtual uci::type::PercentTypeValue getProbabilityOfDetection() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ProbabilityOfDetection.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setProbabilityOfDetection(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProbabilityOfDetection exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProbabilityOfDetection is emabled or not
           */
         virtual bool hasProbabilityOfDetection() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProbabilityOfDetection
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableProbabilityOfDetection(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProbabilityOfDetection */
         virtual void clearProbabilityOfDetection()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Emcon.
           *
           * @return The acecssor that provides access to the complex content that is identified by Emcon.
           */
         virtual const uci::type::EmconConstraintType& getEmcon() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Emcon.
           *
           * @return The acecssor that provides access to the complex content that is identified by Emcon.
           */
         virtual uci::type::EmconConstraintType& getEmcon()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Emcon to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Emcon
           */
         virtual void setEmcon(const uci::type::EmconConstraintType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Emcon exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Emcon is emabled or not
           */
         virtual bool hasEmcon() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Emcon
           *
           * @param type = uci::type::accessorType::emconConstraintType This Accessor's accessor type
           */
         virtual void enableEmcon(uci::base::accessorType::AccessorType type = uci::type::accessorType::emconConstraintType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Emcon */
         virtual void clearEmcon()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OwnshipLocationConstraint.
           *
           * @return The acecssor that provides access to the complex content that is identified by OwnshipLocationConstraint.
           */
         virtual const uci::type::PositionLocationUncertaintyType& getOwnshipLocationConstraint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OwnshipLocationConstraint.
           *
           * @return The acecssor that provides access to the complex content that is identified by OwnshipLocationConstraint.
           */
         virtual uci::type::PositionLocationUncertaintyType& getOwnshipLocationConstraint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OwnshipLocationConstraint to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OwnshipLocationConstraint
           */
         virtual void setOwnshipLocationConstraint(const uci::type::PositionLocationUncertaintyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OwnshipLocationConstraint exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OwnshipLocationConstraint is emabled or not
           */
         virtual bool hasOwnshipLocationConstraint() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OwnshipLocationConstraint
           *
           * @param type = uci::type::accessorType::positionLocationUncertaintyType This Accessor's accessor type
           */
         virtual void enableOwnshipLocationConstraint(uci::base::accessorType::AccessorType type = uci::type::accessorType::positionLocationUncertaintyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OwnshipLocationConstraint */
         virtual void clearOwnshipLocationConstraint()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SMTI_CollectionConstraintsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SMTI_CollectionConstraintsType to copy from
           */
         SMTI_CollectionConstraintsType(const SMTI_CollectionConstraintsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SMTI_CollectionConstraintsType to the contents of the
           * SMTI_CollectionConstraintsType on the right hand side (rhs) of the assignment operator.SMTI_CollectionConstraintsType
           * [only available to derived classes]
           *
           * @param rhs The SMTI_CollectionConstraintsType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::SMTI_CollectionConstraintsType
           * @return A constant reference to this SMTI_CollectionConstraintsType.
           */
         const SMTI_CollectionConstraintsType& operator=(const SMTI_CollectionConstraintsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SMTI_CollectionConstraintsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SMTI_CollectionConstraintsType_h

