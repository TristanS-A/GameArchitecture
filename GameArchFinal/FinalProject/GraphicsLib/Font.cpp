#include "Font.h"
#include "Font.h"
#include <cassert>

using namespace std;

Font::Font(const std::string& filename, int size)
{
	mpFont = TTF_OpenFont(filename.c_str(), size);
	mSize = size;
}

Font::~Font()
{
	TTF_CloseFont(mpFont);
	mpFont = nullptr;
}
