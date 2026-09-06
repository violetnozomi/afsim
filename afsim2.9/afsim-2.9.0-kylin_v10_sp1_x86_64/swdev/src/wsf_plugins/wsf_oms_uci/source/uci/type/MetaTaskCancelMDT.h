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
#ifndef Uci__Type__MetaTaskCancelMDT_h
#define Uci__Type__MetaTaskCancelMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MetaTaskCancelID_Type_h)
# include "uci/type/MetaTaskCancelID_Type.h"
#endif

#if !defined(Uci__Type__MetaTaskID_Type_h)
# include "uci/type/MetaTaskID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MetaTaskCancelMDT sequence accessor class */
      class MetaTaskCancelMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MetaTaskCancelMDT()
         { }

         /** Returns this accessor's type constant, i.e. MetaTaskCancelMDT
           *
           * @return This accessor's type constant, i.e. MetaTaskCancelMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::metaTaskCancelMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MetaTaskCancelMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MetaTaskCancelID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MetaTaskCancelID.
           */
         virtual const uci::type::MetaTaskCancelID_Type& getMetaTaskCancelID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MetaTaskCancelID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MetaTaskCancelID.
           */
         virtual uci::type::MetaTaskCancelID_Type& getMetaTaskCancelID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MetaTaskCancelID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MetaTaskCancelID
           */
         virtual void setMetaTaskCancelID(const uci::type::MetaTaskCancelID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MetaTaskID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MetaTaskID.
           */
         virtual const uci::type::MetaTaskID_Type& getMetaTaskID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MetaTaskID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MetaTaskID.
           */
         virtual uci::type::MetaTaskID_Type& getMetaTaskID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MetaTaskID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MetaTaskID
           */
         virtual void setMetaTaskID(const uci::type::MetaTaskID_Type& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MetaTaskCancelMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MetaTaskCancelMDT to copy from
           */
         MetaTaskCancelMDT(const MetaTaskCancelMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MetaTaskCancelMDT to the contents of the MetaTaskCancelMDT on the
           * right hand side (rhs) of the assignment operator.MetaTaskCancelMDT [only available to derived classes]
           *
           * @param rhs The MetaTaskCancelMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::MetaTaskCancelMDT
           * @return A constant reference to this MetaTaskCancelMDT.
           */
         const MetaTaskCancelMDT& operator=(const MetaTaskCancelMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MetaTaskCancelMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MetaTaskCancelMDT_h

