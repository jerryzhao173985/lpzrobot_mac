# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## CRITICAL: macOS ARM64 Migration Progress Report

### ✅ Completed
1. **Build System Enhancement (Phase 1)**
   - Updated m4 configuration files for Homebrew paths
   - Added ARM64 detection to opende/configure.in
   - Fixed framework dependencies (Carbon → Cocoa)
   - Updated include paths for both MacPorts and Homebrew

2. **ODE Configuration (Phase 2)**
   - Fixed ARM64 system detection in odeconfig.h
   - Removed deprecated framework dependencies
   - Configuration script updated for modern macOS

3. **Qt5 Migration (Phase 3 - Partial)**
   - ✅ guilogger: Migrated from Qt3Support to Qt5
   - ✅ matrixviz: Updated from QGLWidget to QOpenGLWidget
   - ✅ configurator: Already uses Qt5

4. **C++ Modernization**
   - Fixed deprecated unary_function usage
   - Fixed GNU-specific iterator usage
   - Fixed namespace conflicts (bind, connect)
   - Resolved C++17 header conflicts

5. **Library Builds**
   - ✅ selforg: Successfully built (libselforg.a - 57MB)
   - ⏳ opende: Configuration in progress
   - ⏳ ode_robots: Waiting for ODE build

### 🚧 In Progress
- Building ODE physics engine with double precision
- Preparing ode_robots build

### 📋 TODO
- Complete ODE build
- Build ode_robots with OpenSceneGraph support  
- Test 3D simulations
- Verify all components work together

**PROJECT GOAL**: Migrate LPZRobots to compile and run correctly on macOS ARM64 (Apple Silicon M4) with the following requirements:
- Transition from Qt4/Qt3Support to Qt5 ✅ (GUI tools migrated)
- Maintain original Make/m4 build system (per user request)
- Adopt C++11/17 standards progressively
- Ensure compatibility with clang/clang++ on macOS
- Maintain all existing functionality
- Follow modern C++ best practices

**Migration Status**: Core library built successfully. Physics and visualization components in progress.
**IMPORTANT**: Preserve original build system architecture as much as possible.

## Project Overview

LPZRobots is a comprehensive C++ robotics simulation framework focused on self-organizing control algorithms. It uses ODE (Open Dynamics Engine) for physics simulation and OpenSceneGraph for visualization.

### Current State
- **Build System**: GNU Make (migrating to CMake)
- **C++ Standard**: C++11 (migrating to C++17)
- **GUI Framework**: Qt4 with Qt3Support (migrating to Qt5)
- **Platform**: Originally Linux, adding macOS ARM64 support
- **Dependencies**: ODE, OpenSceneGraph, Qt, GSL, readline

## Migration Priority Areas

### 1. Build System Migration (Make → CMake)
- Replace all Makefiles with CMakeLists.txt
- Use modern CMake (3.16+) features
- Support both Homebrew and manual builds
- Add proper dependency detection for ARM64

### 2. Qt4 → Qt5 Migration
- **guilogger**: Remove Qt3Support (Q3PtrList → QList)
- **matrixviz**: Migrate QGLWidget → QOpenGLWidget
- **configurator**: Update includes and add widgets module
- Update all signal/slot connections to new syntax

### 3. macOS ARM64 Compatibility
- Fix hardcoded Intel paths (/opt/local → /opt/homebrew)
- Add ARM64 detection in configure scripts
- Update ODE configuration for ARM64
- Use proper framework linking on macOS

### 4. C++17 Modernization
- Replace deprecated features
- Use std::optional, std::filesystem
- Update to modern STL containers
- Use structured bindings where appropriate

## Build Commands (Current System)

### Initial Setup and Full Build
```bash
make all            # Configure and build everything (utils, selforg, ode, ode_robots, ga_tools)
```

### Individual Component Builds
```bash
make selforg        # Build self-organizing controllers library
make ode_robots     # Build robot simulation library
make utils          # Build utility tools (guilogger, matrixviz)
make ga_tools       # Build genetic algorithms tools
```

### Building and Running Simulations
```bash
# Create new simulation from template
cd ode_robots/simulations
./createNewSimulation.sh template_sphererobot my_simulation

# Build simulation
cd ode_robots/simulations/my_simulation
make                # Debug build
make opt            # Optimized build

# Run simulation
./start             # Run debug version
./start -g          # Run with guilogger for parameter visualization
./start_opt         # Run optimized version
```

### Development Commands
```bash
make clean          # Remove object files
make clean-all      # Clean everything including libraries
make doc            # Generate Doxygen documentation
make tags           # Generate TAGS for code navigation
make todo           # Show TODOs in source code
```

## Architecture Overview

### Core Components

1. **Controllers** (`selforg/controller/`): Learning algorithms
   - Base class: `AbstractController`
   - Key implementations: DEP, Sox, Pimax, various neural network controllers
   - All controllers implement: `init()`, `step()`, `stepNoLearning()`

2. **Robots** (`ode_robots/robots/`): Physical robot models
   - Base class: `OdeRobot` (inherits from `AbstractRobot`)
   - Examples: Sphererobot3Masses, Nimm2/4, Schlange, Hexapod
   - Interface: `getSensors()`, `setMotors()`, physical primitive management

3. **Simulations** (`ode_robots/simulations/`): Experiment setups
   - Inherit from `Simulation` class
   - Implement `start()` to create robots, controllers, and environment
   - Handle keyboard commands via `command()`

### Key Design Patterns

1. **Agent Pattern**: Combines robot + controller + wiring
   ```
   Robot ← Wiring ← Controller
     ↑        ↑         ↑
     └────Agent─────────┘
   ```

2. **Wiring**: Transforms between robot sensors/motors and controller I/O
   - `One2OneWiring`: Direct connection
   - `DerivativeWiring`: Adds velocity information
   - `FeedbackWiring`: Adds motor feedback to sensors

3. **Handle Pattern**:
   - `OdeHandle`: Physics world access
   - `OsgHandle`: Graphics context
   - `GlobalData`: Shared simulation state

4. **Configuration System**:
   - All components inherit from `Configurable`
   - Runtime parameter adjustment
   - Automatic GUI integration

### Typical Simulation Structure

```cpp
class MySimulation : public Simulation {
    void start(const OdeHandle& odeHandle, const OsgHandle& osgHandle, GlobalData& global) {
        // 1. Create playground/environment
        Playground* playground = new Playground(odeHandle, osgHandle, ...);
        
        // 2. Create and configure robot
        Sphererobot3MassesConf conf = Sphererobot3Masses::getDefaultConf();
        OdeRobot* robot = new Sphererobot3Masses(odeHandle, osgHandle, conf, "MyRobot");
        robot->place(Pos(0,0,0.5));
        
        // 3. Create controller
        AbstractController* controller = new Sox();
        controller->setParam("epsC", 0.1);
        
        // 4. Create wiring
        AbstractWiring* wiring = new One2OneWiring(new ColorUniformNoise(0.1));
        
        // 5. Create agent and add to simulation
        OdeAgent* agent = new OdeAgent(global);
        agent->init(controller, robot, wiring);
        global.agents.push_back(agent);
        global.configs.push_back(controller);
    }
};
```

### Important Conventions

- **Matrix Operations**: Controllers use the custom matrix library (`selforg/matrix/`)
- **Sensor/Motor Arrays**: Always use `sensor` and `motor` types (doubles)
- **Timestep**: Default is 0.01s, configurable in simulation
- **Coordinate System**: Z-axis points up
- **Units**: Meters for distances, radians for angles

### Common Development Tasks

1. **Adding a New Robot**: 
   - Inherit from `OdeRobot`
   - Create primitives in constructor
   - Implement sensor/motor interfaces
   - See existing robots in `ode_robots/robots/` for examples

2. **Adding a New Controller**:
   - Inherit from `AbstractController`
   - Implement required virtual methods
   - Add to `selforg/controller/`
   - Include in `Makefile.conf`

3. **Creating a Simulation**:
   - Use `createNewSimulation.sh` script
   - Modify `main.cpp` to set up your experiment
   - Edit `Makefile.conf` to include needed files

### Debugging Tips

- Use `./start -g` to visualize controller parameters in real-time
- Enable logging with `./start -f` for post-analysis
- Use `./start -pause` to start simulation paused
- Check `~/.lpzrobots/` for log files and configurations

## Migration Guidelines

### CRITICAL: Preserve Original Build System

**The original build system is well-constructed and carefully designed for each component. Our migration strategy must:**

1. **Utilize the existing build system** as much as possible
2. **Make minimal changes** only where necessary for:
   - macOS ARM64 compatibility
   - Modern Ubuntu package compatibility
   - Qt5/C++17 requirements
3. **Document every change** with clear justification
4. **Test that original functionality** remains intact

### Build System Preservation Strategy

#### What to Keep:
- **Make-based structure**: The hierarchical make system works well
- **Configuration scripts**: m4-based configuration is sophisticated
- **Component separation**: Each component's build independence
- **Installation paths**: PREFIX-based installation system
- **Dependency detection**: Existing scripts for finding libraries

#### What Must Change:
1. **Platform Detection**:
   ```bash
   # Add to existing configure scripts
   arm64* | aarch64* )  # New ARM64 detection
     # Keep existing logic, add ARM64 flags
   ```

2. **Library Paths**:
   ```bash
   # Update m4 files to support both:
   MACINCLUDE="-I/opt/local/include -I/opt/homebrew/include"  # Add homebrew
   ```

3. **Qt Detection**:
   - Extend existing qt.m4 to find Qt5
   - Keep Qt4 detection for compatibility

4. **Compiler Flags**:
   - Add `-std=c++17` alongside existing flags
   - Keep optimization levels and warning flags

### When Making Changes

1. **Study the original first**: Understand why it was designed that way
2. **Check Migration Plan**: Consult MIGRATION_PLAN_MACOS_ARM64.md
3. **Minimal intervention**: Change only what breaks on ARM64/Ubuntu
4. **Preserve functionality**: Original behavior must remain intact
5. **Document rationale**: Every change needs clear justification
6. **Dual compatibility**: Support both old and new systems

### Specific Build System Strengths to Preserve

#### 1. **Sophisticated Configuration System**
The original uses m4 macros for configuration - this is powerful and should be extended, not replaced:
- `selforg-config.m4`: Provides flags for dependent projects
- `ode_robots-config.m4`: Manages complex OSG/ODE dependencies
- `acinclude.m4`: Custom autoconf macros

**Approach**: Add new macros for Qt5/ARM64 detection rather than rewriting

#### 2. **Modular Make Structure**
```make
# Original pattern to preserve:
all: selforg ode ode_robots utils ga_tools
selforg:
	$(MAKE) -C selforg
```
This allows independent component builds and is well-organized.

#### 3. **Intelligent Dependency Management**
The `*-config` scripts (e.g., `selforg-config --libs`) provide proper linking flags. Extend these for new dependencies rather than replacing.

### Ubuntu/Debian Compatibility Updates

#### Package Names (Modern Ubuntu 20.04+):
```bash
# Old packages (may not exist)
libqt4-dev libqt4-opengl qt4-qmake

# New packages to detect
libqt5-dev libqt5-opengl5-dev qtbase5-dev
```

#### Library Detection Updates:
```m4
# In qt.m4, add alongside Qt4 detection:
AC_PATH_PROG(QMAKE, qmake-qt5, no)
if test "$QMAKE" = no; then
  AC_PATH_PROG(QMAKE, qmake, no)  # Fallback
fi
```

### Key Migration Tasks

1. **For Qt Code**:
   - Replace Qt4 includes with Qt5 style
   - Remove Qt3Support usage
   - Add widgets module to all GUI apps
   - Update deprecated APIs
   - **Keep original signal/slot naming** where possible

2. **For Build Files**:
   - **Extend Makefiles**, don't replace them
   - Add Qt5 detection to existing m4 scripts
   - Update library paths for both MacPorts and Homebrew
   - Add ARM64 flags conditionally

3. **For Platform Code**:
   - Add platform detection, don't remove existing
   - Use `#ifdef __APPLE__` for macOS-specific code
   - Add `__arm64__` or `__aarch64__` checks
   - **Preserve existing Linux code paths**

### Testing Requirements

- All components must build with: `clang++ -std=c++17 -arch arm64`
- GUI tools must run natively without Rosetta 2
- Performance should match or exceed Intel builds
- No functionality regression from original Linux version

## Implementation Resources

### Migration Examples
The `migration_examples/` directory contains production-ready code for key migration tasks:

1. **CMake Templates**:
   - `selforg_CMakeLists.txt`: Modern CMake setup for selforg library
   - `platform_detection.cmake`: Cross-platform detection including ARM64

2. **Qt5 Migration**:
   - `guilogger_qt5_migration.cpp`: Complete Qt3Support removal examples
   - Shows Q3PtrList → QList, Q3ScrollView → QGraphicsView conversions

3. **C++17 Modernization**:
   - `controller_cpp17_modernization.cpp`: Modern AbstractController implementation
   - Demonstrates std::optional, structured bindings, RAII patterns

4. **Platform Integration**:
   - `complete_example_usage.cpp`: Integration example showing all components
   - Platform-specific compilation with proper feature detection

### Migration Workflow

1. **Start with Build System**: Create CMakeLists.txt using provided templates
2. **Update Dependencies**: Use platform_detection.cmake for proper ARM64 support
3. **Modernize Code**: Apply C++17 patterns from examples
4. **Test Incrementally**: Build and test each component before moving to next
5. **Document Changes**: Update component documentation as you migrate

## Component Architecture Details

### Component Overview and Dependencies

```
┌─────────────┐     ┌──────────────┐     ┌───────────────┐
│   selforg   │     │   opende     │     │  ga_tools     │
│ (Qt-free)   │     │  (ode-dbl)   │     │ (optional)    │
└─────┬───────┘     └──────┬───────┘     └───────┬───────┘
      │                     │                     │
      └──────────┬──────────┴─────────────────────┘
                 │
           ┌─────▼──────┐
           │ ode_robots │
           │   (OSG)    │
           └─────┬──────┘
                 │
    ┌────────────┼────────────┬──────────────┐
    │            │            │              │
┌───▼────┐  ┌───▼─────┐  ┌───▼────┐   ┌────▼─────┐
│guilogger│  │matrixviz│  │config- │   │simulations│
│ (Qt4)   │  │ (Qt4)   │  │urator  │   │(50+ demos)│
│         │  │         │  │(Qt4)   │   │          │
└─────────┘  └─────────┘  └────────┘   └──────────┘
```

### Component-Specific Details

#### 1. selforg (Core Library)
- **Status**: Qt-free, relatively easy migration
- **Key Features**: 
  - Matrix library (potential ARM64 NEON optimization)
  - Controller framework (AbstractController base)
  - Agent/Wiring system
  - Configurable/Inspectable interfaces
- **Platform Issues**:
  - Hardcoded /opt/local paths for macOS
  - Uses custom drand48r implementation for macOS
  - QuickMP threading (not Qt)
- **Migration Priority**: HIGH (foundation for everything)

#### 2. ode_robots (Simulation Framework)
- **Status**: Complex due to OSG dependency
- **Key Features**:
  - Heavy OpenSceneGraph integration
  - Custom LPZViewer extends osgViewer
  - Shadow systems (multiple techniques)
  - Threading options (-odethread, -osgthread)
- **Critical Dependencies**:
  - OpenSceneGraph 2.x/3.x
  - ODE physics (via opende)
  - OpenGL/GLUT
- **Migration Challenges**:
  - OSG may need replacement or significant updates
  - Shadow rendering system is OSG-specific
  - Resource loading (.osg mesh formats)

#### 3. opende (Physics Engine)
- **Status**: Critical dependency with platform issues
- **Key Issues**:
  - No ARM64 detection in configure.in
  - Uses deprecated Carbon/AGL frameworks on macOS
  - X86_64_SYSTEM macro doesn't recognize ARM64
- **Migration Requirements**:
  ```bash
  # Add to configure.in
  arm64* | aarch64* )
    cpu64=yes
    AC_DEFINE(ARM64_SYSTEM,1,[ARM64 system])
  ```
  - Replace Carbon with Cocoa framework
  - Update odeconfig.h for ARM64

#### 4. GUI Tools
- **guilogger**:
  - Uses Qt3Support (Q3PtrList)
  - Gnuplot integration via pipes
  - Platform-specific gnuplot commands
- **matrixviz**:
  - Uses QGLWidget (deprecated in Qt5)
  - Real-time OpenGL visualization
  - No Qt3Support dependency
- **configurator**:
  - Pure Qt4, no Qt3Support
  - Well-designed library
  - Ready for Qt5 migration

#### 5. ga_tools
- **Status**: Low priority, "not well maintained"
- **Features**: Genetic algorithm framework
- **Dependencies**: selforg only
- **Migration**: Simple, just needs build system updates

## Risk Assessment and Priority

### Critical Path (Must Migrate)
1. **Qt3Support removal** in guilogger
2. **ARM64 detection** in opende
3. **Deprecated macOS frameworks** in opende
4. **Build system** migration to CMake

### High Risk Components
1. **OpenSceneGraph dependency** - May need alternative 3D engine
2. **Shadow rendering system** - Tightly coupled to OSG
3. **Threading model** - Platform-specific considerations
4. **Matrix operations** - Performance critical

### Medium Risk
1. **Gnuplot integration** - Platform-specific paths
2. **Resource loading** - .osg format files
3. **Color schema system** - File path resolution

### Low Risk
1. **ga_tools** - Simple, isolated component
2. **configurator** - Already Qt4 compliant
3. **Controller algorithms** - Pure C++

## Platform-Specific Considerations

### macOS ARM64 Requirements
1. **Compiler Flags**:
   ```cmake
   if(APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch arm64")
     set(CMAKE_OSX_ARCHITECTURES "arm64")
   endif()
   ```

2. **Framework Updates**:
   - Carbon → Cocoa
   - AGL → Metal/OpenGL via NSOpenGLContext
   - QuickTime → AVFoundation (if used)

3. **Path Updates**:
   - /opt/local → /opt/homebrew
   - Check for Rosetta 2 translation

### Threading Considerations
- ODE thread: Physics runs separately (1-frame sensor delay)
- OSG thread: Graphics runs separately (recommended)
- QuickMP: May need std::thread migration
- Thread affinity: ARM64 big.LITTLE architecture

## Performance Optimization Opportunities

### 1. Matrix Library (selforg/matrix/)
- **Current**: Basic loops, memcpy/memset
- **Optimization**: ARM64 NEON SIMD
- **Alternative**: Apple Accelerate framework
```cpp
#ifdef __APPLE__
  #include <Accelerate/Accelerate.h>
  // Use vDSP for vector operations
  // Use BLAS for matrix multiplication
#endif
```

### 2. Collision Detection
- Spatial hashing for broad phase
- ARM64 cache-friendly data structures
- Parallel collision detection with GCD

### 3. Rendering Pipeline
- Metal backend for macOS (if replacing OSG)
- Instanced rendering for multiple robots
- Compute shaders for particle systems

## Testing Strategy

### Unit Testing
1. **Matrix operations**: Compare NEON vs scalar
2. **Controller algorithms**: Verify numerical stability
3. **Collision detection**: Edge cases

### Integration Testing
1. **Each robot type** with reference controller
2. **GUI tools** with sample data streams
3. **Threading modes** stability tests

### Performance Testing
1. **Benchmark suite**: Matrix operations, collision detection
2. **Frame rate targets**: 60 FPS minimum
3. **Memory usage**: Profile with Instruments

### Platform Testing
1. **Native ARM64**: No Rosetta 2
2. **Universal Binary**: Both architectures
3. **Cross-compilation**: Linux compatibility

## Command Line Options Reference

The simulator supports these runtime options (from README.md):
- `-g [interval]`: Enable guilogger (default interval 1)
- `-f [interval]`: Write logging file (default interval 5)
- `-m [interval]`: Use matrixviz (default interval 10)
- `-noshadow`: Disable shadows (helps with graphics compatibility)
- `-shadow [0-5]`: Shadow types (0=none, 5=ShadowMap default)
- `-odethread`: Run ODE in separate thread
- `-osgthread`: Run OSG in separate thread (recommended)
- `-threads N`: Thread count (defaults to CPU count)

## Incremental Migration Approach

### DO NOT Change:
1. **Directory structure** - Keep all paths and organization
2. **Make targets** - Preserve all existing targets
3. **Configuration options** - Keep all existing flags
4. **Installation logic** - PREFIX system works well
5. **Component dependencies** - Build order is carefully designed
6. **Script interfaces** - Keep *-config script APIs
7. **Default behaviors** - All defaults must remain the same

### Change ONLY When Necessary:
1. **Add ARM64 detection** - Extend, don't replace platform detection
2. **Add Qt5 support** - Keep Qt4 as fallback option
3. **Update paths** - Add new paths alongside old ones
4. **Fix deprecated APIs** - Only when they break compilation
5. **Add C++17 flag** - Keep existing C++ standard flags

### Validation at Each Step:
```bash
# After each change, verify:
make clean
make all
# Test on Linux to ensure no regression
# Test on macOS Intel if possible
# Test on macOS ARM64
```

### Example: Minimal Qt5 Detection Change
```m4
# In m4/qt.m4 - ADD don't replace:
AC_ARG_WITH(qt5,
  [  --with-qt5              use Qt5 instead of Qt4],
  [TRY_QT5=yes], [TRY_QT5=no])

if test "$TRY_QT5" = yes; then
  # Try Qt5 first
  AC_PATH_PROG(QMAKE, qmake-qt5, no)
fi

# Keep all existing Qt4 detection code unchanged
```

## Known Issues and Workarounds

### Graphics Issues
- If crash on startup: use `-noshadow`
- Shadow rendering removed from recent OSG versions
- Some .osg mesh files may need conversion

### Build Issues
- Configure scripts may fail on ARM64: manually set flags
- Qt3Support missing on newer systems: must migrate code
- OpenSceneGraph ARM64 builds may need patches

### Performance Issues
- Thread affinity on ARM64 needs tuning
- Memory bandwidth limitations on unified memory
- Power efficiency vs performance trade-offs

## Build System Analysis Notes

### Why the Original Design Works Well:

1. **Component Independence**: Each component can be built/installed separately
2. **Flexible Configuration**: m4 macros allow complex dependency detection
3. **Cross-Platform**: Already handles Linux/Mac differences elegantly
4. **User/System Install**: Supports both with automatic sudo detection
5. **Dependency Resolution**: *-config scripts handle complex linking

### Integration Points to Preserve:

1. **make all**: Builds everything in correct order
2. **make conf**: Reconfigures without losing settings
3. **Component makefiles**: Call sub-makefiles cleanly
4. **Install detection**: Automatic sudo when needed
5. **Uninstall support**: Clean removal of all components

The migration should enhance this system, not replace it. Think of it as adding ARM64/Qt5 support to an already excellent build system.