#pragma once

#include <Windows.h>
#include <string>

class TrollService
{
public:
	TrollService();
	~TrollService();

	void changeWallpaper(const std::string& absolutePath)
	{
			auto result = SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, 
				const_cast<char*>(absolutePath.c_str()), SPIF_UPDATEINIFILE);
	}
};

