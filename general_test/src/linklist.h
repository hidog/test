#ifndef LINKLIST_H
#define LINKLIST_H



struct Node
{
	int		data;
	Node	*next	=	nullptr;
};




class LinkList
{
public:

	LinkList();
	~LinkList();
	LinkList( const LinkList& list );

	int		size() const;
	void	push_back( int data );
	void	insert( int index, int data );

	void	print();

private:

	Node	*head	=	nullptr;
	Node	*end	=	nullptr;
	int		_size	=	0;

};



void linklist_test();




#endif