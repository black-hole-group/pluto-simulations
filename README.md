# PLUTO Astrophysical Simulations

This repository contains astrophysical fluid dynamics simulations using [PLUTO](http://plutocode.ph.unito.it/) - a modular, Godunov-type code for astrophysical fluids. These simulations focus on black hole accretion flows and related phenomena.

Developed by the **Black Hole Group at Universidade de São Paulo (USP)**.

## Available Simulations

Located in `hd/` directory:

- **`bondi_2d/`** - 2D Bondi accretion (spherical accretion onto a black hole)
- **`torus_2D/`** - 2D thick torus (Papaloizou-Pringle equilibrium)
- **`torus_2d_pn/`** - 2D torus with pseudo-Newtonian (Paczynski-Wiita) potential
- **`torus_2d_pn_alphar/`** - Torus with radially-varying alpha viscosity
- **`torus_2D_pn_cool/`** - Torus with radiative cooling (synchrotron, bremsstrahlung, SSC)

## Quick Start

### Prerequisites
- PLUTO framework installed (`$PLUTO_DIR` environment variable set)
- C compiler (gcc/icc)
- Python 2.7+ or 3.x
- MPI (for parallel runs)
- pandoc + xelatex (optional, for PDF documentation)

### Running a Simulation

```bash
# 1. Navigate to a simulation directory
cd hd/torus_2D_pn_cool/

# 2. Configure and compile
python $PLUTO_DIR/setup.py
make

# 3. Run
./pluto                    # Serial
mpirun -np 4 ./pluto       # Parallel (4 cores)
sbatch mpirun.sh           # SLURM cluster
```

## Repository Structure

Each simulation directory contains:

| File | Purpose |
|------|---------|
| `init.c` | Initial conditions & user-defined boundary conditions |
| `definitions.h` | Physics setup (HD/MHD, geometry, dimensions, cooling) |
| `pluto.ini` | Runtime parameters (grid, CFL, boundaries, output) |
| `README.md` | Physics documentation with LaTeX equations |
| `mpirun.sh` | SLURM batch script template |
| `visc_nu.c` | Viscosity prescription (if applicable) |
| `radiat.c` | Cooling function (if applicable) |

## Configuration

Edit `definitions.h` to change physics:
```c
#define PHYSICS     HD          // HD, MHD, RHD, RMHD
#define DIMENSIONS  2           // 1, 2, or 3
#define GEOMETRY    SPHERICAL   // CARTESIAN, CYLINDRICAL, SPHERICAL, POLAR
#define COOLING     TABULATED   // Cooling options
#define VISCOSITY   EXPLICIT    // Viscosity treatment
```

Edit `pluto.ini` to change runtime parameters (grid resolution, output frequency, etc.).

## Documentation

Each simulation has detailed physics documentation in its README.md. Generate PDFs:

```bash
# Using pandoc
pandoc --verbose --from=markdown --output=README.pdf README.md \
  --variable=geometry:"margin=0.5cm, paperheight=500pt, paperwidth=400pt" \
  --highlight-style=espresso --pdf-engine=xelatex
```

## Adding New Simulations

1. Copy an existing directory as template:
   ```bash
   cp -r hd/torus_2d_pn hd/my_new_sim
   ```

2. Modify key files:
   - `init.c` - Change initial conditions
   - `definitions.h` - Adjust physics parameters
   - `pluto.ini` - Set grid resolution and runtime parameters
   - `README.md` - Document the physics

3. Rebuild and test

## Code Conventions

See [AGENTS.md](./AGENTS.md) for detailed coding guidelines including:
- Naming conventions (snake_case variables, PascalCase functions)
- Documentation style (Doxygen with LaTeX equations)
- PLUTO-specific patterns and macros

## Citation

If you use these simulations or code in your research, please cite:

- **Nemmen et al. (2024)**, "Emergence of hot corona and truncated disc in simulations of accreting stellar mass black holes", *MNRAS*, 531, 805. DOI: [10.1093/mnras/stae1133](https://doi.org/10.1093/mnras/stae1133)

- **Almeida & Nemmen (2020)**, "Winds and feedback from supermassive black holes accreting at low rates: hydrodynamical treatment", *MNRAS*, 492, 2553. DOI: [10.1093/mnras/staa033](https://doi.org/10.1093/mnras/staa033)

- **PLUTO Code:** Mignone et al. (2007), *ApJS*, 170, 228

## License

This project is licensed under the GNU General Public License v3.0 (GPL-3.0) - see the [LICENSE](LICENSE) file for details. This is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation.

## Contact

**Black Hole Group**  
Universidade de São Paulo (USP)
