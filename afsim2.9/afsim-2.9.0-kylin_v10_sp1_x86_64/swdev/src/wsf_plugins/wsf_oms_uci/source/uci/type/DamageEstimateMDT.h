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
#ifndef Uci__Type__DamageEstimateMDT_h
#define Uci__Type__DamageEstimateMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DamageEstimateID_Type_h)
# include "uci/type/DamageEstimateID_Type.h"
#endif

#if !defined(Uci__Type__DamageCategoryEnum_h)
# include "uci/type/DamageCategoryEnum.h"
#endif

#if !defined(Uci__Type__TaskID_Type_h)
# include "uci/type/TaskID_Type.h"
#endif

#if !defined(Uci__Type__Point3D_Type_h)
# include "uci/type/Point3D_Type.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__ZoneType_h)
# include "uci/type/ZoneType.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Type__DamageEstimateDetailsType_h)
# include "uci/type/DamageEstimateDetailsType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DMPI_ViolationType_h)
# include "uci/type/DMPI_ViolationType.h"
#endif

#if !defined(Uci__Type__DamageSummaryType_h)
# include "uci/type/DamageSummaryType.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

#if !defined(Uci__Type__DamageInformationWeatherSourceType_h)
# include "uci/type/DamageInformationWeatherSourceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DamageEstimateMDT sequence accessor class */
      class DamageEstimateMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DamageEstimateMDT()
         { }

         /** Returns this accessor's type constant, i.e. DamageEstimateMDT
           *
           * @return This accessor's type constant, i.e. DamageEstimateMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::damageEstimateMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DamageEstimateMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates any air corridors violated from the release point to the indicated DMPI(s), or any NoFire OpZone collateral
           * damage by the indicated DMPI(s). If EstimateType=Collateral, the desired result is for this list to be empty.
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DMPI_ViolationType, uci::type::accessorType::dMPI_ViolationType> Violation;

         /** Returns the accessor that provides access to the complex content that is identified by the DamageEstimateID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DamageEstimateID.
           */
         virtual const uci::type::DamageEstimateID_Type& getDamageEstimateID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DamageEstimateID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DamageEstimateID.
           */
         virtual uci::type::DamageEstimateID_Type& getDamageEstimateID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DamageEstimateID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DamageEstimateID
           */
         virtual void setDamageEstimateID(const uci::type::DamageEstimateID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EstimateType.
           *
           * @return The value of the enumeration identified by EstimateType.
           */
         virtual const uci::type::DamageCategoryEnum& getEstimateType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EstimateType.
           *
           * @return The value of the enumeration identified by EstimateType.
           */
         virtual uci::type::DamageCategoryEnum& getEstimateType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EstimateType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEstimateType(const uci::type::DamageCategoryEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EstimateType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEstimateType(uci::type::DamageCategoryEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskID.
           */
         virtual const uci::type::TaskID_Type& getTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the TaskID.
           *
           * @return The acecssor that provides access to the complex content that is identified by TaskID.
           */
         virtual uci::type::TaskID_Type& getTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the TaskID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by TaskID
           */
         virtual void setTaskID(const uci::type::TaskID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReleasePoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReleasePoint.
           */
         virtual const uci::type::Point3D_Type& getReleasePoint() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReleasePoint.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReleasePoint.
           */
         virtual uci::type::Point3D_Type& getReleasePoint()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ReleasePoint to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ReleasePoint
           */
         virtual void setReleasePoint(const uci::type::Point3D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ReleasePoint exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReleasePoint is emabled or not
           */
         virtual bool hasReleasePoint() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReleasePoint
           *
           * @param type = uci::type::accessorType::point3D_Type This Accessor's accessor type
           */
         virtual void enableReleasePoint(uci::base::accessorType::AccessorType type = uci::type::accessorType::point3D_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReleasePoint */
         virtual void clearReleasePoint()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CompletionTime.
           *
           * @return The value of the simple primitive data type identified by CompletionTime.
           */
         virtual uci::type::DateTimeTypeValue getCompletionTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CompletionTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCompletionTime(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EstimateZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by EstimateZone.
           */
         virtual const uci::type::ZoneType& getEstimateZone() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EstimateZone.
           *
           * @return The acecssor that provides access to the complex content that is identified by EstimateZone.
           */
         virtual uci::type::ZoneType& getEstimateZone()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EstimateZone to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EstimateZone
           */
         virtual void setEstimateZone(const uci::type::ZoneType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimateZone exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimateZone is emabled or not
           */
         virtual bool hasEstimateZone() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimateZone
           *
           * @param type = uci::type::accessorType::zoneType This Accessor's accessor type
           */
         virtual void enableEstimateZone(uci::base::accessorType::AccessorType type = uci::type::accessorType::zoneType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimateZone */
         virtual void clearEstimateZone()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ReportingThreshold.
           *
           * @return The value of the simple primitive data type identified by ReportingThreshold.
           */
         virtual uci::type::PercentTypeValue getReportingThreshold() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ReportingThreshold.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setReportingThreshold(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Details.
           *
           * @return The acecssor that provides access to the complex content that is identified by Details.
           */
         virtual const uci::type::DamageEstimateDetailsType& getDetails() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Details.
           *
           * @return The acecssor that provides access to the complex content that is identified by Details.
           */
         virtual uci::type::DamageEstimateDetailsType& getDetails()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Details to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Details
           */
         virtual void setDetails(const uci::type::DamageEstimateDetailsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Details exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Details is emabled or not
           */
         virtual bool hasDetails() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Details
           *
           * @param type = uci::type::accessorType::damageEstimateDetailsType This Accessor's accessor type
           */
         virtual void enableDetails(uci::base::accessorType::AccessorType type = uci::type::accessorType::damageEstimateDetailsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Details */
         virtual void clearDetails()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Violation.
           *
           * @return The bounded list identified by Violation.
           */
         virtual const uci::type::DamageEstimateMDT::Violation& getViolation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Violation.
           *
           * @return The bounded list identified by Violation.
           */
         virtual uci::type::DamageEstimateMDT::Violation& getViolation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Violation.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setViolation(const uci::type::DamageEstimateMDT::Violation& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Summary.
           *
           * @return The acecssor that provides access to the complex content that is identified by Summary.
           */
         virtual const uci::type::DamageSummaryType& getSummary() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Summary.
           *
           * @return The acecssor that provides access to the complex content that is identified by Summary.
           */
         virtual uci::type::DamageSummaryType& getSummary()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Summary to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Summary
           */
         virtual void setSummary(const uci::type::DamageSummaryType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Summary exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Summary is emabled or not
           */
         virtual bool hasSummary() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Summary
           *
           * @param type = uci::type::accessorType::damageSummaryType This Accessor's accessor type
           */
         virtual void enableSummary(uci::base::accessorType::AccessorType type = uci::type::accessorType::damageSummaryType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Summary */
         virtual void clearSummary()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the EstimateNarrative.
           *
           * @return The value of the string data type identified by EstimateNarrative.
           */
         virtual const uci::type::VisibleString1024Type& getEstimateNarrative() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the EstimateNarrative.
           *
           * @return The value of the string data type identified by EstimateNarrative.
           */
         virtual uci::type::VisibleString1024Type& getEstimateNarrative()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the EstimateNarrative to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setEstimateNarrative(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the EstimateNarrative to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setEstimateNarrative(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the EstimateNarrative to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setEstimateNarrative(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EstimateNarrative exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EstimateNarrative is emabled or not
           */
         virtual bool hasEstimateNarrative() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EstimateNarrative
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enableEstimateNarrative(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EstimateNarrative */
         virtual void clearEstimateNarrative()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the InformationSource.
           *
           * @return The acecssor that provides access to the complex content that is identified by InformationSource.
           */
         virtual const uci::type::DamageInformationWeatherSourceType& getInformationSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the InformationSource.
           *
           * @return The acecssor that provides access to the complex content that is identified by InformationSource.
           */
         virtual uci::type::DamageInformationWeatherSourceType& getInformationSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the InformationSource to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by InformationSource
           */
         virtual void setInformationSource(const uci::type::DamageInformationWeatherSourceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InformationSource exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InformationSource is emabled or not
           */
         virtual bool hasInformationSource() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InformationSource
           *
           * @param type = uci::type::accessorType::damageInformationWeatherSourceType This Accessor's accessor type
           */
         virtual void enableInformationSource(uci::base::accessorType::AccessorType type = uci::type::accessorType::damageInformationWeatherSourceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InformationSource */
         virtual void clearInformationSource()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DamageEstimateMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DamageEstimateMDT to copy from
           */
         DamageEstimateMDT(const DamageEstimateMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DamageEstimateMDT to the contents of the DamageEstimateMDT on the
           * right hand side (rhs) of the assignment operator.DamageEstimateMDT [only available to derived classes]
           *
           * @param rhs The DamageEstimateMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::DamageEstimateMDT
           * @return A constant reference to this DamageEstimateMDT.
           */
         const DamageEstimateMDT& operator=(const DamageEstimateMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DamageEstimateMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DamageEstimateMDT_h

