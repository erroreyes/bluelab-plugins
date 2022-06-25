#ifndef SO_SOURCE_IMPL_H
#define SO_SOURCE_IMPL_H

#include <BLTypes.h>
#include <BLUtilsFile.h>

#include <Morpho_defs.h>

class SoSourceImpl
{
 public:
    SoSourceImpl();
    virtual ~SoSourceImpl();

    virtual void GetName(char name[FILENAME_SIZE]) = 0;
};

#endif
