// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef TRANSFORMFILE_HPP
#define TRANSFORMFILE_HPP

#include "artificer_export.h"

#include <string>

#include "UtPath.hpp"

namespace artificer
{
///@brief Reads file `aFilename`, and uses the output type `aOutputType` transformation on its data
void ARTIFICER_EXPORT TransformFile(const std::string& aOutputType, const UtPath& aFilename);
} // namespace artificer

#endif
