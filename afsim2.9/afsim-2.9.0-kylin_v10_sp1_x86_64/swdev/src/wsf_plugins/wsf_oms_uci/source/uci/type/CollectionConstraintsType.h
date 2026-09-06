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
#ifndef Uci__Type__CollectionConstraintsType_h
#define Uci__Type__CollectionConstraintsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AngleHalfPairType_h)
# include "uci/type/AngleHalfPairType.h"
#endif

#if !defined(Uci__Type__AngleQuarterPairType_h)
# include "uci/type/AngleQuarterPairType.h"
#endif

#if !defined(Uci__Type__AnglePairType_h)
# include "uci/type/AnglePairType.h"
#endif

#if !defined(Uci__Type__SlantRangeConstraintsType_h)
# include "uci/type/SlantRangeConstraintsType.h"
#endif

#if !defined(Uci__Type__CollectionPatternEnum_h)
# include "uci/type/CollectionPatternEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Modeled after SensorSpecificData in the ACTDF standard. */
      class CollectionConstraintsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CollectionConstraintsType()
         { }

         /** Returns this accessor's type constant, i.e. CollectionConstraintsType
           *
           * @return This accessor's type constant, i.e. CollectionConstraintsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::collectionConstraintsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CollectionConstraintsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MapAngleConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by MapAngleConstraints.
           */
         virtual const uci::type::AngleHalfPairType& getMapAngleConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MapAngleConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by MapAngleConstraints.
           */
         virtual uci::type::AngleHalfPairType& getMapAngleConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MapAngleConstraints to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MapAngleConstraints
           */
         virtual void setMapAngleConstraints(const uci::type::AngleHalfPairType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MapAngleConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MapAngleConstraints is emabled or not
           */
         virtual bool hasMapAngleConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MapAngleConstraints
           *
           * @param type = uci::type::accessorType::angleHalfPairType This Accessor's accessor type
           */
         virtual void enableMapAngleConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleHalfPairType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MapAngleConstraints */
         virtual void clearMapAngleConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GrazingAngleConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by GrazingAngleConstraints.
           */
         virtual const uci::type::AngleQuarterPairType& getGrazingAngleConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GrazingAngleConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by GrazingAngleConstraints.
           */
         virtual uci::type::AngleQuarterPairType& getGrazingAngleConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the GrazingAngleConstraints to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by GrazingAngleConstraints
           */
         virtual void setGrazingAngleConstraints(const uci::type::AngleQuarterPairType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by GrazingAngleConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by GrazingAngleConstraints is emabled or not
           */
         virtual bool hasGrazingAngleConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by GrazingAngleConstraints
           *
           * @param type = uci::type::accessorType::angleQuarterPairType This Accessor's accessor type
           */
         virtual void enableGrazingAngleConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleQuarterPairType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by GrazingAngleConstraints */
         virtual void clearGrazingAngleConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LookAngleConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by LookAngleConstraints.
           */
         virtual const uci::type::AnglePairType& getLookAngleConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LookAngleConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by LookAngleConstraints.
           */
         virtual uci::type::AnglePairType& getLookAngleConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LookAngleConstraints to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LookAngleConstraints
           */
         virtual void setLookAngleConstraints(const uci::type::AnglePairType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by LookAngleConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by LookAngleConstraints is emabled or not
           */
         virtual bool hasLookAngleConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by LookAngleConstraints
           *
           * @param type = uci::type::accessorType::anglePairType This Accessor's accessor type
           */
         virtual void enableLookAngleConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::anglePairType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by LookAngleConstraints */
         virtual void clearLookAngleConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ElevationAngleConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by ElevationAngleConstraints.
           */
         virtual const uci::type::AngleHalfPairType& getElevationAngleConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ElevationAngleConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by ElevationAngleConstraints.
           */
         virtual uci::type::AngleHalfPairType& getElevationAngleConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ElevationAngleConstraints to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ElevationAngleConstraints
           */
         virtual void setElevationAngleConstraints(const uci::type::AngleHalfPairType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ElevationAngleConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ElevationAngleConstraints is emabled or not
           */
         virtual bool hasElevationAngleConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ElevationAngleConstraints
           *
           * @param type = uci::type::accessorType::angleHalfPairType This Accessor's accessor type
           */
         virtual void enableElevationAngleConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::angleHalfPairType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ElevationAngleConstraints */
         virtual void clearElevationAngleConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SlantRangeConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by SlantRangeConstraints.
           */
         virtual const uci::type::SlantRangeConstraintsType& getSlantRangeConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SlantRangeConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by SlantRangeConstraints.
           */
         virtual uci::type::SlantRangeConstraintsType& getSlantRangeConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SlantRangeConstraints to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SlantRangeConstraints
           */
         virtual void setSlantRangeConstraints(const uci::type::SlantRangeConstraintsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SlantRangeConstraints exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SlantRangeConstraints is emabled or not
           */
         virtual bool hasSlantRangeConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SlantRangeConstraints
           *
           * @param type = uci::type::accessorType::slantRangeConstraintsType This Accessor's accessor type
           */
         virtual void enableSlantRangeConstraints(uci::base::accessorType::AccessorType type = uci::type::accessorType::slantRangeConstraintsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SlantRangeConstraints */
         virtual void clearSlantRangeConstraints()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CollectionPattern.
           *
           * @return The value of the enumeration identified by CollectionPattern.
           */
         virtual const uci::type::CollectionPatternEnum& getCollectionPattern() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CollectionPattern.
           *
           * @return The value of the enumeration identified by CollectionPattern.
           */
         virtual uci::type::CollectionPatternEnum& getCollectionPattern()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CollectionPattern.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCollectionPattern(const uci::type::CollectionPatternEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CollectionPattern.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCollectionPattern(uci::type::CollectionPatternEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield CollectionPatternis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasCollectionPattern() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getCollectionPattern will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::collectionPatternEnum This Accessor's accessor type
           */
         virtual void enableCollectionPattern(uci::base::accessorType::AccessorType type = uci::type::accessorType::collectionPatternEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearCollectionPattern()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CollectionConstraintsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CollectionConstraintsType to copy from
           */
         CollectionConstraintsType(const CollectionConstraintsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CollectionConstraintsType to the contents of the
           * CollectionConstraintsType on the right hand side (rhs) of the assignment operator.CollectionConstraintsType [only
           * available to derived classes]
           *
           * @param rhs The CollectionConstraintsType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::CollectionConstraintsType
           * @return A constant reference to this CollectionConstraintsType.
           */
         const CollectionConstraintsType& operator=(const CollectionConstraintsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CollectionConstraintsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CollectionConstraintsType_h

