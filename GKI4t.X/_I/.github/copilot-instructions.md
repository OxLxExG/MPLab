# Copilot Instructions: GKI4t Inclinometer Firmware

## Project Overview
This is AVR microcontroller firmware for an inclinometer/gravimeter sensor system (ADXL356 accelerometer) with temperature compensation and magnetometer support. The project combines field-calibrated sensor data with real-time processing and persistent storage on EEPROM.

**Key Metrics:** Temperature compensation at 3 points, 4-point matrix transformations, charge tracking, sensor offset calibration.

## Architecture

### Data Structure Hierarchy (`Data.h`)
The entire system revolves around `AllDataStruct_t` - a packed binary structure split into three memory regions:

- **WorkData_t (RAM/Working):** Real-time sensor readings
  - `InclW_t` - Processed incline data (accel, magnit, calculated angles: Gtf, Zenit, Azimut)
  - `Gk_t` - Gravity constant measurement
  - `charge_t` - Battery status tracking
  - Time stamp (frame counter, metr=WT)

- **RamData_t (RAM Buffer):** Historical frame buffer for logging
  - Same structure as WorkData_t with timestamp prefix

- **EepData_t (EEPROM):** Persistent calibration/config data
  - Temperature compensation matrices: `TrrTemp_t` (T[3] for 3 temp points)
  - Sensor transformations: `TrrInclin_t` (3x4 matrix per axis)
  - Calibration coefficients: `koeff_t` (k, d, d0 for each axis at each temp)
  - Battery capacity volume, error tracking, ADS131 ADC settings
  - DAC configuration for gravity constant

### Transformation System
Two key transformation types:
1. **Temperature Correction:** `TrrT_t` - Applies linear k/d coefficients per axis-temperature pair
2. **3x4 Matrix Transform:** `m3x4_t` - Inclination correction for both accel and magnetometer

Units system embedded in comments:
- Acceleration: mG (milligauss)
- Magnetism: nT (nanoTesla)
- Angles: grad (gradient/degrees)
- Time metric: WT (working time, frame counter)
- Gravity: μR (microradians)

## Critical Patterns

### Metadata & Configuration Comments
The code uses special inline comments for field metadata (non-standard, tool-generated):
```cpp
//-readonly        // Read-only in UI
//-eu=mG           // Unit display
//-digits=10       // Total digits for display
//-precision=1     // Decimal places
//-color=0xRRGGBB  // Display color in hex
//-name=Russian    // Cyrillic field names for UI
//-password=KEY    // EEPROM write protection
```
These comments drive both MetaData generation and field display in companion tools.

### EEPROM Layout & Access Control
EepData_t uses offset-based password protection:
- **Offset 0:** Battery change flag (`//-password=BAT`)
- **Offset 512:** Device state and charge (`//-password=Reset_BAT_enable`)
- **Offset 1024:** DAC gravity config (`//-password=DAC`)
- **Offset 2000:** Standard sensor transform (`//-password=STDM`)
- **Offset 3000:** Extended metrics (`//-password=EXM`)
- **Offset 4000:** ADS131 ADC registers (`//-password=rwelgrtgh`)

Do not reorder or modify offsets without coordinating with companion tools parsing `Data.h.xml`.

### Struct Packing Convention
All data structures use `__attribute__((packed))` for binary compatibility:
- Direct memory serialization for EEPROM/UART
- Careful field ordering (no implicit padding)
- 32-bit floats for calibration data (5-8 digit precision comments guide display)

## Developer Workflows

### Modifying Sensor Data Format
1. Edit `Data.h` struct definitions
2. Update field metadata comments (name, units, precision)
3. Run metadata generator (likely creates `MetaData.h` from `Data.h.xml`)
4. Validate EEPROM offsets in `eep_` structs haven't shifted

### Adding New Calibration Parameters
1. Insert into appropriate `TrrXxx_t` struct (temperature, matrix, or coefficient-based)
2. Set EEPROM offset via `//-from=NNNN` comment
3. Add password protection `//-password=KEY`
4. Update UI metadata comment strings (name, units, precision)

### Testing Transformation Math
- Matrix multiply references `m3x4_t` for inclination correction
- Coefficient application uses `koeff_t.k` (gain) and `d` (drift per °C)
- Temperature interpolation spans 3 points (`TEMPR_POINTS=3`)

## Integration Points

### UART Protocol
- Frame counter (`int32_t kadr` / `ramtime`) enables resync
- Metrics namespace in comments (GK1, INKLGK3, AU, WT) indicate message type routing
- Battery charge exported in working frame

### EEPROM Persistence
- `AllDataStruct_t` maps directly to EEPROM binary layout
- `eep_t` struct offset chain: 2000→3000→4000 determines read priority
- Clear separation: config in EepData_t, runtime state in WorkData_t

### ADC Configuration (ADS131)
Registers stored as read-only snapshot in `eep_ads131_t` (offset 4000):
- Synchronize register writes with stored backup
- Protects against accidental reconfiguration

## Files to Know

- [Data.h](Data.h) - Master data structure definitions (417 lines, heavily commented)
- [MetaData.h](MetaData.h) - Binary metadata array (tool-generated from Data.h.xml)
- [MetaData.h.xml](MetaData.h.xml) - Metadata template for code generation
- [Patern.h](Patern.h) - Template for MetaData.h generation

## Common Mistakes to Avoid

1. **Reordering packed struct fields** - breaks binary compatibility with stored EEPROM data
2. **Changing field width** (int to short, float precision) - corrupts offset alignment
3. **Omitting password protection** when adding EEPROM fields
4. **Forgetting field metadata comments** - breaks companion UI/display tools
5. **Hardcoding calibration values** - always use TrrXxx_t temperature/matrix approach

## Commands & Build Notes
Not discoverable from file inspection. Likely MPLAB IDE build - assume standard AVR toolchain (avr-gcc, avrdude).
