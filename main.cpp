#include <cstdio>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

#include "black_scholes.hpp"

// ImGui / ImPlot
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "implot.h"

#include <GLFW/glfw3.h>

static void glfw_error_callback(int error, const char* description) {
    std::fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static std::vector<float> linspace(float a, float b, int n) {
    std::vector<float> x(n);
    if (n == 1) { x[0] = a; return x; }
    float step = (b - a) / float(n - 1);
    for (int i = 0; i < n; ++i) x[i] = a + step * float(i);
    return x;
}

//small helper to draw “metric cards” like CALL/PUT boxes
static void MetricCard(const char* title, double value, ImVec4 bg, ImVec4 fg) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, bg);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
    ImGui::BeginChild(title, ImVec2(0, 80), true, ImGuiWindowFlags_NoScrollbar);

    ImGui::PushStyleColor(ImGuiCol_Text, fg);
    ImGui::TextUnformatted(title);
    ImGui::Spacing();
    ImGui::SetWindowFontScale(1.4f);
    ImGui::Text("$%.4f", value);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

int main() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    // OpenGL3 + GLSL
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1400, 800, "Black-Scholes Pricer (C++ / ImGui)", nullptr, nullptr);
    if (!window) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    //state (inputs like Streamlit sidebar)
    BlackScholesInputs in{};
    int heatN = 10;

    float spot_min = float(in.S * 0.8);
    float spot_max = float(in.S * 1.2);
    float vol_min  = float(in.v * 0.5);
    float vol_max  = float(in.v * 1.5);

    bool lock_strike_for_heatmap = true;

    // heatmap buffers
    std::vector<float> spot_axis(heatN), vol_axis(heatN);
    std::vector<float> call_map(heatN * heatN), put_map(heatN * heatN);

    auto recompute_heatmaps = [&]() {
        spot_axis = linspace(spot_min, spot_max, heatN);
        vol_axis  = linspace(vol_min,  vol_max,  heatN);

        for (int yi = 0; yi < heatN; ++yi) {
            for (int xi = 0; xi < heatN; ++xi) {
                BlackScholesInputs tmp = in;
                tmp.S = spot_axis[xi];
                tmp.v = vol_axis[yi];
                if (!lock_strike_for_heatmap) tmp.K = in.K;

                auto out = black_scholes_price(tmp);
                call_map[yi * heatN + xi] = float(out.call);
                put_map [yi * heatN + xi] = float(out.put);
            }
        }
    };

    recompute_heatmaps();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // main window
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(1380, 780), ImGuiCond_Once);

        ImGui::Begin("Black-Scholes Pricing Model");

        // layout: left “sidebar” + right content
        ImGui::Columns(2, nullptr, true);
        ImGui::SetColumnWidth(0, 360.0f);

        // sidebar!
        ImGui::Text("📊 Black-Scholes Model (C++)");
        ImGui::Separator();

        bool changed = false;
        changed |= ImGui::InputDouble("Current Asset Price (S)", &in.S, 0.0, 0.0, "%.6f");
        changed |= ImGui::InputDouble("Strike Price (K)", &in.K, 0.0, 0.0, "%.6f");
        changed |= ImGui::InputDouble("Time to Maturity (T, years)", &in.T, 0.0, 0.0, "%.6f");
        changed |= ImGui::InputDouble("Volatility (σ)", &in.v, 0.0, 0.0, "%.6f");
        changed |= ImGui::InputDouble("Risk-Free Rate (r)", &in.r, 0.0, 0.0, "%.6f");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Heatmap Parameters");
        ImGui::Spacing();

        changed |= ImGui::SliderInt("Grid size", &heatN, 5, 30);
        changed |= ImGui::InputFloat("Min Spot", &spot_min, 0.0f, 0.0f, "%.4f");
        changed |= ImGui::InputFloat("Max Spot", &spot_max, 0.0f, 0.0f, "%.4f");
        changed |= ImGui::SliderFloat("Min Vol", &vol_min, 0.001f, 2.0f, "%.4f");
        changed |= ImGui::SliderFloat("Max Vol", &vol_max, 0.001f, 2.0f, "%.4f");

        if (spot_min > spot_max) std::swap(spot_min, spot_max);
        if (vol_min  > vol_max)  std::swap(vol_min,  vol_max);

        ImGui::Checkbox("Lock strike for heatmap", &lock_strike_for_heatmap);

        if (ImGui::Button("Recompute Heatmaps")) {
            recompute_heatmaps();
        }

        // auto updating for conv
        if (changed) {
            // resize buffers if heatN changed
            spot_axis.resize(heatN);
            vol_axis.resize(heatN);
            call_map.resize(heatN * heatN);
            put_map.resize(heatN * heatN);
            recompute_heatmaps();
        }

        ImGui::NextColumn();

        //right content
        auto out = black_scholes_price(in);

        ImGui::Text("Inputs");
        ImGui::Separator();
        if (ImGui::BeginTable("inputs_table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            auto row = [&](const char* k, const char* v) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(k);
                ImGui::TableSetColumnIndex(1); ImGui::TextUnformatted(v);
            };

            char buf[64];
            std::snprintf(buf, sizeof(buf), "%.6f", in.S); row("Current Asset Price", buf);
            std::snprintf(buf, sizeof(buf), "%.6f", in.K); row("Strike Price", buf);
            std::snprintf(buf, sizeof(buf), "%.6f", in.T); row("Time to Maturity (Years)", buf);
            std::snprintf(buf, sizeof(buf), "%.6f", in.v); row("Volatility (σ)", buf);
            std::snprintf(buf, sizeof(buf), "%.6f", in.r); row("Risk-Free Interest Rate", buf);

            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::Text("Prices");
        ImGui::Separator();

        ImGui::Columns(2, nullptr, false);
        MetricCard("CALL Value", out.call, ImVec4(0.56f, 0.93f, 0.56f, 0.25f), ImVec4(1,1,1,1));
        ImGui::NextColumn();
        MetricCard("PUT Value",  out.put,  ImVec4(1.00f, 0.60f, 0.60f, 0.25f), ImVec4(1,1,1,1));
        ImGui::Columns(1);

        ImGui::Spacing();
        ImGui::Text("Greeks");
        ImGui::Separator();
        ImGui::Text("Call Delta: %.6f | Put Delta: %.6f | Gamma: %.6f", out.call_delta, out.put_delta, out.gamma);

        ImGui::Spacing();
        ImGui::Text("Options Price - Interactive Heatmaps (Spot × Volatility)");
        ImGui::Separator();

        // heatmap plotting
        // ImPlot expects row-major; map is [y][x] so it matches
        ImGui::Columns(2, nullptr, false);

        if (ImPlot::BeginPlot("Call Price Heatmap", ImVec2(-1, 360))) {
            ImPlot::SetupAxes("Spot Price", "Volatility", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
            ImPlot::PlotHeatmap("CALL", call_map.data(), heatN, heatN, 0.0, 0.0,
                                "%.2f", ImPlotPoint(spot_min, vol_min), ImPlotPoint(spot_max, vol_max));
            ImPlot::EndPlot();
        }

        ImGui::NextColumn();

        if (ImPlot::BeginPlot("Put Price Heatmap", ImVec2(-1, 360))) {
            ImPlot::SetupAxes("Spot Price", "Volatility", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
            ImPlot::PlotHeatmap("PUT", put_map.data(), heatN, heatN, 0.0, 0.0,
                                "%.2f", ImPlotPoint(spot_min, vol_min), ImPlotPoint(spot_max, vol_max));
            ImPlot::EndPlot();
        }

        ImGui::Columns(1);

        ImGui::End(); // main window

        // the render
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.06f, 0.07f, 0.09f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImPlot::DestroyContext();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

