// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "Graphbase.hpp"
#include "control.hpp"
#include "listbox.hpp"
#include "display.h"
#include "Container.hpp"


/***********************************************************************************/
Item::Item()
{
	Next = NULL;
	Prev = NULL;
}

Item::Item( void* datum )
{
	Next = NULL;
	Prev = NULL;
	ptr = datum;
}
/***********************************************************************************/
Container::Container()
{
	Head 	= NULL;
	Tail 	= NULL;
	numItems= 0;
}

Container::Container( Item* FirstItem )
{
	Head 	= NULL;
	Tail 	= NULL;
	numItems= 0;	
	Add(FirstItem);
}

Item* Container::getItem(byte mIndex)
{
	int   count = 0;
	Item* tmp;
	Item* curr = Head;
	while ((curr) && (count<mIndex))
	{
		tmp = curr->getNext();
		curr = tmp;
		count++;
	}
	return curr;
}

int Container::Add( Item* NewItem  )
{
	NewItem->setPrev( Tail );
	NewItem->setNext( NULL );
	if (Head == NULL)
		Head = NewItem;

	if (Tail)
		Tail->setNext( NewItem );
	Tail = NewItem;
	numItems++;
}

/* Caller must free the memory associated with the item and the datum. */
void Container::Remove( Item* OldItem )
{
	Item* Pptr = OldItem->getPrev( );
	Item* Nptr = OldItem->getNext( );

	Nptr->setPrev( Pptr );
	Pptr->setNext( Nptr );
	
	OldItem->setNext( NULL );
	OldItem->setPrev( NULL );
	
	if (Head == OldItem)
		Head = Nptr;
	if (Tail == OldItem)
		Tail = NULL;	
	numItems--;		
}

void Container::FreeList( )
{
	Item* tmp;
	Item* curr = Head;
	while (curr)
	{
		tmp = curr->getNext();
		// FREE THE DATUM (has to be done before we delete the item!)
		//delete curr->getData());	// we stored a pointer to some datum.
		// Delete the 
		delete curr;
		curr = tmp;
	}
	Head = NULL;
	Tail = NULL;
	numItems = 0;
}
	
/* preform a bubble sort on the linked list */
void Container::sort_list( BOOL (*comparison)(Item *a, Item *b) )
{
	Item *a = NULL;
	Item *b = NULL; 
	Item *c = NULL;
	Item *e = NULL; 
	Item *tmp = NULL; 

 /* 
 // the `c' node precedes the `a' and `e' node 
 // pointing up the node to which the comparisons
 // are being made. 
 */
 while(e != Head->getNext()) {
 c = a = Head;
 b = a->getNext();
  while(a != e) {
   
   if(comparison(a, b)) {
    if(a == Head) {
     tmp = b->getNext();
     b->setNext(a);
     a->setNext(tmp);
     Head = b;
     c = b;
    } else {
     tmp = b->getNext();
     b->setNext(a);
     a->setNext(tmp);
     c->setNext(b);
     c = b;
    }
   } else {
    c = a;
    a = a->getNext();
   }
   b = a->getNext();
   if(b == e)
    e = a;
  }
 }
}
	