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
#ifndef Uci__Type__CapabilityCommandTemporalConstraintsType_h
#define Uci__Type__CapabilityCommandTemporalConstraintsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SchedulingCriticalityEnum_h)
# include "uci/type/SchedulingCriticalityEnum.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CapabilityCommandTemporalConstraintsType sequence accessor class */
      class CapabilityCommandTemporalConstraintsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CapabilityCommandTemporalConstraintsType()
         { }

         /** Returns this accessor's type constant, i.e. CapabilityCommandTemporalConstraintsType
           *
           * @return This accessor's type constant, i.e. CapabilityCommandTemporalConstraintsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::capabilityCommandTemporalConstraintsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CapabilityCommandTemporalConstraintsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TemporalCriticality.
           *
           * @return The value of the enumeration identified by TemporalCriticality.
           */
         virtual const uci::type::SchedulingCriticalityEnum& getTemporalCriticality() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the TemporalCriticality.
           *
           * @return The value of the enumeration identified by TemporalCriticality.
           */
         virtual uci::type::SchedulingCriticalityEnum& getTemporalCriticality()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TemporalCriticality.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTemporalCriticality(const uci::type::SchedulingCriticalityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the TemporalCriticality.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTemporalCriticality(uci::type::SchedulingCriticalityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield TemporalCriticalityis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasTemporalCriticality() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getTemporalCriticality will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::schedulingCriticalityEnum This Accessor's accessor type
           */
         virtual void enableTemporalCriticality(uci::base::accessorType::AccessorType type = uci::type::accessorType::schedulingCriticalityEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearTemporalCriticality()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the StartTimeWindow.
           *
           * @return The acecssor that provides access to the complex content that is identified by StartTimeWindow.
           */
         virtual const uci::type::DateTimeRangeType& getStartTimeWindow() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the StartTimeWindow.
           *
           * @return The acecssor that provides access to the complex content that is identified by StartTimeWindow.
           */
         virtual uci::type::DateTimeRangeType& getStartTimeWindow()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the StartTimeWindow to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by StartTimeWindow
           */
         virtual void setStartTimeWindow(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by StartTimeWindow exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by StartTimeWindow is emabled or not
           */
         virtual bool hasStartTimeWindow() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by StartTimeWindow
           *
           * @param type = uci::type::accessorType::dateTimeRangeType This Accessor's accessor type
           */
         virtual void enableStartTimeWindow(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by StartTimeWindow */
         virtual void clearStartTimeWindow()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the WindowOnFirstOnly.
           *
           * @return The value of the simple primitive data type identified by WindowOnFirstOnly.
           */
         virtual xs::Boolean getWindowOnFirstOnly() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the WindowOnFirstOnly.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setWindowOnFirstOnly(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by WindowOnFirstOnly exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by WindowOnFirstOnly is emabled or not
           */
         virtual bool hasWindowOnFirstOnly() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by WindowOnFirstOnly
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableWindowOnFirstOnly(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by WindowOnFirstOnly */
         virtual void clearWindowOnFirstOnly()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EndTimeWindow.
           *
           * @return The acecssor that provides access to the complex content that is identified by EndTimeWindow.
           */
         virtual const uci::type::DateTimeRangeType& getEndTimeWindow() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EndTimeWindow.
           *
           * @return The acecssor that provides access to the complex content that is identified by EndTimeWindow.
           */
         virtual uci::type::DateTimeRangeType& getEndTimeWindow()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EndTimeWindow to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EndTimeWindow
           */
         virtual void setEndTimeWindow(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EndTimeWindow exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EndTimeWindow is emabled or not
           */
         virtual bool hasEndTimeWindow() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EndTimeWindow
           *
           * @param type = uci::type::accessorType::dateTimeRangeType This Accessor's accessor type
           */
         virtual void enableEndTimeWindow(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EndTimeWindow */
         virtual void clearEndTimeWindow()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CapabilityCommandTemporalConstraintsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CapabilityCommandTemporalConstraintsType to copy from
           */
         CapabilityCommandTemporalConstraintsType(const CapabilityCommandTemporalConstraintsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CapabilityCommandTemporalConstraintsType to the contents of the
           * CapabilityCommandTemporalConstraintsType on the right hand side (rhs) of the assignment
           * operator.CapabilityCommandTemporalConstraintsType [only available to derived classes]
           *
           * @param rhs The CapabilityCommandTemporalConstraintsType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::CapabilityCommandTemporalConstraintsType
           * @return A constant reference to this CapabilityCommandTemporalConstraintsType.
           */
         const CapabilityCommandTemporalConstraintsType& operator=(const CapabilityCommandTemporalConstraintsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CapabilityCommandTemporalConstraintsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CapabilityCommandTemporalConstraintsType_h

