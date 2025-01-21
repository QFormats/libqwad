#pragma once

#include <string>
#include <map>
#include <memory>
#include <fstream>
#include "palette.h"

namespace qformats::wad
{
	static const int TEXTURE_NAME_LENGTH = 16;
	static const int MAX_MIP_LEVELS = 4;

	struct QuakeTexture
	{
		unsigned int width;
		unsigned int height;
		unsigned int mipOffsets[MAX_MIP_LEVELS];
		cvec raw;
	};

	struct QuakeWadEntry
	{
		enum QuakeWadEntryType
		{
			QWET_Palette = 0x40,
			QWET_SBarPic = 0x42,
			QWET_MipsTexture = 0x44,
			QWET_ConsolePic = 0x45
		};
		struct header
		{
			unsigned int offset;
			unsigned int inWadSize;
			unsigned int size;
			unsigned char type;
			unsigned char compression;
			unsigned short unknown;
		} header;
		std::string name;
		QuakeTexture texture;
		QuakeWadEntryType Type()
		{
			return (QuakeWadEntryType)header.type;
		}
	};

	class QuakeWad;
	using QuakeWadPtr = std::shared_ptr<QuakeWad>;

	struct QuakeWadOptions
	{
		bool flipTexHorizontal;
	};

	class QuakeWad
	{
	public:
		QuakeWadOptions opts;

	public:
		static QuakeWadPtr FromFile(const std::string &fileName, QuakeWadOptions opts = QuakeWadOptions());
		static QuakeWadPtr NewQuakeWad() { return std::make_shared<QuakeWad>(); }
		QuakeTexture *FromBuffer(const uint8_t *buff, int width, int height);
		~QuakeWad();
		QuakeTexture *GetTexture(const std::string &textureName);
		const std::map<std::string, QuakeWadEntry> &Textures() { return entries; };
		void SetPalette(const Palette &p) { this->pal = p; };
		const Palette &GetPalette() { return pal; };

	private:
		void fillTextureData(const std::vector<uint8_t> buff, QuakeTexture &tex);
		void fillTextureData(const uint8_t *buff, size_t size, QuakeTexture &tex);
		unsigned int numEntries;
		unsigned int dirOffset;
		Palette pal = default_palette;
		std::ifstream istream;
		std::map<std::string, QuakeWadEntry> entries;

		friend class QuakeWadManager;
	};
}