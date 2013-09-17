/*
 * AESCypher.cpp
 *
 *  Created on: Sep 5, 2013
 *      Author: clm
 */

#include "AESCypher.h"

/*****************************************************************************
 * DEFAULT CONSTRUCTOR
 ******************************************************************************/
AESCypher::AESCypher()
{
	currentRound   = 0;
	numberOfRounds = 0;
}

/*****************************************************************************
 * CONSTRUCTOR:
 * Init state with plainText.
 ******************************************************************************/
AESCypher::AESCypher(unsigned char plainText[])
{
	setState(plainText);

	currentRound   = 0;
	numberOfRounds = 0;
}

/*****************************************************************************
 * Return a copy of the current state.
 ******************************************************************************/
unsigned char* AESCypher::getState()
{
	unsigned char *cpyState = new unsigned char[16];

	int i = 0;
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
		{
			cpyState[i++] = state[r][c];
		}

	return cpyState;
}

/*****************************************************************************
 * Prints the current state matrix in hex.
 ******************************************************************************/
void AESCypher::setState(unsigned char vals[])
{
	int i = 0;
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
		{
			state[r][c] = vals[i++];
		}

	return;
}

/*****************************************************************************
 * Prints the current state matrix in hex.
 ******************************************************************************/
void AESCypher::printState()
{
	printf("\n");
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			printf("%X ", state[r][c]);
		}
		printf("\n");
	}
}

/*****************************************************************************
 * Set the cypt key with vals.
 ******************************************************************************/
void AESCypher::setKey(unsigned char vals[], unsigned int size)
{
	if (key.size() > 0)
		throw key.size();

	for (unsigned int i = 0; i+3 < size; i+=4)
	{
		unsigned char *word = new unsigned char[4];
		word[0] = vals[i];
		word[1] = vals[i+1];
		word[2] = vals[i+2];
		word[3] = vals[i+3];
		key.push_back(word);
	}

	switch (size / 4)
	{
	case 4:
		numberOfRounds = 10;
		break;
	case 6:
		numberOfRounds = 12;
		break;
	case 8:
		numberOfRounds = 14;
		break;
	default:
		throw size;
	}

	updateRoundKey();
	return;
}

/*****************************************************************************
 ******************************************************************************/
void AESCypher::printKey()
{
	printf("\n");
	for (unsigned int r = 0; r < 4; r++)
	{
		for (unsigned int i = 0; i < key.size(); i++)
		{
			if (i % 4 == 0 && i != 0)
							printf(" | ");
						else
							printf(" ");

			printf("%.2X", (key[i])[r]);
		}

		printf("\n");
	}
	return;
}

/*****************************************************************************
 * Set the contents of roundKey to the contents of vals in column major
 * fashion.
 ******************************************************************************/
void AESCypher::setRoundKey(unsigned char vals[])
{
	int i = 0;
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
		{
			roundKey[r][c] = vals[i++];
		}
	return;
}

/*****************************************************************************
 * This function will take each column of the state matrix and multiply
 * it by a fixed matrix.
 *
 * The fixed matrix is:
 * [ 02, 03, 01, 01]
 * [ 01, 02, 03, 01]
 * [ 01, 01, 02, 03]
 * [ 03, 01, 01, 02]
 ******************************************************************************/
void AESCypher::mixColumns()
{
	unsigned char ans[4];
	for (int c = 0; c < 4; c++)
	{
		ans[0] = ffMultiply(0x02, state[0][c]) ^
			     ffMultiply(0x03, state[1][c]) ^
				 state[2][c]^
				 state[3][c];

		ans[1] = state[0][c] ^
				 ffMultiply(0x02, state[1][c]) ^
				 ffMultiply(0x03, state[2][c]) ^
				 state[3][c];

		ans[2] = state[0][c] ^
				 state[1][c] ^
				 ffMultiply(0x02, state[2][c]) ^
				 ffMultiply(0x03, state[3][c]);

		ans[3] = ffMultiply(0x03, state[0][c]) ^
				 state[1][c] ^
				 state[2][c] ^
				 ffMultiply(0x02, state[3][c]);

		state[0][c] = ans[0];
		state[1][c] = ans[1];
		state[2][c] = ans[2];
		state[3][c] = ans[3];
	}
	return;
}

//======Field Arithmetic Helpers
/*****************************************************************************
 * Calls xtime on num till the multiple of two in numTimes is reached.
 ******************************************************************************/
unsigned char AESCypher::xtimes(unsigned char num, unsigned char numTimes)
{
	unsigned char times = 0x01;
	while (times != numTimes)
	{
		num   = xtime(num);
		times = times << 1;
	}
	return num;
}

//======Field Arithmetic
/*****************************************************************************
 * Performs xor on one and two
 ******************************************************************************/
unsigned char AESCypher::ffAdd(unsigned char one, unsigned char two)
{
	return one ^ two;
}

/*****************************************************************************
 * Adds x to fField
 * ie: left shift clear high bit
 ******************************************************************************/
unsigned char AESCypher::xtime(unsigned char fField)
{
	if (0x80 & fField)
		return (fField << 1) ^ 0x1b;
	else
		return fField << 1;
}

/*****************************************************************************
 * Performs fixed field multiplication on bytes one and two.
 ******************************************************************************/
unsigned char AESCypher::ffMultiply(unsigned char one, unsigned char two)
{
	bool set = false;
	unsigned char ans = 0x00;

	for (int i = 0; i < 8; i++,two = two >> 1)
	{
		if (two & 0x01)
		{
			if (!set)
			{
				set = true;
				ans = xtimes(one, BitPositions[i]);
			}
			else
				ans = ffAdd(ans, xtimes(one, BitPositions[i]));
		}
	}

	return ans;
}

/*****************************************************************************
 * Extends crypto key to desired length.
 ******************************************************************************/
void AESCypher::keyExpansion()
{
	unsigned int keySize = key.size();
	unsigned int i       = keySize;

	while (i < (numberOfRounds + 1) * 4 )
	{
		unsigned char *temp = new unsigned char[4];
		temp[0] = key[i-1][0];
		temp[1] = key[i-1][1];
		temp[2] = key[i-1][2];
		temp[3] = key[i-1][3];

		if (i % keySize == 0)
		{
			rotWord(temp);
			subWord(temp);
			temp[0] ^= (Rcon[i/keySize]);
		}
		else if (keySize > 6 && i % keySize == 4)
			subWord(temp);

		temp[0] ^= key[i-keySize][0];
		temp[1] ^= key[i-keySize][1];
		temp[2] ^= key[i-keySize][2];
		temp[3] ^= key[i-keySize][3];

		key.push_back(temp);
		i++;
	}

	return;
}

/*****************************************************************************
 * takes a four-byte input word and applies the S-box (Sec. 5.1.1,Fig. 7)
 * to each of the four bytes to produce an output word.
 ******************************************************************************/
void AESCypher::subWord(unsigned char word[])
{
	for (int i = 0; i < 4; i++)
	{
		unsigned int row = word[i] >> 4;
		unsigned int col = word[i] & 0x0f;

		word[i] = Sbox[row][col];
	}
	return;
}

/*****************************************************************************
 * Rotates the given word:
 * [0 1 2 3] to [1 2 3 0]
 ******************************************************************************/
void AESCypher::rotWord(unsigned char word[])
{
	unsigned char temp;
	for (int i = 0; i+1 < 4; i++)
	{
		temp      = word[i];
		word[i]   = word[i+1];
		word[i+1] = temp;
	}
	return;
}

/*****************************************************************************
 * Increments the currentRound and update the roundKey accordingly.
 ******************************************************************************/
void AESCypher::advanceRoundAndUpdateRoundKey()
{
	currentRound++;
	updateRoundKey();
	return;
}

/*****************************************************************************
 * Decrements the currentRound and update the roundKey accordingly.
 ******************************************************************************/
void AESCypher::decrementRoundAndUpdateRoundKey()
{
	currentRound--;
	updateRoundKey();
	return;
}

/*****************************************************************************
 * Updages roundKey from key given the currentRound.
 ******************************************************************************/
void AESCypher::updateRoundKey()
{
	unsigned char setRoundArray[16];

	int i = currentRound * 4;
	int cnt = 0;
	for (; cnt < 16;)
	{
		setRoundArray[cnt++] = key[i][0];
		setRoundArray[cnt++] = key[i][1];
		setRoundArray[cnt++] = key[i][2];
		setRoundArray[cnt++] = key[i][3];
		i++;
	}

	setRoundKey(setRoundArray);

	return;
}

/*****************************************************************************
 * Prints the current roundKey
 ******************************************************************************/
void AESCypher::printRoundKey()
{
	printf("\n");
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			printf("%X ", roundKey[r][c]);
		}
		printf("\n");
	}
	return;
}

/*****************************************************************************
 * XOR the contents of roundKey columns into the state columns.
 ******************************************************************************/
void AESCypher::addRoundKey()
{
	for (int c = 0; c < 4; c++)
	{
		state[0][c] ^= roundKey[0][c];
		state[1][c] ^= roundKey[1][c];
		state[2][c] ^= roundKey[2][c];
		state[3][c] ^= roundKey[3][c];
	}
	return;
}

/*****************************************************************************
 * Subs each byte in state via Sbox[].
 ******************************************************************************/
void AESCypher::subBytes()
{
	for (int r = 0; r < 4; r++)
		for (int c = 0; c < 4; c++)
		{
			unsigned int row = state[r][c] >> 4;
			unsigned int col = state[r][c] & 0x0f;

			state[r][c] = Sbox[row][col];
		}
	return;
}

/*****************************************************************************
 * Sifts rows according to their position.
 ******************************************************************************/
void AESCypher::shiftRows()
{
	const int Num_Columns = 4;

	for (int r = 0; r < 4; r++) 					 // For Each Row
		for (int sift = r; sift > 0; sift--)		 // Shift row number of times
			for (int m = 0; m < Num_Columns - 1; m++)// Move first to end.
			{
				unsigned char temp = state[r][m];
				state[r][m] = state[r][m+1];
				state[r][m+1] = temp;
			}

	return;
}

/*****************************************************************************
 * Encrypt input and return cipher text.
 ******************************************************************************/
unsigned char* AESCypher::cipher(unsigned char input[],
									   unsigned char keyInBytes[],
									   unsigned int keySizeInBytes)
{
	setState(input);

	setKey(keyInBytes, keySizeInBytes);
	keyExpansion();

	addRoundKey();
	advanceRoundAndUpdateRoundKey();

	while (currentRound < numberOfRounds)
	{
		subBytes();
		shiftRows();
		mixColumns();
		addRoundKey();
		advanceRoundAndUpdateRoundKey();
	}

	subBytes();
	shiftRows();
	addRoundKey();

	printf("Cipher State: \n");
	printState();

	return getState();
}



//====== Invert Functions =======//

/*****************************************************************************
 * Decrpyt input and return plain text.
 ******************************************************************************/
unsigned char* AESCypher::invCipher(unsigned char input[],
									unsigned char keyInBytes[],
									unsigned int keySizeInBytes)
{
	setState(input);

	setKey(keyInBytes, keySizeInBytes);
	keyExpansion();

	currentRound = numberOfRounds;
	updateRoundKey();

	addRoundKey();
	decrementRoundAndUpdateRoundKey();

	while (currentRound > 0)
	{
		invShiftRows();
		invSubBytes();

		addRoundKey();
		decrementRoundAndUpdateRoundKey();

		invMixColumns();
	}

	invShiftRows();
	invSubBytes();
	addRoundKey();

	printf("Plain Text Was: \n");
	printState();

	return getState();
}

/*****************************************************************************
 * Reverse subBytes.
 ******************************************************************************/
void AESCypher::invSubBytes()
{
	for (int r = 0; r < 4; r++)
		for (int c = 0; c < 4; c++)
		{
			unsigned int row = state[r][c] >> 4;
			unsigned int col = state[r][c] & 0x0f;

			state[r][c] = InvSbox[row][col];
		}

	return;
}

/*****************************************************************************
 * Reverse shiftRows.
 ******************************************************************************/
void AESCypher::invShiftRows()
{
	const int Num_Columns = 4;

	for (int r = 0; r < 4; r++) 					 // For Each Row
		for (int sift = r; sift > 0; sift--)		 // Shift row number of times
			for (int m = Num_Columns - 1; m - 1 >= 0; m--)// Move last to first.
			{
				unsigned char temp = state[r][m - 1];
				state[r][m - 1] = state[r][m];
				state[r][m] = temp;
			}

	return;
}

/*****************************************************************************
 * Reverse mixColumns.
 ******************************************************************************/
void AESCypher::invMixColumns()
{
	unsigned char ans[4];
	for (int c = 0; c < 4; c++)
	{
		ans[0] = ffMultiply(0x0e, state[0][c]) ^
			     ffMultiply(0x0b, state[1][c]) ^
			     ffMultiply(0x0d, state[2][c]) ^
			     ffMultiply(0x09, state[3][c]);

		ans[1] = ffMultiply(0x09, state[0][c]) ^
			     ffMultiply(0x0e, state[1][c]) ^
			     ffMultiply(0x0b, state[2][c]) ^
			     ffMultiply(0x0d, state[3][c]);

		ans[2] = ffMultiply(0x0d, state[0][c]) ^
			     ffMultiply(0x09, state[1][c]) ^
			     ffMultiply(0x0e, state[2][c]) ^
			     ffMultiply(0x0b, state[3][c]);

		ans[3] = ffMultiply(0x0b, state[0][c]) ^
			     ffMultiply(0x0d, state[1][c]) ^
			     ffMultiply(0x09, state[2][c]) ^
			     ffMultiply(0x0e, state[3][c]);

		state[0][c] = ans[0];
		state[1][c] = ans[1];
		state[2][c] = ans[2];
		state[3][c] = ans[3];
	}

	return;
}


