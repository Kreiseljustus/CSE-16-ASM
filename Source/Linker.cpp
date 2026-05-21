#include <string_view>

#include "Linker.h"
#include "ObjectFile.h"

void Linker::link(std::string_view path) {

}

void Linker::addObjectFile(std::string_view path) {
	ObjectFile o = ObjectFile();
	o.parseFile(path, nullptr);

	objectFiles.push_back(o);
}
