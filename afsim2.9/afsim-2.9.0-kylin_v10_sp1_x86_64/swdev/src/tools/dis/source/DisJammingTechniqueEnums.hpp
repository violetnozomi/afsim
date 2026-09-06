// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DisJammingTechniqueEnums_hpp
#define DisJammingTechniqueEnums_hpp

#include <string>

#include "DisTypes.hpp"

// The following Enumerations for the Jamming Technique Record are obtained from
// the SISO-REF-010-00v20-0 (published 19 November 2013).

class DisJammingTechniqueRecord;

namespace DisEnum
{
namespace jamming_technique
{
enum KindTypes
{
   Noise             = 1,
   Deception         = 2,
   DeceptionAndNoise = 3,
   Special           = 4
};
inline bool KindIsValid(DisEnum8 kind)
{
   return (kind <= Special);
}
const std::string& KindToString(DisEnum8 kind);

namespace noise
{
enum CategoryTypes
{
   AmplitudeModulationNoise   = 5,
   BarrageNoise               = 10,
   BistaticClutter            = 15,
   Comb                       = 20,
   CooperativeBlinkedNoise    = 25,
   DopplerNoise               = 30,
   FrequencyModulationByNoise = 35,
   ImpulseNoise               = 40,
   PartialBand                = 45,
   Pseudorandom_AM            = 50,
   PulseNoise                 = 55,
   QuasiNoise_Pseudorandom    = 60,
   RangeBinMasking            = 65,
   RepeaterNoise              = 70,
   SpotNoise                  = 75,
   SweptNoise                 = 80,
   VelocityBinMasking         = 85
};
inline bool CategoryIsValid(DisEnum8 cat)
{
   return (cat <= VelocityBinMasking) && (cat % 5) == 0;
}
const std::string& CategoryToString(DisEnum8 cat);
namespace barrage
{
enum SubcategoryTypes
{
   Click       = 5,
   SourceNoise = 10
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= SourceNoise) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
} // namespace barrage
namespace rbm
{
enum SubcategoryTypes
{
   RangeBinMaskingWithVelocityBinMasking = 5
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= RangeBinMaskingWithVelocityBinMasking) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
} // namespace rbm
namespace repeater
{
enum SubcategoryTypes
{
   NarrowBandRepeaterNoise = 5,
   WideBandRepeaterNoise   = 10
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= WideBandRepeaterNoise) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
} // namespace repeater
namespace spot
{
enum SubcategoryTypes
{
   AutomaticSpotNoise = 5,
   BlinkingSpotNoise  = 10,
   BurstSpotNoise     = 15,
   DopplerSpotNoise   = 20,
   SkirtFrequency     = 25
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= SkirtFrequency) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
} // namespace spot
namespace swept
{
enum SubcategoryTypes
{
   FrequencySwept = 5,
   SweptAM        = 10
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= SweptAM) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
} // namespace swept
} // namespace noise

namespace deception
{
enum CategoryTypes
{
   Analyzer                         = 5,
   Angle                            = 10,
   AngleAndGateStealer              = 15,
   AngleAndFalseTarget              = 20,
   AngleAndRandomRangePrograms      = 25,
   AngleAndVelocity                 = 30,
   AutomaticGainControl_AGC         = 35,
   AGC_AndGateStealer               = 40,
   Colinear                         = 45,
   ConstantFalseAlarmRate           = 50,
   DoubleCross                      = 55,
   DownLink                         = 60,
   FalseTarget                      = 65,
   FigureEight                      = 70,
   GateStealer                      = 75,
   GateStealerAndRepeater           = 80,
   GlintEnhance                     = 85,
   ImageFrequency                   = 90,
   JitteredPulseRepetitionFrequency = 95,
   JitteredPulseWidth               = 100,
   PseudorandomNoise                = 105,
   Pulse                            = 110,
   PulseCompressionDeception        = 115,
   RandomRangePrograms_RANRAP       = 120,
   Refraction                       = 125,
   Repeater                         = 130,
   Scintillation                    = 135,
   Serrodyne                        = 140,
   Velocity                         = 145
};
inline bool CategoryIsValid(DisEnum8 cat)
{
   return (cat <= Velocity) && (cat % 5) == 0;
}
const std::string& CategoryToString(DisEnum8 cat);
namespace angle
{
enum SubcategoryTypes
{
   AngleGeteWalkOff  = 5,
   CooperativeAngle  = 10,
   CrossEye          = 15,
   CrossPolarization = 20,
   Delta             = 25,
   InverseGain       = 30,
   SeaBounced        = 35,
   SweptSquareWave   = 40,
   TerrainBounce     = 45
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= TerrainBounce) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
} // namespace angle
namespace angle_and_gate
{
enum SubcategoryTypes
{
   CrossPolarizationAndRangeGatePullOff    = 5,
   CrossPolarizationAndVelocityGatePullOff = 10,
   CrossPolarizationRGPO_and_VGPO          = 15,
   InverseGain_and_RPGO                    = 20,
   InverseGain_and_RGPO_and_VGPO           = 25,
   InverseGain_and_VGPO                    = 30,
   RGPO_and_SSW                            = 35,
   SSW_and_VGPO                            = 40
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= SSW_and_VGPO) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
} // namespace angle_and_gate
namespace angle_and_false_target
{
enum SubcategoryTypes
{
   AngleAndVelocityFalseTargets    = 5,
   RangeFalseTargetsAndInverseGain = 10,
   RFT_and_VFT                     = 15
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= RFT_and_VFT) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
namespace vft
{
enum SpecificTypes
{
   InverseGainAndVFT = 5,
   SSW_and_VFT       = 10
};
inline bool SpecificIsValid(DisEnum8 spec)
{
   return (spec < SSW_and_VFT) && (spec % 5) == 0;
}
const std::string& SpecificToString(DisEnum8 spec);
} // namespace vft
} // namespace angle_and_false_target
namespace angle_and_ranrap
{
enum SubcategoryTypes
{
   RANRAP_and_SSW = 5
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= RANRAP_and_SSW) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
} // namespace angle_and_ranrap
namespace angle_and_velocity
{
enum SubcategoryTypes
{
   InverseGainAndVBM = 5,
   SSW_and_VBM       = 10
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= SSW_and_VBM) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
} // namespace angle_and_velocity
namespace agc_and_gate
{
enum SubcategoryTypes
{
   AGC_and_RGPO          = 5,
   AGC_and_VGPO          = 10,
   AGC_and_RGPO_and_VGPO = 15
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= AGC_and_RGPO_and_VGPO) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
} // namespace agc_and_gate
namespace false_target
{
enum SubcategoryTypes
{
   CoherentFalseTargets = 5,
   FalseDopplerTarget   = 10,
   MultipleFalseTargets = 15,
   RangeFalseTargets    = 20,
   Transponder          = 25,
   VelocityFalseTargets = 30
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= VelocityFalseTargets) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
} // namespace false_target
namespace gate
{
enum SubcategoryTypes
{
   ChirpGateStealer = 5,
   RangeGatePullOff = 10,
   RGPO_and_VGPO    = 15,
   VGPO             = 20
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= VGPO) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
} // namespace gate
namespace gate_and_repeater
{
enum SubcategoryTypes
{
   RepeaterSweptAmplitudeModulationAndVGPO = 5
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= RepeaterSweptAmplitudeModulationAndVGPO) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
} // namespace gate_and_repeater
namespace repeater
{
enum SubcategoryTypes
{
   ContinuousWaveRepeater              = 5,
   RepeaterNoise                       = 10,
   MultipleFrequencyRepeater           = 15,
   NarrowBandRepeaterNoise             = 20,
   RandomDoppler                       = 25,
   RepeaterDigitalRadioFrequencyMemory = 30,
   RepeaterSweptAmplitudeModulation    = 35
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= RepeaterSweptAmplitudeModulation) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
} // namespace repeater
} // namespace deception
namespace deception_and_noise
{
enum CategoryTypes
{
   AngleAndNoise                     = 5,
   AngleAndFM_ByNoise                = 10,
   AngleAndPseudorandomAM            = 15,
   AngleAndSpot                      = 20,
   GateStealerAndNoise               = 25,
   FalseTargetDeceptionAndSweptNoise = 30
};
inline bool CategoryIsValid(DisEnum8 cat)
{
   return (cat <= FalseTargetDeceptionAndSweptNoise) && (cat % 5) == 0;
}
const std::string& CategoryToString(DisEnum8 cat);
namespace angle_and_noise
{
enum SubcategoryTypes
{
   AngleAndBarrage = 5
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= AngleAndBarrage) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
namespace barrage
{
enum SpecificTypes
{
   BarrageAndInverseGain = 5,
   BarrageAndSSW         = 10
};
inline bool SpecificIsValid(DisEnum8 spec)
{
   return (spec <= BarrageAndSSW) && (spec % 5) == 0;
}
const std::string& SpecificToString(DisEnum8 spec);
} // namespace barrage
} // namespace angle_and_noise
namespace angle_and_fm
{
enum SubcategoryTypes
{
   FM_byNoiseAndInverseGain = 5,
   FM_byNoiseAndSSW         = 10
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= FM_byNoiseAndSSW) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
} // namespace angle_and_fm
namespace angle_and_am
{
enum SubcategoryTypes
{
   InverseGainAndPseudorandomAM = 5,
   PseudorandomAM_and_SSW       = 10
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= PseudorandomAM_and_SSW) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
} // namespace angle_and_am
namespace angle_and_spot
{
enum SubcategoryTypes
{
   InverseGainAndLowLevelNoise = 5,
   InverseGainAndSpotNoise     = 10,
   SpotAndSSW                  = 15
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= SpotAndSSW) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
} // namespace angle_and_spot
namespace gate_and_noise
{
enum SubcategoryTypes
{
   NoiseAndRGPO = 5,
   NoiseAndVGPO = 10
};
inline bool SubcategoryIsValid(DisEnum8 scat)
{
   return (scat <= NoiseAndVGPO) && (scat % 5) == 0;
}
const std::string& SubcategoryToString(DisEnum8 scat);
namespace rgpo
{
enum SpecificTypes
{
   LowLevelNoiseAndRGPO = 5
};
inline bool SpecificIsValid(DisEnum8 spec)
{
   return (spec <= LowLevelNoiseAndRGPO) && (spec % 5) == 0;
}
const std::string& SpecificToString(DisEnum8 spec);
} // namespace rgpo
namespace vgpo
{
enum SpecificTypes
{
   LowLevelNoiseAndVGPO = 5
};
inline bool SpecificIsValid(DisEnum8 spec)
{
   return (spec <= LowLevelNoiseAndVGPO) && (spec % 5) == 0;
}
const std::string& SpecificToString(DisEnum8 spec);
} // namespace vgpo
} // namespace gate_and_noise
} // namespace deception_and_noise
namespace special
{
enum CategoryTypes
{
   SuperJam = 1
};
inline bool CategoryIsValid(DisEnum8 cat)
{
   return (cat <= SuperJam);
}
const std::string& CategoryToString(DisEnum8 cat);
} // namespace special

//! Helper method to validate enumerations fo a DisJammingTechniqueRecord
bool IsValid(const DisJammingTechniqueRecord& jtr);
//! Helper method to convert a DisJammingTechniqueRecord to a readable
//! string
/*!
 *  \return A string in the format "K:C:SC:SP [kind|cat|scat|spec]"
 *          where:
 *          K  - Enumerated value of the Kind
 *          C  - Enumerated value of the Category
 *          SC - Enumerated value of the Subcategory
 *          SP - Enumerated value of the Specific
 *
 *          kind - string value of the Kind
 *          cat  - string value of the Category
 *          scat - string value of the Subcategory
 *          spec - string value of the Specific
 */
std::string ToString(const DisJammingTechniqueRecord& jtr);
} // namespace jamming_technique
} // namespace DisEnum

#endif // DisJammingTechniqueEnums_hpp
