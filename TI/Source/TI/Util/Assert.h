#pragma once

#ifdef _DEBUG
	#define ASSERT(var) if (var == 0) { __debugbreak; }
#else
	#define ASSERT(var) var
#endif
