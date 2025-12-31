# Black-Scholes Option Pricer (C++)

A fast, native implementation of the Black-Scholes option pricing model with interactive visualizations, built using Dear ImGui, ImPlot, and OpenGL.

![alt text](https://github.com/AndrewJoshuaFreeman/bs-pde-pricer/blob/main/uiImage.png?raw=true)

## Features

- **Real-time pricing** for European call and put options
- **Interactive heatmaps** showing how option prices vary with spot price and volatility
- **Greeks calculation** (Delta, Gamma)
- **Clean, responsive UI** with sidebar controls and metric cards
- **Cross-platform** support (macOS, Linux, Windows)

## Screenshots

The application displays:
- Input parameters in a sidebar
- Calculated option prices in colored metric cards
- Interactive heatmaps for call and put prices across spot/volatility ranges
- Real-time updates as you adjust parameters

## Requirements

- CMake 3.16+
- C++17 compiler
- OpenGL 3.2+ (automatically satisfied on modern systems)

## Building

```bash
mkdir -p build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Running

```bash
./build/bs_pricer
```

## Project Structure

```
bs_pricer_cpp/
├── CMakeLists.txt          # Build configuration with dependency management
├── main.cpp                # Application entry point and UI logic
└── black_scholes.hpp       # Black-Scholes pricing formulas
```

## Dependencies

All dependencies are automatically fetched via CMake:
- **GLFW** (3.4) - Window management
- **Dear ImGui** (1.90.9) - Immediate mode GUI
- **ImPlot** (0.16) - Plotting and heatmaps

## Usage

1. Adjust input parameters in the left sidebar:
   - Current Asset Price (S)
   - Strike Price (K)
   - Time to Maturity (T)
   - Volatility (σ)
   - Risk-Free Rate (r)

2. Configure heatmap visualization:
   - Grid size (resolution)
   - Spot price range
   - Volatility range

3. View results:
   - Call and put prices with color-coded cards
   - Greeks (Delta, Gamma)
   - Interactive heatmaps showing price sensitivity

## Platform Notes

### macOS
The application uses OpenGL 3.2 Core Profile with forward compatibility, which is the correct configuration for macOS. Earlier versions (3.0/3.1) are not supported by Apple.

### Linux/Windows
The code automatically works on these platforms with OpenGL 3.2+.

## Implementation Details

- **Normal CDF/PDF**: Implemented using `std::erf` (standard library)
- **Pricing model**: Standard Black-Scholes formula for European options
- **Greeks**: Analytical formulas for Delta and Gamma
- **Heatmaps**: Row-major format compatible with ImPlot

## Customization

You can easily extend the application to add:
- More Greeks (Vega, Theta, Rho)
- Implied volatility calculations
- Payoff diagrams
- Export functionality (CSV, PNG)
- Additional option types (American, Asian, etc.)

## License

MIT License - feel free to use and modify as needed.
