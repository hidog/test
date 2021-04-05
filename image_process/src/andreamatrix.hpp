#ifndef _ANDREAMATRIX_HPP
#define _ANDREAMATRIX_HPP

#include "andreavector.hpp"

namespace andrea{



template<class T>
class matrix
{
	private:
		andrea::vector<T>	*ptr;
		int					_size1;
		int					_size2;

	public:
		matrix()
		{
			ptr		=	0;
			_size1	=	0;
			_size2	=	0;
		}
		~matrix()
		{
			delete [] ptr;
			ptr		=	0;
			_size1	=	0;
			_size2	=	0;
		}
		matrix( const int _s1, const int _s2 )
		{
			ptr		=	0;
			_size1	=	0;
			_size2	=	0;
			resize( _s1, _s2 );
		}
		matrix( const matrix<T> &m )
		{
			ptr		=	0;
			_size1	=	0;
			_size2	=	0;
			*this	=	m;
		}

		void	operator = ( const matrix<T> &m )
		{
			int		i;
			if( _size1 != m._size1 || _size2 != m._size2 )
				resize( m._size1, m._size2 );
			
			for( i = 0; i < _size1; i++ )
				*(ptr + i)	=	*(m.ptr + i);

		}

		T&	operator () ( int i, int j )
		{
			assert( i < _size1 && j < _size2 && i >= 0 && j >= 0 );
			return	(*(ptr+i))(j);
		}

		void	resize( const int _s1, const int _s2 )
		{
			assert( _s1 >= 0 && _s2 >= 0 );

			
			if( _s1 == 0 || _s2 == 0 )
			{
				delete	[] ptr;
				ptr			=	0;
				_size1		=	0;
				_size2		=	0;
			}
			else if( _size1 != _s1 || _size2 != _s2 )
			{
				delete	[]	ptr;
				_size1	=	_s1;	// 重新取得 size
				_size2	=	_s2;
				ptr		=	new vector<T> [_size1];
				for( int i = 0; i < _size1; i++ )
					*( ptr + i )	=	vector<T>(_size2);
			}
		}

		int		size1()	{ return	_size1; };
		int		size2() { return	_size2; };


		void	clear()
		{
			for( int i = 0; i < _size1; i++ )
				(*(ptr + i )).clear();
		}

};







}

#endif