# Phased Array Chamber Test Automation

## Overview
This project provides an automated testing framework for characterizing phased array antennas in an anechoic chamber environment. The system coordinates hardware control, chamber equipment, and measurement processes to perform comprehensive antenna pattern measurements.

## Features
- Automated phased array antenna sweep testing
- Configurable frequency ranges (Ku-band and L-band support)
- Customizable sweep patterns for azimuth and elevation
- Power cycling and array initialization
- Real-time measurement data collection
- Integrated chamber control system

## Project Structure
```
phased_array_chamber_test/
├── phased_array_sweep_automation.py  # Main automation script
└── lib/                             # Supporting modules
    ├── array_controller.py          # Phased array control interface
    ├── chamber_controller.py        # Chamber equipment control
    ├── config.py                    # System configuration
    ├── hardware_interface.py        # Hardware communication
    ├── system_messages.py           # System command definitions
    └── __init__.py
```

## Configuration
Key system parameters can be configured in `lib/config.py`:
- Frequency settings (Modem and Ku-band frequencies)
- Sweep angle ranges (Theta and Phi)
- Turn table limits and positions
- Serial communication settings
- Test output paths

## Usage
1. Configure the system parameters in `config.py`
2. Run the main automation script:
```bash
python phased_array_sweep_automation.py
```

## Key Components

### Array Controller
- Manages phased array antenna control
- Handles power cycling
- Controls array pointing and attenuation
- Supports multiple array interfaces

### Chamber Controller
- Coordinates chamber equipment
- Manages sweep sequences
- Controls turn table positioning
- Handles measurement data collection

### Hardware Interface
- Provides low-level hardware communication
- Manages serial communication
- Implements command protocols

## Test Sequence
1. System initialization and power cycling
2. Array configuration for specified frequency
3. Automated sweep through configured angles
4. Data collection at each position
5. Results storage and processing

## Requirements
- Python 3.x
- Serial communication capability
- Compatible chamber equipment
- Phased array antenna system

## Notes
- Ensure proper hardware connections before running tests
- Verify chamber calibration before measurements
- Monitor system during long sweep sequences
- Check output data for consistency 