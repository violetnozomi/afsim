# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2003-2015 The Boeing Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# ****************************************************************************
# Updated by Infoscitex, a DCS Company.
# ****************************************************************************

# UtUnits.py -- Generates code for units utilities
#  This script udpates certain regions of c++ code in UtUnitTypes.hpp, UtUnits.hpp, UtUnits.cpp
# usage:  c:/util/source$>  python UtUnits.py
import math
import sys

# --- REQUIRES Python 3 ---
assert(sys.version_info > (3,0,0))

def backup_file(fileName):
   import os
   txt = open(fileName, 'r').read()
   i = 1
   ext = '.bak1'
   while os.path.exists(fileName + ext):
      i += 1
      ext = '.bak' + str(i)
   open(fileName + ext,'w').write(txt)
def subst_range(text, marker, insert):
   beginMark = '\n//' + marker + '{\n'
   endMark = '\n//' + marker + '}\n'
   try:
      before, after = text.split(beginMark)
   except:
      print("Failed to find begin marker:")
      print(beginMark)
      raise
   try:
      middle, after = after.split(endMark)
   except:
      print("Failed to find end marker:")
      print(endMark)
      raise
   return before + beginMark + insert + endMark + after
def update_file(name, regions):
   source = open(name, 'r').read()
   sourceNew = source
   for k,v in regions.items():
      sourceNew = subst_range(sourceNew, k, v)
   if sourceNew != source:
      backup_file(name)
      open(name, 'w').write(sourceNew)
   else:
      print("No changes to ", name)

# Notes:
# The first unit for each unit type is the default (standard) unit
# Each row defines a unit
#    name multiplier aliases...
#    - name is used to generate the unit's enumeration
#    - multiplier is the multiplier used to arrive at the unit from the standard unit
#    - aliases is a list of strings that may be used to input the unit
#      The first alias is always the preferred representation (for writing units back)
#  For compound units, the multiplier should be CU -- in which case the multiplier is computed
#  This is required so that compound unit enumerations can match correctly
#    'default:' prefixing a unit name allows changing the default unit representation when doing a compound unit
#  For units which are not multiples of the standard unit, '0' is used.  The conversion is then hand-written.
units = '''
BEGIN Length cLENGTH
   meters 1.0 meters meter m
   kilometers 1.0E3 kilometers kilometer km
   megameters 1.0E6 megameters megameter
   feet 0.3048 feet foot ft
   kilofeet 304.8 kfeet kft
   miles 1609.344 miles mile mi
   nautical_miles 1852.0 nm nmi
   centimeters 1.0E-2 centimeters centimeter cm
   millimeters 1.0E-3 millimeters millimiter mm
   micrometers 1.0E-6 micrometers micrometer um microns micron
   nanometers 1.0E-9 nanometers nanometer
   angstroms 1.0E-10 angstroms angstrom
   inches (0.3048/12.0) inches inch in
   astronomical_unit 1.496E+11 au ua
BEGIN Time cTIME
   seconds 1.0 seconds second secs sec s
   minutes 60.0 minutes minute mins min m
   hours (60.0*60.0) hours hour hrs hr h
   milliseconds 1.0E-3 milliseconds millisecond msecs msec ms
   microseconds 1.0E-6 microseconds microsecond usecs usec us
   nanoseconds 1.0E-9 nanoseconds nanosecond nsecs nsec ns
   days (60.0*60.0*24.0) days day
BEGIN Speed cSPEED Length/Time
   meters_per_second CU m/s
   kilometers_per_hour CU km/h kmh
   feet_per_second CU ft/s fps
   feet_per_minute CU ft/m fpm
   miles_per_hour CU mi/h mph
   knots CU nm/h knots kts
BEGIN DataSize cDATA_SIZE
   bits 1.0 bits bit
   kbits 1.0E3 kbits kbit
   mbits 1.0E6 mbits mbit
   gbits 1.0E9 gbits gbit
   bytes 8.0 bytes byte
   kbytes 8.0E3 kbytes kbyte
   mbytes 8.0E6 mbytes mbyte
   gbytes 8.0E9 gbytes gbyte
BEGIN Power cPOWER
   watts 1.0 watts w
   kilowatts 1.0E3 kilowatts kw
   megawatts 1.0E6 megawatts mw
   gigawatts 1.0E9 gigawatts gw
   milliwatts 1.0E-3 milliwatts
   microwatts 1.0E-6 microwatts
BEGIN PowerDB cPOWER_DB
   watts 1.0 watts w
   kilowatts 1.0E3 kilowatts kw
   megawatts 1.0E6 megawatts mw
   gigawatts 1.0E9 gigawatts gw
   milliwatts 1.0E-3 milliwatts
   microwatts 1.0E-6 microwatts
   dbw 0 dbw
   dbm 0 dbm
BEGIN SolidAngle cSOLID_ANGLE
   steradians 1.0 steradians steradian sr
BEGIN Mass cMASS
   kilograms 1.0 kilograms kg kilo kilogram
   grams 1.0E-3 grams gram g
   pounds (1.0/2.2046226) pounds pound lbs lbm lb
   klb (1.0E3/2.2046226) klb
   tons (2.0E3/2.2046226) tons ton
   tonnes (2204.6226/2.2046226) tonnes tonne
   slugs 14.5939029 slugs slug
BEGIN Force cFORCE
   newtons 1.0 newtons newton nt
   kgf 9.80665 kgf
   lbf (1.0/0.22480894) lbf lbsf
BEGIN Torque cTORQUE Force*Length
   newton_meters CU newtons*meters
BEGIN Area cAREA Length^2
   meters2 CU m2
   feet2 CU ft2
   inches2 CU in2
BEGIN AreaDB cAREA_DB Length^2
   meters2 CU m2
   dbsm 0 dbsm
BEGIN Volume cVOLUME Length^3
   meters3 CU m3 meters^3
BEGIN Time2 cTIME2 Time^2
   seconds2 CU s2 seconds^2
BEGIN Acceleration cACCELERATION Length/Time2
   meters_per_second2 CU m/s2
   feet_per_second2 CU ft/s2
   g 9.80665 g
BEGIN Frequency cFREQUENCY
   hz 1.0 hz
   khz 1.0E3 khz
   mhz 1.0E6 mhz
   ghz 1.0E9 ghz
BEGIN Angle cANGLE
   radians 1.0 radians radian rad
   degrees 0.01745329251994329576923 degrees degree deg
   mils 6.283185307179586476920/6400.0 mils mil
   arcseconds 4.84813681109535993589914E-6 arcseconds arcsecond
BEGIN AngularRate cANGULAR_RATE Angle/Time
   radians_per_second CU rad/s
   degrees_per_second CU deg/s
   rpm 6.283185307179586476920/60.0 rpm
BEGIN AngularAcceleration cANGULAR_ACCELERATION Angle/Time2
   radians_per_second2 CU rad/s2
   degrees_per_second2 CU deg/s2
BEGIN DataRate cDATA_RATE DataSize/Time
   bit_per_second CU bit/s
BEGIN MassDensity cMASS_DENSITY Mass/Volume
   kilograms_per_meter3 CU kg/m3
BEGIN MassTransfer cMASS_TRANSFER Mass/Time
   kilograms_per_second CU kg/s
BEGIN Energy cENERGY
   joules 1.0 joules j
   kilojoules 1.0E3 kilojoules kj
BEGIN Fluence cFLUENCE Energy/Area
   joules_per_meter2 CU j/m2
BEGIN Irradiance cIRRADIANCE Power/Area
   watts_per_meter2 CU w/m2
BEGIN SpectralIrradiance cSPECTRAL_IRRADIANCE Irradiance/Length
   watts_per_meter2_per_meter CU w/m2/m
BEGIN Radiance cRADIANCE Irradiance/SolidAngle
   watts_per_meter2_per_steradian CU w/m2/sr
BEGIN SpectralRadiance cSPECTRAL_RADIANCE Radiance/Length
   watts_per_meter2_per_steradian_per_meter CU w/m2/sr/m
BEGIN Ratio cRATIO
   absolute 1.0 absolute
   db 0 db
BEGIN NoisePressure cNOISE_PRESSURE
   absolute 1.0 absolute
   db_20upa 0 db_20upa
BEGIN Pressure cPRESSURE
   pascal 1.0 pascal pa
   kilopascals 1.0E3 kilopascals kilopascal kpa
   micropascals 1.0E-6 micropascals micropascal upa
   nanopascals 1.0E-9 nanopascal nanopascals
   psi 6894.75729 psi
   psf 6894.75729/144.0 psf
   dbpa 0 dbpa
   dbupa 0 dbupa
BEGIN Temperature cTEMPERATURE
   kelvin 1.0 kelvin k
   celsius 0 celsius c
   fahrenheit 0 fahrenheit f
BEGIN SpecificRange cSPECIFIC_RANGE
   meters_per_kilogram 1.0 m/kg
   miles_per_pound 1609.344*2.2046226 mi/lb
   miles_per_kilopound 1609.344*2.2046226/1000.0 mi/klb
   nautical_miles_per_pound 1852.0*2.2046226 nmi/lb
   nautical_miles_per_kilopound 1852.0*2.2046226/1000.0 nmi/klb
BEGIN AngularInertia cANGULAR_INERTIA Mass*Area
   kilograms_meters2 1.0 kg*m2
   slug_feet2 1.355817961893 slug*ft2
   lbm_inches2 0.0002926396534292 lbm*in2
BEGIN Voltage cVOLTAGE
   volts       1.0    volts      volt v
   millivolts  1.0E-3 millivolts millivolt
   microvolts  1.0E-6 microvolts microvolt
   kilovolts   1.0E3  kilovolts  kilovolt
   megavolts   1.0E6  megavolts  megavolt
BEGIN Current cCURRENT
   amps        1.0    amps      amp      amperes ampere
   milliamps   1.0E-3 milliamps milliamp ma
   microamps   1.0E-6 microamps microamp ua
   nanoamps    1.0E-9 nanoamps  nanoamp  na
BEGIN CurrentDensity cCURRENT_DENSITY Current/Area
   amps_per_meter2 CU amps/m2
BEGIN Resistance cRESISTANCE
   ohms      1.0    ohms      ohm
   microohms 1.0E-6 microohms microohm
   milliohms 1.0E-3 milliohms milliohm
   kiloohms  1.0E3  kiloohms  kiloohm  kilohms  kilohm
   megaohms  1.0E6  megaohms  megaohm  megohms  megohm
   gigaohms  1.0E9  gigaohms  gigaohm  gigohms  gigohm
BEGIN Capacitance cCAPACITANCE
   farads      1.0     farads      farad
   millifarads 1.0E-3  millifarads millifarad
   microfarads 1.0E-6  microfarads microfarad
   nanofarads  1.0E-9  nanofarads  nanofarad
   picofarads  1.0E-12 picofarads  picofarad
   femtofarads 1.0E-15 femtofarads femtofarad
BEGIN Impulse cIMPULSE Force*Time
   newton_seconds 1.0 newtons*seconds
BEGIN Responsivity cRESPONSIVITY Current/Power
   amps_per_watt 1.0 amp/watt
BEGIN Stop
'''

#  Note: This variable holds the names of any units that require a negative enum for non compound-units
#  The negative enum is used to avoid conflict with BaseUnit enum values in the AFSIM code
negativeEnumUnits = ["AreaDB"]  #cAREA_DB

allUnits = {}
class Unit:
   def __init__(self):
      self.name, self.mult, self.aliases, self.compound, self.compoundOp, self.kind, self.enum, self.default = [None] * 8
   def __repr__(self):
      return (self.name, self.mult, self.aliases, self.compound, self.compoundOp, self.kind, self.enum).__repr__()
   def unitType(self):
      return allUnits[self.kind]
   def get_default_name(self):
      if self.default is None: return self.aliases[0]
      return self.default
class UnitType:
   def __init__(self):
      self.name,self.compound, self.units, self.resolved, self.bits, self.shift = [None] * 6
      self.dim = 1
   def __repr__(self):
      return (self.name, self.compound, self.units, self.bits, self.shift).__repr__()
   def className(self):
      return 'UtUnit' + self.name

def namedstruct(name, attrs):
   s = object()
   for attr in attrs:
      setattr(s, attr, None)
   s.__new__ = lambda x: object(s)
compound = None
unitType = None
allUnitsList = []
unitId = 0
for line in units.splitlines():
   line = line.strip()
   if line == "": continue
   if line.startswith("BEGIN "):
      if unitType:
         unitType.enum = unitId
         unitId += 1
         allUnits[unitType.name] = unitType
         allUnitsList += [unitType]
      params = line.split()
      unitType = UnitType()
      unitType.name = params[1]
      if unitType.name == "Stop": break
      unitType.enumName = params[2]
      if len(params) > 3:
         if '/' in params[3]:
            unitType.compound = params[3].split('/')
            unitType.compoundOp = '/'
         if '*' in params[3]:
            unitType.compound = params[3].split('*')
            unitType.compoundOp = '*'
         if '^' in params[3]:
            unitType.compound, unitType.dim = params[3].split('^')
            unitType.compound = [unitType.compound]
            unitType.dim = int(unitType.dim)
      unitType.units = []
   else:
      u = Unit()
      parts = line.split()
      u.name, u.mult = parts[:2]
      u.name = 'c' + u.name.upper()
      u.aliases = parts[2:]
      for ai, a in enumerate(u.aliases):
         if a.startswith('default:'):
            u.aliases[ai] = a[8:]
            u.default = u.aliases[ai]
      if u.mult == 'CU':
         if hasattr(unitType, 'compoundOp'):
             op = unitType.compoundOp
             if op in u.aliases[0]:
                 u.compound = u.aliases[0].rsplit(op,1)
                 u.compoundOp = unitType.compoundOp
         elif '^' in u.aliases[0]:
            u.compound = [u.aliases[0].split('^')[0]]
         elif u.aliases[0][-1] in '23':
            u.compound = [u.aliases[0][:-1]]
         else:
            raise Exception("No delimiter")
      u.kind = unitType.name
      unitType.units.append(u)
classSkeleton = '''
struct UT_EXPORT $CLASS_NAME
{
   enum BaseUnit
   {
$ENUM_LIST
   };
   static constexpr int                      cSTANDARD_UNIT_ID = $STD_UNIT_ID;
   static constexpr int                      cBASE_UNIT_COUNT  = $BU_COUNT;
   static constexpr int                      cUSED_BITS        = $BITS;
   static constexpr int                      cUNIT_TYPE_ID     = $UNIT_TYPE_ID;
   static constexpr int                      cLAST_SIMPLE_UNIT = $LAST_SIMPLE_UNIT;
   static constexpr bool                     cIS_COMPOUND_UNIT = $IS_COMPOUND;
   static constexpr bool                     cIS_MULTIDIM      = $IS_MULTIDIM;
$COMPOUND_TYPES
   static double                             ConvertToStandard(const double aValue, const int aUnit)$CONVERT_TO
   static double                             ConvertFromStandard(const double aValue, const int aUnit)$CONVERT_FROM
   static std::string                        FindUnitName(int aUnitId)                                 { return UnitFunctions::FindUnitName(aUnitId); }
   static int                                ReadUnit(const std::string& aUnitName)                    { return UnitFunctions::ReadUnit(aUnitName); }
   static bool                               IsUnitValid(int aUnitId)                                  { return UnitFunctions::IsUnitValid(aUnitId); }
   static bool                               IsUnitValid(const std::string& aName)                     { return UnitFunctions::IsUnitValid(aName); }
   static const double                       mBaseUnitMultiplier[cLAST_SIMPLE_UNIT + 2];
   static const std::map<std::string, int>   mUnitStrings;
   static const std::map<int, std::string>   mUnitToString;
};
'''
definitionSkeleton = '''
const double $CLASS_NAME::mBaseUnitMultiplier[$CLASS_NAME::cLAST_SIMPLE_UNIT + 2] =
{
$UNIT_MULTIPLIERS 0
};
const std::map<std::string, int> $CLASS_NAME_UnitStrings()
{
   std::map<std::string, int> sm;
$UNIT_STRING_INIT
   return sm;
}
const std::map<std::string, int> $CLASS_NAME::mUnitStrings = $CLASS_NAME_UnitStrings();
const std::map<int, std::string> $CLASS_NAME_UnitToStrings()
{
   std::map<int, std::string> sm;
$UNIT_TO_STRING_INIT
   return sm;
}
const std::map<int, std::string> $CLASS_NAME::mUnitToString = $CLASS_NAME_UnitToStrings();

'''
unitaryClassSkeleton = '''
class UT_EXPORT Ut$UNIT_NAMEValue : public UtUnitaryValue<$CLASS_NAME>
{
   public:
      Ut$UNIT_NAMEValue() : UtUnitaryValue<$CLASS_NAME>() {}
      Ut$UNIT_NAMEValue(double aValue) : UtUnitaryValue<$CLASS_NAME>(aValue) {}
      Ut$UNIT_NAMEValue(double aValue, int aUnit) : UtUnitaryValue<$CLASS_NAME>() { Set(aValue, aUnit); }
      Ut$UNIT_NAMEValue(double aValue, const std::string& aUnit) : UtUnitaryValue<$CLASS_NAME>() { Set(aValue, aUnit); }
};
'''
def find_unit(unitlist, string):
   for u in unitlist:
      if string in u.aliases:
         return u
   print("NO UNIT: ", string)
   return None
def get_compound_types(kind):
   cts = [allUnits[un] for un in allUnits[kind].compound]
   if not all(cts):
      raise Exception("Unit does not exist")
   return cts
def get_compound(kind,units):
   cTypes = get_compound_types(kind)
   return [find_unit(ct.units, units[i]) for i,ct in enumerate(cTypes)]
def get_mult(u):
   if u.mult == 'CU':
      cus = get_compound(u.kind, u.compound)
      cuMul = [get_mult(cu) for cu in cus]
      if len(cuMul) == 2:
         m1,m2 = cuMul
         if u.compoundOp == '/':
            return m1 / m2
         else:
            return m1 * m2
      else:
         cu = cuMul[0]
         mul = cu
         dim = allUnits[u.kind].dim
         return pow(mul, dim)
   else: return u.multVal
def get_compound_enum(u):
   u1, u2 = get_compound(u.kind, u.compound)
   e1, e2 = u1.enum, u2.enum

def find_multi_dim_alias_enum(u, cu):
   for un in cu.aliases:
      if un in u.get_default_name():
         return cu.enum
   # It should never get here
   return u.enum

def resolve_unit_enums(name):
   unitType = allUnits[name]
   if unitType.resolved: return
   if unitType.compound:
      [resolve_unit_enums(cu) for cu in unitType.compound]
   enumVal = 0
   if unitType.compound:
      cuts = get_compound_types(name)
      unitType.bits = sum([us.bits for us in cuts])
   unitType.shift = 0
   for u in unitType.units:
      if u.mult != 'CU':
         if enumVal == 0 and unitType.compound and len(unitType.compound) == 2:
            enumVal = 1
         u.enum = enumVal
         try:
            u.multVal = float(u.mult)
            u.multStr = u.mult
         except:
            u.multVal = float(eval(u.mult))
            u.multStr = u.mult
         # Don't use '0' when doing a compound type, or we can't tell the difference
         enumVal += 1
         #if enumVal == 1 and unitType.compound: enumVal += 1         # reserve 1 for compound flag
   unitType.lastSimpleUnit = max(0, (enumVal - 1))
   if unitType.compound and len(unitType.compound) == 2 and enumVal == 0:
      enumVal = 1
   unitType.shift = math.ceil(math.log(enumVal + 1, 2))
   for u in unitType.units:
      if u.mult == 'CU':
         cus = get_compound(u.kind, u.compound)
         if len(cus) == 2:
            u1, u2 = cus
            u.enum = (u1.enum << u2.unitType().bits << unitType.shift) | (u2.enum << unitType.shift)
            u.multVal = get_mult(u)
         else:
            cu = cus[0]
            u.multVal = get_mult(u)
            # Assign the unit the enumeration of the OneDimensionType it shadows.
            u.enum = find_multi_dim_alias_enum(u, cu)
   if unitType.compound:
      uts = get_compound_types(name)
      unitType.bits = sum([cu.bits for cu in uts]) + unitType.shift
   else:
      unitType.bits = unitType.shift
   #This case is hard-coded due to its uniqueness and to avoid conflict with BaseUnit enums.
   #We skip -1 because the enum is checked against -1 in code, in particular, the IsUnitValid function.
   if any (u == unitType.name for u in negativeEnumUnits):
      typeUnit = allUnits[unitType.name]
      if any(x.mult == 'CU' for x in typeUnit.units):
         for u in typeUnit.units:
            if u.mult != 'CU':
               u.enum = -1 * (u.enum + 2)
   unitType.resolved = True
def className(name):
   return 'UtUnit' + name

allDefinitionCode = ""
allClassCode = ""
unitaryClasses = ''
allUnitTypeInterfaces = ''
for unitType in allUnitsList:
   k = unitType.name
   resolve_unit_enums(k)
   classCode = classSkeleton
   enumCode = ""
   unitMultipliers = ""
   unitStringInit = ""
   unitToStringInit = ''
   stdUnitId = unitType.units[0].enum
   hasNonMultipliers = False
   for unit in unitType.units:
      if unit.multVal == 0:
         hasNonMultipliers = True
      enumCode += '      ' + unit.name + ' = ' + str(unit.enum) + ',\n'
      if not unit.compound:
         if unitType.compound and len(unitType.compound) == 2 and len(unitMultipliers) == 0:
            unitMultipliers += '   0, // unused\n'

         # 17 decimal digits is the most digits required to saturate a double-precision number
         if hasattr(unit, 'multStr'):
            unitMultipliers += '   {0}, // {1}\n'.format(unit.multStr, unit.name)
         else:
            unitMultipliers += '   {0:.17}, // {1}\n'.format(unit.multVal, unit.name)
      for s in unit.aliases:
         unitStringInit += '   sm["{0}"] = $CLASS_NAME::{1};\n'.format(s, unit.name)
      unitToStringInit += '   sm[{0}] = "{1}";\n'.format(unit.enum, unit.get_default_name())
   convertTo = ';'
   convertFrom = ';'
   if not hasNonMultipliers:
      convertTo = '   { return UnitFunctions::ConvertToStandard(aValue, aUnit); }'
      convertFrom = ' { return UnitFunctions::ConvertFromStandard(aValue, aUnit); }'
   compoundTypeStr = '   using UnitFunctions = UtUnitsDetail::UnitFunctionsT<$CLASS_NAME, cIS_COMPOUND_UNIT, cIS_MULTIDIM>;\n'
   if unitType.compound and len(unitType.compound) == 2:
      ct1, ct2 = get_compound_types(unitType.name)
      compoundTypeStr += '''
   using SubjectType = {0};
   using PredicateType = {1};
   static constexpr int  cSUBJECT_BITS      = SubjectType::cUSED_BITS;
   static constexpr int  cPREDICATE_BITS    = PredicateType::cUSED_BITS;
   static constexpr char cCOMPOUND_OPERATOR = '{2}';
'''.format(ct1.className(), ct2.className(), unitType.compoundOp)
   if unitType.compound and len(unitType.compound) == 1:
      ct = get_compound_types(unitType.name)[0]
      compoundTypeStr += '''   using OneDimensionType = {0};
   static constexpr int cDIM = {1};\n'''.format(ct.className(), unitType.dim)
   enumCode = enumCode[:-2]
   unitTypeInterfaces = '   mUnitTypes[$UNIT_ENUM_NAME] = ut::make_unique<UtUnitTypeT<$CLASS_NAME>>("$UNIT_CAPS_NAME");\n'
   def fix_string(s):
      s = s.replace('$UNIT_ENUM_NAME', unitType.enumName)
      s = s.replace('$CONVERT_TO', convertTo)
      s = s.replace('$CONVERT_FROM', convertFrom)
      s = s.replace('$ENUM_LIST', enumCode)
      s = s.replace('$UNIT_STRING_INIT', unitStringInit)
      s = s.replace('$UNIT_TO_STRING_INIT', unitToStringInit)
      s = s.replace('$UNIT_MULTIPLIERS', unitMultipliers)
      s = s.replace('$BU_COUNT', str(len([u for u in unitType.units if not u.compound])))
      s = s.replace('$BITS', str(unitType.bits))
      s = s.replace('$STD_UNIT_ID', str(stdUnitId))
      s = s.replace('$IS_COMPOUND', 'true' if unitType.compound and len(unitType.compound) == 2 else 'false')
      s = s.replace('$IS_MULTIDIM', 'true' if unitType.compound and len(unitType.compound) == 1 else 'false')
      s = s.replace('$COMPOUND_TYPES', compoundTypeStr)
      s = s.replace('$UNIT_NAME', unitType.name)
      s = s.replace('$UNIT_CAPS_NAME', unitType.enumName[1:])
      s = s.replace('$UNIT_TYPE_ID', str(unitType.enum))
      s = s.replace('$LAST_SIMPLE_UNIT', str(unitType.lastSimpleUnit))
      s = s.replace('$CLASS_NAME', unitType.className())
      return s
   allClassCode += fix_string(classSkeleton)
   allDefinitionCode += fix_string(definitionSkeleton)
   unitaryClasses += fix_string(unitaryClassSkeleton)
   allUnitTypeInterfaces += fix_string(unitTypeInterfaces)

unitTypeEnum = ''
for ut in allUnitsList:
   unitTypeEnum += '         ' + ut.enumName + ' = ' + str(ut.enum) + ',\n'
unitTypeEnum = unitTypeEnum[:-2]

# Back up source files and replace generated code
update_file('UtUnitTypes.hpp', {'UtUnitHeader': allClassCode, 'UnitValueClasses': unitaryClasses})
update_file('UtUnits.hpp', {'UtUnitTypeEnums': unitTypeEnum})
update_file('UtUnits.cpp', {'CreateUnitTypeInterfaces': allUnitTypeInterfaces, 'UtUnitSources': allDefinitionCode})
