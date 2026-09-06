// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UT_LOG_HPP
#define UT_LOG_HPP

#include "ut_export.h"

#include "UtLogMessage.hpp"
#include "UtLogStream.hpp"

namespace ut
{
namespace log
{
/*
Notes for writing using the ut::log framework:

The MessageStream class is designed to use operator<< for output, just like std::ostream.

Here is a basic hello world program using this framework:

``` #include "UtLog.hpp"
```
``` int main()
``` {
```    ut::log::info() << "Hello, World!";
```    return 0;
``` }

In the case that writing a message is broken up between multiple lines, do so like this:

``` // Opening new scope for RAII.
``` {
```    auto out = ut::log::info() << "Data is still allowed to go here.";
```
```    if (condition)
```    {
```       out.AddNote() << "Additional data.";
```    }
``` }

The out variable will send its data once it is destroyed or its Send() method is called.
In the above example, if the programmer does not create a new RAII scope and forgets to call Send(),
then MessageStreams created later in this scope will print first, causing messages to appear out of order.
When possible, surround the creation of complex messages with braces {} so the MessageStream destructor
is responsible for sending the message.

If a MessageStream needs to be passed to a function taking a std::ostream&, use MessageStream::GetStream().

If a plugin or other application is created that needs a custom message type, pass the type or list of types to the
stream factory functions and subscribe to those types.

In the console, a newline character will be automatically appended to every message.
Notes will be prepended with a tab character and appended to with a newline character.

See the ut::log::Batch class for automatically applying types to new messages, and for a thread-safe way of
creating sets of messages that cannot be interleaved in multi-threaded environments.
*/

//! Creates a stream with the specified types.
//! Note: Can be called with a single string or a list of strings.
//! {
MessageStream UT_EXPORT write(TypeList aTypes) noexcept;
MessageStream UT_EXPORT fatal(TypeList aTypes = TypeList()) noexcept;
MessageStream UT_EXPORT error(TypeList aTypes = TypeList()) noexcept;
MessageStream UT_EXPORT warning(TypeList aTypes = TypeList()) noexcept;
MessageStream UT_EXPORT debug(TypeList aTypes = TypeList()) noexcept;
MessageStream UT_EXPORT info(TypeList aTypes = TypeList()) noexcept;
MessageStream UT_EXPORT developer(TypeList aTypes = TypeList()) noexcept;
//! }

//! Creates and sends a "format" stream to create an empty line in the console.
void UT_EXPORT newline(TypeList aTypes = TypeList()) noexcept;

//! On destruction, writes a label to a stream if it is not empty.
//! Useful in cases where all notes are only conditionally added to the stream.
class UT_EXPORT DataMessageHelper final
{
public:
   DataMessageHelper(MessageStream& aStream, std::string aLabel) noexcept;
   ~DataMessageHelper() noexcept;

private:
   MessageStream& mStream;
   std::string    mLabel;
};
} // namespace log
} // namespace ut

#endif
