//============================================================================
// Name        : AES.cpp
// Author      : Christopher LaJon Morgan
// Version     :
// Copyright   : @clm
// Description : AES encryption implementation.
//============================================================================

#include <iostream>
#include <iomanip>
#include "AESCypher.h"
using namespace std;

void printHex(unsigned int val)
{
	printf("%X\n", val);
}

bool test_ffAdd(AESCypher &cypher)
{
	return cypher.ffAdd(0x57, 0x83) == (unsigned char)0xd4;
}

bool test_xtime(AESCypher &cypher)
{
	unsigned char ans[] = {0xae, 0x47, 0x8e, 0x07};

	unsigned char temp = cypher.xtime(0x57);
	for (int i = 0; i < 4; i++)
	{
		if (temp != ans[i])
			return false;
		temp = cypher.xtime(temp);
	}
	return true;
}

bool test_ffMultiply(AESCypher &cypher)
{
	return cypher.ffMultiply(0x57, 0x13) == (unsigned char)0xfe;
}

bool test_mixColumns()
{
	unsigned char setup[] =
	{
		0xd4, 0xbf, 0x5d, 0x30, 0xe0, 0xb4, 0x52, 0xae, 0xb8, 0x41, 0x11, 0xf1, 0x1e, 0x27, 0x98, 0xe5
	};
	unsigned char ans[][4] =
	{
			{0x04, 0xe0, 0x48, 0x28},
			{0x66, 0xcb, 0xf8, 0x06},
			{0x81, 0x19, 0xd3, 0x26},
			{0xe5, 0x9a, 0x7a, 0x4c}
	};


	AESCypher cypher(setup);

	cypher.mixColumns();

	int r = 0,c = 0;
	for (; r < 4; r++)
		for(c = 0;c < 4; c++)
		{
			if (ans[r][c] != cypher.state[r][c])
				return false;
		}
	return true;
}

bool test_shiftRows()
{
	unsigned char setup[] =
	{
		0xd4, 0xbf, 0x5d, 0x30, 0xe0, 0xb4, 0x52, 0xae, 0xb8, 0x41, 0x11, 0xf1, 0x1e, 0x27, 0x98, 0xe5
	};
	unsigned char ans[][4] =
	{
			{0x04, 0xe0, 0x48, 0x28},
			{0xcb, 0xf8, 0x06, 0x66},
			{0xd3, 0x26, 0x81, 0x19},
			{0x4c, 0xe5, 0x9a, 0x7a}
	};


	AESCypher cypher(setup);
    cypher.mixColumns();
	cypher.shiftRows();

	int r = 0,c = 0;
	for (; r < 4; r++)
		for(c = 0;c < 4; c++)
		{
			if (ans[r][c] != cypher.state[r][c])
				return false;
		}
	return true;
}

bool test_subBytes()
{
	unsigned char setup[] =
	{
			0x19, 0x3d, 0xe3, 0xbe, 0xa0, 0xf4, 0xe2, 0x2b, 0x9a, 0xc6, 0x8d, 0x2a, 0xe9, 0xf8, 0x48, 0x08
	};

	unsigned char ans[][4] =
	{
			{0xd4, 0xe0, 0xb8, 0x1e},
			{0x27, 0xbf, 0xb4, 0x41},
			{0x11, 0x98, 0x5d, 0x52},
			{0xae, 0xf1, 0xe5, 0x30}
	};

	AESCypher cypher(setup);
	cypher.subBytes();

	int r = 0,c = 0;
	for (; r < 4; r++)
		for(c = 0;c < 4; c++)
		{
			if (ans[r][c] != cypher.state[r][c])
				return false;
		}
	return true;

}

bool test_addRoundKey()
{
	unsigned char setup[] =
	{
		0xd4, 0xbf, 0x5d, 0x30, 0xe0, 0xb4, 0x52, 0xae, 0xb8, 0x41, 0x11, 0xf1, 0x1e, 0x27, 0x98, 0xe5
	};
	unsigned char aRoundKey[] =
	{
		0xa0, 0xfa, 0xfe, 0x17, 0x88, 0x54, 0x2c, 0xb1, 0x23, 0xa3, 0x39, 0x39, 0x2a, 0x6c, 0x76, 0x05
	};
	unsigned char ans[][4] =
	{
			{0xa4, 0x68, 0x6b, 0x02},
			{0x9c, 0x9f, 0x5b, 0x6a},
			{0x7f, 0x35, 0xea, 0x50},
			{0xf2, 0x2b, 0x43, 0x49}
	};


	AESCypher cypher(setup);
	cypher.mixColumns();

	cypher.setRoundKey(aRoundKey);
	cypher.addRoundKey();

	int r = 0,c = 0;
	for (; r < 4; r++)
		for(c = 0;c < 4; c++)
		{
			if (ans[r][c] != cypher.state[r][c])
				return false;
		}
	return true;
}

bool test_extendKey()
{
	unsigned char setup[] =
	{
		0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
	};

	AESCypher cypher;

	cypher.setKey(setup,16);
	cypher.keyExpansion();

	cypher.printKey();

	return true;
}

bool test_updateRoundKey()
{
	unsigned char setup[] =
	{
		0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
	};

	AESCypher cypher;

	cypher.setKey(setup,16);
	cypher.keyExpansion();

	cypher.printKey();

	cypher.updateRoundKey();
	cypher.printRoundKey();

	for (int i = 0; i < 10; i++)
	{
		cypher.advanceRoundAndUpdateRoundKey();
		cypher.printRoundKey();
	}

	return true;
}

void performFineGrainTests()
{
	AESCypher cypher;

	cout << "ffAdd : ";
	if (test_ffAdd(cypher))
		 cout << "Passed" << endl;
	else
		cout << "Failed" << endl;

	cout << "xtime : ";
	if (test_xtime(cypher))
		 cout << "Passed" << endl;
	else
		cout << "Failed" << endl;

	cout << "multiply : ";
	if (test_ffMultiply(cypher))
		 cout << "Passed" << endl;
	else
		cout << "Failed" << endl;


	cout << "mix Columns : ";
	if (test_mixColumns())
		 cout << "Passed" << endl;
	else
		cout << "Failed" << endl;

	cout << "Shift rows : ";
	if (test_shiftRows())
		 cout << "Passed" << endl;
	else
		cout << "Failed" << endl;

	cout << "Sub Bytes : ";
	if (test_subBytes())
		 cout << "Passed" << endl;
	else
		cout << "Failed" << endl;

	cout << "Add Round Key: ";
	if (test_addRoundKey())
		 cout << "Passed" << endl;
	else
		cout << "Failed" << endl;

	cout << "Extend Crypto Key to Rounds: Manual Verification Required.\n";
	test_extendKey();

	cout << "Update Round Key: Manual Verify.\n";
	test_updateRoundKey();

	return;
}

void testCypher()
{
	unsigned char plainText[] =
	{
			0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34
	};

	unsigned int keySize = 16;
	unsigned char key[] =
	{
			0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
	};

	AESCypher cypher;
	unsigned char* cypherText = cypher.cipher(plainText, key, keySize);

	AESCypher cypherDe;
	cypherDe.invCipher(cypherText, key, keySize);

	return;
}

void testCypher128()
{
	unsigned char plainText[] =
	{
			0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
	};

	unsigned int keySize = 16;
	unsigned char key[] =
	{
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
	};

	AESCypher cypher;
	unsigned char* cypherText = cypher.cipher(plainText, key, keySize);

	AESCypher cypherDe;
	cypherDe.invCipher(cypherText, key, keySize);

	return;
}

void testCypher192()
{
	unsigned char plainText[] =
	{
			0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
	};

	unsigned int keySize = 24;
	unsigned char key[] =
	{
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17
	};

	AESCypher cypher;
	unsigned char* cypherText = cypher.cipher(plainText, key, keySize);

	AESCypher cypherDe;
	cypherDe.invCipher(cypherText, key, keySize);

	return;
}

void testCypher256()
{
	unsigned char plainText[] =
	{
			0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
	};

	unsigned int keySize = 32;
	unsigned char key[] =
	{
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
	};

	AESCypher cypher;
	unsigned char* cypherText = cypher.cipher(plainText, key, keySize);

	AESCypher cypherDe;
	cypherDe.invCipher(cypherText, key, keySize);

	return;
}


int main(int argc, char* argv[])
{
//	performFineGrainTests();

	cout << "--------Block Example: \n";
	testCypher();
	cout << "--------128 Example: \n";
	testCypher128();
	cout << "--------192 Example: \n";
	testCypher192();
	cout << "--------256 Example: \n";
	testCypher256();

	return 0;
}
