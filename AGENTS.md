# AGENTS.md - Coding Guidelines for PLUTO Simulations

## Project Overview

This repository contains astrophysical fluid dynamics simulations using the [PLUTO code](http://plutocode.ph.unito.it/) - a CFD framework for astrophysical plasma physics. The codebase is primarily C with PLUTO-specific conventions.

## Build Commands

### Initial Setup
```bash
# Navigate to a simulation directory
cd hd/<simulation_name>/

# Configure and build using PLUTO's Python script
python $PLUTO_DIR/setup.py

# Or with specific architecture
python $PLUTO_DIR/setup.py -arch=linux
```

### Running Simulations
```bash
# Serial execution
./pluto

# Parallel execution (MPI)
mpirun -np 4 ./pluto

# Batch submission (SLURM)
sbatch mpirun.sh
```

### Documentation Generation
```bash
# Generate PDF from Markdown+LaTeX README files
pandoc --verbose --from=markdown --output=<OUTPUT.PDF> <INPUT.MD> \
  --variable=geometry:"margin=0.5cm, paperheight=500pt, paperwidth=400pt" \
  --highlight-style=espresso --pdf-engine=xelatex
```

## Code Style Guidelines

### File Organization
- **Simulation directories**: Each under `hd/` contains:
  - `init.c` - Initial conditions and boundary conditions
  - `definitions.h` - Physics constants and parameters
  - `pluto.ini` - Runtime configuration
  - `README.md` - Physics documentation with LaTeX math
  - `mpirun.sh` - SLURM batch script template
  - Optional: `visc_nu.c`, `radiat.c`, `viscous_flux.c`

### Naming Conventions
- **Variables**: `snake_case` (e.g., `rmin`, `rho_max`, `g_inputParam`)
- **Constants**: `UPPER_CASE` (e.g., `RMIN`, `RMAX`, `GAMMA`)
- **Functions**: `PascalCase` for user-defined (e.g., `Init`, `Analysis`, `UserDefBoundary`)
- **Macros**: `UPPER_CASE` (e.g., `EXPAND`, `D_EXPAND`, `ARRAY_1D`)
- **Global variables**: prefixed with `g_` (e.g., `g_gamma`, `g_time`)

### Formatting
- **Indentation**: 2 spaces (no tabs)
- **Line length**: ~100 characters
- **Braces**: Opening brace on same line for functions, new line for struct/union
- **Comments**: Use C-style `/* */` blocks for documentation, `//` for inline

### Documentation Style
- Use Doxygen format with `/*!` and `\` commands
- Required tags: `\file`, `\brief`, `\param`, `\return`
- Document physics equations using LaTeX notation in `\f[ \f]` blocks
- Include author and date: `\author`, `\date`
- Reference academic papers in `\b References` sections

### Code Patterns

#### Include Structure
```c
#include "pluto.h"  // Always first
```

#### Function Template
```c
/* ********************************************************************* */
void FunctionName (double *v, double x1, double x2, double x3)
/*!
 * Brief description.
 * \param [in] v  pointer to data array
 * \param [in] x1 coordinate value
 * \return This function has no return value.
 *********************************************************************** */
{
  // Implementation
}
```

#### Conditional Compilation
```c
#if GEOMETRY == SPHERICAL
  // Spherical-specific code
#elif GEOMETRY == CYLINDRICAL
  // Cylindrical-specific code
#endif
```

### Physics Configuration
Edit `definitions.h` to set:
- `PHYSICS`: HD, MHD, RHD, RMHD
- `DIMENSIONS`: 1, 2, or 3
- `GEOMETRY`: CARTESIAN, CYLINDRICAL, SPHERICAL, POLAR
- `COOLING`, `VISCOSITY`, `BODY_FORCE` options

### Error Handling
- Use `QUIT_PLUTO(1)` for fatal errors
- Use `print1()` for informative messages
- Check for NaN: `if (T != T)` pattern

### Best Practices
1. Always use `g_inputParam[]` for user-defined parameters from `pluto.ini`
2. Use PLUTO macros: `EXPAND()`, `D_EXPAND()`, `VAR_LOOP()`, `DOM_LOOP()`
3. Access grid via `grid[IDIR].x`, `grid[JDIR].x`, `grid[KDIR].x`
4. Store static/persistent data with `static` qualifier
5. Use `ARRAY_1D()` and `ARRAY_3D()` for dynamic allocation
6. Follow existing simulation directories as templates

### Testing
- No automated test suite
- Validate by checking physical output and conservation
- Compare with analytic solutions when available
- Monitor energy and mass conservation

## Additional Notes

- This is a research codebase focused on astrophysical accretion flows
- Each simulation directory is self-contained
- Code must be compiled separately for each simulation setup
- Physics documentation should reference peer-reviewed papers
- Unit conversions handled via `UNIT_DENSITY`, `UNIT_LENGTH`, `UNIT_VELOCITY`
