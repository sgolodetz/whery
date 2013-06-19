/**
 * whery: Transaction.h
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#ifndef H_WHERY_TRANSACTION
#define H_WHERY_TRANSACTION

namespace whery {

/**
\brief An instance of this class tracks a program that is run against the database.

In practical terms, transaction objects are passed around and used to mediate between
higher-level code and the entity cache, keeping track of which entities are accessed
in the course of executing the program and whether they are modified.
*/
class Transaction
{
	// TODO
};

}

#endif
