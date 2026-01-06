# Black-Scholes Option Pricer (C++)

A fast, native implementation of the Black Scholes option pricing model built with interactive visualizations using OpenGl and ImGUI. A continuation off of my previous Black Scholes mathematical simulator project, [see here](https://github.com/AndrewJoshuaFreeman/bs-pde-math-simulator)! Meant to show an elementary demonstration of the Black Scholes fomrula with European call options. Let me know if you have any questions!

![alt text](https://github.com/AndrewJoshuaFreeman/bs-pde-pricer/blob/main/assets/uiImage.png?raw=true)

## Black Scholes Heatmap Demo

![ImGui Black–Scholes Heatmap](assets/demo.gif)


## features

- **Real-time pricing** for European call and put options
- **Interactive heatmaps** showing how option prices vary with spot price and volatility
- **Greeks calculation** (Delta, Gamma)
- **Clean, responsive UI** with sidebar controls and metric cards
- **Cross-platform** support (macOS, Linux, Windows)

## screenshots

The application displays:
- input parameters in a sidebar
- calculated option prices in colored metric cards
- interactive heatmaps for call and put prices across spot/volatility ranges
- real time updates as you adjust parameters

## Requirements

- CMake 3.16+
- C++17 compiler(or later)
- OpenGL 3.2+

## building

```bash
mkdir -p build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## running

```bash
./build/bs_pricer
```

## dependencies

all dependencies auto fetched via CMake:
- **GLFW** (3.4) - Window management
- **Dear ImGui** (1.90.9) - Immediate mode GUI
- **ImPlot** (0.16) - Plotting and heatmaps

## usage

1. adjust input parameters in the left sidebar:
   - current asset price (S)
   - strike price (K)
   - time to maturity (T)
   - volatility (σ)
   - risk free rate (r)

2. configure heatmap visualization:
   - grid size (resolution)
   - spot price range
   - volatility range

3. view results:
   - call and put prices with color coded cards
   - Greeks (Delta, Gamma)
   - interactive heatmaps showing price sensitivity

## platform notes

### macOS
I use OpenGL 3.2 Core Profile, which is the configfor macOS, it will not work otherwise.

### Linux/Windows
The code automatically works on these platforms with OpenGL 3.2+

## Implementation Details

- **Normal CDF/PDF**: implemented using `std::erf` (standard library)
- **Pricing model**: standard Black Scholes formula for European options
- **Greeks**: analytical formulas for Delta and Gamma
- **Heatmaps**: row major format compatible with ImPlot
