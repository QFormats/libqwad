#include <qwad/wad.h>

qformats::wad::QuakeWad *OpenWadFile(const char *wadfilepath)
{
    auto wad = qformats::wad::QuakeWad::FromFile(wadfilepath);
    return wad.get();
}

void GetTexture(qformats::wad::QuakeWad *ptr, const char *name)
{
    ptr->GetTexture(name);
}