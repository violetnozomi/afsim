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
#ifndef Uci__Type__POST_ComponentApertureType_h
#define Uci__Type__POST_ComponentApertureType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__POST_ApertureID_Type_h)
# include "uci/type/POST_ApertureID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__PO_FPA_ID_Type_h)
# include "uci/type/PO_FPA_ID_Type.h"
#endif

#if !defined(Uci__Type__POST_ComponentF_StopType_h)
# include "uci/type/POST_ComponentF_StopType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_ComponentApertureType sequence accessor class */
      class POST_ComponentApertureType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_ComponentApertureType()
         { }

         /** Returns this accessor's type constant, i.e. POST_ComponentApertureType
           *
           * @return This accessor's type constant, i.e. POST_ComponentApertureType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_ComponentApertureType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_ComponentApertureType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Lens exist within the optical pathway that leads to one or more Focal Plane Arrays (FPAs). This field specifies those
           * FPAs that are associated with this lens, i.e. those FPAs that record light that passes through this lens. [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::PO_FPA_ID_Type, uci::type::accessorType::pO_FPA_ID_Type> FPA_ID;

         /** Returns the accessor that provides access to the complex content that is identified by the ApertureID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApertureID.
           */
         virtual const uci::type::POST_ApertureID_Type& getApertureID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ApertureID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ApertureID.
           */
         virtual uci::type::POST_ApertureID_Type& getApertureID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ApertureID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ApertureID
           */
         virtual void setApertureID(const uci::type::POST_ApertureID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FPA_ID.
           *
           * @return The bounded list identified by FPA_ID.
           */
         virtual const uci::type::POST_ComponentApertureType::FPA_ID& getFPA_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FPA_ID.
           *
           * @return The bounded list identified by FPA_ID.
           */
         virtual uci::type::POST_ComponentApertureType::FPA_ID& getFPA_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FPA_ID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFPA_ID(const uci::type::POST_ComponentApertureType::FPA_ID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the F_Stop.
           *
           * @return The acecssor that provides access to the complex content that is identified by F_Stop.
           */
         virtual const uci::type::POST_ComponentF_StopType& getF_Stop() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the F_Stop.
           *
           * @return The acecssor that provides access to the complex content that is identified by F_Stop.
           */
         virtual uci::type::POST_ComponentF_StopType& getF_Stop()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the F_Stop to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by F_Stop
           */
         virtual void setF_Stop(const uci::type::POST_ComponentF_StopType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_ComponentApertureType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_ComponentApertureType to copy from
           */
         POST_ComponentApertureType(const POST_ComponentApertureType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_ComponentApertureType to the contents of the
           * POST_ComponentApertureType on the right hand side (rhs) of the assignment operator.POST_ComponentApertureType [only
           * available to derived classes]
           *
           * @param rhs The POST_ComponentApertureType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::POST_ComponentApertureType
           * @return A constant reference to this POST_ComponentApertureType.
           */
         const POST_ComponentApertureType& operator=(const POST_ComponentApertureType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_ComponentApertureType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_ComponentApertureType_h

