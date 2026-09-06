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
#ifndef Uci__Type__NavigationSourceType_h
#define Uci__Type__NavigationSourceType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MissionPlanNavigationType_h)
# include "uci/type/MissionPlanNavigationType.h"
#endif

#if !defined(Uci__Type__Point3D_Type_h)
# include "uci/type/Point3D_Type.h"
#endif

#if !defined(Uci__Type__EmptyType_h)
# include "uci/type/EmptyType.h"
#endif

#if !defined(Uci__Type__AutopilotModeEnum_h)
# include "uci/type/AutopilotModeEnum.h"
#endif

#if !defined(Uci__Type__RelativeNavigationType_h)
# include "uci/type/RelativeNavigationType.h"
#endif

#if !defined(Uci__Type__SlavedNavigationType_h)
# include "uci/type/SlavedNavigationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the NavigationSourceType choice accessor class */
      class NavigationSourceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~NavigationSourceType()
         { }

         /** Returns this accessor's type constant, i.e. NavigationSourceType
           *
           * @return This accessor's type constant, i.e. NavigationSourceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::navigationSourceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const NavigationSourceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum NavigationSourceTypeChoice {
            NAVIGATIONSOURCETYPE_CHOICE_NONE,
            NAVIGATIONSOURCETYPE_CHOICE_MISSIONPLANNAVIGATION,
            NAVIGATIONSOURCETYPE_CHOICE_FIXEDNAVIGATION,
            NAVIGATIONSOURCETYPE_CHOICE_MANUALNAVIGATION,
            NAVIGATIONSOURCETYPE_CHOICE_AUTOPILOTNAVIGATION,
            NAVIGATIONSOURCETYPE_CHOICE_RELATIVENAVIGATION,
            NAVIGATIONSOURCETYPE_CHOICE_SLAVEDNAVIGATION
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual NavigationSourceTypeChoice getNavigationSourceTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getNavigationSourceTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In addition, the
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
         virtual void setNavigationSourceTypeChoiceOrdinal(NavigationSourceTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanNavigation.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanNavigation.
           */
         virtual const uci::type::MissionPlanNavigationType& getMissionPlanNavigation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanNavigation.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanNavigation.
           */
         virtual uci::type::MissionPlanNavigationType& getMissionPlanNavigation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionPlanNavigation to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionPlanNavigation
           */
         virtual void setMissionPlanNavigation(const uci::type::MissionPlanNavigationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FixedNavigation.
           *
           * @return The acecssor that provides access to the complex content that is identified by FixedNavigation.
           */
         virtual const uci::type::Point3D_Type& getFixedNavigation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FixedNavigation.
           *
           * @return The acecssor that provides access to the complex content that is identified by FixedNavigation.
           */
         virtual uci::type::Point3D_Type& getFixedNavigation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FixedNavigation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FixedNavigation
           */
         virtual void setFixedNavigation(const uci::type::Point3D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ManualNavigation.
           *
           * @return The value of the string data type identified by ManualNavigation.
           */
         virtual const uci::type::EmptyType& getManualNavigation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ManualNavigation.
           *
           * @return The value of the string data type identified by ManualNavigation.
           */
         virtual uci::type::EmptyType& getManualNavigation()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ManualNavigation to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setManualNavigation(const uci::type::EmptyType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ManualNavigation to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setManualNavigation(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ManualNavigation to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setManualNavigation(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AutoPilotNavigation.
           *
           * @return The value of the enumeration identified by AutoPilotNavigation.
           */
         virtual const uci::type::AutopilotModeEnum& getAutoPilotNavigation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the AutoPilotNavigation.
           *
           * @return The value of the enumeration identified by AutoPilotNavigation.
           */
         virtual uci::type::AutopilotModeEnum& getAutoPilotNavigation()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AutoPilotNavigation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAutoPilotNavigation(const uci::type::AutopilotModeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the AutoPilotNavigation.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setAutoPilotNavigation(uci::type::AutopilotModeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RelativeNavigation.
           *
           * @return The acecssor that provides access to the complex content that is identified by RelativeNavigation.
           */
         virtual const uci::type::RelativeNavigationType& getRelativeNavigation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RelativeNavigation.
           *
           * @return The acecssor that provides access to the complex content that is identified by RelativeNavigation.
           */
         virtual uci::type::RelativeNavigationType& getRelativeNavigation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RelativeNavigation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RelativeNavigation
           */
         virtual void setRelativeNavigation(const uci::type::RelativeNavigationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SlavedNavigation.
           *
           * @return The acecssor that provides access to the complex content that is identified by SlavedNavigation.
           */
         virtual const uci::type::SlavedNavigationType& getSlavedNavigation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SlavedNavigation.
           *
           * @return The acecssor that provides access to the complex content that is identified by SlavedNavigation.
           */
         virtual uci::type::SlavedNavigationType& getSlavedNavigation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SlavedNavigation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SlavedNavigation
           */
         virtual void setSlavedNavigation(const uci::type::SlavedNavigationType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         NavigationSourceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The NavigationSourceType to copy from
           */
         NavigationSourceType(const NavigationSourceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this NavigationSourceType to the contents of the NavigationSourceType
           * on the right hand side (rhs) of the assignment operator.NavigationSourceType [only available to derived classes]
           *
           * @param rhs The NavigationSourceType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::NavigationSourceType
           * @return A constant reference to this NavigationSourceType.
           */
         const NavigationSourceType& operator=(const NavigationSourceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // NavigationSourceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__NavigationSourceType_h

