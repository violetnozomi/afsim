// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__RFD_GainSettingType_h
#define Uci__Type__RFD_GainSettingType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__GainRangeType_h)
# include "uci/type/GainRangeType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__VisibleString32Type_h)
# include "uci/type/VisibleString32Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RFD_GainSettingType choice accessor class */
      class RFD_GainSettingType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RFD_GainSettingType()
         { }

         /** Returns this accessor's type constant, i.e. RFD_GainSettingType
           *
           * @return This accessor's type constant, i.e. RFD_GainSettingType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rFD_GainSettingType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RFD_GainSettingType& accessor)
            throw(uci::base::UCIException) = 0;


         /** A list of pre-defined gain settings. Can be defined as Low, Med, High, or 1,2,3, or other subsystem-specific list.
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::VisibleString32Type, uci::type::accessorType::visibleString32Type> ValidGainSettings;

         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum RFD_GainSettingTypeChoice {
            RFD_GAINSETTINGTYPE_CHOICE_NONE,
            RFD_GAINSETTINGTYPE_CHOICE_GAINRANGE,
            RFD_GAINSETTINGTYPE_CHOICE_VALIDGAINSETTINGS
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual RFD_GainSettingTypeChoice getRFD_GainSettingTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getRFD_GainSettingTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition, the
           * access methods associated with the chosen element will be enabled and will provide access to the chosen element.
           *
           * @param chosenElementOrdinal The ordinal to set this choice's selected ordinal to.
           * @param type = uci::base::accessorType::null The type of data that is to be made available when the ordinal is set.
           *      The use of this parameter provides support for inheritable types. This parameter defaults to
           *      uci::base::accessorType::null that is used to indicate that the access methods associated with the chosen
           *      element shall provide access to data of the type that was specified for that element in the choice in the OMS
           *      schema, i.e. the chosen element's base type. If specified, this field must either be a type ID associated with
           *      the chosen element's base type or a type ID associated with a type that is derived from the chosen element's
           *      base type.
           */
         virtual void setRFD_GainSettingTypeChoiceOrdinal(RFD_GainSettingTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GainRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by GainRange.
           */
         virtual const uci::type::GainRangeType& getGainRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GainRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by GainRange.
           */
         virtual uci::type::GainRangeType& getGainRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the GainRange to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by GainRange
           */
         virtual void setGainRange(const uci::type::GainRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ValidGainSettings.
           *
           * @return The bounded list identified by ValidGainSettings.
           */
         virtual const uci::type::RFD_GainSettingType::ValidGainSettings& getValidGainSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ValidGainSettings.
           *
           * @return The bounded list identified by ValidGainSettings.
           */
         virtual uci::type::RFD_GainSettingType::ValidGainSettings& getValidGainSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ValidGainSettings.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setValidGainSettings(const uci::type::RFD_GainSettingType::ValidGainSettings& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RFD_GainSettingType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RFD_GainSettingType to copy from
           */
         RFD_GainSettingType(const RFD_GainSettingType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RFD_GainSettingType to the contents of the RFD_GainSettingType on
           * the right hand side (rhs) of the assignment operator.RFD_GainSettingType [only available to derived classes]
           *
           * @param rhs The RFD_GainSettingType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::RFD_GainSettingType
           * @return A constant reference to this RFD_GainSettingType.
           */
         const RFD_GainSettingType& operator=(const RFD_GainSettingType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RFD_GainSettingType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RFD_GainSettingType_h

