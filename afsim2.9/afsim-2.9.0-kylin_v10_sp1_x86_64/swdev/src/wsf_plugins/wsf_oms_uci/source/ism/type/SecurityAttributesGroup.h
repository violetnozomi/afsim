// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Ism__Type__SecurityAttributesGroup_h
#define Ism__Type__SecurityAttributesGroup_h 1

#if !defined(Ism__Type__Classification_h)
# include "ism/type/Classification.h"
#endif

#if !defined(Ism__Type__OwnerProducer_h)
# include "ism/type/OwnerProducer.h"
#endif

#if !defined(Ism__Type__SCIcontrols_h)
# include "ism/type/SCIcontrols.h"
#endif

#if !defined(Ism__Type__SARIdentifier_h)
# include "ism/type/SARIdentifier.h"
#endif

#if !defined(Ism__Type__AtomicEnergyMarkings_h)
# include "ism/type/AtomicEnergyMarkings.h"
#endif

#if !defined(Ism__Type__DisseminationControls_h)
# include "ism/type/DisseminationControls.h"
#endif

#if !defined(Ism__Type__DisplayOnlyTo_h)
# include "ism/type/DisplayOnlyTo.h"
#endif

#if !defined(Ism__Type__FGIsourceOpen_h)
# include "ism/type/FGIsourceOpen.h"
#endif

#if !defined(Ism__Type__FGIsourceProtected_h)
# include "ism/type/FGIsourceProtected.h"
#endif

#if !defined(Ism__Type__ReleasableTo_h)
# include "ism/type/ReleasableTo.h"
#endif

#if !defined(Ism__Type__NonICmarkings_h)
# include "ism/type/NonICmarkings.h"
#endif

#if !defined(Ism__Type__ClassifiedBy_h)
# include "ism/type/ClassifiedBy.h"
#endif

#if !defined(Ism__Type__CompilationReason_h)
# include "ism/type/CompilationReason.h"
#endif

#if !defined(Ism__Type__DerivativelyClassifiedBy_h)
# include "ism/type/DerivativelyClassifiedBy.h"
#endif

#if !defined(Ism__Type__ClassificationReason_h)
# include "ism/type/ClassificationReason.h"
#endif

#if !defined(Ism__Type__NonUSControls_h)
# include "ism/type/NonUSControls.h"
#endif

#if !defined(Ism__Type__DerivedFrom_h)
# include "ism/type/DerivedFrom.h"
#endif

#if !defined(Ism__Type__DeclassDate_h)
# include "ism/type/DeclassDate.h"
#endif

#if !defined(Ism__Type__DeclassEvent_h)
# include "ism/type/DeclassEvent.h"
#endif

#if !defined(Ism__Type__DeclassException_h)
# include "ism/type/DeclassException.h"
#endif

//  
namespace ism {

   //  The namespace in which all generated data types are declared
   namespace type {

      /**  This group is to be contrasted with group 'SecurityAttributesOptionGroup' in which use of those attributes is
        * optional.
        */
      class SecurityAttributesGroup : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SecurityAttributesGroup()
         { }

         /** Returns this accessor's type constant, i.e. SecurityAttributesGroup
           *
           * @return This accessor's type constant, i.e. SecurityAttributesGroup
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return ism::type::accessorType::securityAttributesGroup;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SecurityAttributesGroup& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Classification.
           *
           * @return The acecssor that provides access to the complex content that is identified by Classification.
           */
         virtual const ism::type::Classification& getClassification() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Classification.
           *
           * @return The acecssor that provides access to the complex content that is identified by Classification.
           */
         virtual ism::type::Classification& getClassification()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Classification to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Classification
           */
         virtual void setClassification(const ism::type::Classification& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OwnerProducer.
           *
           * @return The acecssor that provides access to the complex content that is identified by OwnerProducer.
           */
         virtual const ism::type::OwnerProducer& getOwnerProducer() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the OwnerProducer.
           *
           * @return The acecssor that provides access to the complex content that is identified by OwnerProducer.
           */
         virtual ism::type::OwnerProducer& getOwnerProducer()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the OwnerProducer to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by OwnerProducer
           */
         virtual void setOwnerProducer(const ism::type::OwnerProducer& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SCIcontrols.
           *
           * @return The acecssor that provides access to the complex content that is identified by SCIcontrols.
           */
         virtual const ism::type::SCIcontrols& getSCIcontrols() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SCIcontrols.
           *
           * @return The acecssor that provides access to the complex content that is identified by SCIcontrols.
           */
         virtual ism::type::SCIcontrols& getSCIcontrols()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SCIcontrols to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SCIcontrols
           */
         virtual void setSCIcontrols(const ism::type::SCIcontrols& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SCIcontrols exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SCIcontrols is emabled or not
           */
         virtual bool hasSCIcontrols() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SCIcontrols
           *
           * @param type = ism::type::accessorType::sCIcontrols This Accessor's accessor type
           */
         virtual void enableSCIcontrols(uci::base::accessorType::AccessorType type = ism::type::accessorType::sCIcontrols)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SCIcontrols */
         virtual void clearSCIcontrols()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SARIdentifier.
           *
           * @return The acecssor that provides access to the complex content that is identified by SARIdentifier.
           */
         virtual const ism::type::SARIdentifier& getSARIdentifier() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SARIdentifier.
           *
           * @return The acecssor that provides access to the complex content that is identified by SARIdentifier.
           */
         virtual ism::type::SARIdentifier& getSARIdentifier()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SARIdentifier to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SARIdentifier
           */
         virtual void setSARIdentifier(const ism::type::SARIdentifier& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SARIdentifier exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SARIdentifier is emabled or not
           */
         virtual bool hasSARIdentifier() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SARIdentifier
           *
           * @param type = ism::type::accessorType::sARIdentifier This Accessor's accessor type
           */
         virtual void enableSARIdentifier(uci::base::accessorType::AccessorType type = ism::type::accessorType::sARIdentifier)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SARIdentifier */
         virtual void clearSARIdentifier()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AtomicEnergyMarkings.
           *
           * @return The acecssor that provides access to the complex content that is identified by AtomicEnergyMarkings.
           */
         virtual const ism::type::AtomicEnergyMarkings& getAtomicEnergyMarkings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AtomicEnergyMarkings.
           *
           * @return The acecssor that provides access to the complex content that is identified by AtomicEnergyMarkings.
           */
         virtual ism::type::AtomicEnergyMarkings& getAtomicEnergyMarkings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AtomicEnergyMarkings to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AtomicEnergyMarkings
           */
         virtual void setAtomicEnergyMarkings(const ism::type::AtomicEnergyMarkings& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AtomicEnergyMarkings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AtomicEnergyMarkings is emabled or not
           */
         virtual bool hasAtomicEnergyMarkings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AtomicEnergyMarkings
           *
           * @param type = ism::type::accessorType::atomicEnergyMarkings This Accessor's accessor type
           */
         virtual void enableAtomicEnergyMarkings(uci::base::accessorType::AccessorType type = ism::type::accessorType::atomicEnergyMarkings)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AtomicEnergyMarkings */
         virtual void clearAtomicEnergyMarkings()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DisseminationControls.
           *
           * @return The acecssor that provides access to the complex content that is identified by DisseminationControls.
           */
         virtual const ism::type::DisseminationControls& getDisseminationControls() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DisseminationControls.
           *
           * @return The acecssor that provides access to the complex content that is identified by DisseminationControls.
           */
         virtual ism::type::DisseminationControls& getDisseminationControls()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DisseminationControls to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DisseminationControls
           */
         virtual void setDisseminationControls(const ism::type::DisseminationControls& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DisseminationControls exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DisseminationControls is emabled or not
           */
         virtual bool hasDisseminationControls() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DisseminationControls
           *
           * @param type = ism::type::accessorType::disseminationControls This Accessor's accessor type
           */
         virtual void enableDisseminationControls(uci::base::accessorType::AccessorType type = ism::type::accessorType::disseminationControls)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DisseminationControls */
         virtual void clearDisseminationControls()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DisplayOnlyTo.
           *
           * @return The acecssor that provides access to the complex content that is identified by DisplayOnlyTo.
           */
         virtual const ism::type::DisplayOnlyTo& getDisplayOnlyTo() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DisplayOnlyTo.
           *
           * @return The acecssor that provides access to the complex content that is identified by DisplayOnlyTo.
           */
         virtual ism::type::DisplayOnlyTo& getDisplayOnlyTo()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DisplayOnlyTo to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DisplayOnlyTo
           */
         virtual void setDisplayOnlyTo(const ism::type::DisplayOnlyTo& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DisplayOnlyTo exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DisplayOnlyTo is emabled or not
           */
         virtual bool hasDisplayOnlyTo() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DisplayOnlyTo
           *
           * @param type = ism::type::accessorType::displayOnlyTo This Accessor's accessor type
           */
         virtual void enableDisplayOnlyTo(uci::base::accessorType::AccessorType type = ism::type::accessorType::displayOnlyTo)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DisplayOnlyTo */
         virtual void clearDisplayOnlyTo()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FGIsourceOpen.
           *
           * @return The acecssor that provides access to the complex content that is identified by FGIsourceOpen.
           */
         virtual const ism::type::FGIsourceOpen& getFGIsourceOpen() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FGIsourceOpen.
           *
           * @return The acecssor that provides access to the complex content that is identified by FGIsourceOpen.
           */
         virtual ism::type::FGIsourceOpen& getFGIsourceOpen()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FGIsourceOpen to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FGIsourceOpen
           */
         virtual void setFGIsourceOpen(const ism::type::FGIsourceOpen& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FGIsourceOpen exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FGIsourceOpen is emabled or not
           */
         virtual bool hasFGIsourceOpen() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FGIsourceOpen
           *
           * @param type = ism::type::accessorType::fGIsourceOpen This Accessor's accessor type
           */
         virtual void enableFGIsourceOpen(uci::base::accessorType::AccessorType type = ism::type::accessorType::fGIsourceOpen)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FGIsourceOpen */
         virtual void clearFGIsourceOpen()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FGIsourceProtected.
           *
           * @return The acecssor that provides access to the complex content that is identified by FGIsourceProtected.
           */
         virtual const ism::type::FGIsourceProtected& getFGIsourceProtected() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FGIsourceProtected.
           *
           * @return The acecssor that provides access to the complex content that is identified by FGIsourceProtected.
           */
         virtual ism::type::FGIsourceProtected& getFGIsourceProtected()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FGIsourceProtected to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FGIsourceProtected
           */
         virtual void setFGIsourceProtected(const ism::type::FGIsourceProtected& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FGIsourceProtected exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FGIsourceProtected is emabled or not
           */
         virtual bool hasFGIsourceProtected() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FGIsourceProtected
           *
           * @param type = ism::type::accessorType::fGIsourceProtected This Accessor's accessor type
           */
         virtual void enableFGIsourceProtected(uci::base::accessorType::AccessorType type = ism::type::accessorType::fGIsourceProtected)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FGIsourceProtected */
         virtual void clearFGIsourceProtected()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReleasableTo.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReleasableTo.
           */
         virtual const ism::type::ReleasableTo& getReleasableTo() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ReleasableTo.
           *
           * @return The acecssor that provides access to the complex content that is identified by ReleasableTo.
           */
         virtual ism::type::ReleasableTo& getReleasableTo()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ReleasableTo to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ReleasableTo
           */
         virtual void setReleasableTo(const ism::type::ReleasableTo& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ReleasableTo exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ReleasableTo is emabled or not
           */
         virtual bool hasReleasableTo() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ReleasableTo
           *
           * @param type = ism::type::accessorType::releasableTo This Accessor's accessor type
           */
         virtual void enableReleasableTo(uci::base::accessorType::AccessorType type = ism::type::accessorType::releasableTo)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ReleasableTo */
         virtual void clearReleasableTo()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NonICmarkings.
           *
           * @return The acecssor that provides access to the complex content that is identified by NonICmarkings.
           */
         virtual const ism::type::NonICmarkings& getNonICmarkings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NonICmarkings.
           *
           * @return The acecssor that provides access to the complex content that is identified by NonICmarkings.
           */
         virtual ism::type::NonICmarkings& getNonICmarkings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NonICmarkings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NonICmarkings
           */
         virtual void setNonICmarkings(const ism::type::NonICmarkings& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NonICmarkings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NonICmarkings is emabled or not
           */
         virtual bool hasNonICmarkings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NonICmarkings
           *
           * @param type = ism::type::accessorType::nonICmarkings This Accessor's accessor type
           */
         virtual void enableNonICmarkings(uci::base::accessorType::AccessorType type = ism::type::accessorType::nonICmarkings)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NonICmarkings */
         virtual void clearNonICmarkings()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ClassifiedBy.
           *
           * @return The acecssor that provides access to the complex content that is identified by ClassifiedBy.
           */
         virtual const ism::type::ClassifiedBy& getClassifiedBy() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ClassifiedBy.
           *
           * @return The acecssor that provides access to the complex content that is identified by ClassifiedBy.
           */
         virtual ism::type::ClassifiedBy& getClassifiedBy()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ClassifiedBy to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ClassifiedBy
           */
         virtual void setClassifiedBy(const ism::type::ClassifiedBy& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ClassifiedBy exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ClassifiedBy is emabled or not
           */
         virtual bool hasClassifiedBy() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ClassifiedBy
           *
           * @param type = ism::type::accessorType::classifiedBy This Accessor's accessor type
           */
         virtual void enableClassifiedBy(uci::base::accessorType::AccessorType type = ism::type::accessorType::classifiedBy)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ClassifiedBy */
         virtual void clearClassifiedBy()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CompilationReason.
           *
           * @return The acecssor that provides access to the complex content that is identified by CompilationReason.
           */
         virtual const ism::type::CompilationReason& getCompilationReason() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CompilationReason.
           *
           * @return The acecssor that provides access to the complex content that is identified by CompilationReason.
           */
         virtual ism::type::CompilationReason& getCompilationReason()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CompilationReason to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CompilationReason
           */
         virtual void setCompilationReason(const ism::type::CompilationReason& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CompilationReason exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CompilationReason is emabled or not
           */
         virtual bool hasCompilationReason() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CompilationReason
           *
           * @param type = ism::type::accessorType::compilationReason This Accessor's accessor type
           */
         virtual void enableCompilationReason(uci::base::accessorType::AccessorType type = ism::type::accessorType::compilationReason)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CompilationReason */
         virtual void clearCompilationReason()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DerivativelyClassifiedBy.
           *
           * @return The acecssor that provides access to the complex content that is identified by DerivativelyClassifiedBy.
           */
         virtual const ism::type::DerivativelyClassifiedBy& getDerivativelyClassifiedBy() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DerivativelyClassifiedBy.
           *
           * @return The acecssor that provides access to the complex content that is identified by DerivativelyClassifiedBy.
           */
         virtual ism::type::DerivativelyClassifiedBy& getDerivativelyClassifiedBy()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DerivativelyClassifiedBy to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DerivativelyClassifiedBy
           */
         virtual void setDerivativelyClassifiedBy(const ism::type::DerivativelyClassifiedBy& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DerivativelyClassifiedBy exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DerivativelyClassifiedBy is emabled or not
           */
         virtual bool hasDerivativelyClassifiedBy() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DerivativelyClassifiedBy
           *
           * @param type = ism::type::accessorType::derivativelyClassifiedBy This Accessor's accessor type
           */
         virtual void enableDerivativelyClassifiedBy(uci::base::accessorType::AccessorType type = ism::type::accessorType::derivativelyClassifiedBy)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DerivativelyClassifiedBy */
         virtual void clearDerivativelyClassifiedBy()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ClassificationReason.
           *
           * @return The acecssor that provides access to the complex content that is identified by ClassificationReason.
           */
         virtual const ism::type::ClassificationReason& getClassificationReason() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ClassificationReason.
           *
           * @return The acecssor that provides access to the complex content that is identified by ClassificationReason.
           */
         virtual ism::type::ClassificationReason& getClassificationReason()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ClassificationReason to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ClassificationReason
           */
         virtual void setClassificationReason(const ism::type::ClassificationReason& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ClassificationReason exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ClassificationReason is emabled or not
           */
         virtual bool hasClassificationReason() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ClassificationReason
           *
           * @param type = ism::type::accessorType::classificationReason This Accessor's accessor type
           */
         virtual void enableClassificationReason(uci::base::accessorType::AccessorType type = ism::type::accessorType::classificationReason)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ClassificationReason */
         virtual void clearClassificationReason()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NonUSControls.
           *
           * @return The acecssor that provides access to the complex content that is identified by NonUSControls.
           */
         virtual const ism::type::NonUSControls& getNonUSControls() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NonUSControls.
           *
           * @return The acecssor that provides access to the complex content that is identified by NonUSControls.
           */
         virtual ism::type::NonUSControls& getNonUSControls()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NonUSControls to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NonUSControls
           */
         virtual void setNonUSControls(const ism::type::NonUSControls& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NonUSControls exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NonUSControls is emabled or not
           */
         virtual bool hasNonUSControls() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NonUSControls
           *
           * @param type = ism::type::accessorType::nonUSControls This Accessor's accessor type
           */
         virtual void enableNonUSControls(uci::base::accessorType::AccessorType type = ism::type::accessorType::nonUSControls)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NonUSControls */
         virtual void clearNonUSControls()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DerivedFrom.
           *
           * @return The acecssor that provides access to the complex content that is identified by DerivedFrom.
           */
         virtual const ism::type::DerivedFrom& getDerivedFrom() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DerivedFrom.
           *
           * @return The acecssor that provides access to the complex content that is identified by DerivedFrom.
           */
         virtual ism::type::DerivedFrom& getDerivedFrom()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DerivedFrom to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DerivedFrom
           */
         virtual void setDerivedFrom(const ism::type::DerivedFrom& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DerivedFrom exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DerivedFrom is emabled or not
           */
         virtual bool hasDerivedFrom() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DerivedFrom
           *
           * @param type = ism::type::accessorType::derivedFrom This Accessor's accessor type
           */
         virtual void enableDerivedFrom(uci::base::accessorType::AccessorType type = ism::type::accessorType::derivedFrom)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DerivedFrom */
         virtual void clearDerivedFrom()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DeclassDate.
           *
           * @return The value of the simple primitive data type identified by DeclassDate.
           */
         virtual ism::type::DeclassDateValue getDeclassDate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DeclassDate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDeclassDate(ism::type::DeclassDateValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DeclassDate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DeclassDate is emabled or not
           */
         virtual bool hasDeclassDate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DeclassDate
           *
           * @param type = ism::type::accessorType::declassDate This Accessor's accessor type
           */
         virtual void enableDeclassDate(uci::base::accessorType::AccessorType type = ism::type::accessorType::declassDate)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DeclassDate */
         virtual void clearDeclassDate()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DeclassEvent.
           *
           * @return The acecssor that provides access to the complex content that is identified by DeclassEvent.
           */
         virtual const ism::type::DeclassEvent& getDeclassEvent() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DeclassEvent.
           *
           * @return The acecssor that provides access to the complex content that is identified by DeclassEvent.
           */
         virtual ism::type::DeclassEvent& getDeclassEvent()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DeclassEvent to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DeclassEvent
           */
         virtual void setDeclassEvent(const ism::type::DeclassEvent& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DeclassEvent exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DeclassEvent is emabled or not
           */
         virtual bool hasDeclassEvent() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DeclassEvent
           *
           * @param type = ism::type::accessorType::declassEvent This Accessor's accessor type
           */
         virtual void enableDeclassEvent(uci::base::accessorType::AccessorType type = ism::type::accessorType::declassEvent)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DeclassEvent */
         virtual void clearDeclassEvent()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DeclassException.
           *
           * @return The acecssor that provides access to the complex content that is identified by DeclassException.
           */
         virtual const ism::type::DeclassException& getDeclassException() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DeclassException.
           *
           * @return The acecssor that provides access to the complex content that is identified by DeclassException.
           */
         virtual ism::type::DeclassException& getDeclassException()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DeclassException to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DeclassException
           */
         virtual void setDeclassException(const ism::type::DeclassException& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DeclassException exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DeclassException is emabled or not
           */
         virtual bool hasDeclassException() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DeclassException
           *
           * @param type = ism::type::accessorType::declassException This Accessor's accessor type
           */
         virtual void enableDeclassException(uci::base::accessorType::AccessorType type = ism::type::accessorType::declassException)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DeclassException */
         virtual void clearDeclassException()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SecurityAttributesGroup()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SecurityAttributesGroup to copy from
           */
         SecurityAttributesGroup(const SecurityAttributesGroup& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SecurityAttributesGroup to the contents of the
           * SecurityAttributesGroup on the right hand side (rhs) of the assignment operator.SecurityAttributesGroup [only
           * available to derived classes]
           *
           * @param rhs The SecurityAttributesGroup on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this ism::type::SecurityAttributesGroup
           * @return A constant reference to this SecurityAttributesGroup.
           */
         const SecurityAttributesGroup& operator=(const SecurityAttributesGroup& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SecurityAttributesGroup


   } // Namespace: type
} // Namespace: ism

#endif // Ism__Type__SecurityAttributesGroup_h

