#include <gtest/gtest.h>
#include <vector>

namespace {

class ArraySum
{
public:
    ArraySum( int n ) 
    {
        array.resize(n + 1);
        for( int i = 0; i <= n; i++ )
            array[i] = i;
    }

    ~ArraySum() = default;

    int sum( int a, int b )
    {
        int sum = 0;
        for( int i = a; i <= b; i++ )
            sum += array[i];
        return sum;
    }

private:
    std::vector<int> array;
};



int get_sum( int a, int b )
{
    int h = b - a + 1;
    return (a + b)*h/2;
}




class ArraySumTest : public ::testing::Test
{
protected:
    void SetUp() override 
    {
        p_as = new ArraySum(100);
    }

    void TearDown() override 
    {
        delete p_as;
        p_as = nullptr;
    }

    ArraySum *p_as = nullptr;
};




TEST_F( ArraySumTest, Case1 ) 
{
    EXPECT_EQ( p_as->sum(10,15), get_sum(10,15) );
    EXPECT_EQ( 1, 2 ) << "test for fail";
}



} // namespace