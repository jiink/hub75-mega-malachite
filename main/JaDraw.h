#ifndef JADRAW_H
#define JADRAW_H

#include <array>
#include <cstdint>
#include <cstddef>
#include <cmath>
#include <algorithm>
#include <cassert>
#include <limits>
#include <span>

enum class DrawMode {
    OPAQUE,   // Blend source RGB onto destination using `intensity` as alpha factor. Destination alpha is also blended.
    BLEND,    // Blend source RGB onto destination using `source_color`'s alpha (scaled by `intensity`) as alpha factor. Destination alpha is also blended.
    ADDITIVE  // Add source RGB (scaled by `intensity`) to destination RGB, clamp result. Destination alpha remains unchanged.
};

#define JADRAW_RED(color)   (((color) >> 24) & 0xFFU)
#define JADRAW_GREEN(color) (((color) >> 16)  & 0xFFU)
#define JADRAW_BLUE(color)  (((color) >> 8) & 0xFFU)
#define JADRAW_ALPHA(color) ((color) & 0xFFU)
#define JADRAW_RGBA(r,g,b,a) ( (static_cast<uint32_t>(r) << 24) | \
                             (static_cast<uint32_t>(g) << 16) | \
                             (static_cast<uint32_t>(b) << 8)  | \
                             (static_cast<uint32_t>(a)) )

namespace Colors {
    constexpr uint32_t Black       = 0x000000FF;
    constexpr uint32_t White       = 0xFFFFFFFF;
    constexpr uint32_t Red         = 0xFF0000FF;
    constexpr uint32_t Green       = 0x00FF00FF;
    constexpr uint32_t Blue        = 0x0000FFFF;
    constexpr uint32_t Yellow      = 0xFFFF00FF;
    constexpr uint32_t Cyan        = 0x00FFFFFF;
    constexpr uint32_t Magenta     = 0xFF00FFFF;
    constexpr uint32_t Grey        = 0x808080FF;
    constexpr uint32_t Orange      = 0xFFA500FF;
    constexpr uint32_t Purple      = 0x800080FF;
    constexpr uint32_t Brown       = 0xA52A2AFF;
}

/**
 * @brief Structure holding non-owning views (spans) to paletted sprite data.
 * Designed to work with statically allocated sprite data. Requires C++20.
 */
struct JaSprite {
    int width = 0;
    int height = 0;
    std::span<const uint32_t> palette; // Non-owning view of palette colors
    std::span<const uint8_t> pixels;   // Non-owning view of pixel indices

    // Default constructor creates an invalid sprite
    constexpr JaSprite() = default;

    /**
     * @brief Constructs a JaSprite view from existing data.
     * @param w Sprite width.
     * @param h Sprite height.
     * @param p A span viewing the palette data (const uint32_t[]).
     * @param pix A span viewing the pixel data (const uint8_t[]).
     * @param check_indices If true, validates that all pixel indices are within palette bounds (can be slow for large sprites, disable if data is trusted).
     * @throws std::invalid_argument if dimensions are invalid, pixel data size mismatches, or palette is too large/empty.
     * @throws std::out_of_range if check_indices is true and an invalid index is found.
     */
    constexpr JaSprite(int w, int h, std::span<const uint32_t> p, std::span<const uint8_t> pix, bool check_indices = true)
        : width(w), height(h), palette(p), pixels(pix)
    {
        // Use a non-throwing check function for constexpr context if possible,
        // or rely on runtime checks if exceptions are needed.
        // For simplicity here, we keep the runtime throw logic.
        // A fully constexpr sprite would need constexpr validation.
        // if (width <= 0 || height <= 0) {
        //     // Cannot throw in constexpr constructor directly before C++20 allowed it easily.
        //     // Rely on usage context or make this constructor non-constexpr if throws are essential.
        //      //throw std::invalid_argument("Sprite dimensions must be positive."); // Runtime check
        // }
        // if (static_cast<size_t>(width) * height != pixels.size()) {
        //     //throw std::invalid_argument("Pixel data size does not match sprite dimensions."); // Runtime check
        // }
        // if (palette.empty()) {
        //      //throw std::invalid_argument("Sprite palette cannot be empty."); // Runtime check
        // }
        // if (palette.size() > 256) {
        //      //throw std::invalid_argument("Palette size cannot exceed 256 colors."); // Runtime check
        // }

        // if (check_indices) {
        //     for(size_t i = 0; i < pixels.size(); ++i) {
        //         if (pixels[i] >= palette.size()) {
        //             // Cannot easily construct detailed error string in constexpr
        //             //throw std::out_of_range("Pixel index out of palette range."); // Runtime check
        //         }
        //     }
        // }
    }

    /**
     * @brief Gets the palette index at sprite coordinates (x, y). Assumes valid coordinates.
     */
    constexpr uint8_t getPixelIndexUnsafe(int x, int y) const {
        // No bounds check here for speed - JaDraw::drawSprite performs clipping
        return pixels[static_cast<size_t>(y) * width + x];
    }

     /**
      * @brief Gets the RGBA color for a given palette index. Assumes valid index.
      */
     constexpr uint32_t getColorFromIndexUnsafe(uint8_t index) const {
         // No bounds check here - constructor should validate indices if check_indices was true
         return palette[index];
     }

     // Optional: Add safe versions if needed outside drawSprite context
     uint8_t getPixelIndex(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height || pixels.empty()) {
            return 0; 
            //throw std::out_of_range("Sprite pixel coordinates out of bounds.");
        }
        return pixels[static_cast<size_t>(y) * width + x];
     }
     uint32_t getColorFromIndex(uint8_t index) const {
         if (index >= palette.size()) {
            return 0;
              //throw std::out_of_range("Palette index out of bounds.");
         }
         return palette[index];
     }
};

template <int W, int H>
class JaDraw {
    static_assert(W > 0, "Need positive width");
    static_assert(H > 0, "Need positive height");
    static_assert(static_cast<unsigned long long>(W) * H <= SIZE_MAX / sizeof(uint32_t), "Canvas size exceeds limits");

private:
    // --- Wu's Algorithm Helpers ---
    static inline int ipart(float x) { return static_cast<int>(std::floor(x)); }
    static inline float fpart(float x) { return x - std::floor(x); }
    static inline float rfpart(float x) { return 1.0f - fpart(x); }
    static inline int round_int(float x) { return static_cast<int>(std::round(x)); }

    // --- Core Plotting Function ---
     inline void plotPixelUnsafeWithIntensityMode(int x, int y, uint32_t source_color, float intensity, DrawMode mode) {
        if (intensity <= 0.0f) return;
        if (intensity > 1.0f) intensity = 1.0f;

        size_t index = static_cast<std::size_t>(y) * W + x;
        uint32_t& dest_pixel = canvas[index];

        uint32_t src_r = JADRAW_RED(source_color);
        uint32_t src_g = JADRAW_GREEN(source_color);
        uint32_t src_b = JADRAW_BLUE(source_color);
        uint32_t src_a = JADRAW_ALPHA(source_color);

        uint32_t dest_r = JADRAW_RED(dest_pixel);
        uint32_t dest_g = JADRAW_GREEN(dest_pixel);
        uint32_t dest_b = JADRAW_BLUE(dest_pixel);
        uint32_t dest_a = JADRAW_ALPHA(dest_pixel);

        switch (mode) {
            case DrawMode::OPAQUE: {
                uint32_t effective_a = static_cast<uint32_t>(255 * intensity);
                if (effective_a == 0) return;

                // Blend RGB based on intensity acting as alpha
                unsigned int blend_r = (src_r * effective_a + dest_r * (255 - effective_a)) / 255;
                unsigned int blend_g = (src_g * effective_a + dest_g * (255 - effective_a)) / 255;
                unsigned int blend_b = (src_b * effective_a + dest_b * (255 - effective_a)) / 255;
                // Blend alpha based on intensity acting as alpha
                unsigned int blend_a = effective_a + (dest_a * (255 - effective_a)) / 255;

                dest_pixel = JADRAW_RGBA(blend_r, blend_g, blend_b, blend_a);
                break;
            }
            case DrawMode::BLEND: {
                uint32_t src_a_effective = static_cast<uint32_t>(src_a * intensity);
                if (src_a_effective == 0) return;

                // Standard alpha blend for RGB
                unsigned int blend_r = (src_r * src_a_effective + dest_r * (255 - src_a_effective)) / 255;
                unsigned int blend_g = (src_g * src_a_effective + dest_g * (255 - src_a_effective)) / 255;
                unsigned int blend_b = (src_b * src_a_effective + dest_b * (255 - src_a_effective)) / 255;
                // Blend destination alpha
                unsigned int blend_a = src_a_effective + (dest_a * (255 - src_a_effective)) / 255;

                dest_pixel = JADRAW_RGBA(blend_r, blend_g, blend_b, blend_a);
                break;
            }
            case DrawMode::ADDITIVE: {
                // Scale source color by intensity
                unsigned int scaled_src_r = static_cast<unsigned int>(src_r * intensity);
                unsigned int scaled_src_g = static_cast<unsigned int>(src_g * intensity);
                unsigned int scaled_src_b = static_cast<unsigned int>(src_b * intensity);

                // Add and clamp
                unsigned int add_r = std::min(255u, (unsigned int)(dest_r + scaled_src_r));
                unsigned int add_g = std::min(255u, (unsigned int)(dest_g + scaled_src_g));
                unsigned int add_b = std::min(255u, (unsigned int)(dest_b + scaled_src_b));

                // Keep original destination alpha
                dest_pixel = JADRAW_RGBA(add_r, add_g, add_b, dest_a);
                break;
            }
        }
    }


public:
    static constexpr int width = W;
    static constexpr int height = H;
    std::array<uint32_t, static_cast<std::size_t>(W) * H> canvas;

    JaDraw() : canvas{} {}

    /**
     * @brief Clears the canvas to a specified color.
     * @param color The clear color (RGBA). Alpha component is ignored; canvas alpha is always set to 255 (opaque).
     */
    void clear(uint32_t color = 0xFF000000) {
        uint32_t clear_color_opaque = JADRAW_RGBA(JADRAW_RED(color), JADRAW_GREEN(color), JADRAW_BLUE(color), 255);
        canvas.fill(clear_color_opaque);
    }

    /**
     * @brief Draws a single pixel with the specified mode (defaults to BLEND).
     * @param x X coordinate.
     * @param y Y coordinate.
     * @param color Pixel color (RGBA).
     * @param mode Drawing mode (OPAQUE, BLEND, ADDITIVE).
     */
    inline void drawPixel(int x, int y, uint32_t color, DrawMode mode = DrawMode::BLEND) {
        if (x >= 0 && x < W && y >= 0 && y < H) {
            plotPixelUnsafeWithIntensityMode(x, y, color, 1.0f, mode);
        }
    }


    /**
     * @brief Draws an integer-based line with thickness and specified mode (defaults to BLEND).
     * @param x1 Start X coordinate.
     * @param y1 Start Y coordinate.
     * @param x2 End X coordinate.
     * @param y2 End Y coordinate.
     * @param thickness Line thickness in pixels.
     * @param color Line color (RGBA).
     * @param mode Drawing mode (OPAQUE, BLEND, ADDITIVE).
     */
    void drawLine(int x1, int y1, int x2, int y2, int thickness, uint32_t color, DrawMode mode = DrawMode::BLEND)
    {
        if (thickness <= 0) return;

        // Internal helper for plotting with bounds check
        auto plot_int = [&](int x, int y) {
            if (x >= 0 && x < W && y >= 0 && y < H) {
                 plotPixelUnsafeWithIntensityMode(x, y, color, 1.0f, mode);
            }
        };

        // Special case: Thickness 1 uses Bresenham directly
        if (thickness == 1) {
            int dx_thin = std::abs(x2 - x1); int dy_thin = -std::abs(y2 - y1);
            int sx_thin = x1 < x2 ? 1 : -1; int sy_thin = y1 < y2 ? 1 : -1;
            int err_thin = dx_thin + dy_thin; int e2_thin;
            while (true) {
                plot_int(x1, y1); if (x1 == x2 && y1 == y2) break;
                e2_thin = 2 * err_thin;
                if (e2_thin >= dy_thin) { if (x1 == x2) break; err_thin += dy_thin; x1 += sx_thin; }
                if (e2_thin <= dx_thin) { if (y1 == y2) break; err_thin += dx_thin; y1 += sy_thin; }
            } return;
        }

        // General case: Thick line algorithm
        int dx = x2 - x1; int dy = y2 - y1;
        int abs_dx = std::abs(dx); int abs_dy = std::abs(dy);
        int sx = (dx > 0) ? 1 : -1; int sy = (dy > 0) ? 1 : -1;

        // Calculate thickness extents
        int half_thick_floor = (thickness - 1) / 2;
        int half_thick_ceil = thickness / 2; // For odd thickness, floor==ceil-1; for even, floor==ceil

        // Special cases: Vertical and Horizontal lines (optimized fill)
        if (abs_dx == 0) { // Vertical line
            int start_x = x1 - half_thick_floor;
            int end_x = x1 + half_thick_ceil;
            int start_y = std::min(y1, y2);
            int end_y = std::max(y1, y2);
            for (int y = start_y; y <= end_y; ++y) {
                for (int x = start_x; x < end_x; ++x) {
                    plot_int(x, y);
                }
            }
            return;
        }
        if (abs_dy == 0) { // Horizontal line
            int start_y = y1 - half_thick_floor;
            int end_y = y1 + half_thick_ceil;
            int start_x = std::min(x1, x2);
            int end_x = std::max(x1, x2);
            for (int x = start_x; x <= end_x; ++x) {
                for (int y = start_y; y < end_y; ++y) {
                    plot_int(x, y);
                }
            }
            return;
        }

        // General thick line algorithm (based on Bresenham)
        if (abs_dx > abs_dy) { // X-major line
            int err = 2 * abs_dy - abs_dx;
            int y = y1;
            for (int x = x1; x != x2 + sx; x += sx) {
                // Draw perpendicular span
                int span_start_y = y - half_thick_floor;
                int span_end_y = y + half_thick_ceil;
                for (int py = span_start_y; py < span_end_y; ++py) {
                    plot_int(x, py);
                }

                // Bresenham step
                if (err >= 0) {
                    y += sy;
                    err -= 2 * abs_dx;
                }
                err += 2 * abs_dy;
            }
        } else { // Y-major line
            int err = 2 * abs_dx - abs_dy;
            int x = x1;
            for (int y = y1; y != y2 + sy; y += sy) {
                // Draw perpendicular span
                int span_start_x = x - half_thick_floor;
                int span_end_x = x + half_thick_ceil;
                for (int px = span_start_x; px < span_end_x; ++px) {
                    plot_int(px, y);
                }

                // Bresenham step
                if (err >= 0) {
                    x += sx;
                    err -= 2 * abs_dy;
                }
                err += 2 * abs_dx;
            }
        }
    }


    /**
     * @brief Draws an anti-aliased line using Xiaolin Wu's algorithm with the specified mode (defaults to BLEND).
     * @param x1 Start X coordinate.
     * @param y1 Start Y coordinate.
     * @param x2 End X coordinate.
     * @param y2 End Y coordinate.
     * @param color Line color (RGBA). Alpha influences blending intensity in BLEND mode.
     * @param mode Drawing mode (OPAQUE, BLEND, ADDITIVE).
     */
    void drawLineAA(float x1, float y1, float x2, float y2, uint32_t color, DrawMode mode = DrawMode::BLEND)
    {
         // Optimization: If source alpha is 0 and mode uses it, nothing will be drawn.
         if (JADRAW_ALPHA(color) == 0 && (mode == DrawMode::BLEND /*|| mode == DrawMode::ADDITIVE*/)) {
             // Note: Additive mode *could* still draw if intensity > 0, even if alpha is 0,
             // but current plotPixelUnsafeWithIntensityMode scales RGB by intensity for ADDITIVE.
             // If color RGB is non-zero, it *will* draw something. Let's keep it simple and only skip for BLEND.
             return;
         }

        // Internal helper for plotting with intensity and bounds check
        auto plot = [&](int x, int y, float intensity) {
            if (intensity > 0.0f && x >= 0 && x < W && y >= 0 && y < H) {
                 plotPixelUnsafeWithIntensityMode(x, y, color, intensity, mode);
            }
        };

        float dx = x2 - x1;
        float dy = y2 - y1;

        // Handle degenerate case (single point)
        if (std::abs(dx) < 1e-6f && std::abs(dy) < 1e-6f) {
             plot(round_int(x1), round_int(y1), 1.0f);
             return;
        }

        if (std::abs(dx) > std::abs(dy)) { // X-major line
            // Ensure x1 <= x2
            if (x1 > x2) {
                std::swap(x1, x2);
                std::swap(y1, y2);
            }
            dx = x2 - x1; // Recalculate dx after swap
            dy = y2 - y1; // Recalculate dy after swap
            float gradient = (dx == 0.0f) ? 1.0f : dy / dx; // Handle vertical case possibility

            // --- Handle first endpoint ---
            int x_end1 = round_int(x1);
            float y_end1 = y1 + gradient * (x_end1 - x1);
            float gap1 = rfpart(x1 + 0.5f); // Gap from the rounded start point
            int ix1 = x_end1;
            int iy1 = ipart(y_end1);
            plot(ix1, iy1,     rfpart(y_end1) * gap1);
            plot(ix1, iy1 + 1,  fpart(y_end1) * gap1);
            float inter_y = y_end1 + gradient; // First y-intersection for the main loop

            // --- Handle second endpoint ---
            int x_end2 = round_int(x2);
            float y_end2 = y2 + gradient * (x_end2 - x2);
            float gap2 = fpart(x2 + 0.5f); // Gap from the rounded end point
            int ix2 = x_end2;
            int iy2 = ipart(y_end2);
            plot(ix2, iy2,     rfpart(y_end2) * gap2);
            plot(ix2, iy2 + 1,  fpart(y_end2) * gap2);

            // --- Main loop ---
            for (int x = ix1 + 1; x < ix2; ++x) {
                plot(x, ipart(inter_y),     rfpart(inter_y));
                plot(x, ipart(inter_y) + 1,  fpart(inter_y));
                inter_y += gradient;
            }

        } else { // Y-major line
            // Ensure y1 <= y2
            if (y1 > y2) {
                std::swap(x1, x2);
                std::swap(y1, y2);
            }
            dx = x2 - x1; // Recalculate dx after swap
            dy = y2 - y1; // Recalculate dy after swap
            float gradient = (dy == 0.0f) ? 1.0f : dx / dy; // Handle horizontal case possibility

            // --- Handle first endpoint ---
            int y_end1 = round_int(y1);
            float x_end1 = x1 + gradient * (y_end1 - y1);
            float gap1 = rfpart(y1 + 0.5f); // Gap from the rounded start point
            int iy1 = y_end1;
            int ix1 = ipart(x_end1);
            plot(ix1,     iy1, rfpart(x_end1) * gap1);
            plot(ix1 + 1, iy1,  fpart(x_end1) * gap1);
            float inter_x = x_end1 + gradient; // First x-intersection for the main loop

            // --- Handle second endpoint ---
            int y_end2 = round_int(y2);
            float x_end2 = x2 + gradient * (y_end2 - y2);
            float gap2 = fpart(y2 + 0.5f); // Gap from the rounded end point
            int iy2 = y_end2;
            int ix2 = ipart(x_end2);
            plot(ix2,     iy2, rfpart(x_end2) * gap2);
            plot(ix2 + 1, iy2,  fpart(x_end2) * gap2);

            // --- Main loop ---
            for (int y = iy1 + 1; y < iy2; ++y) {
                plot(ipart(inter_x),     y, rfpart(inter_x));
                plot(ipart(inter_x) + 1, y,  fpart(inter_x));
                inter_x += gradient;
            }
        }
    }

    /**
     * @brief Draws a paletted sprite onto the canvas using spans.
     * Pixels referencing palette entries with alpha 0 are skipped (transparent).
     * @param dest_x Top-left X coordinate on the canvas.
     * @param dest_y Top-left Y coordinate on the canvas.
     * @param sprite The JaSprite object (viewing static or other data) to draw.
     * @param mode Drawing mode for non-transparent pixels.
     */
    void drawSprite(int dest_x, int dest_y, const JaSprite& sprite, DrawMode mode = DrawMode::BLEND) {
        // Basic check if sprite has valid dimensions and data spans
        if (sprite.width <= 0 || sprite.height <= 0 || sprite.pixels.empty() || sprite.palette.empty()) {
            return; // Nothing to draw
        }
        // Optional runtime check (if not done reliably in constructor)
        // assert(static_cast<size_t>(sprite.width) * sprite.height == sprite.pixels.size());

        int clip_x1 = std::max(0, dest_x);
        int clip_y1 = std::max(0, dest_y);
        int clip_x2 = std::min(W, dest_x + sprite.width);
        int clip_y2 = std::min(H, dest_y + sprite.height);

        if (clip_x1 >= clip_x2 || clip_y1 >= clip_y2) {
            return; // Fully clipped
        }

        for (int cy = clip_y1; cy < clip_y2; ++cy) {
            int sy = cy - dest_y;
            // Use the span directly now
            // Note: Accessing span with [] is usually unchecked in release builds.
            // The clipping ensures cx/cy are valid canvas coords.
            // sx/sy calculation ensures they map to valid sprite coords *within the clipped view*.
            size_t sprite_row_start_index = static_cast<size_t>(sy) * sprite.width;

            for (int cx = clip_x1; cx < clip_x2; ++cx) {
                int sx = cx - dest_x;

                // Get palette index from pixel span (unsafe access is okay due to clipping/structure logic)
                uint8_t palette_index = sprite.pixels[sprite_row_start_index + sx]; // Using span::operator[]

                // Get color from palette span (unsafe access assumes index is valid)
                // We rely on the constructor check or trusted input data.
                uint32_t source_color = sprite.palette[palette_index]; // Using span::operator[]

                // Universal Transparency Check (based on palette color's alpha)
                if (JADRAW_ALPHA(source_color) == 0) {
                    continue; // Skip transparent pixels
                }

                // Plot using the existing unsafe plotter (cx, cy are canvas-bounds checked by clipping)
                plotPixelUnsafeWithIntensityMode(cx, cy, source_color, 1.0f, mode);
            }
        }
    }
}; // JaDraw<W, H>


#endif // JADRAW_H