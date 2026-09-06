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
#ifndef Uci__Type__PO_ComponentStatusLensAssemblyType_h
#define Uci__Type__PO_ComponentStatusLensAssemblyType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PO_LensID_Type_h)
# include "uci/type/PO_LensID_Type.h"
#endif

#if !defined(Uci__Type__ComponentControlsStatusType_h)
# include "uci/type/ComponentControlsStatusType.h"
#endif

#if !defined(Uci__Type__PO_ComponentStatusLensAssemblyFieldOfViewType_h)
# include "uci/type/PO_ComponentStatusLensAssemblyFieldOfViewType.h"
#endif

#if !defined(Uci__Type__PO_ComponentStatusLensAssemblyFocusType_h)
# include "uci/type/PO_ComponentStatusLensAssemblyFocusType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_ComponentStatusLensAssemblyType sequence accessor class */
      class PO_ComponentStatusLensAssemblyType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PO_ComponentStatusLensAssemblyType()
         { }

         /** Returns this accessor's type constant, i.e. PO_ComponentStatusLensAssemblyType
           *
           * @return This accessor's type constant, i.e. PO_ComponentStatusLensAssemblyType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_ComponentStatusLensAssemblyType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_ComponentStatusLensAssemblyType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LensID.
           *
           * @return The acecssor that provides access to the complex content that is identified by LensID.
           */
         virtual const uci::type::PO_LensID_Type& getLensID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LensID.
           *
           * @return The acecssor that provides access to the complex content that is identified by LensID.
           */
         virtual uci::type::PO_LensID_Type& getLensID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LensID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LensID
           */
         virtual void setLensID(const uci::type::PO_LensID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LensControlsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by LensControlsStatus.
           */
         virtual const uci::type::ComponentControlsStatusType& getLensControlsStatus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LensControlsStatus.
           *
           * @return The acecssor that provides access to the complex content that is identified by LensControlsStatus.
           */
         virtual uci::type::ComponentControlsStatusType& getLensControlsStatus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LensControlsStatus to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LensControlsStatus
           */
         virtual void setLensControlsStatus(const uci::type::ComponentControlsStatusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FOV.
           *
           * @return The acecssor that provides access to the complex content that is identified by FOV.
           */
         virtual const uci::type::PO_ComponentStatusLensAssemblyFieldOfViewType& getFOV() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FOV.
           *
           * @return The acecssor that provides access to the complex content that is identified by FOV.
           */
         virtual uci::type::PO_ComponentStatusLensAssemblyFieldOfViewType& getFOV()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FOV to the contents of the complex content that is accessed by the
           * specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FOV
           */
         virtual void setFOV(const uci::type::PO_ComponentStatusLensAssemblyFieldOfViewType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FOV exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FOV is emabled or not
           */
         virtual bool hasFOV() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FOV
           *
           * @param type = uci::type::accessorType::pO_ComponentStatusLensAssemblyFieldOfViewType This Accessor's accessor type
           */
         virtual void enableFOV(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_ComponentStatusLensAssemblyFieldOfViewType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FOV */
         virtual void clearFOV()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Focus.
           *
           * @return The acecssor that provides access to the complex content that is identified by Focus.
           */
         virtual const uci::type::PO_ComponentStatusLensAssemblyFocusType& getFocus() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Focus.
           *
           * @return The acecssor that provides access to the complex content that is identified by Focus.
           */
         virtual uci::type::PO_ComponentStatusLensAssemblyFocusType& getFocus()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Focus to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Focus
           */
         virtual void setFocus(const uci::type::PO_ComponentStatusLensAssemblyFocusType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Focus exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Focus is emabled or not
           */
         virtual bool hasFocus() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Focus
           *
           * @param type = uci::type::accessorType::pO_ComponentStatusLensAssemblyFocusType This Accessor's accessor type
           */
         virtual void enableFocus(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_ComponentStatusLensAssemblyFocusType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Focus */
         virtual void clearFocus()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_ComponentStatusLensAssemblyType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_ComponentStatusLensAssemblyType to copy from
           */
         PO_ComponentStatusLensAssemblyType(const PO_ComponentStatusLensAssemblyType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_ComponentStatusLensAssemblyType to the contents of the
           * PO_ComponentStatusLensAssemblyType on the right hand side (rhs) of the assignment
           * operator.PO_ComponentStatusLensAssemblyType [only available to derived classes]
           *
           * @param rhs The PO_ComponentStatusLensAssemblyType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::PO_ComponentStatusLensAssemblyType
           * @return A constant reference to this PO_ComponentStatusLensAssemblyType.
           */
         const PO_ComponentStatusLensAssemblyType& operator=(const PO_ComponentStatusLensAssemblyType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_ComponentStatusLensAssemblyType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_ComponentStatusLensAssemblyType_h

