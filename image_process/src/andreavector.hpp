#ifndef _ANDREAVECTOR_HPP
#define _ANDREAVECTOR_HPP

#include <assert.h>

#define	AND_VECTOR_OFFSET	1			// 可以補值的數量

namespace andrea{



template<class T>
class	vector
{
	private:
		T		*ptr;
		int		_size;
#if ( AND_VECTOR_OFFSET == 1 )
		bool	add_flag;			// 是否有補值的 flag
#else
		需要改寫這邊的code!!
#endif

	public:
		// 預設建構
		vector()
		{
			ptr			=	0;
			_size		=	0;
			add_flag	=	false;
		}
		// 解構
		~vector()
		{
			delete [] ptr;
			ptr			=	0;
			_size		=	0;
			add_flag	=	false;
		}
		// 複製建構
		vector( const andrea::vector<T> &v )
		{
			ptr			=	0;
			_size		=	0;
			add_flag	=	false;
			*this		=	v;
		}
		// 給size建構
		vector( const int _s )
		{
			ptr			=	0;
			_size		=	0;
			add_flag	=	false;
			resize(_s);
		}
		// 等於
		void	operator = ( const andrea::vector<T> &v )
		{
			if( _size != v._size )
				resize( v._size );
			if( _size > 0 )
				memcpy( ptr, v.ptr, _size*sizeof(T) );		// 不能用offset 不然會有bug (有機會使用到非法記憶體)
			// add_flag在resize的地方做更新.
		}
		// 讀取位置
		T&	operator () ( int index )
		{
			assert( index < _size && ptr != 0 );
			return	*(ptr + index);		// 這邊不加判斷 是因為這樣做速度會比較快 (debug底下用assert擋掉)
		}
		// 讀取位置
		T&	operator [] ( int index )
		{
			assert( index < _size && ptr != 0 );
			return	*(ptr + index);
		}
		// size
		int		size()				{	return	_size;	}
		// 重新取得記憶體空間
		void	resize( int _s )
		{
			assert( _s >= 0 );

			if( _s == 0 )
			{
				delete	[] ptr;
				ptr			=	0;
				_size		=	0;
				add_flag	=	false;
			}
			else if( _size != _s )
			{
				delete	[]	ptr;
				ptr			=	0;
				_size		=	_s;	// 重新取得 size
				ptr			=	new	T[ AND_VECTOR_OFFSET + _size ];		// size不同的時候才重新取得空間
				add_flag	=	false;									// 重新取得空間的時候才將offset_count復原.  不然只能透過clear的動作來清除補值.
				assert( ptr != 0 );
			}
		}
		// 歸零
		void	clear()
		{
			memset( ptr, 0,  _size*sizeof(T) );				// 只在size範圍內做動作 避免記憶體出界
		}
		// 補值
		void	add_value( T _d )
		{
			assert( _size > 0 && ptr != 0 );		// 沒資料情況不准補值
			assert( !add_flag );			// 沒補值才能進來這邊  演算法設計上  禁止補值兩次

			// 沒補值  可以補值
			if( !add_flag )
			{
				*(ptr + _size)	=	_d;
				_size++;
				add_flag		=	true;
			}
		}
		// 清除補值
		void	clear_add_value()
		{
			assert( _size > 0 && ptr != 0 );		// 沒資料情況不准補值

// 如果允許多次補值 這邊需要修改!!
#if (AND_VECTOR_OFFSET == 1)		
			if( add_flag )
			{
				_size--;
				add_flag	=	false;
			}
#else
			THIS IS AVOID ADD_VECTOR_OFFSET > 0  need compile error!!
#endif
		}
		// 指令一個範圍 給定值
		void	set_value( int start, int end, T value )
		{
			assert( start >= 0 && end < _size && start < end && ptr != 0 );
			memset( ptr + start, value, (end-start)*sizeof(T) );
		}
		// 將一個陣列的資料接在後面
		void	add_vector( int start, const andrea::vector<T> &v )
		{
			assert( v._size > 0 && start + v._size <= _size && start >= 0 );
			assert( v.ptr != 0 && ptr != 0 );

			memcpy( ptr + start, v.ptr, v._size*sizeof(T) );
		}
		// 將一個陣列的資料接在後面  (限定範圍)  用在刪除補值資料
		void	add_vector( andrea::vector<T> &v, int v_start, int v_end )
		{
			//assert( v._size > 0 && start + v._size <= _size && start >= 0 );
			assert( v.ptr != 0 && ptr != 0 );
			assert( v_start >= 0 && v_end <= v.size() && (v_end-v_start) <= _size );

			//memcpy( ptr + start, v.ptr, v._size*sizeof(T) );
			memcpy( ptr, v.ptr + v_start, (v_end-v_start)*sizeof(T) );
		}

};

















}

#endif