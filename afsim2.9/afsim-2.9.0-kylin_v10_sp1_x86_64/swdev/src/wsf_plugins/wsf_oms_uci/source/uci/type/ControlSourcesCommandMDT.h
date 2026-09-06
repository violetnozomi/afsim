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
#ifndef Uci__Type__ControlSourcesCommandMDT_h
#define Uci__Type__ControlSourcesCommandMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ControlSourcesCommandID_Type_h)
# include "uci/type/ControlSourcesCommandID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Type__ControlSourcesControlType_h)
# include "uci/type/ControlSourcesControlType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ControlSourcesCommandMDT sequence accessor class */
      class ControlSourcesCommandMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ControlSourcesCommandMDT()
         { }

         /** Returns this accessor's type constant, i.e. ControlSourcesCommandMDT
           *
           * @return This accessor's type constant, i.e. ControlSourcesCommandMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::controlSourcesCommandMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ControlSourcesCommandMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ControlSourcesCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ControlSourcesCommandID.
           */
         virtual const uci::type::ControlSourcesCommandID_Type& getControlSourcesCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ControlSourcesCommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ControlSourcesCommandID.
           */
         virtual uci::type::ControlSourcesCommandID_Type& getControlSourcesCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ControlSourcesCommandID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ControlSourcesCommandID
           */
         virtual void setControlSourcesCommandID(const uci::type::ControlSourcesCommandID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ControleeSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ControleeSystemID.
           */
         virtual const uci::type::SystemID_Type& getControleeSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ControleeSystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ControleeSystemID.
           */
         virtual uci::type::SystemID_Type& getControleeSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ControleeSystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ControleeSystemID
           */
         virtual void setControleeSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ControlType.
           *
           * @return The acecssor that provides access to the complex content that is identified by ControlType.
           */
         virtual const uci::type::ControlSourcesControlType& getControlType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ControlType.
           *
           * @return The acecssor that provides access to the complex content that is identified by ControlType.
           */
         virtual uci::type::ControlSourcesControlType& getControlType()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ControlType to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ControlType
           */
         virtual void setControlType(const uci::type::ControlSourcesControlType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ControlSourcesCommandMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ControlSourcesCommandMDT to copy from
           */
         ControlSourcesCommandMDT(const ControlSourcesCommandMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ControlSourcesCommandMDT to the contents of the
           * ControlSourcesCommandMDT on the right hand side (rhs) of the assignment operator.ControlSourcesCommandMDT [only
           * available to derived classes]
           *
           * @param rhs The ControlSourcesCommandMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ControlSourcesCommandMDT
           * @return A constant reference to this ControlSourcesCommandMDT.
           */
         const ControlSourcesCommandMDT& operator=(const ControlSourcesCommandMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ControlSourcesCommandMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ControlSourcesCommandMDT_h

