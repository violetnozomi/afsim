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
#ifndef Uci__Type__COMINT_AcquisitionTargetType_h
#define Uci__Type__COMINT_AcquisitionTargetType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__COMINT_TargetType_h)
# include "uci/type/COMINT_TargetType.h"
#endif

#if !defined(Uci__Type__RankingType_h)
# include "uci/type/RankingType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the COMINT_AcquisitionTargetType sequence accessor class */
      class COMINT_AcquisitionTargetType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~COMINT_AcquisitionTargetType()
         { }

         /** Returns this accessor's type constant, i.e. COMINT_AcquisitionTargetType
           *
           * @return This accessor's type constant, i.e. COMINT_AcquisitionTargetType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::cOMINT_AcquisitionTargetType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const COMINT_AcquisitionTargetType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Target.
           *
           * @return The acecssor that provides access to the complex content that is identified by Target.
           */
         virtual const uci::type::COMINT_TargetType& getTarget() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Target.
           *
           * @return The acecssor that provides access to the complex content that is identified by Target.
           */
         virtual uci::type::COMINT_TargetType& getTarget()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Target to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Target
           */
         virtual void setTarget(const uci::type::COMINT_TargetType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Priority.
           *
           * @return The acecssor that provides access to the complex content that is identified by Priority.
           */
         virtual const uci::type::RankingType& getPriority() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Priority.
           *
           * @return The acecssor that provides access to the complex content that is identified by Priority.
           */
         virtual uci::type::RankingType& getPriority()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Priority to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Priority
           */
         virtual void setPriority(const uci::type::RankingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Priority exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Priority is emabled or not
           */
         virtual bool hasPriority() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Priority
           *
           * @param type = uci::type::accessorType::rankingType This Accessor's accessor type
           */
         virtual void enablePriority(uci::base::accessorType::AccessorType type = uci::type::accessorType::rankingType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Priority */
         virtual void clearPriority()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         COMINT_AcquisitionTargetType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The COMINT_AcquisitionTargetType to copy from
           */
         COMINT_AcquisitionTargetType(const COMINT_AcquisitionTargetType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this COMINT_AcquisitionTargetType to the contents of the
           * COMINT_AcquisitionTargetType on the right hand side (rhs) of the assignment operator.COMINT_AcquisitionTargetType
           * [only available to derived classes]
           *
           * @param rhs The COMINT_AcquisitionTargetType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::COMINT_AcquisitionTargetType
           * @return A constant reference to this COMINT_AcquisitionTargetType.
           */
         const COMINT_AcquisitionTargetType& operator=(const COMINT_AcquisitionTargetType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // COMINT_AcquisitionTargetType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__COMINT_AcquisitionTargetType_h

