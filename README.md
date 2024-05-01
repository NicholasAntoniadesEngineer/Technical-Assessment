# Technical Assessment
This repo will be used to hold the assignment set out as part of the interview process for an embedded software role:


# Building the project and running the demo app


1. To build the project run ./build_project.sh

2. To run the demo app enter the build directory and run ./AppMain

# Open subject for reflection

Constraints for On-board Computer Architecture/Selection Phase:
1. Processor Compatibility: Choose processors that support a unified architecture (like ARM Cortex) to simplify BSP development across different on-board computers.
2. Power Consumption: Opt for processors with low power consumption to ensure longevity and reliability in space environments.
3. Environmental Resilience: The processors must withstand extreme temperatures and radiation levels characteristic of space.
4. Real-time Capabilities: Select processors with real-time performance capabilities to handle time-sensitive data from scientific instruments.

Architectural Approach for the BSP:
1. Modular Design: Develop the BSP modularly where each module (e.g., GPIO, UART, SPI) is separated and abstracted, allowing easy adaptation for different processors.
2. Hardware Abstraction Layer (HAL): Implement a HAL to abstract hardware specifics from the software, enabling portability across different hardware platforms.
3. Common API: Design a common API for all supported hardware features to ensure that higher-level software can operate irrespective of the underlying hardware.
4. Testing on Host: Use conditional compilation or a simulation environment to test the BSP on a Linux PC.

Development Sequencing/Prioritization:
1. Define Requirements: Start with gathering and defining detailed requirements for each subsystem of the BSP.
2. Hardware Abstraction Layer: Develop the HAL first to decouple the hardware specifics from higher-level logic.
3. Core Modules: Develop core modules like GPIO, and UART based on priority and dependency.
4. Integration Testing: As each module is developed, perform integration testing with simulated inputs.
5. Continuous Integration: Use CI/CD pipelines to ensure that changes are tested and validated continuously.

Adaptation Without IMUs:
1. Mock Modules: Develop mock modules for IMUs to allow software development and testing to proceed.
2. Simulation: Utilize software simulations to validate algorithms and other integrations.
3. Incremental Integration: Plan for an incremental integration phase where real IMUs will replace mock modules when available.
