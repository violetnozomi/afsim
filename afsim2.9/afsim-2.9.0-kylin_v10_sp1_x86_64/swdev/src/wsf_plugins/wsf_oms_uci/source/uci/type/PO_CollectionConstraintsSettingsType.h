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
#ifndef Uci__Type__PO_CollectionConstraintsSettingsType_h
#define Uci__Type__PO_CollectionConstraintsSettingsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PO_AngleConstraintControlsType_h)
# include "uci/type/PO_AngleConstraintControlsType.h"
#endif

#if !defined(Uci__Type__PO_SlantRangeConstraintControlsType_h)
# include "uci/type/PO_SlantRangeConstraintControlsType.h"
#endif

#if !defined(Uci__Type__PO_CollectionPatternConstraintControlsType_h)
# include "uci/type/PO_CollectionPatternConstraintControlsType.h"
#endif

#if !defined(Uci__Type__PO_SweepSpeedConstraintControlsType_h)
# include "uci/type/PO_SweepSpeedConstraintControlsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type is used to specify a set of constraints that constrain when a PO or AO sensor can start collecting the
        * requested image data.
        */
      class PO_CollectionConstraintsSettingsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PO_CollectionConstraintsSettingsType()
         { }

         /** Returns this accessor's type constant, i.e. PO_CollectionConstraintsSettingsType
           *
           * @return This accessor's type constant, i.e. PO_CollectionConstraintsSettingsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_CollectionConstraintsSettingsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_CollectionConstraintsSettingsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MapAngle.
           *
           * @return The acecssor that provides access to the complex content that is identified by MapAngle.
           */
         virtual const uci::type::PO_AngleConstraintControlsType& getMapAngle() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MapAngle.
           *
           * @return The acecssor that provides access to the complex content that is identified by MapAngle.
           */
         virtual uci::type::PO_AngleConstraintControlsType& getMapAngle()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MapAngle to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MapAngle
           */
         virtual void setMapAngle(const uci::type::PO_AngleConstraintControlsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MapAngle exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MapAngle is emabled or not
           */
         virtual bool hasMapAngle() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MapAngle
           *
           * @param type = uci::type::accessorType::pO_AngleConstraintControlsType This Accessor's accessor type
           */
         virtual void enableMapAngle(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_AngleConstraintControlsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MapAngle */
         virtual void clearMapAngle()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GrazingAngle.
           *
           * @return The acecssor that provides access to the complex content that is identified by GrazingAngle.
           */
         virtual const uci::type::PO_AngleConstraintControlsType& getGrazingAngle() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GrazingAngle.
           *
           * @return The acecssor that provides access to the complex content that is identified by GrazingAngle.
           */
         virtual uci::type::PO_AngleConstraintControlsType& getGrazingAngle()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the GrazingAngle to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by GrazingAngle
           */
         virtual void setGrazingAngle(const uci::type::PO_AngleConstraintControlsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by GrazingAngle exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by GrazingAngle is emabled or not
           */
         virtual bool hasGrazingAngle() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by GrazingAngle
           *
           * @param type = uci::type::accessorType::pO_AngleConstraintControlsType This Accessor's accessor type
           */
         virtual void enableGrazingAngle(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_AngleConstraintControlsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by GrazingAngle */
         virtual void clearGrazingAngle()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LookAngle.
           *
           * @return The acecssor that provides access to the complex content that is identified by LookAngle.
           */
         virtual const uci::type::PO_AngleConstraintControlsType& getLookAngle() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LookAngle.
           *
           * @return The acecssor that provides access to the complex content that is identified by LookAngle.
           */
         virtual uci::type::PO_AngleConstraintControlsType& getLookAngle()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LookAngle to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LookAngle
           */
         virtual void setLookAngle(const uci::type::PO_AngleConstraintControlsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LookAngle exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LookAngle is emabled or not
           */
         virtual bool hasLookAngle() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LookAngle
           *
           * @param type = uci::type::accessorType::pO_AngleConstraintControlsType This Accessor's accessor type
           */
         virtual void enableLookAngle(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_AngleConstraintControlsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LookAngle */
         virtual void clearLookAngle()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ElevationAngle.
           *
           * @return The acecssor that provides access to the complex content that is identified by ElevationAngle.
           */
         virtual const uci::type::PO_AngleConstraintControlsType& getElevationAngle() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ElevationAngle.
           *
           * @return The acecssor that provides access to the complex content that is identified by ElevationAngle.
           */
         virtual uci::type::PO_AngleConstraintControlsType& getElevationAngle()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ElevationAngle to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ElevationAngle
           */
         virtual void setElevationAngle(const uci::type::PO_AngleConstraintControlsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ElevationAngle exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ElevationAngle is emabled or not
           */
         virtual bool hasElevationAngle() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ElevationAngle
           *
           * @param type = uci::type::accessorType::pO_AngleConstraintControlsType This Accessor's accessor type
           */
         virtual void enableElevationAngle(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_AngleConstraintControlsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ElevationAngle */
         virtual void clearElevationAngle()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SlantRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by SlantRange.
           */
         virtual const uci::type::PO_SlantRangeConstraintControlsType& getSlantRange() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SlantRange.
           *
           * @return The acecssor that provides access to the complex content that is identified by SlantRange.
           */
         virtual uci::type::PO_SlantRangeConstraintControlsType& getSlantRange()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SlantRange to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SlantRange
           */
         virtual void setSlantRange(const uci::type::PO_SlantRangeConstraintControlsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SlantRange exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SlantRange is emabled or not
           */
         virtual bool hasSlantRange() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SlantRange
           *
           * @param type = uci::type::accessorType::pO_SlantRangeConstraintControlsType This Accessor's accessor type
           */
         virtual void enableSlantRange(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_SlantRangeConstraintControlsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SlantRange */
         virtual void clearSlantRange()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionPattern.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionPattern.
           */
         virtual const uci::type::PO_CollectionPatternConstraintControlsType& getCollectionPattern() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionPattern.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionPattern.
           */
         virtual uci::type::PO_CollectionPatternConstraintControlsType& getCollectionPattern()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CollectionPattern to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CollectionPattern
           */
         virtual void setCollectionPattern(const uci::type::PO_CollectionPatternConstraintControlsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CollectionPattern exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CollectionPattern is emabled or not
           */
         virtual bool hasCollectionPattern() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CollectionPattern
           *
           * @param type = uci::type::accessorType::pO_CollectionPatternConstraintControlsType This Accessor's accessor type
           */
         virtual void enableCollectionPattern(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_CollectionPatternConstraintControlsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CollectionPattern */
         virtual void clearCollectionPattern()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SunElevAngles.
           *
           * @return The acecssor that provides access to the complex content that is identified by SunElevAngles.
           */
         virtual const uci::type::PO_AngleConstraintControlsType& getSunElevAngles() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SunElevAngles.
           *
           * @return The acecssor that provides access to the complex content that is identified by SunElevAngles.
           */
         virtual uci::type::PO_AngleConstraintControlsType& getSunElevAngles()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SunElevAngles to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SunElevAngles
           */
         virtual void setSunElevAngles(const uci::type::PO_AngleConstraintControlsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SunElevAngles exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SunElevAngles is emabled or not
           */
         virtual bool hasSunElevAngles() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SunElevAngles
           *
           * @param type = uci::type::accessorType::pO_AngleConstraintControlsType This Accessor's accessor type
           */
         virtual void enableSunElevAngles(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_AngleConstraintControlsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SunElevAngles */
         virtual void clearSunElevAngles()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SunAzimuthAngles.
           *
           * @return The acecssor that provides access to the complex content that is identified by SunAzimuthAngles.
           */
         virtual const uci::type::PO_AngleConstraintControlsType& getSunAzimuthAngles() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SunAzimuthAngles.
           *
           * @return The acecssor that provides access to the complex content that is identified by SunAzimuthAngles.
           */
         virtual uci::type::PO_AngleConstraintControlsType& getSunAzimuthAngles()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SunAzimuthAngles to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SunAzimuthAngles
           */
         virtual void setSunAzimuthAngles(const uci::type::PO_AngleConstraintControlsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SunAzimuthAngles exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SunAzimuthAngles is emabled or not
           */
         virtual bool hasSunAzimuthAngles() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SunAzimuthAngles
           *
           * @param type = uci::type::accessorType::pO_AngleConstraintControlsType This Accessor's accessor type
           */
         virtual void enableSunAzimuthAngles(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_AngleConstraintControlsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SunAzimuthAngles */
         virtual void clearSunAzimuthAngles()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SweepSpeed.
           *
           * @return The acecssor that provides access to the complex content that is identified by SweepSpeed.
           */
         virtual const uci::type::PO_SweepSpeedConstraintControlsType& getSweepSpeed() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SweepSpeed.
           *
           * @return The acecssor that provides access to the complex content that is identified by SweepSpeed.
           */
         virtual uci::type::PO_SweepSpeedConstraintControlsType& getSweepSpeed()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SweepSpeed to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SweepSpeed
           */
         virtual void setSweepSpeed(const uci::type::PO_SweepSpeedConstraintControlsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SweepSpeed exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SweepSpeed is emabled or not
           */
         virtual bool hasSweepSpeed() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SweepSpeed
           *
           * @param type = uci::type::accessorType::pO_SweepSpeedConstraintControlsType This Accessor's accessor type
           */
         virtual void enableSweepSpeed(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_SweepSpeedConstraintControlsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SweepSpeed */
         virtual void clearSweepSpeed()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_CollectionConstraintsSettingsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_CollectionConstraintsSettingsType to copy from
           */
         PO_CollectionConstraintsSettingsType(const PO_CollectionConstraintsSettingsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_CollectionConstraintsSettingsType to the contents of the
           * PO_CollectionConstraintsSettingsType on the right hand side (rhs) of the assignment
           * operator.PO_CollectionConstraintsSettingsType [only available to derived classes]
           *
           * @param rhs The PO_CollectionConstraintsSettingsType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::PO_CollectionConstraintsSettingsType
           * @return A constant reference to this PO_CollectionConstraintsSettingsType.
           */
         const PO_CollectionConstraintsSettingsType& operator=(const PO_CollectionConstraintsSettingsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_CollectionConstraintsSettingsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_CollectionConstraintsSettingsType_h

