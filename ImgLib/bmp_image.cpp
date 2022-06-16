#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {

	PACKED_STRUCT_BEGIN BitmapFileHeader {
		char c1 = 'B'; 
		char c2 = 'M'; 
		uint32_t size; //4 byte
		uint32_t reserve_space = 0; //4 byte
		uint32_t indent = 54; //4 byte
	}
	PACKED_STRUCT_END

	PACKED_STRUCT_BEGIN BitmapInfoHeader {
		uint32_t size = 40; //4 byte
		uint32_t width; //4 byte
		uint32_t height; //4 byte
		uint16_t n_layers = 1; //2 byte
		uint16_t bites = 24; //2 byte
		uint32_t compression = 0; //4 byte
		uint32_t indent; //4 byte
		int32_t vertical_resolution = 11811; //4 byte 300 DPI.
		int32_t horizontal_resolution = 11811; //4 byte 300 DPI.
		int32_t used_colors = 0; //4 byte
		int32_t colors = 0x1000000; //4 byte
	}
	PACKED_STRUCT_END

	//calculate indent by width
	static int GetBMPStride(int w) {
		return 4 * ((w * 3 + 3) / 4);
	}

	bool SaveBMP(const Path& file, const Image& image) {
		ofstream out(file, ios::binary);
		
		img_lib::BitmapFileHeader bfh;

		out.write(reinterpret_cast<const char*>(&bfh.c1), 1);
		out.write(reinterpret_cast<const char*>(&bfh.c2), 1);

		bfh.size = 54 + GetBMPStride(image.GetWidth()) * image.GetHeight();
		out.write(reinterpret_cast<const char*>(&bfh.size), sizeof(bfh.size));
		out.write(reinterpret_cast<const char*>(&bfh.reserve_space), sizeof(bfh.reserve_space));	
		out.write(reinterpret_cast<const char*>(&bfh.indent), sizeof(bfh.indent));
		
		img_lib::BitmapInfoHeader bih;
		out.write(reinterpret_cast<const char*>(&bih.size), sizeof(bih.size));
		
		bih.width = image.GetWidth();
		bih.height = image.GetHeight();
		out.write(reinterpret_cast<const char*>(&bih.width), sizeof(bih.width));
		out.write(reinterpret_cast<const char*>(&bih.height), sizeof(bih.height));
		
		out.write(reinterpret_cast<const char*>(&bih.n_layers), sizeof(bih.n_layers));
		out.write(reinterpret_cast<const char*>(&bih.bites), sizeof(bih.bites));
		
		out.write(reinterpret_cast<const char*>(&bih.compression), sizeof(bih.compression));
		
		bih.indent = GetBMPStride(image.GetWidth()) * image.GetHeight();
		out.write(reinterpret_cast<const char*>(&bih.indent), sizeof(bih.indent));
		
		out.write(reinterpret_cast<const char*>(&bih.vertical_resolution), sizeof(bih.vertical_resolution));
		out.write(reinterpret_cast<const char*>(&bih.horizontal_resolution), sizeof(bih.horizontal_resolution));
		
		out.write(reinterpret_cast<const char*>(&bih.used_colors), sizeof(bih.used_colors));
		out.write(reinterpret_cast<const char*>(&bih.colors), sizeof(bih.colors));
		
		for (int y = bih.height-1; y >= 0; --y) {
			const Color* line = image.GetLine(y);
			std::vector<char> buff(GetBMPStride(bih.width));
			for (int x = 0; x < bih.width; ++x) {
				buff[x * 3 + 0] = static_cast<char>(line[x].b);
				buff[x * 3 + 1] = static_cast<char>(line[x].g);
				buff[x * 3 + 2] = static_cast<char>(line[x].r);
			}
			out.write(buff.data(), buff.size());
		}
		
		return out.good();
	}

	Image LoadBMP(const Path& file) {
		ifstream ifs(file, ios::binary);
		
		img_lib::BitmapFileHeader bfh;
		img_lib::BitmapInfoHeader bih;
		
		ifs.read(reinterpret_cast<char*>(&bfh.c1), 1);
		ifs.read(reinterpret_cast<char*>(&bfh.c2), 1);
		ifs.read(reinterpret_cast<char*>(&bfh.size), sizeof(bfh.size));
		ifs.read(reinterpret_cast<char*>(&bfh.reserve_space), sizeof(bfh.reserve_space));	
		ifs.read(reinterpret_cast<char*>(&bfh.indent), sizeof(bfh.indent));
		
		ifs.read(reinterpret_cast<char*>(&bih.size), sizeof(bih.size));
		ifs.read(reinterpret_cast<char*>(&bih.width), sizeof(bih.width));
		ifs.read(reinterpret_cast<char*>(&bih.height), sizeof(bih.height));
		ifs.read(reinterpret_cast<char*>(&bih.n_layers), sizeof(bih.n_layers));
		ifs.read(reinterpret_cast<char*>(&bih.bites), sizeof(bih.bites));
		ifs.read(reinterpret_cast<char*>(&bih.compression), sizeof(bih.compression));
		ifs.read(reinterpret_cast<char*>(&bih.indent), sizeof(bih.indent));
		ifs.read(reinterpret_cast<char*>(&bih.vertical_resolution), sizeof(bih.vertical_resolution));
		ifs.read(reinterpret_cast<char*>(&bih.horizontal_resolution), sizeof(bih.horizontal_resolution));
		ifs.read(reinterpret_cast<char*>(&bih.used_colors), sizeof(bih.used_colors));
		ifs.read(reinterpret_cast<char*>(&bih.colors), sizeof(bih.colors));
		
		Image result(bih.width, bih.height, Color::Black());
		
		for (int y = bih.height - 1; y >= 0; --y) {
			Color* line = result.GetLine(y);
			std::vector<char> buff(GetBMPStride(bih.width));
			
			ifs.read(buff.data(), GetBMPStride(bih.width));

			for (int x = 0; x < bih.width; ++x) {
				line[x].b = static_cast<byte>(buff[x * 3 + 0]);
				line[x].g = static_cast<byte>(buff[x * 3 + 1]);
				line[x].r = static_cast<byte>(buff[x * 3 + 2]);
			}
		}
		
		return result;		
	}

}  // namespace img_lib