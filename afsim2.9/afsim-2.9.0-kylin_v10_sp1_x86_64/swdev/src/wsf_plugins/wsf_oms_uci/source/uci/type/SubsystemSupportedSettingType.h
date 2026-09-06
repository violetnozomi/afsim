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
#ifndef Uci__Type__SubsystemSupportedSettingType_h
#define Uci__Type__SubsystemSupportedSettingType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SubsystemSettingEnum_h)
# include "uci/type/SubsystemSettingEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SubsystemSettingParameterType_h)
# include "uci/type/SubsystemSettingParameterType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SubsystemSupportedSettingType sequence accessor class */
      class SubsystemSupportedSettingType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SubsystemSupportedSettingType()
         { }

         /** Returns this accessor's type constant, i.e. SubsystemSupportedSettingType
           *
           * @return This accessor's type constant, i.e. SubsystemSupportedSettingType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::subsystemSupportedSettingType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SubsystemSupportedSettingType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a Subsystem defined/unique amplifying "parameter" of the sibling SubsystemSetting that is supported. A
           * parameter is flexible abstraction that might indicate a "level" or other granular means of controlling/configuring a
           * Subsystem setting (SubsystemSettingEnum) to meet the unique needs of a specific Subsystem. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SubsystemSettingParameterType, uci::type::accessorType::subsystemSettingParameterType> SettingParameter;

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


         /** Returns the bounded list that is identified by the SettingParameter.
           *
           * @return The bounded list identified by SettingParameter.
           */
         virtual const uci::type::SubsystemSupportedSettingType::SettingParameter& getSettingParameter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SettingParameter.
           *
           * @return The bounded list identified by SettingParameter.
           */
         virtual uci::type::SubsystemSupportedSettingType::SettingParameter& getSettingParameter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SettingParameter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSettingParameter(const uci::type::SubsystemSupportedSettingType::SettingParameter& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SubsystemSupportedSettingType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SubsystemSupportedSettingType to copy from
           */
         SubsystemSupportedSettingType(const SubsystemSupportedSettingType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SubsystemSupportedSettingType to the contents of the
           * SubsystemSupportedSettingType on the right hand side (rhs) of the assignment operator.SubsystemSupportedSettingType
           * [only available to derived classes]
           *
           * @param rhs The SubsystemSupportedSettingType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::SubsystemSupportedSettingType
           * @return A constant reference to this SubsystemSupportedSettingType.
           */
         const SubsystemSupportedSettingType& operator=(const SubsystemSupportedSettingType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SubsystemSupportedSettingType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SubsystemSupportedSettingType_h

