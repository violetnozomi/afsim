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
#ifndef Uci__Type__POST_ComponentSettingsFilterType_h
#define Uci__Type__POST_ComponentSettingsFilterType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PO_FilterID_Type_h)
# include "uci/type/PO_FilterID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__PO_FPA_ID_Type_h)
# include "uci/type/PO_FPA_ID_Type.h"
#endif

#if !defined(Uci__Type__ComponentControlsA_Type_h)
# include "uci/type/ComponentControlsA_Type.h"
#endif

#if !defined(Uci__Type__PO_ComponentSettingsBandpassFrequencyType_h)
# include "uci/type/PO_ComponentSettingsBandpassFrequencyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_ComponentSettingsFilterType sequence accessor class */
      class POST_ComponentSettingsFilterType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_ComponentSettingsFilterType()
         { }

         /** Returns this accessor's type constant, i.e. POST_ComponentSettingsFilterType
           *
           * @return This accessor's type constant, i.e. POST_ComponentSettingsFilterType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_ComponentSettingsFilterType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_ComponentSettingsFilterType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Filters exist within the optical pathway that leads to one or more Focal Plane Arrays (FPAs). This field specifies
           * those FPAs that are associated with this filter, i.e. those FPAs that record light that passes through this filter.
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PO_FPA_ID_Type, uci::type::accessorType::pO_FPA_ID_Type> FPA_ID;

         /** For those filters that operate over a range of frequencies (as opposed to those that operate on all frequencies),
           * this field is used to specify what those frequencies are. If not specified, then it is assumed that the filter does
           * not pass through light of any frequency. If a single frequency is desired, then set min and max to the same value.
           * [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PO_ComponentSettingsBandpassFrequencyType, uci::type::accessorType::pO_ComponentSettingsBandpassFrequencyType> BandpassFrequency;

         /** Returns the accessor that provides access to the complex content that is identified by the FilterID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FilterID.
           */
         virtual const uci::type::PO_FilterID_Type& getFilterID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FilterID.
           *
           * @return The acecssor that provides access to the complex content that is identified by FilterID.
           */
         virtual uci::type::PO_FilterID_Type& getFilterID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FilterID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FilterID
           */
         virtual void setFilterID(const uci::type::PO_FilterID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FPA_ID.
           *
           * @return The bounded list identified by FPA_ID.
           */
         virtual const uci::type::POST_ComponentSettingsFilterType::FPA_ID& getFPA_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FPA_ID.
           *
           * @return The bounded list identified by FPA_ID.
           */
         virtual uci::type::POST_ComponentSettingsFilterType::FPA_ID& getFPA_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FPA_ID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFPA_ID(const uci::type::POST_ComponentSettingsFilterType::FPA_ID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FilterControls.
           *
           * @return The acecssor that provides access to the complex content that is identified by FilterControls.
           */
         virtual const uci::type::ComponentControlsA_Type& getFilterControls() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FilterControls.
           *
           * @return The acecssor that provides access to the complex content that is identified by FilterControls.
           */
         virtual uci::type::ComponentControlsA_Type& getFilterControls()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FilterControls to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FilterControls
           */
         virtual void setFilterControls(const uci::type::ComponentControlsA_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FilterControls exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FilterControls is emabled or not
           */
         virtual bool hasFilterControls() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FilterControls
           *
           * @param type = uci::type::accessorType::componentControlsA_Type This Accessor's accessor type
           */
         virtual void enableFilterControls(uci::base::accessorType::AccessorType type = uci::type::accessorType::componentControlsA_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FilterControls */
         virtual void clearFilterControls()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the BandpassFrequency.
           *
           * @return The bounded list identified by BandpassFrequency.
           */
         virtual const uci::type::POST_ComponentSettingsFilterType::BandpassFrequency& getBandpassFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the BandpassFrequency.
           *
           * @return The bounded list identified by BandpassFrequency.
           */
         virtual uci::type::POST_ComponentSettingsFilterType::BandpassFrequency& getBandpassFrequency()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the BandpassFrequency.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setBandpassFrequency(const uci::type::POST_ComponentSettingsFilterType::BandpassFrequency& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_ComponentSettingsFilterType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_ComponentSettingsFilterType to copy from
           */
         POST_ComponentSettingsFilterType(const POST_ComponentSettingsFilterType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_ComponentSettingsFilterType to the contents of the
           * POST_ComponentSettingsFilterType on the right hand side (rhs) of the assignment
           * operator.POST_ComponentSettingsFilterType [only available to derived classes]
           *
           * @param rhs The POST_ComponentSettingsFilterType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::POST_ComponentSettingsFilterType
           * @return A constant reference to this POST_ComponentSettingsFilterType.
           */
         const POST_ComponentSettingsFilterType& operator=(const POST_ComponentSettingsFilterType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_ComponentSettingsFilterType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_ComponentSettingsFilterType_h

