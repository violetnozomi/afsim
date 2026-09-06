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
#ifndef Uci__Type__PathSegmentConditionType_h
#define Uci__Type__PathSegmentConditionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AltitudeRangeType_h)
# include "uci/type/AltitudeRangeType.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__SegmentCaptureType_h)
# include "uci/type/SegmentCaptureType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__EnduranceRemainingType_h)
# include "uci/type/EnduranceRemainingType.h"
#endif

#if !defined(Uci__Type__SystemContingencyLevelEnum_h)
# include "uci/type/SystemContingencyLevelEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PathSegmentConditionType sequence accessor class */
      class PathSegmentConditionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PathSegmentConditionType()
         { }

         /** Returns this accessor's type constant, i.e. PathSegmentConditionType
           *
           * @return This accessor's type constant, i.e. PathSegmentConditionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pathSegmentConditionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PathSegmentConditionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AltitudeRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by AltitudeRange.
           */
         virtual const uci::type::AltitudeRangeType& getAltitudeRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AltitudeRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by AltitudeRange.
           */
         virtual uci::type::AltitudeRangeType& getAltitudeRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AltitudeRange to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AltitudeRange
           */
         virtual void setAltitudeRange(const uci::type::AltitudeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AltitudeRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AltitudeRange is emabled or not
           */
         virtual bool hasAltitudeRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AltitudeRange
           *
           * @param type = uci::type::accessorType::altitudeRangeType This Accessor's accessor type
           */
         virtual void enableAltitudeRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::altitudeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AltitudeRange */
         virtual void clearAltitudeRange()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimeWindow.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimeWindow.
           */
         virtual const uci::type::DateTimeRangeType& getTimeWindow() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TimeWindow.
           *
           * @return The acecssor that provides access to the complex content that is identified by TimeWindow.
           */
         virtual uci::type::DateTimeRangeType& getTimeWindow()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TimeWindow to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TimeWindow
           */
         virtual void setTimeWindow(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by TimeWindow exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by TimeWindow is emabled or not
           */
         virtual bool hasTimeWindow() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by TimeWindow
           *
           * @param type = uci::type::accessorType::dateTimeRangeType This Accessor's accessor type
           */
         virtual void enableTimeWindow(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by TimeWindow */
         virtual void clearTimeWindow()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SegmentCapture.
           *
           * @return The acecssor that provides access to the complex content that is identified by SegmentCapture.
           */
         virtual const uci::type::SegmentCaptureType& getSegmentCapture() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SegmentCapture.
           *
           * @return The acecssor that provides access to the complex content that is identified by SegmentCapture.
           */
         virtual uci::type::SegmentCaptureType& getSegmentCapture()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SegmentCapture to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SegmentCapture
           */
         virtual void setSegmentCapture(const uci::type::SegmentCaptureType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SegmentCapture exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SegmentCapture is emabled or not
           */
         virtual bool hasSegmentCapture() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SegmentCapture
           *
           * @param type = uci::type::accessorType::segmentCaptureType This Accessor's accessor type
           */
         virtual void enableSegmentCapture(uci::base::accessorType::AccessorType type = uci::type::accessorType::segmentCaptureType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SegmentCapture */
         virtual void clearSegmentCapture()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the OperatorInput.
           *
           * @return The value of the simple primitive data type identified by OperatorInput.
           */
         virtual xs::Boolean getOperatorInput() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the OperatorInput.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setOperatorInput(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OperatorInput exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OperatorInput is emabled or not
           */
         virtual bool hasOperatorInput() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OperatorInput
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableOperatorInput(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OperatorInput */
         virtual void clearOperatorInput()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Endurance.
           *
           * @return The acecssor that provides access to the complex content that is identified by Endurance.
           */
         virtual const uci::type::EnduranceRemainingType& getEndurance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Endurance.
           *
           * @return The acecssor that provides access to the complex content that is identified by Endurance.
           */
         virtual uci::type::EnduranceRemainingType& getEndurance()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Endurance to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Endurance
           */
         virtual void setEndurance(const uci::type::EnduranceRemainingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Endurance exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Endurance is emabled or not
           */
         virtual bool hasEndurance() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Endurance
           *
           * @param type = uci::type::accessorType::enduranceRemainingType This Accessor's accessor type
           */
         virtual void enableEndurance(uci::base::accessorType::AccessorType type = uci::type::accessorType::enduranceRemainingType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Endurance */
         virtual void clearEndurance()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ContingencyLevel.
           *
           * @return The value of the enumeration identified by ContingencyLevel.
           */
         virtual const uci::type::SystemContingencyLevelEnum& getContingencyLevel() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ContingencyLevel.
           *
           * @return The value of the enumeration identified by ContingencyLevel.
           */
         virtual uci::type::SystemContingencyLevelEnum& getContingencyLevel()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ContingencyLevel.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setContingencyLevel(const uci::type::SystemContingencyLevelEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ContingencyLevel.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setContingencyLevel(uci::type::SystemContingencyLevelEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield ContingencyLevelis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasContingencyLevel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getContingencyLevel will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::systemContingencyLevelEnum This Accessor's accessor type
           */
         virtual void enableContingencyLevel(uci::base::accessorType::AccessorType type = uci::type::accessorType::systemContingencyLevelEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearContingencyLevel()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PathSegmentConditionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PathSegmentConditionType to copy from
           */
         PathSegmentConditionType(const PathSegmentConditionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PathSegmentConditionType to the contents of the
           * PathSegmentConditionType on the right hand side (rhs) of the assignment operator.PathSegmentConditionType [only
           * available to derived classes]
           *
           * @param rhs The PathSegmentConditionType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::PathSegmentConditionType
           * @return A constant reference to this PathSegmentConditionType.
           */
         const PathSegmentConditionType& operator=(const PathSegmentConditionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PathSegmentConditionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PathSegmentConditionType_h

