// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Base__Accessor_h
#define Uci__Base__Accessor_h 1

#if !defined(StdLibString)
#  include <string>
//  @cond
#  define StdLibString 1
//  @endcond
#endif


#if !defined(Uci__Base__UCIException_h)
# include "uci/base/UCIException.h"
#endif

#if !defined(Uci__Base__AccessorType_h)
# include "uci/base/accessorType.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all base data types are declared
   namespace base {

      /** The Accessor is the base class of all accessors. */
      class Accessor {
      public:

         /** The destructor */
         virtual ~Accessor()
         { }

         /** Returns this accessor's type constant, i.e. null
           *
           * @return This accessor's type constant, i.e. null
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::base::accessorType::null;
         }


         /** Returns an XML formatted, stringified version of the actual message this accessor provides access to
           *
           * @return The stringified version of the message.
           */
         virtual const std::string toXMLString() const
            throw(uci::base::UCIException) = 0;


         /** This method resets the contents of this Accessor to their initial state, i.e. the state of the contents when the
           * Accessor was instantiated. At a minimum, this will include the following: erasing the contents of any string, erasing
           * the contents of any list, resetting any union so that no field is enabled, erasing the contents of any bounded list,
           * reseting any choice accessor so that no field is enabled. Implementations are free to perform additional operations
           * as part of resetting an accessor.
           */
         virtual void reset()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor */
         Accessor()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The Accessor to copy from
           */
         Accessor(const Accessor& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this Accessor to the contents of the Accessor on the right hand side
           * (rhs) of the assignment operator.Accessor [only available to derived classes]
           *
           * @param rhs The Accessor on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::base::Accessor
           * @return A constant reference to this Accessor.
           */
         const Accessor& operator=(const Accessor& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // Accessor


   } // Namespace: base
} // Namespace: uci

#endif // Uci__Base__Accessor_h

