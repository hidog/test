#ifndef SP_CLASS_H
#define SP_CLASS_H

#include <utility>
#include <memory>



class SpFoo
{
public:
    SpFoo();
    SpFoo( int a, double h, int value = 0);
    virtual ~SpFoo();
    
    SpFoo( const SpFoo& foo );
    SpFoo& operator = ( const SpFoo& foo );
    
    SpFoo( SpFoo&& foo );
    SpFoo& operator = ( SpFoo&& foo );
    
    virtual void output();
    void shuffle_array( int lower = 100, int upper = 200 );
    
    
    int     age;
    double  *height = nullptr;
    short   *array = nullptr;
    
    static constexpr int size  =   10;
    
};




class SpBar : public SpFoo
{
public:
    SpBar();
    SpBar( int a, double h, const char* const n );
    virtual ~SpBar();
    
    SpBar( const SpBar& bar );
    SpBar& operator = ( const SpBar& bar );
    
    SpBar( SpBar&& bar );
    SpBar& operator = ( SpBar&& bar );
    
    virtual void output();

    char *name = nullptr;
    int name_len = 0;

};

SpBar operator + ( const SpBar& a, const SpBar& b );







class SpQux : public SpFoo
{
public:
    SpQux();
    SpQux( int a, double h, char c );
    virtual ~SpQux();
    
    SpQux( const SpQux& qux );
    SpQux& operator = ( const SpQux& qux );
    
    SpQux( SpQux&& qux );
    SpQux& operator = ( SpQux&& qux );
    
    virtual void output();

    char abbr;
    
    std::shared_ptr<SpQux> next; // 用來測試遞迴計數問題
    std::weak_ptr<SpQux>   w_next;
};




template<typename T, typename U>
std::unique_ptr<T> create_obj( U&& obj )
{
    return std::unique_ptr<T>( new T( std::forward<U>(obj) ) );
}







#endif
