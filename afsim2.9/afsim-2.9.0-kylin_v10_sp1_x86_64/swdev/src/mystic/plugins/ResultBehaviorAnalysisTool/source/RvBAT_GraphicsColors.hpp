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

#ifndef RVBAT_GRAPHICSCOLORS_HPP
#define RVBAT_GRAPHICSCOLORS_HPP

#include <QColor>

namespace RvBAT
{
const unsigned int cNODE_LINE_LENGTH        = 40; // Length of input and output lines from a node vertically, in pixels
const unsigned int cNODE_VERTICAL_SPACING   = 40; // Spacing vertically in between nodes, in pixels
const unsigned int cNODE_HORIZONTAL_SPACING = 20; // Spacing horizontally in between nodes, in pixels

// Coloring constants
// Dark Theme
// Pen coloring
const QColor cDEFAULT_TEXT_COLOR_DARK = QColor("#BBBBBB"); // Off-white
const QColor cDEFAULT_PEN_COLOR_DARK  = QColor("#AAAAAA"); // Off-white
const QColor cRUNNING_PEN_COLOR_DARK  = QColor("#B3994B"); // Yellow
const QColor cSUCCESS_PEN_COLOR_DARK  = QColor("#49A367"); // Green
const QColor cHALTED_PEN_COLOR_DARK   = QColor("#A36249"); // Red-Orange
const QColor cFAILURE_PEN_COLOR_DARK  = QColor("#A34951"); // Red

// Gradients for backgrounds of nodes
const QColor cRUNNING_GRADIENT_TOP_DARK    = QColor("#8A7E45"); // Bright Yellow
const QColor cRUNNING_GRADIENT_BOTTOM_DARK = QColor("#574E23"); // Dark Yellow
const QColor cSUCCESS_GRADIENT_TOP_DARK    = QColor("#458A5C"); // Bright Green
const QColor cSUCCESS_GRADIENT_BOTTOM_DARK = QColor("#235736"); // Dark Green
const QColor cFAILURE_GRADIENT_TOP_DARK    = QColor("#8A4550"); // Bright Red
const QColor cFAILURE_GRADIENT_BOTTOM_DARK = QColor("#57232B"); // Dark Red
const QColor cHALTED_GRADIENT_TOP_DARK     = QColor("#8A5645"); // Bright Red-Orange
const QColor cHALTED_GRADIENT_BOTTOM_DARK  = QColor("#573023"); // Dark Red-Orange
const QColor cDEFAULT_GRADIENT_TOP_DARK    = QColor("#555557"); // Bright Grey
const QColor cDEFAULT_GRADIENT_BOTTOM_DARK = QColor("#333335"); // Dark Grey

// Gradients for the outlines around the squares
const QColor cRUNNING_OUTLINE_GRADIENT_TOP_DARK    = QColor("#A69853"); // Bright Yellow
const QColor cRUNNING_OUTLINE_GRADIENT_BOTTOM_DARK = QColor("#574E23"); // Dark Yellow
const QColor cSUCCESS_OUTLINE_GRADIENT_TOP_DARK    = QColor("#53A66F"); // Bright Green
const QColor cSUCCESS_OUTLINE_GRADIENT_BOTTOM_DARK = QColor("#235734"); // Dark Green
const QColor cFAILURE_OUTLINE_GRADIENT_TOP_DARK    = QColor("#A65462"); // Bright Red
const QColor cFAILURE_OUTLINE_GRADIENT_BOTTOM_DARK = QColor("#57232B"); // Dark Red
const QColor cHALTED_OUTLINE_GRADIENT_TOP_DARK     = QColor("#A66955"); // Bright Red-Orange
const QColor cHALTED_OUTLINE_GRADIENT_BOTTOM_DARK  = QColor("#573023"); // Dark Red-Orange
const QColor cDEFAULT_OUTLINE_GRADIENT_TOP_DARK    = QColor("#777778"); // Bright Grey
const QColor cDEFAULT_OUTLINE_GRADIENT_BOTTOM_DARK = QColor("#333333"); // Dark Grey

// Light Theme
// Pen coloring
const QColor cDEFAULT_TEXT_COLOR_LIGHT = QColor("#222222"); // Dark Grey
const QColor cDEFAULT_PEN_COLOR_LIGHT  = QColor("#777777"); // Grey
const QColor cRUNNING_PEN_COLOR_LIGHT  = QColor("#CCBF81"); // Yellow
const QColor cSUCCESS_PEN_COLOR_LIGHT  = QColor("#81CCA5"); // Green
const QColor cFAILURE_PEN_COLOR_LIGHT  = QColor("#CC8181"); // Red
const QColor cHALTED_PEN_COLOR_LIGHT   = QColor("#CC9983"); // Red-Orange

// Gradients for backgrounds of nodes
const QColor cRUNNING_GRADIENT_HIGH_LIGHT   = QColor("#D6BC71"); // Dark Yellow
const QColor cRUNNING_GRADIENT_LOW_LIGHT    = QColor("#FFE8A3"); // Bright Yellow
const QColor cSUCCESS_GRADIENT_TOP_LIGHT    = QColor("#A1FFCE"); // Bright Green
const QColor cSUCCESS_GRADIENT_BOTTOM_LIGHT = QColor("#72D67C"); // Dark Green
const QColor cFAILURE_GRADIENT_HIGH_LIGHT   = QColor("#D67272"); // Dark Red
const QColor cFAILURE_GRADIENT_LOW_LIGHT    = QColor("#FFA1A1"); // Bright Red
const QColor cHALTED_GRADIENT_HIGH_LIGHT    = QColor("#D69072"); // Dark Red-Orange
const QColor cHALTED_GRADIENT_LOW_LIGHT     = QColor("#FFBFA3"); // Bright Red-Orange
const QColor cDEFAULT_GRADIENT_TOP_LIGHT    = QColor("#EEEEEE"); // Bright Grey
const QColor cDEFAULT_GRADIENT_BOTTOM_LIGHT = QColor("#AAAAAA"); // Dark Grey

// Gradients for the outlines around the squares
const QColor cRUNNING_OUTLINE_GRADIENT_TOP_LIGHT    = QColor("#CCC395"); // Bright Yellow
const QColor cRUNNING_OUTLINE_GRADIENT_BOTTOM_LIGHT = QColor("#A39656"); // Dark Yellow
const QColor cSUCCESS_OUTLINE_GRADIENT_TOP_LIGHT    = QColor("#95CCB0"); // Bright Green
const QColor cSUCCESS_OUTLINE_GRADIENT_BOTTOM_LIGHT = QColor("#56A35E"); // Dark Green
const QColor cFAILURE_OUTLINE_GRADIENT_TOP_LIGHT    = QColor("#CC959E"); // Bright Red
const QColor cFAILURE_OUTLINE_GRADIENT_BOTTOM_LIGHT = QColor("#A35663"); // Dark Red
const QColor cHALTED_OUTLINE_GRADIENT_TOP_LIGHT     = QColor("#CCA395"); // Bright Red-Orange
const QColor cHALTED_OUTLINE_GRADIENT_BOTTOM_LIGHT  = QColor("#A36A56"); // Dark Red-Orange
const QColor cDEFAULT_OUTLINE_GRADIENT_TOP_LIGHT    = QColor("#AAAAAA"); // Bright Grey
const QColor cDEFAULT_OUTLINE_GRADIENT_BOTTOM_LIGHT = QColor("#777777"); // Dark Grey
} // namespace RvBAT

#endif // RVBAT_GRAPHICSCOLORS_HPP
