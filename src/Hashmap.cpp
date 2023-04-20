#include "Hashmap.h"

Lint HashMap::hash(int syndrome[SYN_LEN])
{
	Lint sum = 0;
BINARY_TO_DECIMAL_LOOP:
	for(int i = 0; i < SYN_LEN; i++)
	{
		sum += syndrome[i] * (1<<i);
	}
	return sum;
}


void HashMap::insert(Lint correction, int syndrome[SYN_LEN])
{
	Lint synDec = this->hash(syndrome);
    int index = synDec % (MAX_SIZE/2);

    this->blocks[this->lastBlockUsed].syndrome = synDec;
	this->blocks[this->lastBlockUsed].correction = correction;
	this->blocks[this->lastBlockUsed].full = true;
    this->blocks[this->lastBlockUsed].left = nullptr;
    this->blocks[this->lastBlockUsed].right = nullptr;

    Entry* traveler = this->map[index];

    if(this->map[index] == nullptr)
    {
        this->map[index] = &this->blocks[this->lastBlockUsed];
    }
    else
    {
        while(true)
        {
            if(synDec > traveler->syndrome)
            {
                if(traveler->right == nullptr)
                {
                    traveler->right = &this->blocks[this->lastBlockUsed];
                    break;
                }
                else
                    traveler = traveler->right;
            }
            else
            {
                if(traveler->left == nullptr)
                {
                    traveler->left = &this->blocks[this->lastBlockUsed];
                    break;
                }
                else
                    traveler = traveler->left;
            }
        }
    }


    ++this->lastBlockUsed;
}

Lint HashMap::retrieve(int syndrome[SYN_LEN])
{
	Lint synDec = this->hash(syndrome);
    int index = synDec%(MAX_SIZE/2);

    Entry* traveler = this->map[index];

    while(true)
    {
        if(traveler->syndrome == synDec)
        {
            return traveler->correction;
        }
        else if(synDec > traveler->syndrome)
        {
            if(traveler->right != nullptr)
                traveler = traveler->right;
            else
                break;
        }
        else
        {
            if(traveler->left != nullptr)
                traveler = traveler->left;
            else
                break;
        }
    }

	return 0;
}
