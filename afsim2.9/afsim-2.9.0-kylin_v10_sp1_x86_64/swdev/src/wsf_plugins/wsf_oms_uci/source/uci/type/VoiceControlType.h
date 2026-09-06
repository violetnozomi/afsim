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
#ifndef Uci__Type__VoiceControlType_h
#define Uci__Type__VoiceControlType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__FrequencyType_h)
# include "uci/type/FrequencyType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type contains the connection data needed to comunicate with a vehicle using voice radios */
      class VoiceControlType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~VoiceControlType()
         { }

         /** Returns this accessor's type constant, i.e. VoiceControlType
           *
           * @return This accessor's type constant, i.e. VoiceControlType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::voiceControlType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const VoiceControlType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum inclusive: 0] [Maximum inclusive: 127] */
         typedef uci::base::UnsignedIntAccessor VoiceChannelA;
         //  The type of primitive data accessed by VoiceChannelA
         typedef xs::UnsignedInt VoiceChannelAValue;

         /** [Minimum inclusive: 0] [Maximum inclusive: 127] */
         typedef uci::base::UnsignedIntAccessor VoiceChannelB;
         //  The type of primitive data accessed by VoiceChannelB
         typedef xs::UnsignedInt VoiceChannelBValue;

         /** Returns the value of the SimplePrimitive data type that is identified by the VoiceFrequency.
           *
           * @return The value of the simple primitive data type identified by VoiceFrequency.
           */
         virtual uci::type::FrequencyTypeValue getVoiceFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VoiceFrequency.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVoiceFrequency(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VoiceFrequency exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VoiceFrequency is emabled or not
           */
         virtual bool hasVoiceFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VoiceFrequency
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableVoiceFrequency(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VoiceFrequency */
         virtual void clearVoiceFrequency()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VoiceChannelA.
           *
           * @return The value of the simple primitive data type identified by VoiceChannelA.
           */
         virtual uci::type::VoiceControlType::VoiceChannelAValue getVoiceChannelA() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VoiceChannelA.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVoiceChannelA(uci::type::VoiceControlType::VoiceChannelAValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VoiceChannelA exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VoiceChannelA is emabled or not
           */
         virtual bool hasVoiceChannelA() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VoiceChannelA
           *
           * @param type = uci::type::voiceControlType::accessorType::voiceChannelA This Accessor's accessor type
           */
         virtual void enableVoiceChannelA(uci::base::accessorType::AccessorType type = uci::type::voiceControlType::accessorType::voiceChannelA)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VoiceChannelA */
         virtual void clearVoiceChannelA()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the VoiceChannelB.
           *
           * @return The value of the simple primitive data type identified by VoiceChannelB.
           */
         virtual uci::type::VoiceControlType::VoiceChannelBValue getVoiceChannelB() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the VoiceChannelB.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setVoiceChannelB(uci::type::VoiceControlType::VoiceChannelBValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VoiceChannelB exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VoiceChannelB is emabled or not
           */
         virtual bool hasVoiceChannelB() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VoiceChannelB
           *
           * @param type = uci::type::voiceControlType::accessorType::voiceChannelB This Accessor's accessor type
           */
         virtual void enableVoiceChannelB(uci::base::accessorType::AccessorType type = uci::type::voiceControlType::accessorType::voiceChannelB)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VoiceChannelB */
         virtual void clearVoiceChannelB()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         VoiceControlType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The VoiceControlType to copy from
           */
         VoiceControlType(const VoiceControlType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this VoiceControlType to the contents of the VoiceControlType on the
           * right hand side (rhs) of the assignment operator.VoiceControlType [only available to derived classes]
           *
           * @param rhs The VoiceControlType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::VoiceControlType
           * @return A constant reference to this VoiceControlType.
           */
         const VoiceControlType& operator=(const VoiceControlType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // VoiceControlType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__VoiceControlType_h

