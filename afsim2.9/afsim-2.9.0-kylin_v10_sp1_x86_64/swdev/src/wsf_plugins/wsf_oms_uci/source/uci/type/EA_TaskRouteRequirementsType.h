// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__EA_TaskRouteRequirementsType_h
#define Uci__Type__EA_TaskRouteRequirementsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PathType_h)
# include "uci/type/PathType.h"
#endif

#if !defined(Uci__Type__LoiterType_h)
# include "uci/type/LoiterType.h"
#endif

#if !defined(Uci__Type__EA_TaskEscortType_h)
# include "uci/type/EA_TaskEscortType.h"
#endif

#if !defined(Uci__Type__ZoneChoiceType_h)
# include "uci/type/ZoneChoiceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EA_TaskRouteRequirementsType choice accessor class */
      class EA_TaskRouteRequirementsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EA_TaskRouteRequirementsType()
         { }

         /** Returns this accessor's type constant, i.e. EA_TaskRouteRequirementsType
           *
           * @return This accessor's type constant, i.e. EA_TaskRouteRequirementsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eA_TaskRouteRequirementsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EA_TaskRouteRequirementsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The following enumeration is used to identify which element of this Choice has been chosen. */
         enum EA_TaskRouteRequirementsTypeChoice {
            EA_TASKROUTEREQUIREMENTSTYPE_CHOICE_NONE,
            EA_TASKROUTEREQUIREMENTSTYPE_CHOICE_PATH,
            EA_TASKROUTEREQUIREMENTSTYPE_CHOICE_LOITER,
            EA_TASKROUTEREQUIREMENTSTYPE_CHOICE_ESCORT,
            EA_TASKROUTEREQUIREMENTSTYPE_CHOICE_LOCATIONCONSTRAINTS
         };


         /** This method returns this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active.
           *
           * @return The selected item's enumerated value
           */
         virtual EA_TaskRouteRequirementsTypeChoice getEA_TaskRouteRequirementsTypeChoiceOrdinal() const
            throw(uci::base::UCIException) = 0;


         /** This method is used to set this choice's "selection ordinal." A choice's "selection ordinal" is used to specify which
           * element in the choice is chosen to be active. There are two mechanisms that can be used to set a choice's "selection
           * ordinal." The first mechanism is by invoking this method. The second mechanism is by invoking one of the set methods
           * associated with one of the elements contained in this choice. Once this method is invoked, the value returned by
           * getEA_TaskRouteRequirementsTypeChoiceOrdinal() will be the ordinal specified when this method was invoked. In
           * addition, the access methods associated with the chosen element will be enabled and will provide access to the chosen
           * element.
           *
           * @param chosenElementOrdinal The ordinal to set this choice's selected ordinal to.
           * @param type = uci::base::accessorType::null The type of data that is to be made available when the ordinal is set.
           *      The use of this parameter provides support for inheritable types. This parameter defaults to
           *      uci::base::accessorType::null that is used to indicate that the access methods associated with the chosen
           *      element shall provide access to data of the type that was specified for that element in the choice in the OMS
           *      schema, i.e. the chosen element's base type. If specified, this field must either be a type ID associated with
           *      the chosen element's base type or a type ID associated with a type that is derived from the chosen element's
           *      base type.
           */
         virtual void setEA_TaskRouteRequirementsTypeChoiceOrdinal(EA_TaskRouteRequirementsTypeChoice chosenElementOrdinal, uci::base::accessorType::AccessorType type = uci::base::accessorType::null)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Path.
           *
           * @return The acecssor that provides access to the complex content that is identified by Path.
           */
         virtual const uci::type::PathType& getPath() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Path.
           *
           * @return The acecssor that provides access to the complex content that is identified by Path.
           */
         virtual uci::type::PathType& getPath()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Path to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Path
           */
         virtual void setPath(const uci::type::PathType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Loiter.
           *
           * @return The acecssor that provides access to the complex content that is identified by Loiter.
           */
         virtual const uci::type::LoiterType& getLoiter() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Loiter.
           *
           * @return The acecssor that provides access to the complex content that is identified by Loiter.
           */
         virtual uci::type::LoiterType& getLoiter()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Loiter to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Loiter
           */
         virtual void setLoiter(const uci::type::LoiterType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Escort.
           *
           * @return The acecssor that provides access to the complex content that is identified by Escort.
           */
         virtual const uci::type::EA_TaskEscortType& getEscort() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Escort.
           *
           * @return The acecssor that provides access to the complex content that is identified by Escort.
           */
         virtual uci::type::EA_TaskEscortType& getEscort()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Escort to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Escort
           */
         virtual void setEscort(const uci::type::EA_TaskEscortType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LocationConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by LocationConstraints.
           */
         virtual const uci::type::ZoneChoiceType& getLocationConstraints() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LocationConstraints.
           *
           * @return The acecssor that provides access to the complex content that is identified by LocationConstraints.
           */
         virtual uci::type::ZoneChoiceType& getLocationConstraints()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LocationConstraints to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LocationConstraints
           */
         virtual void setLocationConstraints(const uci::type::ZoneChoiceType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EA_TaskRouteRequirementsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EA_TaskRouteRequirementsType to copy from
           */
         EA_TaskRouteRequirementsType(const EA_TaskRouteRequirementsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EA_TaskRouteRequirementsType to the contents of the
           * EA_TaskRouteRequirementsType on the right hand side (rhs) of the assignment operator.EA_TaskRouteRequirementsType
           * [only available to derived classes]
           *
           * @param rhs The EA_TaskRouteRequirementsType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::EA_TaskRouteRequirementsType
           * @return A constant reference to this EA_TaskRouteRequirementsType.
           */
         const EA_TaskRouteRequirementsType& operator=(const EA_TaskRouteRequirementsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EA_TaskRouteRequirementsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EA_TaskRouteRequirementsType_h

