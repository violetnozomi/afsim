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
#ifndef Uci__Type__STANAG_4607_PackingPlanType_h
#define Uci__Type__STANAG_4607_PackingPlanType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Xs__Type__StringXmlSchemaPrimitives_h)
# include "xs/type/stringXmlSchemaPrimitives.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the STANAG_4607_PackingPlanType sequence accessor class */
      class STANAG_4607_PackingPlanType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~STANAG_4607_PackingPlanType()
         { }

         /** Returns this accessor's type constant, i.e. STANAG_4607_PackingPlanType
           *
           * @return This accessor's type constant, i.e. STANAG_4607_PackingPlanType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sTANAG_4607_PackingPlanType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const STANAG_4607_PackingPlanType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum length: 0] [Maximum length: 3] [White space: preserve] [Pattern: "[A-Z ]{2}[A-Z ]?"] */
         typedef xs::String ClassificationSystem;

         /** [Minimum length: 0] [Maximum length: 11] [Pattern: "[A-Za-z0-9 \-_]{0,11}"] */
         typedef xs::String OriginatorIdentifier;

         /** [Minimum length: 0] [Maximum length: 13] [Pattern: "[A-Za-z0-9 \-_]{0,13}"] */
         typedef xs::String MissionPlan;

         /** [Minimum length: 0] [Maximum length: 11] [Pattern: "[A-Za-z0-9 \-_]{0,11}"] */
         typedef xs::String Recipient;

         /** [Minimum length: 0] [Maximum length: 81] [Pattern: "[ -~]{0,81}"] */
         typedef xs::String FreeText;

         /** Returns the value of the SimplePrimitive data type that is identified by the ExerciseIdentifier.
           *
           * @return The value of the simple primitive data type identified by ExerciseIdentifier.
           */
         virtual xs::UnsignedInt getExerciseIdentifier() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ExerciseIdentifier.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setExerciseIdentifier(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ExerciseIdentifier exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ExerciseIdentifier is emabled or not
           */
         virtual bool hasExerciseIdentifier() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ExerciseIdentifier
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableExerciseIdentifier(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ExerciseIdentifier */
         virtual void clearExerciseIdentifier()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ClassificationSystem.
           *
           * @return The value of the string data type identified by ClassificationSystem.
           */
         virtual const uci::type::STANAG_4607_PackingPlanType::ClassificationSystem& getClassificationSystem() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ClassificationSystem.
           *
           * @return The value of the string data type identified by ClassificationSystem.
           */
         virtual uci::type::STANAG_4607_PackingPlanType::ClassificationSystem& getClassificationSystem()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ClassificationSystem to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setClassificationSystem(const uci::type::STANAG_4607_PackingPlanType::ClassificationSystem& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ClassificationSystem to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setClassificationSystem(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ClassificationSystem to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setClassificationSystem(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ClassificationSystem exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ClassificationSystem is emabled or not
           */
         virtual bool hasClassificationSystem() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ClassificationSystem
           *
           * @param type = uci::type::sTANAG_4607_PackingPlanType::accessorType::classificationSystem This Accessor's accessor
           *      type
           */
         virtual void enableClassificationSystem(uci::base::accessorType::AccessorType type = uci::type::sTANAG_4607_PackingPlanType::accessorType::classificationSystem)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ClassificationSystem */
         virtual void clearClassificationSystem()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the OriginatorIdentifier.
           *
           * @return The value of the string data type identified by OriginatorIdentifier.
           */
         virtual const uci::type::STANAG_4607_PackingPlanType::OriginatorIdentifier& getOriginatorIdentifier() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the OriginatorIdentifier.
           *
           * @return The value of the string data type identified by OriginatorIdentifier.
           */
         virtual uci::type::STANAG_4607_PackingPlanType::OriginatorIdentifier& getOriginatorIdentifier()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the OriginatorIdentifier to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setOriginatorIdentifier(const uci::type::STANAG_4607_PackingPlanType::OriginatorIdentifier& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the OriginatorIdentifier to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setOriginatorIdentifier(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the OriginatorIdentifier to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setOriginatorIdentifier(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OriginatorIdentifier exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OriginatorIdentifier is emabled or not
           */
         virtual bool hasOriginatorIdentifier() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OriginatorIdentifier
           *
           * @param type = uci::type::sTANAG_4607_PackingPlanType::accessorType::originatorIdentifier This Accessor's accessor
           *      type
           */
         virtual void enableOriginatorIdentifier(uci::base::accessorType::AccessorType type = uci::type::sTANAG_4607_PackingPlanType::accessorType::originatorIdentifier)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OriginatorIdentifier */
         virtual void clearOriginatorIdentifier()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MissionPlan.
           *
           * @return The value of the string data type identified by MissionPlan.
           */
         virtual const uci::type::STANAG_4607_PackingPlanType::MissionPlan& getMissionPlan() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MissionPlan.
           *
           * @return The value of the string data type identified by MissionPlan.
           */
         virtual uci::type::STANAG_4607_PackingPlanType::MissionPlan& getMissionPlan()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionPlan to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setMissionPlan(const uci::type::STANAG_4607_PackingPlanType::MissionPlan& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionPlan to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMissionPlan(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MissionPlan to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMissionPlan(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MissionPlan exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MissionPlan is emabled or not
           */
         virtual bool hasMissionPlan() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MissionPlan
           *
           * @param type = uci::type::sTANAG_4607_PackingPlanType::accessorType::missionPlan This Accessor's accessor type
           */
         virtual void enableMissionPlan(uci::base::accessorType::AccessorType type = uci::type::sTANAG_4607_PackingPlanType::accessorType::missionPlan)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MissionPlan */
         virtual void clearMissionPlan()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Recipient.
           *
           * @return The value of the string data type identified by Recipient.
           */
         virtual const uci::type::STANAG_4607_PackingPlanType::Recipient& getRecipient() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Recipient.
           *
           * @return The value of the string data type identified by Recipient.
           */
         virtual uci::type::STANAG_4607_PackingPlanType::Recipient& getRecipient()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Recipient to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setRecipient(const uci::type::STANAG_4607_PackingPlanType::Recipient& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Recipient to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRecipient(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Recipient to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRecipient(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Recipient exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Recipient is emabled or not
           */
         virtual bool hasRecipient() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Recipient
           *
           * @param type = uci::type::sTANAG_4607_PackingPlanType::accessorType::recipient This Accessor's accessor type
           */
         virtual void enableRecipient(uci::base::accessorType::AccessorType type = uci::type::sTANAG_4607_PackingPlanType::accessorType::recipient)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Recipient */
         virtual void clearRecipient()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FreeText.
           *
           * @return The value of the string data type identified by FreeText.
           */
         virtual const uci::type::STANAG_4607_PackingPlanType::FreeText& getFreeText() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FreeText.
           *
           * @return The value of the string data type identified by FreeText.
           */
         virtual uci::type::STANAG_4607_PackingPlanType::FreeText& getFreeText()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FreeText to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFreeText(const uci::type::STANAG_4607_PackingPlanType::FreeText& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FreeText to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFreeText(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FreeText to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFreeText(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FreeText exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FreeText is emabled or not
           */
         virtual bool hasFreeText() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FreeText
           *
           * @param type = uci::type::sTANAG_4607_PackingPlanType::accessorType::freeText This Accessor's accessor type
           */
         virtual void enableFreeText(uci::base::accessorType::AccessorType type = uci::type::sTANAG_4607_PackingPlanType::accessorType::freeText)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FreeText */
         virtual void clearFreeText()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         STANAG_4607_PackingPlanType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The STANAG_4607_PackingPlanType to copy from
           */
         STANAG_4607_PackingPlanType(const STANAG_4607_PackingPlanType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this STANAG_4607_PackingPlanType to the contents of the
           * STANAG_4607_PackingPlanType on the right hand side (rhs) of the assignment operator.STANAG_4607_PackingPlanType [only
           * available to derived classes]
           *
           * @param rhs The STANAG_4607_PackingPlanType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::STANAG_4607_PackingPlanType
           * @return A constant reference to this STANAG_4607_PackingPlanType.
           */
         const STANAG_4607_PackingPlanType& operator=(const STANAG_4607_PackingPlanType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // STANAG_4607_PackingPlanType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__STANAG_4607_PackingPlanType_h

