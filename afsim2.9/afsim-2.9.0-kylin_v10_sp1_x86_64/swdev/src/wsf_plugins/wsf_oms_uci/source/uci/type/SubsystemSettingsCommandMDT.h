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
#ifndef Uci__Type__SubsystemSettingsCommandMDT_h
#define Uci__Type__SubsystemSettingsCommandMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SubsystemCommandID_Type_h)
# include "uci/type/SubsystemCommandID_Type.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Type__SubsystemSettingEnum_h)
# include "uci/type/SubsystemSettingEnum.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__CapabilityStateCommandEnum_h)
# include "uci/type/CapabilityStateCommandEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SubsystemSettingsCommandMDT sequence accessor class */
      class SubsystemSettingsCommandMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SubsystemSettingsCommandMDT()
         { }

         /** Returns this accessor's type constant, i.e. SubsystemSettingsCommandMDT
           *
           * @return This accessor's type constant, i.e. SubsystemSettingsCommandMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::subsystemSettingsCommandMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SubsystemSettingsCommandMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandID.
           */
         virtual const uci::type::SubsystemCommandID_Type& getCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandID.
           */
         virtual uci::type::SubsystemCommandID_Type& getCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommandID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommandID
           */
         virtual void setCommandID(const uci::type::SubsystemCommandID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual const uci::type::SubsystemID_Type& getSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual uci::type::SubsystemID_Type& getSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubsystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubsystemID
           */
         virtual void setSubsystemID(const uci::type::SubsystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SubsystemSetting.
           *
           * @return The value of the enumeration identified by SubsystemSetting.
           */
         virtual const uci::type::SubsystemSettingEnum& getSubsystemSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SubsystemSetting.
           *
           * @return The value of the enumeration identified by SubsystemSetting.
           */
         virtual uci::type::SubsystemSettingEnum& getSubsystemSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SubsystemSetting.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSubsystemSetting(const uci::type::SubsystemSettingEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SubsystemSetting.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSubsystemSetting(uci::type::SubsystemSettingEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ParameterNumber.
           *
           * @return The value of the simple primitive data type identified by ParameterNumber.
           */
         virtual xs::UnsignedInt getParameterNumber() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ParameterNumber.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setParameterNumber(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ParameterNumber exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ParameterNumber is emabled or not
           */
         virtual bool hasParameterNumber() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ParameterNumber
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableParameterNumber(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ParameterNumber */
         virtual void clearParameterNumber()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommandedState.
           *
           * @return The value of the enumeration identified by CommandedState.
           */
         virtual const uci::type::CapabilityStateCommandEnum& getCommandedState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommandedState.
           *
           * @return The value of the enumeration identified by CommandedState.
           */
         virtual uci::type::CapabilityStateCommandEnum& getCommandedState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommandedState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommandedState(const uci::type::CapabilityStateCommandEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommandedState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommandedState(uci::type::CapabilityStateCommandEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SubsystemSettingsCommandMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SubsystemSettingsCommandMDT to copy from
           */
         SubsystemSettingsCommandMDT(const SubsystemSettingsCommandMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SubsystemSettingsCommandMDT to the contents of the
           * SubsystemSettingsCommandMDT on the right hand side (rhs) of the assignment operator.SubsystemSettingsCommandMDT [only
           * available to derived classes]
           *
           * @param rhs The SubsystemSettingsCommandMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SubsystemSettingsCommandMDT
           * @return A constant reference to this SubsystemSettingsCommandMDT.
           */
         const SubsystemSettingsCommandMDT& operator=(const SubsystemSettingsCommandMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SubsystemSettingsCommandMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SubsystemSettingsCommandMDT_h

