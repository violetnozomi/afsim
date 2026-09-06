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
#ifndef Uci__Type__PO_CollectionConstraintsStatusType_h
#define Uci__Type__PO_CollectionConstraintsStatusType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PO_AngleConstraintControlsStatusType_h)
# include "uci/type/PO_AngleConstraintControlsStatusType.h"
#endif

#if !defined(Uci__Type__PO_SlantRangeConstraintControlsStatusType_h)
# include "uci/type/PO_SlantRangeConstraintControlsStatusType.h"
#endif

#if !defined(Uci__Type__PO_ColorPatternConstraintControlsStatusType_h)
# include "uci/type/PO_ColorPatternConstraintControlsStatusType.h"
#endif

#if !defined(Uci__Type__PO_SweepSpeedConstraintControlsStatusType_h)
# include "uci/type/PO_SweepSpeedConstraintControlsStatusType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This type is used to return the current settings of the set of constraints that constrain when a PO sensor can start
        * collecting the requested image data.
        */
      class PO_CollectionConstraintsStatusType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PO_CollectionConstraintsStatusType()
         { }

         /** Returns this accessor's type constant, i.e. PO_CollectionConstraintsStatusType
           *
           * @return This accessor's type constant, i.e. PO_CollectionConstraintsStatusType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_CollectionConstraintsStatusType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_CollectionConstraintsStatusType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MapAngleConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by MapAngleConstraints.
           */
         virtual const uci::type::PO_AngleConstraintControlsStatusType& getMapAngleConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MapAngleConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by MapAngleConstraints.
           */
         virtual uci::type::PO_AngleConstraintControlsStatusType& getMapAngleConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MapAngleConstraints to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MapAngleConstraints
           */
         virtual void setMapAngleConstraints(const uci::type::PO_AngleConstraintControlsStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MapAngleConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MapAngleConstraints is emabled or not
           */
         virtual bool hasMapAngleConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MapAngleConstraints
           *
           * @param type = uci::type::accessorType::pO_AngleConstraintControlsStatusType This Accessor's accessor type
           */
         virtual void enableMapAngleConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_AngleConstraintControlsStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MapAngleConstraints */
         virtual void clearMapAngleConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GrazingAngleConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by GrazingAngleConstraints.
           */
         virtual const uci::type::PO_AngleConstraintControlsStatusType& getGrazingAngleConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GrazingAngleConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by GrazingAngleConstraints.
           */
         virtual uci::type::PO_AngleConstraintControlsStatusType& getGrazingAngleConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the GrazingAngleConstraints to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by GrazingAngleConstraints
           */
         virtual void setGrazingAngleConstraints(const uci::type::PO_AngleConstraintControlsStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by GrazingAngleConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by GrazingAngleConstraints is emabled or not
           */
         virtual bool hasGrazingAngleConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by GrazingAngleConstraints
           *
           * @param type = uci::type::accessorType::pO_AngleConstraintControlsStatusType This Accessor's accessor type
           */
         virtual void enableGrazingAngleConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_AngleConstraintControlsStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by GrazingAngleConstraints */
         virtual void clearGrazingAngleConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LookAngleConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by LookAngleConstraints.
           */
         virtual const uci::type::PO_AngleConstraintControlsStatusType& getLookAngleConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LookAngleConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by LookAngleConstraints.
           */
         virtual uci::type::PO_AngleConstraintControlsStatusType& getLookAngleConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LookAngleConstraints to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LookAngleConstraints
           */
         virtual void setLookAngleConstraints(const uci::type::PO_AngleConstraintControlsStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LookAngleConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LookAngleConstraints is emabled or not
           */
         virtual bool hasLookAngleConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LookAngleConstraints
           *
           * @param type = uci::type::accessorType::pO_AngleConstraintControlsStatusType This Accessor's accessor type
           */
         virtual void enableLookAngleConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_AngleConstraintControlsStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LookAngleConstraints */
         virtual void clearLookAngleConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ElevationAngleConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by ElevationAngleConstraints.
           */
         virtual const uci::type::PO_AngleConstraintControlsStatusType& getElevationAngleConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ElevationAngleConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by ElevationAngleConstraints.
           */
         virtual uci::type::PO_AngleConstraintControlsStatusType& getElevationAngleConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ElevationAngleConstraints to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ElevationAngleConstraints
           */
         virtual void setElevationAngleConstraints(const uci::type::PO_AngleConstraintControlsStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ElevationAngleConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ElevationAngleConstraints is emabled or not
           */
         virtual bool hasElevationAngleConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ElevationAngleConstraints
           *
           * @param type = uci::type::accessorType::pO_AngleConstraintControlsStatusType This Accessor's accessor type
           */
         virtual void enableElevationAngleConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_AngleConstraintControlsStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ElevationAngleConstraints */
         virtual void clearElevationAngleConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SlantRangeConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by SlantRangeConstraints.
           */
         virtual const uci::type::PO_SlantRangeConstraintControlsStatusType& getSlantRangeConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SlantRangeConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by SlantRangeConstraints.
           */
         virtual uci::type::PO_SlantRangeConstraintControlsStatusType& getSlantRangeConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SlantRangeConstraints to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SlantRangeConstraints
           */
         virtual void setSlantRangeConstraints(const uci::type::PO_SlantRangeConstraintControlsStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SlantRangeConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SlantRangeConstraints is emabled or not
           */
         virtual bool hasSlantRangeConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SlantRangeConstraints
           *
           * @param type = uci::type::accessorType::pO_SlantRangeConstraintControlsStatusType This Accessor's accessor type
           */
         virtual void enableSlantRangeConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_SlantRangeConstraintControlsStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SlantRangeConstraints */
         virtual void clearSlantRangeConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionPattern.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionPattern.
           */
         virtual const uci::type::PO_ColorPatternConstraintControlsStatusType& getCollectionPattern() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CollectionPattern.
           *
           * @return The acecssor that provides access to the complex content that is identified by CollectionPattern.
           */
         virtual uci::type::PO_ColorPatternConstraintControlsStatusType& getCollectionPattern()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CollectionPattern to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CollectionPattern
           */
         virtual void setCollectionPattern(const uci::type::PO_ColorPatternConstraintControlsStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CollectionPattern exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CollectionPattern is emabled or not
           */
         virtual bool hasCollectionPattern() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CollectionPattern
           *
           * @param type = uci::type::accessorType::pO_ColorPatternConstraintControlsStatusType This Accessor's accessor type
           */
         virtual void enableCollectionPattern(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_ColorPatternConstraintControlsStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CollectionPattern */
         virtual void clearCollectionPattern()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SunElevAngles.
           *
           * @return The acecssor that provides access to the complex content that is identified by SunElevAngles.
           */
         virtual const uci::type::PO_AngleConstraintControlsStatusType& getSunElevAngles() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SunElevAngles.
           *
           * @return The acecssor that provides access to the complex content that is identified by SunElevAngles.
           */
         virtual uci::type::PO_AngleConstraintControlsStatusType& getSunElevAngles()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SunElevAngles to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SunElevAngles
           */
         virtual void setSunElevAngles(const uci::type::PO_AngleConstraintControlsStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SunElevAngles exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SunElevAngles is emabled or not
           */
         virtual bool hasSunElevAngles() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SunElevAngles
           *
           * @param type = uci::type::accessorType::pO_AngleConstraintControlsStatusType This Accessor's accessor type
           */
         virtual void enableSunElevAngles(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_AngleConstraintControlsStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SunElevAngles */
         virtual void clearSunElevAngles()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SunAzimuthAngles.
           *
           * @return The acecssor that provides access to the complex content that is identified by SunAzimuthAngles.
           */
         virtual const uci::type::PO_AngleConstraintControlsStatusType& getSunAzimuthAngles() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SunAzimuthAngles.
           *
           * @return The acecssor that provides access to the complex content that is identified by SunAzimuthAngles.
           */
         virtual uci::type::PO_AngleConstraintControlsStatusType& getSunAzimuthAngles()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SunAzimuthAngles to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SunAzimuthAngles
           */
         virtual void setSunAzimuthAngles(const uci::type::PO_AngleConstraintControlsStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SunAzimuthAngles exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SunAzimuthAngles is emabled or not
           */
         virtual bool hasSunAzimuthAngles() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SunAzimuthAngles
           *
           * @param type = uci::type::accessorType::pO_AngleConstraintControlsStatusType This Accessor's accessor type
           */
         virtual void enableSunAzimuthAngles(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_AngleConstraintControlsStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SunAzimuthAngles */
         virtual void clearSunAzimuthAngles()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SweepSpeed.
           *
           * @return The acecssor that provides access to the complex content that is identified by SweepSpeed.
           */
         virtual const uci::type::PO_SweepSpeedConstraintControlsStatusType& getSweepSpeed() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SweepSpeed.
           *
           * @return The acecssor that provides access to the complex content that is identified by SweepSpeed.
           */
         virtual uci::type::PO_SweepSpeedConstraintControlsStatusType& getSweepSpeed()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SweepSpeed to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SweepSpeed
           */
         virtual void setSweepSpeed(const uci::type::PO_SweepSpeedConstraintControlsStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SweepSpeed exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SweepSpeed is emabled or not
           */
         virtual bool hasSweepSpeed() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SweepSpeed
           *
           * @param type = uci::type::accessorType::pO_SweepSpeedConstraintControlsStatusType This Accessor's accessor type
           */
         virtual void enableSweepSpeed(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_SweepSpeedConstraintControlsStatusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SweepSpeed */
         virtual void clearSweepSpeed()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_CollectionConstraintsStatusType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_CollectionConstraintsStatusType to copy from
           */
         PO_CollectionConstraintsStatusType(const PO_CollectionConstraintsStatusType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_CollectionConstraintsStatusType to the contents of the
           * PO_CollectionConstraintsStatusType on the right hand side (rhs) of the assignment
           * operator.PO_CollectionConstraintsStatusType [only available to derived classes]
           *
           * @param rhs The PO_CollectionConstraintsStatusType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::PO_CollectionConstraintsStatusType
           * @return A constant reference to this PO_CollectionConstraintsStatusType.
           */
         const PO_CollectionConstraintsStatusType& operator=(const PO_CollectionConstraintsStatusType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_CollectionConstraintsStatusType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_CollectionConstraintsStatusType_h

