// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UT_QT_EXCEPTION_HPP
#define UT_QT_EXCEPTION_HPP

#include "utilqt_export.h"

#include <QMessageBox>
#include <QObject>

#include "UtException.hpp"

namespace ut
{
namespace qt
{
namespace detail
{
//! Class used by implementation of PresentMessageBox.
//! Exposed in header file due to Qt MOC requirements.
class UTILQT_EXPORT QueueableExceptionMessage : public QObject
{
   Q_OBJECT
public:
   QueueableExceptionMessage() noexcept;
signals:
   void ErrorSignal(const QString& aTitle, const QString& aDescription);
public slots:
   void ErrorSlot(const QString& aTitle, const QString& aDescription);
};
} // namespace detail

//! Presents a critical popup box to the user.
//! Blocks the calling thread until the box is closed.
//! May be called on any thread.
//! May be called even when there is no active QApplication.
void UTILQT_EXPORT ErrorMessageBox(const QString& aTitle, const QString& aDescription) noexcept;

//! Presents a popup box to the user. The application will close once the popup is closed.
//! Can be called from any thread.
void UTILQT_EXPORT TerminateHandler();

//! To be used when catching exceptions from the main function of the visual applications.
//! Presents a popup box to the user with the error message.
//! @code
//! catch (...)
//! {
//!    ut::qt::ReportException("AFSIM", std::current_exception());
//! }
//! @endcode
void UTILQT_EXPORT ReportException(const char* aSource, std::exception_ptr aException, const char* aAdditionalInfo = nullptr);

} // namespace qt
} // namespace ut

#endif
