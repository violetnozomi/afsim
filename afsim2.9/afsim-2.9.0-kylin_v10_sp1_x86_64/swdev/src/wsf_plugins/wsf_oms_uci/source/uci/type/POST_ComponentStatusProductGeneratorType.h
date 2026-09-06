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
#ifndef Uci__Type__POST_ComponentStatusProductGeneratorType_h
#define Uci__Type__POST_ComponentStatusProductGeneratorType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PO_ProductGeneratorID_Type_h)
# include "uci/type/PO_ProductGeneratorID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__PO_FPA_ID_Type_h)
# include "uci/type/PO_FPA_ID_Type.h"
#endif

#if !defined(Uci__Type__ComponentControlsStatusType_h)
# include "uci/type/ComponentControlsStatusType.h"
#endif

#if !defined(Uci__Type__POST_ComponentSettingsProductGeneratorSettingsType_h)
# include "uci/type/POST_ComponentSettingsProductGeneratorSettingsType.h"
#endif

#if !defined(Uci__Type__POST_ComponentStatusOutputProductSettingsType_h)
# include "uci/type/POST_ComponentStatusOutputProductSettingsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_ComponentStatusProductGeneratorType sequence accessor class */
      class POST_ComponentStatusProductGeneratorType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_ComponentStatusProductGeneratorType()
         { }

         /** Returns this accessor's type constant, i.e. POST_ComponentStatusProductGeneratorType
           *
           * @return This accessor's type constant, i.e. POST_ComponentStatusProductGeneratorType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_ComponentStatusProductGeneratorType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_ComponentStatusProductGeneratorType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Processing stages process the optical data produced by one or more Focal Plane Arrays (FPAs). This field specifies
           * those FPAs whose generated data is processed by this stage. This information is useful in identifying which
           * processing stage to configure if required. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PO_FPA_ID_Type, uci::type::accessorType::pO_FPA_ID_Type> FPA_ID;

         /** Indicates the settings that apply to this output product. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::POST_ComponentStatusOutputProductSettingsType, uci::type::accessorType::pOST_ComponentStatusOutputProductSettingsType> OutputProductSettings;

         /** Returns the accessor that provides access to the complex content that is identified by the GeneratorID.
           *
           * @return The acecssor that provides access to the complex content that is identified by GeneratorID.
           */
         virtual const uci::type::PO_ProductGeneratorID_Type& getGeneratorID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the GeneratorID.
           *
           * @return The acecssor that provides access to the complex content that is identified by GeneratorID.
           */
         virtual uci::type::PO_ProductGeneratorID_Type& getGeneratorID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the GeneratorID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by GeneratorID
           */
         virtual void setGeneratorID(const uci::type::PO_ProductGeneratorID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FPA_ID.
           *
           * @return The bounded list identified by FPA_ID.
           */
         virtual const uci::type::POST_ComponentStatusProductGeneratorType::FPA_ID& getFPA_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FPA_ID.
           *
           * @return The bounded list identified by FPA_ID.
           */
         virtual uci::type::POST_ComponentStatusProductGeneratorType::FPA_ID& getFPA_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FPA_ID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFPA_ID(const uci::type::POST_ComponentStatusProductGeneratorType::FPA_ID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductGeneratorStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductGeneratorStatus.
           */
         virtual const uci::type::ComponentControlsStatusType& getProductGeneratorStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductGeneratorStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductGeneratorStatus.
           */
         virtual uci::type::ComponentControlsStatusType& getProductGeneratorStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductGeneratorStatus to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductGeneratorStatus
           */
         virtual void setProductGeneratorStatus(const uci::type::ComponentControlsStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductGeneratorSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductGeneratorSettings.
           */
         virtual const uci::type::POST_ComponentSettingsProductGeneratorSettingsType& getProductGeneratorSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProductGeneratorSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProductGeneratorSettings.
           */
         virtual uci::type::POST_ComponentSettingsProductGeneratorSettingsType& getProductGeneratorSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProductGeneratorSettings to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProductGeneratorSettings
           */
         virtual void setProductGeneratorSettings(const uci::type::POST_ComponentSettingsProductGeneratorSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProductGeneratorSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProductGeneratorSettings is emabled or not
           */
         virtual bool hasProductGeneratorSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProductGeneratorSettings
           *
           * @param type = uci::type::accessorType::pOST_ComponentSettingsProductGeneratorSettingsType This Accessor's accessor
           *      type
           */
         virtual void enableProductGeneratorSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::pOST_ComponentSettingsProductGeneratorSettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProductGeneratorSettings */
         virtual void clearProductGeneratorSettings()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OutputProductSettings.
           *
           * @return The bounded list identified by OutputProductSettings.
           */
         virtual const uci::type::POST_ComponentStatusProductGeneratorType::OutputProductSettings& getOutputProductSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the OutputProductSettings.
           *
           * @return The bounded list identified by OutputProductSettings.
           */
         virtual uci::type::POST_ComponentStatusProductGeneratorType::OutputProductSettings& getOutputProductSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the OutputProductSettings.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setOutputProductSettings(const uci::type::POST_ComponentStatusProductGeneratorType::OutputProductSettings& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_ComponentStatusProductGeneratorType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_ComponentStatusProductGeneratorType to copy from
           */
         POST_ComponentStatusProductGeneratorType(const POST_ComponentStatusProductGeneratorType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_ComponentStatusProductGeneratorType to the contents of the
           * POST_ComponentStatusProductGeneratorType on the right hand side (rhs) of the assignment
           * operator.POST_ComponentStatusProductGeneratorType [only available to derived classes]
           *
           * @param rhs The POST_ComponentStatusProductGeneratorType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::POST_ComponentStatusProductGeneratorType
           * @return A constant reference to this POST_ComponentStatusProductGeneratorType.
           */
         const POST_ComponentStatusProductGeneratorType& operator=(const POST_ComponentStatusProductGeneratorType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_ComponentStatusProductGeneratorType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_ComponentStatusProductGeneratorType_h

