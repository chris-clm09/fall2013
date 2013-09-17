//============================================================================
// Name        : HashCrack.cpp
// Author      : Christopher Morgan
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cstdlib>
#include <vector>

#include "sha1.h"
using namespace std;

/*******************************************************************************
 * From two hashs return if the first numBits are equal.
 *******************************************************************************/
bool hashsAreEqualToNumBits(const unsigned char sha1[20],
							const unsigned char sha2[20],
							unsigned int numBits)
{
	unsigned int byte      = 0;
	unsigned char checkBit = 0x80;
	while (numBits)
	{
		unsigned char ans1 = sha1[byte] & checkBit;
		unsigned char ans2 = sha2[byte] & checkBit;

		if (ans1 != ans2) return false;

		checkBit = checkBit >> 1;
		if (checkBit == 0)
		{
			byte++;
			checkBit = 0x80;
		}
		numBits--;
	}
	return true;
}

/*******************************************************************************
 * KLUDGE: Just used to save a hash for future look up.
 *******************************************************************************/
class SavedHash
{
public:
	//Functions
	SavedHash(unsigned char set[20], unsigned int numBits)
	{
		for (int i = 0; i < 5; i++)
			hash[i] = set[i];

		this->numBits = numBits;
	}

	//Variables
	unsigned char hash[5];
	unsigned int numBits;
};


/*******************************************************************************
 * Generates a shaw hash from data.
 *******************************************************************************/
void genShaw(unsigned char* data, unsigned int size, unsigned char sha[20])
{
	CSHA1 hasher;

	hasher.Reset();
	hasher.Update(data, size);
	hasher.Final();

	hasher.GetHash(sha);
	return;
}

/*******************************************************************************
 * From two hashs return if the first numBits are equal.
 *******************************************************************************/
bool hashsAreEqualToNumBits(unsigned char sha1[20],
							unsigned char sha2[20],
							unsigned int numBits)
{
	unsigned int byte      = 0;
	unsigned char checkBit = 0x80;
	while (numBits)
	{
		unsigned char ans1 = sha1[byte] & checkBit;
		unsigned char ans2 = sha2[byte] & checkBit;

		if (ans1 != ans2) return false;

		checkBit = checkBit >> 1;
		if (checkBit == 0)
		{
			byte++;
			checkBit = 0x80;
		}
		numBits--;
	}
	return true;
}

/*******************************************************************************
 * Fill data[] with random data.
 *******************************************************************************/
void genRandData(unsigned char data[10])
{
	for (int i = 0; i < 10; i++)
		data[i] = rand();
	return;
}

/*******************************************************************************
 * KLUDGE : Search -- Find test in saved.
 *******************************************************************************/
bool contains(vector<SavedHash>& saved, SavedHash &test,unsigned int numBitsToTest)
{
	for (unsigned int i = 0; i < saved.size(); i++)
		if (hashsAreEqualToNumBits(saved[i].hash, test.hash,numBitsToTest))
			return true;
	return false;
}

/*******************************************************************************
 * Generate hashes on random data arrays till identical hash is generated.
 *******************************************************************************/
unsigned int genCollision(unsigned int numBitsToTest)
{
	vector<SavedHash> saved;

	unsigned int timesTillCollision = 0;

	unsigned char data[10];
	unsigned char data2[10];

	unsigned char saw[20];
	unsigned char saw2[20];

	bool collision = false;

	do
	{
		genRandData(data);
		genRandData(data2);

		genShaw(data, 10, saw);
		genShaw(data2, 10, saw2);

		collision = hashsAreEqualToNumBits(saw, saw2, numBitsToTest);
		timesTillCollision += 2;

		SavedHash one(saw, numBitsToTest);
		SavedHash two(saw2, numBitsToTest);

		//Kludge
		collision = collision ||
					contains(saved, one, numBitsToTest) ||
					contains(saved, two, numBitsToTest);

		saved.push_back(one);
		saved.push_back(two);

	}while(!collision);

	return timesTillCollision;
}

/*******************************************************************************
 * Run collision attack by performing experiment with n bits and with
 * each bit selection r times.
 * Print out the average of r runs on n bits.
 *******************************************************************************/
void collisionAttack()
{
	cout << "CollisionAttack: \n";
	//run experiment r-times with n-bits
	for (unsigned int n = 1; n <= 18; n++)
	{
		double total = 0;
		for (int r = 1; r <= 50; r++)
		{
			total += genCollision(n);
		}
		cout << total / 50.0 << ", ";
	}
	cout << endl;
	return;
}

/*******************************************************************************
 * Generate a saw on random data until it matches saw.
 *******************************************************************************/
unsigned int genImageAttack(unsigned int numBitsToTest)
{
	unsigned int timesTillCollision = 0;

	unsigned char data[] = {0xfa, 0xbc, 0xaf, 0x11, 0x22, 0x55, 0xff, 0xaa, 0xbb, 0xbb};
	unsigned char saw[20];
	genShaw(data, 10, saw);

	unsigned char data2[10];
	unsigned char saw2[20];

	bool collision = false;

	do
	{
		genRandData(data2);
		genShaw(data2, 10, saw2);

		collision = hashsAreEqualToNumBits(saw, saw2, numBitsToTest);
		timesTillCollision++;

	}while(!collision);

	return timesTillCollision;
}

/*******************************************************************************
 * Run preImage attack by performing experiment with n bits and with
 * each bit selection r times.
 * Print out the average of r runs on n bits.
 *******************************************************************************/
void preimageAttack()
{
	cout << "PreImageAttack: \n";
	//run experiment m-times with n-bits
	for (int n = 1; n <= 18; n++)
	{
		double total = 0;
		for (int r = 1; r <= 50; r++)
		{
			total += genImageAttack(n);
		}
		cout << total / 50.0 << ", ";
	}
	cout << endl;
	return;
}

/*******************************************************************************
 *******************************************************************************/
int main()
{
	collisionAttack();
	preimageAttack();

	return 0;
}
