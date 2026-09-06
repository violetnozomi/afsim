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
#ifndef Uci__Type__SignalIdentityQueryType_h
#define Uci__Type__SignalIdentityQueryType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EmitterFunctionType_h)
# include "uci/type/EmitterFunctionType.h"
#endif

#if !defined(Uci__Type__CountryCodeType_h)
# include "uci/type/CountryCodeType.h"
#endif

#if !defined(Uci__Type__EnvironmentEnum_h)
# include "uci/type/EnvironmentEnum.h"
#endif

#if !defined(Uci__Type__ThreatEnum_h)
# include "uci/type/ThreatEnum.h"
#endif

#if !defined(Uci__Type__NotationType_h)
# include "uci/type/NotationType.h"
#endif

#if !defined(Xs__Type__StringXmlSchemaPrimitives_h)
# include "xs/type/stringXmlSchemaPrimitives.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type is intended to support a QFT for message that contain Signal data, filtered by identity characteristics of
        * the Signal.
        */
      class SignalIdentityQueryType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SignalIdentityQueryType()
         { }

         /** Returns this accessor's type constant, i.e. SignalIdentityQueryType
           *
           * @return This accessor's type constant, i.e. SignalIdentityQueryType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::signalIdentityQueryType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SignalIdentityQueryType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The "emitter function" or one of the functions of an Emitter to query. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EmitterFunctionType, uci::type::accessorType::emitterFunctionType> EmitterFunction;

         /** The DOD standard country code designator to which an entity owes its allegiance to query. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CountryCodeType, uci::type::accessorType::countryCodeType> Allegiance;

         /** The environment or "domain" in which an Entity is operating to query. Derived from MIL-STD-6016; see MIL-STD-6016
           * DFI/DUI 275/004 for additional details. [Minimum occurrences: 0] [Maximum occurrences: 6]
           */
         typedef uci::base::BoundedList<uci::type::EnvironmentEnum, uci::type::accessorType::environmentEnum> EnvironmentType;

         /** The threat type of an entity to query. Derived from MIL-STD-6016. [Minimum occurrences: 0] [Maximum occurrences: 20] */
         typedef uci::base::BoundedList<uci::type::ThreatEnum, uci::type::accessorType::threatEnum> ThreatType;

         /** The ELNOT or CENOT notation of an entity or signal to query. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::NotationType, uci::type::accessorType::notationType> Notation;

         /** [Maximum length: 5] [Pattern: "[0-9A-Z\*]{5}"] */
         typedef xs::String NotationWildcard;

         /** Returns the bounded list that is identified by the EmitterFunction.
           *
           * @return The bounded list identified by EmitterFunction.
           */
         virtual const uci::type::SignalIdentityQueryType::EmitterFunction& getEmitterFunction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EmitterFunction.
           *
           * @return The bounded list identified by EmitterFunction.
           */
         virtual uci::type::SignalIdentityQueryType::EmitterFunction& getEmitterFunction()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EmitterFunction.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEmitterFunction(const uci::type::SignalIdentityQueryType::EmitterFunction& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Allegiance.
           *
           * @return The bounded list identified by Allegiance.
           */
         virtual const uci::type::SignalIdentityQueryType::Allegiance& getAllegiance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Allegiance.
           *
           * @return The bounded list identified by Allegiance.
           */
         virtual uci::type::SignalIdentityQueryType::Allegiance& getAllegiance()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Allegiance.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAllegiance(const uci::type::SignalIdentityQueryType::Allegiance& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EnvironmentType.
           *
           * @return The bounded list identified by EnvironmentType.
           */
         virtual const uci::type::SignalIdentityQueryType::EnvironmentType& getEnvironmentType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the EnvironmentType.
           *
           * @return The bounded list identified by EnvironmentType.
           */
         virtual uci::type::SignalIdentityQueryType::EnvironmentType& getEnvironmentType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the EnvironmentType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setEnvironmentType(const uci::type::SignalIdentityQueryType::EnvironmentType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ThreatType.
           *
           * @return The bounded list identified by ThreatType.
           */
         virtual const uci::type::SignalIdentityQueryType::ThreatType& getThreatType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ThreatType.
           *
           * @return The bounded list identified by ThreatType.
           */
         virtual uci::type::SignalIdentityQueryType::ThreatType& getThreatType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ThreatType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setThreatType(const uci::type::SignalIdentityQueryType::ThreatType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Notation.
           *
           * @return The bounded list identified by Notation.
           */
         virtual const uci::type::SignalIdentityQueryType::Notation& getNotation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Notation.
           *
           * @return The bounded list identified by Notation.
           */
         virtual uci::type::SignalIdentityQueryType::Notation& getNotation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Notation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setNotation(const uci::type::SignalIdentityQueryType::Notation& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the NotationWildcard.
           *
           * @return The value of the string data type identified by NotationWildcard.
           */
         virtual const uci::type::SignalIdentityQueryType::NotationWildcard& getNotationWildcard() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the NotationWildcard.
           *
           * @return The value of the string data type identified by NotationWildcard.
           */
         virtual uci::type::SignalIdentityQueryType::NotationWildcard& getNotationWildcard()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the NotationWildcard to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setNotationWildcard(const uci::type::SignalIdentityQueryType::NotationWildcard& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the NotationWildcard to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setNotationWildcard(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the NotationWildcard to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setNotationWildcard(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NotationWildcard exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NotationWildcard is emabled or not
           */
         virtual bool hasNotationWildcard() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NotationWildcard
           *
           * @param type = uci::type::signalIdentityQueryType::accessorType::notationWildcard This Accessor's accessor type
           */
         virtual void enableNotationWildcard(uci::base::accessorType::AccessorType type = uci::type::signalIdentityQueryType::accessorType::notationWildcard)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NotationWildcard */
         virtual void clearNotationWildcard()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SignalIdentityQueryType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SignalIdentityQueryType to copy from
           */
         SignalIdentityQueryType(const SignalIdentityQueryType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SignalIdentityQueryType to the contents of the
           * SignalIdentityQueryType on the right hand side (rhs) of the assignment operator.SignalIdentityQueryType [only
           * available to derived classes]
           *
           * @param rhs The SignalIdentityQueryType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SignalIdentityQueryType
           * @return A constant reference to this SignalIdentityQueryType.
           */
         const SignalIdentityQueryType& operator=(const SignalIdentityQueryType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SignalIdentityQueryType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SignalIdentityQueryType_h

