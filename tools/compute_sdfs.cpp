#include <lodepng.h>
#include <string>
#include <iostream>

static const unsigned int DOWNSCALE = 2;

class Image 
{
    std::vector<unsigned char> m_data;
    unsigned int m_width;
    unsigned int m_height;

public:
    unsigned int width()  { return m_width;  }
    unsigned int height() { return m_height; }

    Image(const std::string& filename) {
        lodepng::decode(m_data, m_width, m_height, filename);
    }

    Image(unsigned int width, unsigned int height) {
        m_data.resize(width * height * 4);
        m_width = width;
        m_height = height;
    }

    void save_to_file(const std::string& filename) {
        lodepng::encode(filename, m_data, m_width, m_height);
    }
};

int main(int argc, char **argv) 
{
    Image input("../res/sprites/guy_frames.png");
    Image output(input.width() / DOWNSCALE, input.height() / DOWNSCALE);

    output.save_to_file("./out.png");

    return 0;
}