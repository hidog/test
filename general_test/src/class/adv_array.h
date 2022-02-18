#ifndef ADV_ARRAY_H
#define ADV_ARRAY_H

#include "array.h"


class AdvArray : public Array
{
public:
    AdvArray();
    ~AdvArray();

    void output();
    virtual void output2() override;
    
private:

};



#endif
