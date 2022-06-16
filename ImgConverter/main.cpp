#include <actions.h>
#include <img_lib.h>
#include <bmp_image.h>
#include <jpeg_image.h>
#include <ppm_image.h>

#include <filesystem>
#include <string_view>
#include <iostream>

using namespace std;

enum class Format {
	JPEG,
	PPM,
	BMP,
	UNKNOWN
};

Format GetFormatByExtension(const img_lib::Path& input_file) {
	const string ext = input_file.extension().string();
	if (ext == ".jpg"sv || ext == ".jpeg"sv) {
		return Format::JPEG;
	}

	if (ext == ".ppm"sv) {
		return Format::PPM;
	}
	
	if (ext == ".bmp"sv) {
        return Format::BMP;
    }

	return Format::UNKNOWN;
}

class ImageFormatInterface {
public:
	virtual bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const = 0;
	virtual img_lib::Image LoadImage(const img_lib::Path& file) const = 0;
}; 

namespace format_interfaces {
	class ImageFormatPPM: public ImageFormatInterface {
	public:
		bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
			return img_lib::SavePPM(file, image);
		}
		
		img_lib::Image LoadImage(const img_lib::Path& file) const override {
			return img_lib::LoadPPM(file);
		}
	};

	class ImageFormatJPEG: public ImageFormatInterface {
	public:
		bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
			return img_lib::SaveJPEG(file, image);
		}
		
		img_lib::Image LoadImage(const img_lib::Path& file) const override {
			return img_lib::LoadJPEG(file);
		}
	};
	
	class ImageFormatBMP: public ImageFormatInterface {
	public:
		bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
			return img_lib::SaveBMP(file, image);
		}
		
		img_lib::Image LoadImage(const img_lib::Path& file) const override {
			return img_lib::LoadBMP(file);
		}
	};
} //namespace format_interfaces

ImageFormatInterface* GetFormatInterface(const img_lib::Path& path) {
	switch(GetFormatByExtension(path)){
		case Format::JPEG:
			static format_interfaces::ImageFormatJPEG jpegInterface;
			return &jpegInterface;
			
		case Format::PPM:
			static format_interfaces::ImageFormatPPM ppmInterface;
			return &ppmInterface;
			
		case Format::BMP:
			static format_interfaces::ImageFormatBMP bmpInterface;
			return &bmpInterface;
			
		case Format::UNKNOWN:
			return nullptr;
	}
	return nullptr;
}

int main(int argc, const char** argv) {
    if (argc != 4 && argc != 5) {
        cerr << "Usage: "sv << argv[0] << "<in_file> <out_file> [convert|hmirror|vmirror|sobel|negate|cypher <key>]"sv << endl;
        return 1;
    }

    img_lib::Path in_path = argv[1];
    img_lib::Path out_path = argv[2];
	
	auto in_interface = GetFormatInterface(in_path);
	if (!in_interface) {
		cerr << "Unknown format of the input file"sv << endl;
        return 2;
	}
	
	auto out_interface = GetFormatInterface(out_path);
	if (!out_interface) {
		cerr << "Unknown format of the output file"sv << endl;
        return 3;
	}
	
	std::string action = argv[3];

    img_lib::Image image = in_interface->LoadImage(in_path);
    if (!image) {
        cerr << "Loading failed"sv << endl;
        return 4;
    }
	
	if(action == "hmirror"sv) {
		HMirrInplace(image);
	}
	else if(action == "vmirror"sv) {
		VMirrInplace(image);
	}
	else if(action == "sobel"sv) {
		image = Sobel(image);
	}
	else if(action == "negate"sv) {
		NegateInplace(image);
	}
	else if(action == "cypher"sv && argc == 5) {
		int key = atoi(argv[4]);
		CypherInplace(key, image);
	}
	else if(action != "convert"sv){
		cerr << "Option is wrong. Usage: [convert|hmirror|vmirror|sobel|negate|cypher <key>]"sv << endl;
        return 6;
	}

    if (!out_interface->SaveImage(out_path, image)) {
        cerr << "Saving failed"sv << endl;
        return 5;
    }

    cout << "Successfully"sv << endl;
}