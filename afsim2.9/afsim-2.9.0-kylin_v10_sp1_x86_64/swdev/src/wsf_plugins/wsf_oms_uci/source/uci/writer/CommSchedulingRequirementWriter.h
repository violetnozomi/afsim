// This file was generated  on 12/5/2018 at 1:1:50 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Writer__CommSchedulingRequirementWriter_h
#define Uci__Writer__CommSchedulingRequirementWriter_h 1

#if !defined(Uci__Type__CommSchedulingRequirement_h)
# include "uci/type/CommSchedulingRequirement.h"
#endif

#if !defined(Uci__Base__Writer_h)
# include "uci/base/Writer.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all writers are declared
   namespace writer {

      /** The writer provides the support to write messages. */
      class CommSchedulingRequirementWriter : public virtual uci::base::Writer {
      public:

         /** The destructor */
         virtual ~CommSchedulingRequirementWriter()
         { }

         /** Writes the OMS message to the topic specified when this Writer was created.
           *
           * @param accessor The Accessor that provides access to the message that is to be written.
           */
         virtual void write(uci::type::CommSchedulingRequirement& accessor)
            throw(uci::base::UCIException) = 0;


         /** Closes the writer. Once closed, any further attempts to use this Writer to write messages will result in an undefined
           * behavior.
           */
         virtual void close()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommSchedulingRequirementWriter()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommSchedulingRequirementWriter to copy from
           */
         CommSchedulingRequirementWriter(const CommSchedulingRequirementWriter& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommSchedulingRequirementWriter to the contents of the
           * CommSchedulingRequirementWriter on the right hand side (rhs) of the assignment
           * operator.CommSchedulingRequirementWriter [only available to derived classes]
           *
           * @param rhs The CommSchedulingRequirementWriter on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this CommSchedulingRequirementWriter
           * @return A constant reference to this CommSchedulingRequirementWriter.
           */
         const CommSchedulingRequirementWriter& operator=(const CommSchedulingRequirementWriter& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommSchedulingRequirementWriter


   } // Namespace: writer
} // Namespace: uci

#endif // Uci__Writer__CommSchedulingRequirementWriter_h

