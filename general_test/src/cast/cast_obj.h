#ifndef CAST_OBJ_H
#define CAST_OBJ_H




class CastBase
{
public:
    CastBase() = default;
    virtual ~CastBase() = default;
    
    virtual void output();
    void test() const;
    
    int a = -10;
    double b = 4.155;    
};





class CastDerive_1 : public CastBase
{
public:
    CastDerive_1() = default;
    ~CastDerive_1() = default;
    
    void output() override;
    void output_1( int n );
    
    char c = 'Z';
    unsigned int d = 51234;
};




class CastDerive_2 : public CastBase
{
public:
    CastDerive_2() = default;
    ~CastDerive_2() = default;
    
    void output() override;
    void output_2( int n );
    
    int start = 10, end = 15;
};






#endif
