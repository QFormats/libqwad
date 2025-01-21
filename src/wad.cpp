#include <qwad/wad.h>

#include <fstream>
#include <iterator>
#include <algorithm>
#include <vector>

namespace qformats::wad
{
	static const int MAGIC_LEN = 4;

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
			fillTextureData(buff, qwe.texture);
		}

		return &qwe.texture;
	}

	QuakeTexture *QuakeWad::FromBuffer(const uint8_t *buff, int width, int height)
	{
		auto qtex = new QuakeTexture();
		qtex->width = width;
		qtex->height = height;
		fillTextureData(buff, width * height, *qtex);
		return qtex;
	}

	void QuakeWad::fillTextureData(const std::vector<uint8_t> buff, QuakeTexture &tex)
	{
		fillTextureData(buff.data(), buff.size(), tex);
	}

	void QuakeWad::fillTextureData(const uint8_t *buff, size_t size, QuakeTexture &tex)
	{
		int k = 0, w = 0;
		int h = opts.flipTexHorizontal ? tex.height - 1 : 0;
		tex.raw = cvec(size);
		for (int idx = size - 1; idx >= 0; idx--)
		{
			auto rgba = pal.GetColor((int)(buff[w + (h * tex.width)]));
			tex.raw[k] = rgba;
			k++, w++;
			if (w == tex.width)
			{
				w = 0;
				opts.flipTexHorizontal ? h-- : h++;
			}
		}
		return;
	}
}