#include "pch.h"

#include "include/BaseIface.h"

namespace Fwrap
{
    void IBase::Delete() 
    { 
        delete this; 
    }
}