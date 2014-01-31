#ifndef DEBUG_H
#define DEBUG_H

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DBG_NEW
#endif
#endif

#endif
