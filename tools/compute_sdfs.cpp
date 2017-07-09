#include <lodepng.h>
#include <cstdint>
#include <string>
#include <cmath>
#include <iostream>

static const uint32_t DOWNSCALE = 8;

class Image
{
    std::vector<uint8_t> m_data;
    uint32_t m_width;
    uint32_t m_height;

public:
    uint32_t width()  { return m_width;  }
    uint32_t height() { return m_height; }

    Image(const std::string& filename) {
        lodepng::decode(m_data, m_width, m_height, filename);
    }

    Image(uint32_t width, uint32_t height) {
        m_data.resize(width * height * 4);
        m_width = width;
        m_height = height;
    }

    void save_to_file(const std::string& filename) {
        lodepng::encode(filename, m_data, m_width, m_height);
    }

    void find_edge(int32_t x, int32_t y, int32_t& out_x, int32_t& out_y) {
        if (is_pixel_on(x, y, 0)) {
            out_x = x;
            out_y = y;
            return;
        }

        auto test = [&](int32_t x, int32_t y) {
            if (is_pixel_on(x, y, 0)) {
                out_x = x;
                out_y = y;
                return true;
            }
            return false;
        };

        for (int32_t radius = 1; radius < 128; ++radius) {
            if (test(x + radius, y)) return;
            if (test(x - radius, y)) return;
            if (test(x, y + radius)) return;
            if (test(x, y - radius)) return;
            for (auto scan = 1; scan <= radius; ++scan) {
                if (test(x + radius, y + scan)) return;
                if (test(x + radius, y - scan)) return;
                if (test(x - radius, y + scan)) return;
                if (test(x - radius, y - scan)) return;
                if (test(x + scan, y + radius)) return;
                if (test(x + scan, y - radius)) return;
                if (test(x - scan, y + radius)) return;
                if (test(x - scan, y - radius)) return;
            }
        }

        out_x = x + 128;
        out_y = y + 128;
    }

    int8_t get_distance_to_edge(int32_t x, int32_t y) {
        int32_t out_x, out_y;
        find_edge(x, y, out_x, out_y);
        float dx = static_cast<float>(x) - static_cast<float>(out_x);
        float dy = static_cast<float>(y) - static_cast<float>(out_y);
        float d = sqrtf(dx*dx + dy*dy);
        if (d < -100.0f) d = -100.0f;
        if (d >  100.0f) d =  100.0f;
        return d;
    }

    bool is_pixel_on(int32_t x, int32_t y, size_t field = 3) {
        if (x < 0 || y < 0 || x >= m_width || y >= m_width) return false;
        return m_data[4*(x + m_width*y) + field] >= 127;
    }

    bool is_pixel_edge(int32_t x, int32_t y) {
        if (!is_pixel_on(x, y)) return false;
        if (x < 1 || x >= m_width - 1 || y < 1 || y >= m_height - 1) return true;
        if (!is_pixel_on(x - 1, y)) return true;
        if (!is_pixel_on(x + 1, y)) return true;
        if (!is_pixel_on(x, y - 1)) return true;
        if (!is_pixel_on(x, y + 1)) return true;
        return false;
    }

    void paint_pixel(int32_t x, int32_t y, uint8_t shade) {
        uint32_t i = 4*(x + m_width*y);
        m_data[i  ] = shade;
        m_data[i+1] = shade;
        m_data[i+2] = shade;
        m_data[i+3] = 255;
    }
};

static uint8_t distance_to_shade(int8_t d)
{
    return 127 + d;
}

int main(int argc, char **argv)
{
    Image input("../res/sprites/guy.png");
    Image edges(input.width(), input.height());
    Image output(input.width() / DOWNSCALE, input.height() / DOWNSCALE);

    for (auto y = 0; y < input.height(); ++y) {
        for (auto x = 0; x < input.width(); ++x) {
            auto shade = input.is_pixel_edge(x, y) ? 255 : 0;
            edges.paint_pixel(x, y, shade);
        }
    }

    for (auto y = 0; y < output.height(); ++y) {
        for (auto x = 0; x < output.width(); ++x) {
            auto dist = edges.get_distance_to_edge(DOWNSCALE * x, DOWNSCALE * y);
            if (! input.is_pixel_on(DOWNSCALE * x, DOWNSCALE * y)) dist *= -1;
            output.paint_pixel(x, y, distance_to_shade(dist));
        }
    }

    output.save_to_file("../res/sprites/guy.sdf.png");
    return 0;
}
