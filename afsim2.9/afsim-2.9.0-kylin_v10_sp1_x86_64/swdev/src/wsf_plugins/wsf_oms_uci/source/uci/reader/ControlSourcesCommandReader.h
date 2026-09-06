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
#ifndef Uci__Reader__ControlSourcesCommandReader_h
#define Uci__Reader__ControlSourcesCommandReader_h 1

#if !defined(Uci__Type__ControlSourcesCommand_h)
# include "uci/type/ControlSourcesCommand.h"
#endif

#if !defined(Uci__Base__Reader_h)
# include "uci/base/Reader.h"
#endif

#if !defined(Uci__Base__Listener_h)
# include "uci/base/Listener.h"
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all readers are declared
   namespace reader {

      /** The ControlSourcesCommandListener provides the support to listen and handle incoming messages of the type
        * uci::type::ControlSourcesCommand
        */
      class ControlSourcesCommandListener : public virtual uci::base::Listener {
      public:

         /** The destructor */
         virtual ~ControlSourcesCommandListener()
         { }

         /** The method is used to handle incoming messages.
           *
           * @param message The newly arrive message that is to be handled by this method
           */
         virtual void handleMessage(const uci::type::ControlSourcesCommand& message)
            throw(uci::base::UCIException) = 0;


      }; // ControlSourcesCommandListener


      /** The Reader provides the support to reader incoming messages. */
      class ControlSourcesCommandReader : public virtual uci::base::Reader {
      public:

         /** The destructor */
         virtual ~ControlSourcesCommandReader()
         { }

         /** Adds a listener to this reader to handle incoming messages.
           *
           * @param listener The listener that will listen for and handle incoming messages.
           */
         virtual void addListener(ControlSourcesCommandListener& listener)
            throw(uci::base::UCIException) = 0;


         /** Removes the specified listener from this reader.
           *
           * @param listener The listener that is to be removed from this reader.
           */
         virtual void removeListener(ControlSourcesCommandListener& listener)
            throw(uci::base::UCIException) = 0;


         /** Reads the next incoming message if available. Does not wait for a message to be received.
           *
           * @param maxNumberOfMessages The maximum number of messages that will be handled by the read operation.
           * @param listener The listener that will handle the incoming messages.
           * @return The number of messages that were handled
           */
         virtual unsigned long readNoWait(unsigned long maxNumberOfMessages, ControlSourcesCommandListener& listener)
            throw(uci::base::UCIException) = 0;


         /** Reads the next incoming message.
           *
           * @param timeout Maximum time in microseconds to wait for new messages to arrive. The timeout is ignored if one or
           *      messages are already queued. After the first invocation of the listener's handleMessage(), this timeout is
           *      ignored. A zero indicates that the read should wait forever.
           * @param maxNumberOfMessages The maximum number of messages that will be handled by the read operation.
           * @param listener The listener that will handle the incoming messages.
           * @return The number of messages that were handled
           */
         virtual unsigned long read(unsigned long timeout, unsigned long maxNumberOfMessages, ControlSourcesCommandListener& listener)
            throw(uci::base::UCIException) = 0;


         /** Closes the reader. Once closed, any further attempts to use this Reader to read messages, either by adding new
           * Listeners or by invoking the read() method, will result in an undefined behavior
           */
         virtual void close()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ControlSourcesCommandReader()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ControlSourcesCommandReader to copy from
           */
         ControlSourcesCommandReader(const ControlSourcesCommandReader& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ControlSourcesCommandReader to the contents of the
           * ControlSourcesCommandReader on the right hand side (rhs) of the assignment operator.ControlSourcesCommandReader [only
           * available to derived classes]
           *
           * @param rhs The ControlSourcesCommandReader on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this ControlSourcesCommandReader
           * @return A constant reference to this ControlSourcesCommandReader.
           */
         const ControlSourcesCommandReader& operator=(const ControlSourcesCommandReader& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ControlSourcesCommandReader


   } // Namespace: reader
} // Namespace: uci

#endif // Uci__Reader__ControlSourcesCommandReader_h

