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

namespace VectorFont {

    constexpr uint8_t LIFT = 0xFF;
    constexpr size_t FONT_CHAR_MIN = 0x00;
    constexpr size_t FONT_CHAR_MAX = 0x7F;
    constexpr size_t FONT_CHAR_COUNT = FONT_CHAR_MAX - FONT_CHAR_MIN + 1;
    constexpr uint8_t CHAR_WIDTH = 6;
    constexpr uint8_t CHAR_HEIGHT = 9;
    constexpr uint8_t HAPPY = (0x01);
    constexpr uint8_t SAD = (0x02);
    constexpr uint8_t HEART = (0x03);
    constexpr uint8_t DIAMOND = (0x04);
    constexpr uint8_t CLUB = (0x05);
    constexpr uint8_t SPADE = (0x06);
    constexpr uint8_t BULLET = (0x07);
    constexpr uint8_t CIRCLE = (0x09);
    constexpr uint8_t R_TRIANGLE = (0x10);
    constexpr uint8_t L_TRIANGLE = (0x11);
    constexpr uint8_t UD_ARROW = (0x12);
    constexpr uint8_t U_ARROW = (0x18);
    constexpr uint8_t D_ARROW = (0x19);
    constexpr uint8_t R_ARROW = (0x1A);
    constexpr uint8_t L_ARROW = (0x1B);
    constexpr uint8_t LR_ARROW = (0x1D);
    constexpr uint8_t U_TRIANGLE = (0x1E);
    constexpr uint8_t D_TRIANGLE = (0x1F);

    struct FontChar {
        std::span<const uint8_t> points; // Contains pointer and size implicitly
        uint8_t width; // in grid units
        constexpr size_t size() const { return points.size(); }
        constexpr const uint8_t* data() const { return points.data(); }
    };

    namespace detail { 
        // the f_ means font_
        constexpr std::array<uint8_t, 1> f_space     = { LIFT }; // Character 0x00 (often NUL, using as space here)
        constexpr std::array<uint8_t, 18> f_happy    = { 0x10, 0x01, 0x05, 0x16, 0x46, 0x55, 0x51, 0x40, 0x10, LIFT, 0x22, LIFT, 0x32, LIFT, 0x14, 0x25, 0x35, 0x44 }; // 0x01
        constexpr std::array<uint8_t, 18> f_sad      = { 0x10, 0x01, 0x05, 0x16, 0x46, 0x55, 0x51, 0x40, 0x10, LIFT, 0x22, LIFT, 0x32, LIFT, 0x15, 0x24, 0x34, 0x45 }; // 0x02
        constexpr std::array<uint8_t, 9> f_heart     = { 0x10, 0x21, 0x30, 0x41, 0x43, 0x26, 0x03, 0x01, 0x10 }; // 0x03
        constexpr std::array<uint8_t, 5> f_diamond   = { 0x20, 0x43, 0x26, 0x03, 0x20 }; // 0x04
        constexpr std::array<uint8_t, 20> f_club     = { 0x20, 0x31, 0x32, 0x23, 0x12, 0x11, 0x20, LIFT, 0x12, 0x03, 0x04, 0x15, 0x24, 0x16, 0x36, 0x24, 0x35, 0x44, 0x43, 0x32 }; // 0x05
        constexpr std::array<uint8_t, 14> f_spade    = { 0x20, 0x32, 0x43, 0x44, 0x35, 0x24, 0x36, 0x16, 0x24, 0x15, 0x04, 0x03, 0x12, 0x20 }; // 0x06
        constexpr std::array<uint8_t, 5> f_bullet    = { 0x12, 0x23, 0x14, 0x03, 0x12 }; // 0x07
        // 0x08 backspace - using space for now
        constexpr std::array<uint8_t, 9> f_circle    = { 0x11, 0x02, 0x03, 0x14, 0x24, 0x33, 0x32, 0x21, 0x11 }; // 0x09 TAB - using circle
        // 0x0A line feed - handled in code
        // 0x0B VT - using space
        // 0x0C FF - using space
        // 0x0D CR - handled in code (often ignored if LF present)
        // 0x0E SO - using space
        // 0x0F SI - using space
        constexpr std::array<uint8_t, 4> f_tri_R     = { 0x00, 0x06, 0x33, 0x00 }; // 0x10
        constexpr std::array<uint8_t, 4> f_tri_L     = { 0x03, 0x30, 0x36, 0x03 }; // 0x11
        constexpr std::array<uint8_t, 14> f_arrow_UD = { 0x20, 0x02, LIFT, 0x20, 0x42, LIFT, 0x26, 0x04, LIFT, 0x26, 0x44, LIFT, 0x20, 0x26 }; // 0x12
        // 0x13 XON - using space
        // 0x14 DC4 - using space
        // 0x15 NAK - using space
        // 0x16 SYN - using space
        // 0x17 ETB - using space
        constexpr std::array<uint8_t, 8> f_arrow_U   = { 0x20, 0x02, LIFT, 0x20, 0x42, LIFT, 0x20, 0x26 }; // 0x18 CAN
        constexpr std::array<uint8_t, 8> f_arrow_D   = { 0x26, 0x04, LIFT, 0x26, 0x44, LIFT, 0x20, 0x26 }; // 0x19 EM
        constexpr std::array<uint8_t, 8> f_arrow_R   = { 0x43, 0x21, LIFT, 0x43, 0x25, LIFT, 0x03, 0x43 }; // 0x1A SUB
        constexpr std::array<uint8_t, 8> f_arrow_L   = { 0x03, 0x21, LIFT, 0x03, 0x25, LIFT, 0x03, 0x43 }; // 0x1B ESC
        // 0x1C FS - using space
        constexpr std::array<uint8_t, 14> f_arrow_LR = { 0x03, 0x11, LIFT, 0x03, 0x15, LIFT, 0x43, 0x31, LIFT, 0x43, 0x35, LIFT, 0x03, 0x43 }; // 0x1D GS
        constexpr std::array<uint8_t, 4> f_tri_U     = { 0x20, 0x46, 0x06, 0x20 }; // 0x1E RS
        constexpr std::array<uint8_t, 4> f_tri_D     = { 0x26, 0x00, 0x40, 0x26 }; // 0x1F US
        // ASCII character definitions (0x20 - 0x7E)
        constexpr std::array<uint8_t, 5> f_expt      = { 0x00, 0x04, LIFT, 0x06, LIFT }; // 0x21 !
        constexpr std::array<uint8_t, 5> f_quote2    = { 0x00, 0x02, LIFT, 0x10, 0x12 }; // 0x22 "
        constexpr std::array<uint8_t, 11> f_hash     = { 0x10, 0x16, LIFT, 0x30, 0x36, LIFT, 0x02, 0x42, LIFT, 0x04, 0x44 }; // 0x23 #
        constexpr std::array<uint8_t, 11> f_dollar   = { 0x41, 0x11, 0x02, 0x13, 0x33, 0x44, 0x35, 0x05, LIFT, 0x20, 0x26 }; // 0x24 $
        constexpr std::array<uint8_t, 7> f_pct       = { 0x21, 0x05, LIFT, 0x01, LIFT, 0x25, LIFT }; // 0x25 %
        constexpr std::array<uint8_t, 13> f_and      = { 0x31, 0x20, 0x10, 0x01, 0x02, 0x46, LIFT, 0x13, 0x04, 0x05, 0x16, 0x26, 0x44 }; // 0x26 &
        constexpr std::array<uint8_t, 2> f_quote1    = { 0x00, 0x02 }; // 0x27 '
        constexpr std::array<uint8_t, 4> f_lparen    = { 0x10, 0x01, 0x05, 0x16 }; // 0x28 (
        constexpr std::array<uint8_t, 4> f_rparen    = { 0x00, 0x11, 0x15, 0x06 }; // 0x29 )
        constexpr std::array<uint8_t, 11> f_star     = { 0x01, 0x45, LIFT, 0x21, 0x25, LIFT, 0x41, 0x05, LIFT, 0x03, 0x43 }; // 0x2A *
        constexpr std::array<uint8_t, 5> f_plus      = { 0x21, 0x25, LIFT, 0x03, 0x43 }; // 0x2B +
        constexpr std::array<uint8_t, 4> f_comma     = { 0x16, LIFT, 0x16, 0x07 }; // 0x2C ,
        constexpr std::array<uint8_t, 2> f_dash      = { 0x03, 0x43 }; // 0x2D -
        constexpr std::array<uint8_t, 2> f_dot       = { 0x06, LIFT }; // 0x2E .
        constexpr std::array<uint8_t, 2> f_slash     = { 0x40, 0x06 }; // 0x2F /
        constexpr std::array<uint8_t, 9> f_0         = { 0x10, 0x01, 0x05, 0x16, 0x36, 0x45, 0x41, 0x30, 0x10 }; // 0x30 0
        constexpr std::array<uint8_t, 6> f_1         = { 0x01, 0x10, 0x16, LIFT, 0x06, 0x26 }; // 0x31 1
        constexpr std::array<uint8_t, 7> f_2         = { 0x01, 0x10, 0x30, 0x41, 0x42, 0x06, 0x46 }; // 0x32 2
        constexpr std::array<uint8_t, 14> f_3        = { 0x01, 0x10, 0x30, 0x41, 0x42, 0x33, 0x23, LIFT, 0x33, 0x44, 0x45, 0x36, 0x16, 0x05 }; // 0x33 3
        constexpr std::array<uint8_t, 5> f_4         = { 0x36, 0x30, 0x03, 0x04, 0x44 }; // 0x34 4
        constexpr std::array<uint8_t, 9> f_5         = { 0x40, 0x00, 0x03, 0x33, 0x44, 0x45, 0x36, 0x16, 0x05 }; // 0x35 5
        constexpr std::array<uint8_t, 11> f_6        = { 0x30, 0x20, 0x02, 0x05, 0x16, 0x36, 0x45, 0x44, 0x33, 0x13, 0x04 }; // 0x36 6
        constexpr std::array<uint8_t, 3> f_7         = { 0x00, 0x40, 0x16 }; // 0x37 7
        constexpr std::array<uint8_t, 16> f_8        = { 0x13, 0x02, 0x01, 0x10, 0x30, 0x41, 0x42, 0x33, 0x44, 0x45, 0x36, 0x16, 0x05, 0x04, 0x13, 0x33 }; // 0x38 8
        constexpr std::array<uint8_t, 11> f_9        = { 0x42, 0x33, 0x13, 0x02, 0x01, 0x10, 0x30, 0x41, 0x44, 0x26, 0x16 }; // 0x39 9
        constexpr std::array<uint8_t, 4> f_colon     = { 0x03, LIFT, 0x06, LIFT }; // 0x3A :
        constexpr std::array<uint8_t, 6> f_semi      = { 0x13, LIFT, 0x16, LIFT, 0x16, 0x07 }; // 0x3B ;
        constexpr std::array<uint8_t, 3> f_less      = { 0x21, 0x03, 0x25 }; // 0x3C <
        constexpr std::array<uint8_t, 5> f_equal     = { 0x02, 0x42, LIFT, 0x04, 0x44 }; // 0x3D =
        constexpr std::array<uint8_t, 3> f_great     = { 0x01, 0x23, 0x05 }; // 0x3E >
        constexpr std::array<uint8_t, 9> f_question  = { 0x01, 0x10, 0x30, 0x41, 0x23, 0x24, LIFT, 0x26, LIFT }; // 0x3F ?
        constexpr std::array<uint8_t, 13> f_at       = { 0x33, 0x22, 0x13, 0x14, 0x25, 0x35, 0x32, 0x21, 0x11, 0x02, 0x05, 0x16, 0x36 }; // 0x40 @
        constexpr std::array<uint8_t, 6> f_A         = { 0x06, 0x20, 0x46, LIFT, 0x13, 0x33 }; // 0x41 A
        constexpr std::array<uint8_t, 11> f_B        = { 0x23, 0x32, 0x31, 0x20, 0x00, 0x06, 0x36, 0x45, 0x44, 0x33, 0x03 }; // 0x42 B
        constexpr std::array<uint8_t, 8> f_C         = { 0x41, 0x30, 0x10, 0x01, 0x05, 0x16, 0x36, 0x45 }; // 0x43 C
        constexpr std::array<uint8_t, 7> f_D         = { 0x00, 0x06, 0x26, 0x44, 0x42, 0x20, 0x00 }; // 0x44 D
        constexpr std::array<uint8_t, 7> f_E         = { 0x40, 0x00, 0x06, 0x46, LIFT, 0x03, 0x33 }; // 0x45 E
        constexpr std::array<uint8_t, 6> f_F         = { 0x40, 0x00, 0x06, LIFT, 0x03, 0x33 }; // 0x46 F
        constexpr std::array<uint8_t, 10> f_G        = { 0x41, 0x30, 0x10, 0x01, 0x05, 0x16, 0x36, 0x45, 0x43, 0x23 }; // 0x47 G
        constexpr std::array<uint8_t, 8> f_H         = { 0x00, 0x06, LIFT, 0x40, 0x46, LIFT, 0x03, 0x43 }; // 0x48 H
        constexpr std::array<uint8_t, 8> f_I         = { 0x10, 0x30, LIFT, 0x16, 0x36, LIFT, 0x20, 0x26 }; // 0x49 I
        constexpr std::array<uint8_t, 6> f_J         = { 0x30, 0x40, 0x45, 0x36, 0x16, 0x05 }; // 0x4A J
        constexpr std::array<uint8_t, 7> f_K         = { 0x00, 0x06, LIFT, 0x30, 0x03, 0x13, 0x46 }; // 0x4B K
        constexpr std::array<uint8_t, 3> f_L         = { 0x00, 0x06, 0x46 }; // 0x4C L
        constexpr std::array<uint8_t, 5> f_M         = { 0x06, 0x00, 0x24, 0x40, 0x46 }; // 0x4D M
        constexpr std::array<uint8_t, 4> f_N         = { 0x06, 0x00, 0x46, 0x40 }; // 0x4E N
        constexpr std::array<uint8_t, 9> f_O         = { 0x10, 0x01, 0x05, 0x16, 0x36, 0x45, 0x41, 0x30, 0x10 }; // 0x4F O
        constexpr std::array<uint8_t, 7> f_P         = { 0x06, 0x00, 0x30, 0x41, 0x42, 0x33, 0x03 }; // 0x50 P
        constexpr std::array<uint8_t, 12> f_Q        = { 0x10, 0x01, 0x05, 0x16, 0x36, 0x45, 0x41, 0x30, 0x10, LIFT, 0x24, 0x46 }; // 0x51 Q
        constexpr std::array<uint8_t, 10> f_R        = { 0x06, 0x00, 0x30, 0x41, 0x42, 0x33, 0x03, LIFT, 0x13, 0x46 }; // 0x52 R
        constexpr std::array<uint8_t, 12> f_S        = { 0x40, 0x30, 0x10, 0x01, 0x02, 0x13, 0x33, 0x44, 0x45, 0x36, 0x16, 0x06 }; // 0x53 S
        constexpr std::array<uint8_t, 5> f_T         = { 0x00, 0x40, LIFT, 0x20, 0x26 }; // 0x54 T
        constexpr std::array<uint8_t, 6> f_U         = { 0x00, 0x05, 0x16, 0x36, 0x45, 0x40 }; // 0x55 U
        constexpr std::array<uint8_t, 3> f_V         = { 0x00, 0x26, 0x40 }; // 0x56 V
        constexpr std::array<uint8_t, 5> f_W         = { 0x00, 0x06, 0x23, 0x46, 0x40 }; // 0x57 W
        constexpr std::array<uint8_t, 5> f_X         = { 0x00, 0x46, LIFT, 0x40, 0x06 }; // 0x58 X
        constexpr std::array<uint8_t, 6> f_Y         = { 0x00, 0x23, 0x26, LIFT, 0x40, 0x23 }; // 0x59 Y
        constexpr std::array<uint8_t, 4> f_Z         = { 0x00, 0x40, 0x06, 0x46 }; // 0x5A Z
        constexpr std::array<uint8_t, 4> f_lbrack    = { 0x20, 0x00, 0x06, 0x26 }; // 0x5B [
        constexpr std::array<uint8_t, 2> f_bslash    = { 0x00, 0x46 }; // 0x5C ''
        constexpr std::array<uint8_t, 4> f_rbrack    = { 0x00, 0x20, 0x26, 0x06 }; // 0x5D ]
        constexpr std::array<uint8_t, 3> f_carat     = { 0x02, 0x11, 0x22 }; // 0x5E ^
        constexpr std::array<uint8_t, 2> f_under     = { 0x06, 0x46 }; // 0x5F _
        constexpr std::array<uint8_t, 2> f_acute     = { 0x00, 0x22 }; // 0x60 `
        constexpr std::array<uint8_t, 8> f_a         = { 0x35, 0x26, 0x16, 0x05, 0x04, 0x13, 0x33, 0x36 }; // 0x61 a
        constexpr std::array<uint8_t, 7> f_b         = { 0x00, 0x06, 0x26, 0x35, 0x34, 0x23, 0x03 }; // 0x62 b
        constexpr std::array<uint8_t, 6> f_c         = { 0x33, 0x13, 0x04, 0x05, 0x16, 0x36 }; // 0x63 c
        constexpr std::array<uint8_t, 7> f_d         = { 0x33, 0x13, 0x04, 0x05, 0x16, 0x36, 0x30 }; // 0x64 d
        constexpr std::array<uint8_t, 8> f_e         = { 0x05, 0x34, 0x23, 0x13, 0x04, 0x05, 0x16, 0x36 }; // 0x65 e
        constexpr std::array<uint8_t, 7> f_f         = { 0x30, 0x20, 0x11, 0x16, LIFT, 0x03, 0x23 }; // 0x66 f
        constexpr std::array<uint8_t, 9> f_g         = { 0x36, 0x16, 0x05, 0x04, 0x13, 0x33, 0x37, 0x28, 0x08 }; // 0x67 g
        constexpr std::array<uint8_t, 7> f_h         = { 0x00, 0x06, LIFT, 0x03, 0x23, 0x34, 0x36 }; // 0x68 h
        constexpr std::array<uint8_t, 4> f_i         = { 0x01, LIFT, 0x03, 0x06 }; // 0x69 i
        constexpr std::array<uint8_t, 6> f_j         = { 0x21, LIFT, 0x23, 0x27, 0x18, 0x08 }; // 0x6A j
        constexpr std::array<uint8_t, 7> f_k         = { 0x00, 0x06, LIFT, 0x22, 0x04, 0x14, 0x36 }; // 0x6B k
        constexpr std::array<uint8_t, 2> f_l         = { 0x00, 0x06 }; // 0x6C l
        constexpr std::array<uint8_t, 10> f_m        = { 0x06, 0x03, 0x13, 0x24, LIFT, 0x25, 0x23, 0x33, 0x44, 0x46 }; // 0x6D m
        constexpr std::array<uint8_t, 8> f_n         = { 0x03, 0x06, LIFT, 0x04, 0x13, 0x23, 0x34, 0x36 }; // 0x6E n
        constexpr std::array<uint8_t, 9> f_o         = { 0x13, 0x04, 0x05, 0x16, 0x26, 0x35, 0x34, 0x23, 0x13 }; // 0x6F o
        constexpr std::array<uint8_t, 8> f_p         = { 0x08, 0x03, 0x23, 0x34, 0x35, 0x26, 0x16, 0x05 }; // 0x70 p
        constexpr std::array<uint8_t, 12> f_q        = { 0x35, 0x26, 0x16, 0x05, 0x04, 0x13, 0x23, 0x34, LIFT, 0x33, 0x38, 0x47 }; // 0x71 q
        constexpr std::array<uint8_t, 7> f_r         = { 0x03, 0x06, LIFT, 0x04, 0x13, 0x23, 0x34 }; // 0x72 r
        constexpr std::array<uint8_t, 6> f_s         = { 0x33, 0x13, 0x04, 0x35, 0x26, 0x06 }; // 0x73 s
        constexpr std::array<uint8_t, 6> f_t         = { 0x11, 0x15, 0x26, LIFT, 0x03, 0x23 }; // 0x74 t
        constexpr std::array<uint8_t, 8> f_u         = { 0x03, 0x05, 0x16, 0x26, 0x35, LIFT, 0x33, 0x36 }; // 0x75 u
        constexpr std::array<uint8_t, 4> f_v         = { 0x03, 0x16, 0x26, 0x33 }; // 0x76 v
        constexpr std::array<uint8_t, 10> f_w        = { 0x03, 0x05, 0x16, 0x25, 0x24, LIFT, 0x25, 0x36, 0x45, 0x43 }; // 0x77 w
        constexpr std::array<uint8_t, 5> f_x         = { 0x03, 0x36, LIFT, 0x33, 0x06 }; // 0x78 x
        constexpr std::array<uint8_t, 11> f_y        = { 0x03, 0x05, 0x16, 0x26, 0x35, LIFT, 0x33, 0x37, 0x28, 0x18, 0x07 }; // 0x79 y
        constexpr std::array<uint8_t, 4> f_z         = { 0x03, 0x33, 0x06, 0x36 }; // 0x7A z
        constexpr std::array<uint8_t, 7> f_lbrace    = { 0x20, 0x11, 0x13, 0x04, 0x15, 0x17, 0x28 }; // 0x7B {
        constexpr std::array<uint8_t, 2> f_bar       = { 0x00, 0x08 }; // 0x7C |
        constexpr std::array<uint8_t, 7> f_rbrace    = { 0x00, 0x11, 0x13, 0x24, 0x15, 0x17, 0x08 }; // 0x7D }
        constexpr std::array<uint8_t, 5> f_tilde     = { 0x10, 0x21, 0x12, 0x01, 0x10 }; // 0x7E ~
        constexpr std::array<uint8_t, 1> f_del       = { LIFT }; // 0x7F DEL

        constexpr uint8_t findCharWidth(std::span<const uint8_t> char_points) {
            if (char_points.empty()) { // Should not happen with current definitions
                return VectorFont::CHAR_WIDTH;
            }
            // Special case for a single LIFT instruction (e.g. our _space)
            if (char_points.size() == 1 && char_points[0] == LIFT) {
                return VectorFont::CHAR_WIDTH;
            }

            uint8_t max_x = 0;
            bool has_visible_points = false;
            for (uint8_t p : char_points) {
                if (p != LIFT) {
                    has_visible_points = true;
                    uint8_t x_coord = (p >> 4) & 0x0F;
                    if (x_coord > max_x) {
                        max_x = x_coord;
                    }
                }
            }
            if (!has_visible_points) { // Only LIFT points, or empty after all
                return VectorFont::CHAR_WIDTH;
            }
            return max_x + 1; // width is rightmost point's X-coordinate + 1 grid unit
        }
    } // detail

    // mfc is short for makeFontChar
    constexpr FontChar mfc(std::span<const uint8_t> points_data) {
        return {points_data, detail::findCharWidth(points_data)};
    }
    // --- Public Font Character Lookup Table ---
    // Use std::array for the main lookup table.
    // Initialize FontChar using aggregate initialization; std::span can be
    // implicitly constructed from std::array.
    constexpr std::array<FontChar, FONT_CHAR_COUNT> font_chars = {
        mfc(detail::f_space),    mfc(detail::f_happy),    mfc(detail::f_sad),      mfc(detail::f_heart),      // 00-03
        mfc(detail::f_diamond),  mfc(detail::f_club),     mfc(detail::f_spade),    mfc(detail::f_bullet),     // 04-07
        mfc(detail::f_space),    mfc(detail::f_circle),   mfc(detail::f_space),    mfc(detail::f_space),      // 08-0B (BS, TAB)
        mfc(detail::f_space),    mfc(detail::f_space),    mfc(detail::f_space),    mfc(detail::f_space),      // 0C-0F
        mfc(detail::f_tri_R),    mfc(detail::f_tri_L),    mfc(detail::f_arrow_UD), mfc(detail::f_space),      // 10-13
        mfc(detail::f_space),    mfc(detail::f_space),    mfc(detail::f_space),    mfc(detail::f_space),      // 14-17
        mfc(detail::f_arrow_U),  mfc(detail::f_arrow_D),  mfc(detail::f_arrow_R),  mfc(detail::f_arrow_L),    // 18-1B (CAN,EM,SUB,ESC)
        mfc(detail::f_space),    mfc(detail::f_arrow_LR), mfc(detail::f_tri_U),    mfc(detail::f_tri_D),      // 1C-1F
        // ASCII (0x20 - 0x7F)
        mfc(detail::f_space),  mfc(detail::f_expt),   mfc(detail::f_quote2), mfc(detail::f_hash),      // 20-23
        mfc(detail::f_dollar), mfc(detail::f_pct),    mfc(detail::f_and),    mfc(detail::f_quote1),     // 24-27
        mfc(detail::f_lparen), mfc(detail::f_rparen), mfc(detail::f_star),   mfc(detail::f_plus),       // 28-2B
        mfc(detail::f_comma),  mfc(detail::f_dash),   mfc(detail::f_dot),    mfc(detail::f_slash),      // 2C-2F
        mfc(detail::f_0),      mfc(detail::f_1),      mfc(detail::f_2),      mfc(detail::f_3),          // 30-33
        mfc(detail::f_4),      mfc(detail::f_5),      mfc(detail::f_6),      mfc(detail::f_7),          // 34-37
        mfc(detail::f_8),      mfc(detail::f_9),      mfc(detail::f_colon),  mfc(detail::f_semi),       // 38-3B
        mfc(detail::f_less),   mfc(detail::f_equal),  mfc(detail::f_great),  mfc(detail::f_question),   // 3C-3F
        mfc(detail::f_at),     mfc(detail::f_A),      mfc(detail::f_B),      mfc(detail::f_C),          // 40-43
        mfc(detail::f_D),      mfc(detail::f_E),      mfc(detail::f_F),      mfc(detail::f_G),          // 44-47
        mfc(detail::f_H),      mfc(detail::f_I),      mfc(detail::f_J),      mfc(detail::f_K),          // 48-4B
        mfc(detail::f_L),      mfc(detail::f_M),      mfc(detail::f_N),      mfc(detail::f_O),          // 4C-4F
        mfc(detail::f_P),      mfc(detail::f_Q),      mfc(detail::f_R),      mfc(detail::f_S),          // 50-53
        mfc(detail::f_T),      mfc(detail::f_U),      mfc(detail::f_V),      mfc(detail::f_W),          // 54-57
        mfc(detail::f_X),      mfc(detail::f_Y),      mfc(detail::f_Z),      mfc(detail::f_lbrack),     // 58-5B
        mfc(detail::f_bslash), mfc(detail::f_rbrack), mfc(detail::f_carat),  mfc(detail::f_under),      // 5C-5F
        mfc(detail::f_acute),  mfc(detail::f_a),      mfc(detail::f_b),      mfc(detail::f_c),          // 60-63
        mfc(detail::f_d),      mfc(detail::f_e),      mfc(detail::f_f),      mfc(detail::f_g),          // 64-67
        mfc(detail::f_h),      mfc(detail::f_i),      mfc(detail::f_j),      mfc(detail::f_k),          // 68-6B
        mfc(detail::f_l),      mfc(detail::f_m),      mfc(detail::f_n),      mfc(detail::f_o),          // 6C-6F
        mfc(detail::f_p),      mfc(detail::f_q),      mfc(detail::f_r),      mfc(detail::f_s),          // 70-73
        mfc(detail::f_t),      mfc(detail::f_u),      mfc(detail::f_v),      mfc(detail::f_w),          // 74-77
        mfc(detail::f_x),      mfc(detail::f_y),      mfc(detail::f_z),      mfc(detail::f_lbrace),     // 78-7B
        mfc(detail::f_bar),    mfc(detail::f_rbrace), mfc(detail::f_tilde),  mfc(detail::f_del),        // 7C-7F
    };

    constexpr const FontChar& getCharDef(uint8_t code) {
        if (code >= FONT_CHAR_MIN && code <= FONT_CHAR_MAX) {
            return font_chars[code - FONT_CHAR_MIN];
        } else {
            // Return definition for space 
            return font_chars[0x20 - FONT_CHAR_MIN];
        }
    }
} // VectorFont

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

    void drawText(const char *text, float tx, float ty, float scale, uint32_t color)
    {
        int thickness = (int)scale;
        if (scale <= 0.0f) return; // Scale must be positive
        if (JADRAW_ALPHA(color) == 0) return; // Fully transparent, nothing to draw

        float current_x = tx;
        float current_y = ty;

        // Calculate the size of one grid unit in pixels based on scale
        float unit_size_px = scale; // Simpler: scale multiplies the base grid unit size

        // Calculate advance width and line height in pixels
        float advance_y = VectorFont::CHAR_HEIGHT * unit_size_px;

        for (size_t i = 0; text[i] != '\0'; ++i) {
            unsigned char c = (unsigned char)text[i];

            // Handle newline character
            if (c == '\n') {
                current_x = tx; // Reset X to the starting X
                current_y += advance_y; // Move Y down by one line height
                continue;
            }
            // Handle carriage return (often ignored if LF follows, but good practice)
            if (c == '\r') {
                current_x = tx; // Reset X to the starting X
                continue;
            }
            // Get the character definition
            const VectorFont::FontChar& fontchar = VectorFont::getCharDef(c);
            float advance_x = fontchar.width * unit_size_px;

            bool last_point_valid = false;
            float last_sx = 0, last_sy = 0;

            // Iterate through the points defining the character
            for (size_t pt_idx = 0; pt_idx < fontchar.size(); ++pt_idx) {
                uint8_t p = fontchar.points[pt_idx];

                if (p == VectorFont::LIFT) {
                    last_point_valid = false; // Lift the pen, break the line
                } else {
                    // Decode grid coordinates (High nibble X, Low nibble Y)
                    int fx = (p >> 4) & 0x0F;
                    int fy = p & 0x0F;

                    // Scale coordinates to screen pixels relative to the character origin (current_x, current_y)
                    float sx = current_x + (float)fx * unit_size_px;
                    float sy = current_y + (float)fy * unit_size_px;

                    if (last_point_valid) {
                        // Draw a line from the last point to the current point
                        drawLineAA(last_sx, last_sy, sx, sy, color);
                    } else {
                        // This is the first point after a LIFT or the start of the character data.
                        // Check if it's a standalone point (i.e., the next item is LIFT or end of data)
                        if (pt_idx + 1 >= fontchar.size() || fontchar.points[pt_idx + 1] == VectorFont::LIFT) {
                            drawPixel((int)sx, (int)sy, color);
                        }
                        // If it's the start of a line segment (next point is not LIFT/end),
                        // we don't draw the point explicitly here. The olivec_line call
                        // in the *next* iteration will draw the line *starting* from this point.
                    }

                    // Update the last point for the next potential line segment
                    last_sx = sx;
                    last_sy = sy;
                    last_point_valid = true;
                }
            }

            // Advance cursor position for the next character
            current_x += advance_x + 1;
        }
    }

}; // JaDraw<W, H>


#endif // JADRAW_H