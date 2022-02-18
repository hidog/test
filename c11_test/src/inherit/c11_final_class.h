#ifndef C11_FINAL_CLASS_H
#define C11_FINAL_CLASS_H



class FinalBase
{
public :
    FinalBase() = default;
    virtual ~FinalBase() = default;
    FinalBase( const FinalBase& ) = default;
    FinalBase& operator = ( const FinalBase& ) = default;
    FinalBase& operator = ( FinalBase&& ) = default;

    void f1();
    virtual void f2();
    virtual void f3() final;
    void f4( int a );

};




class FinalObj final: public FinalBase
{
public:
    /*
        這行的作用是
        FinalObj* ptr2 = new FinalObj;
        ptr2->f4(2);
        會執行FinalBase::f4
        反之會執行FinalObj::f4
    */
    using FinalBase::f4; 

    void f4(double a);
    void f1();
    void f2() override;

    //void f3() {} // 編譯錯誤, f3是final

};



/*
編譯錯誤, FinalObj是final
class FinalObj2 : FinalObj
{
public:
};
*/



namespace c11 {

void test_final_class();

} // end namespace c11




#endif