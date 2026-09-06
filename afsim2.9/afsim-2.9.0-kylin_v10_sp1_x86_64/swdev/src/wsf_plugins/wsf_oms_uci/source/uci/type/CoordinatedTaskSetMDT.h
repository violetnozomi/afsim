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
#ifndef Uci__Type__CoordinatedTaskSetMDT_h
#define Uci__Type__CoordinatedTaskSetMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CoordinatedTaskSetID_Type_h)
# include "uci/type/CoordinatedTaskSetID_Type.h"
#endif

#if !defined(Uci__Type__SecurityInformationType_h)
# include "uci/type/SecurityInformationType.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

#if !defined(Uci__Type__TimingConstraintsType_h)
# include "uci/type/TimingConstraintsType.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__LinkedTaskType_h)
# include "uci/type/LinkedTaskType.h"
#endif

#if !defined(Uci__Type__FrequencyType_h)
# include "uci/type/FrequencyType.h"
#endif

#if !defined(Uci__Type__CallSignType_h)
# include "uci/type/CallSignType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CoordinatedTaskSetMDT sequence accessor class */
      class CoordinatedTaskSetMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CoordinatedTaskSetMDT()
         { }

         /** Returns this accessor's type constant, i.e. CoordinatedTaskSetMDT
           *
           * @return This accessor's type constant, i.e. CoordinatedTaskSetMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::coordinatedTaskSetMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CoordinatedTaskSetMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Identifies the individual Tasks which belong to the coordinated task set [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::LinkedTaskType, uci::type::accessorType::linkedTaskType> LinkedTask;

         /** Indicates call signs for the operators of systems executing tasks in the coordinated task set. See type annotations
           * for further details. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CallSignType, uci::type::accessorType::callSignType> CallSign;

         /** Returns the accessor that provides access to the complex content that is identified by the CoordinatedTaskSetID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CoordinatedTaskSetID.
           */
         virtual const uci::type::CoordinatedTaskSetID_Type& getCoordinatedTaskSetID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CoordinatedTaskSetID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CoordinatedTaskSetID.
           */
         virtual uci::type::CoordinatedTaskSetID_Type& getCoordinatedTaskSetID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CoordinatedTaskSetID to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CoordinatedTaskSetID
           */
         virtual void setCoordinatedTaskSetID(const uci::type::CoordinatedTaskSetID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * AcceptableClassificationLevel.
           *
           * @return The acecssor that provides access to the complex content that is identified by AcceptableClassificationLevel.
           */
         virtual const uci::type::SecurityInformationType& getAcceptableClassificationLevel() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * AcceptableClassificationLevel.
           *
           * @return The acecssor that provides access to the complex content that is identified by AcceptableClassificationLevel.
           */
         virtual uci::type::SecurityInformationType& getAcceptableClassificationLevel()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AcceptableClassificationLevel to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AcceptableClassificationLevel
           */
         virtual void setAcceptableClassificationLevel(const uci::type::SecurityInformationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AcceptableClassificationLevel exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AcceptableClassificationLevel is emabled or not
           */
         virtual bool hasAcceptableClassificationLevel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AcceptableClassificationLevel
           *
           * @param type = uci::type::accessorType::securityInformationType This Accessor's accessor type
           */
         virtual void enableAcceptableClassificationLevel(uci::base::accessorType::AccessorType type = uci::type::accessorType::securityInformationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AcceptableClassificationLevel */
         virtual void clearAcceptableClassificationLevel()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SpecialInstructions.
           *
           * @return The value of the string data type identified by SpecialInstructions.
           */
         virtual const uci::type::VisibleString1024Type& getSpecialInstructions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the SpecialInstructions.
           *
           * @return The value of the string data type identified by SpecialInstructions.
           */
         virtual uci::type::VisibleString1024Type& getSpecialInstructions()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SpecialInstructions to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setSpecialInstructions(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SpecialInstructions to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSpecialInstructions(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the SpecialInstructions to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setSpecialInstructions(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SpecialInstructions exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SpecialInstructions is emabled or not
           */
         virtual bool hasSpecialInstructions() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SpecialInstructions
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enableSpecialInstructions(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SpecialInstructions */
         virtual void clearSpecialInstructions()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimingConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimingConstraints.
           */
         virtual const uci::type::TimingConstraintsType& getTimingConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimingConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimingConstraints.
           */
         virtual uci::type::TimingConstraintsType& getTimingConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TimingConstraints to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TimingConstraints
           */
         virtual void setTimingConstraints(const uci::type::TimingConstraintsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TimingConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TimingConstraints is emabled or not
           */
         virtual bool hasTimingConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TimingConstraints
           *
           * @param type = uci::type::accessorType::timingConstraintsType This Accessor's accessor type
           */
         virtual void enableTimingConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::timingConstraintsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TimingConstraints */
         virtual void clearTimingConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the CoordinationParameter.
           *
           * @return The value of the string data type identified by CoordinationParameter.
           */
         virtual const uci::type::VisibleString256Type& getCoordinationParameter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the CoordinationParameter.
           *
           * @return The value of the string data type identified by CoordinationParameter.
           */
         virtual uci::type::VisibleString256Type& getCoordinationParameter()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CoordinationParameter to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setCoordinationParameter(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CoordinationParameter to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCoordinationParameter(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CoordinationParameter to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCoordinationParameter(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CoordinationParameter exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CoordinationParameter is emabled or not
           */
         virtual bool hasCoordinationParameter() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CoordinationParameter
           *
           * @param type = uci::type::accessorType::visibleString256Type This Accessor's accessor type
           */
         virtual void enableCoordinationParameter(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString256Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CoordinationParameter */
         virtual void clearCoordinationParameter()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the LinkedTask.
           *
           * @return The bounded list identified by LinkedTask.
           */
         virtual const uci::type::CoordinatedTaskSetMDT::LinkedTask& getLinkedTask() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the LinkedTask.
           *
           * @return The bounded list identified by LinkedTask.
           */
         virtual uci::type::CoordinatedTaskSetMDT::LinkedTask& getLinkedTask()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the LinkedTask.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setLinkedTask(const uci::type::CoordinatedTaskSetMDT::LinkedTask& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the VOIP_Number.
           *
           * @return The value of the string data type identified by VOIP_Number.
           */
         virtual const uci::type::VisibleString256Type& getVOIP_Number() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the VOIP_Number.
           *
           * @return The value of the string data type identified by VOIP_Number.
           */
         virtual uci::type::VisibleString256Type& getVOIP_Number()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the VOIP_Number to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setVOIP_Number(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the VOIP_Number to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setVOIP_Number(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the VOIP_Number to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setVOIP_Number(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by VOIP_Number exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by VOIP_Number is emabled or not
           */
         virtual bool hasVOIP_Number() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by VOIP_Number
           *
           * @param type = uci::type::accessorType::visibleString256Type This Accessor's accessor type
           */
         virtual void enableVOIP_Number(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString256Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by VOIP_Number */
         virtual void clearVOIP_Number()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ChatRoom.
           *
           * @return The value of the string data type identified by ChatRoom.
           */
         virtual const uci::type::VisibleString256Type& getChatRoom() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ChatRoom.
           *
           * @return The value of the string data type identified by ChatRoom.
           */
         virtual uci::type::VisibleString256Type& getChatRoom()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ChatRoom to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setChatRoom(const uci::type::VisibleString256Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ChatRoom to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setChatRoom(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ChatRoom to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setChatRoom(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ChatRoom exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ChatRoom is emabled or not
           */
         virtual bool hasChatRoom() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ChatRoom
           *
           * @param type = uci::type::accessorType::visibleString256Type This Accessor's accessor type
           */
         virtual void enableChatRoom(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString256Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ChatRoom */
         virtual void clearChatRoom()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Frequency.
           *
           * @return The value of the simple primitive data type identified by Frequency.
           */
         virtual uci::type::FrequencyTypeValue getFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Frequency.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFrequency(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Frequency exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Frequency is emabled or not
           */
         virtual bool hasFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Frequency
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableFrequency(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Frequency */
         virtual void clearFrequency()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CallSign.
           *
           * @return The bounded list identified by CallSign.
           */
         virtual const uci::type::CoordinatedTaskSetMDT::CallSign& getCallSign() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CallSign.
           *
           * @return The bounded list identified by CallSign.
           */
         virtual uci::type::CoordinatedTaskSetMDT::CallSign& getCallSign()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CallSign.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCallSign(const uci::type::CoordinatedTaskSetMDT::CallSign& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CoordinatedTaskSetMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CoordinatedTaskSetMDT to copy from
           */
         CoordinatedTaskSetMDT(const CoordinatedTaskSetMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CoordinatedTaskSetMDT to the contents of the CoordinatedTaskSetMDT
           * on the right hand side (rhs) of the assignment operator.CoordinatedTaskSetMDT [only available to derived classes]
           *
           * @param rhs The CoordinatedTaskSetMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::CoordinatedTaskSetMDT
           * @return A constant reference to this CoordinatedTaskSetMDT.
           */
         const CoordinatedTaskSetMDT& operator=(const CoordinatedTaskSetMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CoordinatedTaskSetMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CoordinatedTaskSetMDT_h

