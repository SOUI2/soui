#ifndef _SEVEN_ZIP_PWD_H
#define _SEVEN_ZIP_PWD_H

#include "SevenString.h"

namespace SevenZip
{

	struct SevenZipPassword
	{
		SevenZipPassword(bool haspwd, const TString &pwd)
		{
			PasswordIsDefined = haspwd;
			Password = pwd;
		}

		bool PasswordIsDefined;
		TString Password;
	};
}

#endif