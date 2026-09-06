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
#ifndef Uci__Type__MissionPlanActivationCommandMDT_h
#define Uci__Type__MissionPlanActivationCommandMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MissionPlanActivationCommandID_Type_h)
# include "uci/type/MissionPlanActivationCommandID_Type.h"
#endif

#if !defined(Uci__Type__MissionPlanID_Type_h)
# include "uci/type/MissionPlanID_Type.h"
#endif

#if !defined(Uci__Type__MissionPlanActivationCommandEnum_h)
# include "uci/type/MissionPlanActivationCommandEnum.h"
#endif

#if !defined(Uci__Type__ActivationPathSegmentType_h)
# include "uci/type/ActivationPathSegmentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MissionPlanActivationCommandMDT sequence accessor class */
      class MissionPlanActivationCommandMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MissionPlanActivationCommandMDT()
         { }

         /** Returns this accessor's type constant, i.e. MissionPlanActivationCommandMDT
           *
           * @return This accessor's type constant, i.e. MissionPlanActivationCommandMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::missionPlanActivationCommandMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MissionPlanActivationCommandMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * MissionPlanActivationCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by
           *      MissionPlanActivationCommandID.
           */
         virtual const uci::type::MissionPlanActivationCommandID_Type& getMissionPlanActivationCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the
           * MissionPlanActivationCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by
           *      MissionPlanActivationCommandID.
           */
         virtual uci::type::MissionPlanActivationCommandID_Type& getMissionPlanActivationCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionPlanActivationCommandID to the contents of the complex
           * content that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionPlanActivationCommandID
           */
         virtual void setMissionPlanActivationCommandID(const uci::type::MissionPlanActivationCommandID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanID.
           */
         virtual const uci::type::MissionPlanID_Type& getMissionPlanID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionPlanID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionPlanID.
           */
         virtual uci::type::MissionPlanID_Type& getMissionPlanID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionPlanID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionPlanID
           */
         virtual void setMissionPlanID(const uci::type::MissionPlanID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommandType.
           *
           * @return The value of the enumeration identified by CommandType.
           */
         virtual const uci::type::MissionPlanActivationCommandEnum& getCommandType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommandType.
           *
           * @return The value of the enumeration identified by CommandType.
           */
         virtual uci::type::MissionPlanActivationCommandEnum& getCommandType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommandType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommandType(const uci::type::MissionPlanActivationCommandEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommandType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommandType(uci::type::MissionPlanActivationCommandEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PathSegment.
           *
           * @return The acecssor that provides access to the complex content that is identified by PathSegment.
           */
         virtual const uci::type::ActivationPathSegmentType& getPathSegment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PathSegment.
           *
           * @return The acecssor that provides access to the complex content that is identified by PathSegment.
           */
         virtual uci::type::ActivationPathSegmentType& getPathSegment()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PathSegment to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PathSegment
           */
         virtual void setPathSegment(const uci::type::ActivationPathSegmentType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PathSegment exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PathSegment is emabled or not
           */
         virtual bool hasPathSegment() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PathSegment
           *
           * @param type = uci::type::accessorType::activationPathSegmentType This Accessor's accessor type
           */
         virtual void enablePathSegment(uci::base::accessorType::AccessorType type = uci::type::accessorType::activationPathSegmentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PathSegment */
         virtual void clearPathSegment()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MissionPlanActivationCommandMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MissionPlanActivationCommandMDT to copy from
           */
         MissionPlanActivationCommandMDT(const MissionPlanActivationCommandMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MissionPlanActivationCommandMDT to the contents of the
           * MissionPlanActivationCommandMDT on the right hand side (rhs) of the assignment
           * operator.MissionPlanActivationCommandMDT [only available to derived classes]
           *
           * @param rhs The MissionPlanActivationCommandMDT on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::MissionPlanActivationCommandMDT
           * @return A constant reference to this MissionPlanActivationCommandMDT.
           */
         const MissionPlanActivationCommandMDT& operator=(const MissionPlanActivationCommandMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MissionPlanActivationCommandMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MissionPlanActivationCommandMDT_h

