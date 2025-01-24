#include <qwad/wad.h>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <locale>
#include <vector>

namespace qformats::wad
{
    static const int MAGIC_LEN = 4;

    std::string to_lower(std::string s)
    {
        for (char &c : s)
            c = tolower(c);
        return s;
    }

    bool QuakeWad::IsSkyTexture(const std::string texname)
    {
        return to_lower(texname).find("sky") != std::string::npos;
    }

    QuakeWadPtr QuakeWad::FromFile(const std::string &fileName, QuakeWadOptions opts)
    {
        auto w = std::make_shared<QuakeWad>();
        w->opts = opts;
        w->istream.open(fileName, std::ios::binary);
        char magic[MAGIC_LEN + 1] = {};
        w->istream.read(magic, MAGIC_LEN);
        if (magic[0] == '\0')
        {
            return nullptr;
        }
        if (std::string(magic) != "WAD2")
            throw std::runtime_error("WAD magic string malformed");

        w->istream.read(reinterpret_cast<char *>(&w->numEntries), sizeof(uint32_t));
        w->istream.read(reinterpret_cast<char *>(&w->dirOffset), sizeof(uint32_t));
        w->istream.seekg(w->dirOffset, w->istream.beg);

        for (unsigned int i = 0; i < w->numEntries; i++)
        {
            auto we = QuakeWadEntry();
            w->istream.read(reinterpret_cast<char *>(&we.header), sizeof(QuakeWadEntry::header));
            char name[TEXTURE_NAME_LENGTH + 1] = {};
            w->istream.read(name, TEXTURE_NAME_LENGTH);
            we.name = std::string(name);
            w->entries[we.name] = we;
        }
        return w;
    }

    QuakeWad::~QuakeWad()
    {
        if (istream.is_open())
            istream.close();
    }

    QuakeTexture *QuakeWad::GetTexture(const std::string &textureName)
    {
        if (entries.find(textureName) == entries.end())
            return nullptr;

        auto &qwe = entries[textureName];

        if (qwe.texture.raw.size() == 0)
        {
            istream.seekg(qwe.header.offset + TEXTURE_NAME_LENGTH, istream.beg);
            istream.read((char *)&qwe.texture.width, sizeof(uint32_t));
            istream.read((char *)&qwe.texture.height, sizeof(uint32_t));
            istream.read((char *)&qwe.texture.mipOffsets, sizeof(uint32_t) * MAX_MIP_LEVELS);
            std::vector<uint8_t> buff(qwe.texture.width * qwe.texture.height);
            istream.read((char *)buff.data(), buff.size());
            qwe.texture = *FromBuffer(buff.data(), IsSkyTexture(textureName), qwe.texture.width, qwe.texture.height);
        }

        return &qwe.texture;
    }

    QuakeTexture *QuakeWad::FromBuffer(const uint8_t *buff, bool isSky, int width, int height)
    {
        QuakeTexture *qtex;
        if (isSky)
        {
            qtex = new QuakeSkyTexture();
        }
        else
        {
            qtex = new QuakeTexture();
        }
        qtex->width = width;
        qtex->height = height;
        qtex->FillTextureData(buff, width * height, opts.flipTexHorizontal, pal);
        return qtex;
    }
} // namespace qformats::wad