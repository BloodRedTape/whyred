#include "utils/fs.hpp"
#include <core/os/file.hpp>

String ReadEntireFile(String filepath) {
	if(!File::Exists(filepath.Data()))
		filepath = "../../../" + filepath;
	SX_ASSERT(File::Exists(filepath.Data()));
	
	File file(filepath.Data(), File::Mode::Read);
	SX_ASSERT(file.IsOpen());
	
	String content(file.Size());
	file.Read(content.Data(), file.Size());
	return content;
}
