// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef UTQTENCODEWIDGETSTATE_HPP
#define UTQTENCODEWIDGETSTATE_HPP

#include "utilqt_export.h"

#include <string>
class QWidget;

//! Encodes the widget's value into a string for decoding later
std::string UTILQT_EXPORT UtQtEncodeWidgetState(QWidget* aWidgetPtr);
//! Decodes a string representing a widget's value and assigns that value to the widget.
bool UTILQT_EXPORT UtQtDecodeWidgetState(const std::string& aStateString, QWidget* aWidgetPtr);

#endif
