#ifndef _CONTAINER_
#define _CONTAINER_


// Items are the nodes of the linked list.
class Item
{
public:
	Item();
	Item(void* datum);	
	void		setData( void* datum ) { ptr = datum; };
	void*		getData( ) { return ptr; };
	
	// Items may be linked:
	void		setNext(Item* next)	{ Next = next; };
	void		setPrev(Item* prev)	{ Prev = prev; };
	Item*		getNext()			{ return Next; };
	Item*		getPrev()			{ return Prev; };

private:
	void* ptr;
	Item* Next;
	Item* Prev;	
};

/* Do not store numbers in the void* member, it must be a pointer to 
	a cpp object.  This is because the FreeList() function does a delete 
	on the datum!
*/
// Container holds the start of the linked list.
class Container 
{
public:
	Container();
	Container( Item* FirstItem );

	int   	Add   		( Item* NewItem  );
	void	Remove		( Item* mOldItem );
	void	FreeList	( 				 );	
	int	    getNumItems	( 				 )  { return numItems; };
	void 	sort_list	( BOOL (*comparison)(Item *a, Item *b) );

	Item*	getHead		(            ) { return Head; };
	Item*	getItem		( byte mIndex);
	Item*	getTail		(            ) { return Tail; };

protected:
	int	  	numItems;
	
private:
	Item*	Head;
	Item*	Tail;
};

 
#endif

