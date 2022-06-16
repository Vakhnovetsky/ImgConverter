#include <algorithm>
#include <cmath>
#include <numeric>
#include <img_lib.h>
#include <iostream>
#include <string_view>
#include <random>

using namespace std;

// Получение равномерно распределённого числа от 0 до max - 1.
// В отличие от uniform_int_distribution его результат будет одинаковым на всех платформах
template <typename Engine>
uint32_t GetRandomNumber(uint32_t max, Engine& engine) {
    uint32_t engine_max = static_cast<uint32_t>(engine.max());

    if (max == engine_max) {
        return static_cast<uint32_t>(engine());
    }

    while (true) {
        uint32_t val = static_cast<uint32_t>(engine());

        if (val / max < engine_max / max || engine_max % max == max - 1u) {
            return val % max;
        }
    }
}

// Свой Shuffle. В отличие от std::shuffle он будет работать одинаково везде
template <typename RandomIt, typename Engine>
void Shuffle(RandomIt first, RandomIt last, Engine& engine) {
    using IterDiff = typename std::iterator_traits<RandomIt>::difference_type;
    IterDiff i, n;
    n = last - first;
    for (i = n - 1; i > 0; --i) {
        swap(first[i], first[GetRandomNumber(static_cast<uint32_t>(i + 1), engine)]);
    }
}

void CypherInplace(unsigned int seed, img_lib::Image& image) {
    mt19937 generator(seed);
    const int area = image.GetWidth() * image.GetHeight();

    // координаты пикселя по индексу
    auto px_num_to_coordinates = [&image](int num) {
        return std::pair{num % image.GetWidth(), num / image.GetWidth()};
    };

    // сложим индексы половины пикселей в вектор и перемешаем их
    std::vector<int> positions(area / 2);
    std::iota(positions.begin(), positions.end(), 0);
    Shuffle(positions.begin(), positions.end(), generator);

    // Будем переставлять чётные и нечётные пиксели между собой.
    // Благодаря этому процедура шифрования и дешифрования будет одинаковой.
    for (int px_num = 0; px_num < area/2; ++px_num) {
        auto [src_x, src_y] = px_num_to_coordinates(px_num * 2);
        auto [dst_x, dst_y] = px_num_to_coordinates(positions[px_num] * 2 + 1);

        std::swap(image.GetPixel(src_x, src_y), image.GetPixel(dst_x, dst_y));
    }
}

void NegateInplace(img_lib::Image& image) {
	for (int y = 0; y < image.GetHeight(); ++y) {
		auto line = image.GetLine(y);
		for (int x = 0; x < image.GetWidth(); ++x) {
			line[x].r = byte(255 - std::to_integer<int>(line[x].r));
			line[x].g = byte(255 - std::to_integer<int>(line[x].g));
			line[x].b = byte(255 - std::to_integer<int>(line[x].b));
		}
	}
}

void VMirrInplace(img_lib::Image& image) {
    int start = 0;
    int finish = image.GetHeight()-1;
    while(start < finish) {
        auto line_s = image.GetLine(start);
        auto line_f = image.GetLine(finish);
        
        for (int x = 0; x < image.GetWidth(); ++x) {
            swap(line_s[x].r, line_f[x].r);
            swap(line_s[x].g, line_f[x].g);
            swap(line_s[x].b, line_f[x].b);
        }
		++start;
        --finish;
    }
}

void HMirrInplace(img_lib::Image& image) {
    for (int y = 0; y < image.GetHeight(); ++y) {
		auto line = image.GetLine(y);
		int start = 0;
		int finish = image.GetWidth()-1;
		while(start < finish) {
			swap(line[start].r, line[finish].r);
			swap(line[start].g, line[finish].g);
			swap(line[start].b, line[finish].b);
			++start;
			--finish;
		}
	}
}

img_lib::Image Sobel(const img_lib::Image& image) {
    img_lib::Image result(image.GetWidth(), image.GetHeight(), img_lib::Color::Black());

    for (int y = 1; y < image.GetHeight()-1; ++y) {
        auto line1 = image.GetLine(y-1);
        auto line2 = image.GetLine(y);
        auto line3 = image.GetLine(y+1);
        img_lib::Color* line = result.GetLine(y);

        for (int x = 1; x < image.GetWidth() - 1; ++x) {
            //сумма цветовых компонент
            int tl = static_cast<int>(line1[x-1].r) + static_cast<int>(line1[x - 1].g) + static_cast<int>(line1[x - 1].b);
            int tc = static_cast<int>(line1[x].r) + static_cast<int>(line1[x].g) + static_cast<int>(line1[x].b);
            int tr = static_cast<int>(line1[x + 1].r) + static_cast<int>(line1[x + 1].g) + static_cast<int>(line1[x + 1].b);

            int cl = static_cast<int>(line2[x - 1].r) + static_cast<int>(line2[x - 1].g) + static_cast<int>(line2[x - 1].b);
            int cr = static_cast<int>(line2[x + 1].r) + static_cast<int>(line2[x + 1].g) + static_cast<int>(line2[x + 1].b);

            int bl = static_cast<int>(line3[x - 1].r) + static_cast<int>(line3[x - 1].g) + static_cast<int>(line3[x - 1].b);
            int bc = static_cast<int>(line3[x].r) + static_cast<int>(line3[x].g) + static_cast<int>(line3[x].b);
            int br = static_cast<int>(line3[x + 1].r) + static_cast<int>(line3[x + 1].g) + static_cast<int>(line3[x + 1].b);

            int gx = -tl - 2 * tc - tr + bl + 2 * bc + br;
            int gy = -tl - 2 * cl - bl + tr + 2 * cr + br;

            double sobel = std::sqrt(gx * gx + gy * gy);

            //std::clamp<double>(sobel);

            std::byte res = static_cast<std::byte>(std::clamp<double>(sobel, 0, 255));

            line[x].r = res;
            line[x].g = res;
            line[x].b = res;
        }
    }
    return result;
}