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
#ifndef Uci__Type__PO_ComponentType_h
#define Uci__Type__PO_ComponentType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ComponentID_Type_h)
# include "uci/type/ComponentID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__PO_ComponentFocalPlaneArrayType_h)
# include "uci/type/PO_ComponentFocalPlaneArrayType.h"
#endif

#if !defined(Uci__Type__PO_ComponentLensAssemblyType_h)
# include "uci/type/PO_ComponentLensAssemblyType.h"
#endif

#if !defined(Uci__Type__PO_ComponentFilterType_h)
# include "uci/type/PO_ComponentFilterType.h"
#endif

#if !defined(Uci__Type__PO_ComponentProcessingStageType_h)
# include "uci/type/PO_ComponentProcessingStageType.h"
#endif

#if !defined(Uci__Type__PO_ComponentProductGeneratorType_h)
# include "uci/type/PO_ComponentProductGeneratorType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_ComponentType sequence accessor class */
      class PO_ComponentType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PO_ComponentType()
         { }

         /** Returns this accessor's type constant, i.e. PO_ComponentType
           *
           * @return This accessor's type constant, i.e. PO_ComponentType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_ComponentType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_ComponentType& accessor)
            throw(uci::base::UCIException) = 0;


         /** One type of Passive Optical Subsystem component is the Focal Plane Array (FPA). An FPA is an array of collection
           * elements that are used to collect and record the number of photons emitted by some surface, e.g. the surface of the
           * earth. FPAs are commonly configured as either linear or grid arrays. A Passive Optical sensor can have any number of
           * FPAs. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PO_ComponentFocalPlaneArrayType, uci::type::accessorType::pO_ComponentFocalPlaneArrayType> FocalPlaneArray;

         /** One type of Passive Optical Subsystem Component is the optical lens. A lens is placed within the Passive Optical
           * sensor's optical pathway and affects (refract to a point or diverge outwards) the path of the photons passing through
           * the lens on their way to the to the sensor's various Focal Plane Arrays (FPAs). [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PO_ComponentLensAssemblyType, uci::type::accessorType::pO_ComponentLensAssemblyType> Lens;

         /** One type of Passive Optical Subsystem Component is the optical filter. A filter is placed within the Passive Optical
           * sensor's optical pathway and stops photons of a certain frequency (possibly of all frequencies) from passing through
           * the filter on their way to the sensor's various Focal Plane Arrays (FPAs). [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PO_ComponentFilterType, uci::type::accessorType::pO_ComponentFilterType> Filter;

         /** Passive Optical sensors generally support a processing engine that takes the raw data collected from the Focal Plane
           * Arrays (FPAs) and processes that data in various way in order to produce an image product(s). Such engines can be
           * viewed as pipelines in which each stage in the pipeline takes the imagery data produced by one or more stages earlier
           * in the pipeline, processes that data in some fashion, and then sends the updated imagery data to the one of more
           * stages further down the pipeline. This field specifies the stages that are available in this sensor's processing
           * pipeline and whether the stage can be manually configured. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PO_ComponentProcessingStageType, uci::type::accessorType::pO_ComponentProcessingStageType> ProcessingStage;

         /** Passive Optical sensors have the capability of producing one or more products using the raw optical data collected by
           * their focal plane arrays. This field lists the products that can be generated by this sensor. [Minimum occurrences:
           * 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PO_ComponentProductGeneratorType, uci::type::accessorType::pO_ComponentProductGeneratorType> ProductGenerator;

         /** Returns the accessor that provides access to the complex content that is identified by the ComponentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentID.
           */
         virtual const uci::type::ComponentID_Type& getComponentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ComponentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentID.
           */
         virtual uci::type::ComponentID_Type& getComponentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ComponentID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ComponentID
           */
         virtual void setComponentID(const uci::type::ComponentID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FocalPlaneArray.
           *
           * @return The bounded list identified by FocalPlaneArray.
           */
         virtual const uci::type::PO_ComponentType::FocalPlaneArray& getFocalPlaneArray() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FocalPlaneArray.
           *
           * @return The bounded list identified by FocalPlaneArray.
           */
         virtual uci::type::PO_ComponentType::FocalPlaneArray& getFocalPlaneArray()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FocalPlaneArray.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFocalPlaneArray(const uci::type::PO_ComponentType::FocalPlaneArray& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Lens.
           *
           * @return The bounded list identified by Lens.
           */
         virtual const uci::type::PO_ComponentType::Lens& getLens() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Lens.
           *
           * @return The bounded list identified by Lens.
           */
         virtual uci::type::PO_ComponentType::Lens& getLens()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Lens.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setLens(const uci::type::PO_ComponentType::Lens& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Filter.
           *
           * @return The bounded list identified by Filter.
           */
         virtual const uci::type::PO_ComponentType::Filter& getFilter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Filter.
           *
           * @return The bounded list identified by Filter.
           */
         virtual uci::type::PO_ComponentType::Filter& getFilter()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Filter.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFilter(const uci::type::PO_ComponentType::Filter& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProcessingStage.
           *
           * @return The bounded list identified by ProcessingStage.
           */
         virtual const uci::type::PO_ComponentType::ProcessingStage& getProcessingStage() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProcessingStage.
           *
           * @return The bounded list identified by ProcessingStage.
           */
         virtual uci::type::PO_ComponentType::ProcessingStage& getProcessingStage()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProcessingStage.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProcessingStage(const uci::type::PO_ComponentType::ProcessingStage& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductGenerator.
           *
           * @return The bounded list identified by ProductGenerator.
           */
         virtual const uci::type::PO_ComponentType::ProductGenerator& getProductGenerator() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductGenerator.
           *
           * @return The bounded list identified by ProductGenerator.
           */
         virtual uci::type::PO_ComponentType::ProductGenerator& getProductGenerator()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductGenerator.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductGenerator(const uci::type::PO_ComponentType::ProductGenerator& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_ComponentType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_ComponentType to copy from
           */
         PO_ComponentType(const PO_ComponentType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_ComponentType to the contents of the PO_ComponentType on the
           * right hand side (rhs) of the assignment operator.PO_ComponentType [only available to derived classes]
           *
           * @param rhs The PO_ComponentType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::PO_ComponentType
           * @return A constant reference to this PO_ComponentType.
           */
         const PO_ComponentType& operator=(const PO_ComponentType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_ComponentType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_ComponentType_h

