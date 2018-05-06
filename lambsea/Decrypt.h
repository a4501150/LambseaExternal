#pragma once
#include <Windows.h>
#include <iostream>
#include <cstdint>

#pragma region DynTable

uint8_t byte_table[] = {
	0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,
	0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
	0x2F, 0x2E, 0x2D, 0x2C, 0x2B, 0x2A, 0x29, 0x28, 0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20,
	0x3F, 0x3E, 0x3D, 0x3C, 0x3B, 0x3A, 0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30,
	0x4F, 0x4E, 0x4D, 0x4C, 0x4B, 0x4A, 0x49, 0x48, 0x47, 0x46, 0x45, 0x44, 0x43, 0x42, 0x41, 0x40,
	0x5F, 0x5E, 0x5D, 0x5C, 0x5B, 0x5A, 0x59, 0x58, 0x57, 0x56, 0x55, 0x54, 0x53, 0x52, 0x51, 0x50,
	0x6F, 0x6E, 0x6D, 0x6C, 0x6B, 0x6A, 0x69, 0x68, 0x67, 0x66, 0x65, 0x64, 0x63, 0x62, 0x61, 0x60,
	0x7F, 0x7E, 0x7D, 0x7C, 0x7B, 0x7A, 0x79, 0x78, 0x77, 0x76, 0x75, 0x74, 0x73, 0x72, 0x71, 0x70,
	0x8F, 0x8E, 0x8D, 0x8C, 0x8B, 0x8A, 0x89, 0x88, 0x87, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81, 0x80,
	0x9F, 0x9E, 0x9D, 0x9C, 0x9B, 0x9A, 0x99, 0x98, 0x97, 0x96, 0x95, 0x94, 0x93, 0x92, 0x91, 0x90,
	0xAF, 0xAE, 0xAD, 0xAC, 0xAB, 0xAA, 0xA9, 0xA8, 0xA7, 0xA6, 0xA5, 0xA4, 0xA3, 0xA2, 0xA1, 0xA0,
	0xBF, 0xBE, 0xBD, 0xBC, 0xBB, 0xBA, 0xB9, 0xB8, 0xB7, 0xB6, 0xB5, 0xB4, 0xB3, 0xB2, 0xB1, 0xB0,
	0xCF, 0xCE, 0xCD, 0xCC, 0xCB, 0xCA, 0xC9, 0xC8, 0xC7, 0xC6, 0xC5, 0xC4, 0xC3, 0xC2, 0xC1, 0xC0,
	0xDF, 0xDE, 0xDD, 0xDC, 0xDB, 0xDA, 0xD9, 0xD8, 0xD7, 0xD6, 0xD5, 0xD4, 0xD3, 0xD2, 0xD1, 0xD0,
	0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1, 0xE0,
	0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8, 0xF7, 0xF6, 0xF5, 0xF4, 0xF3, 0xF2, 0xF1, 0xF0
};


class DynTable {
private:
	uint32_t table[2048];

public:
	void inline generate(int32_t rotator, int32_t(&decrypt_offsets)[7]);
	uint32_t inline get(uint32_t idx);
};

void DynTable::generate(int32_t rotator, int32_t(&decrypt_offsets)[7]) {
	for (uint32_t i = 0; i < 0x100; i++) {
		uint32_t tmp = i;
		if ((tmp & 1) != 0)
			tmp ^= rotator;

		tmp >>= 1;
		if ((tmp & 1) != 0)
			tmp ^= rotator;

		tmp >>= 1;
		if ((tmp & 1) != 0)
			tmp ^= rotator;

		tmp >>= 1;
		if ((tmp & 1) != 0)
			tmp ^= rotator;

		tmp >>= 1;
		if ((tmp & 1) != 0)
			tmp ^= rotator;

		tmp >>= 1;
		if ((tmp & 1) != 0)
			tmp ^= rotator;

		tmp >>= 1;
		if ((tmp & 1) != 0)
			tmp ^= rotator;

		tmp >>= 1;
		if ((tmp & 1) != 0)
			tmp ^= rotator;

		this->table[i] = tmp >> 1;
	}

	uint32_t off = 0x200;
	for (int i = 0; i < 0x100; i++) {
		uint32_t tmp = this->table[off - 0x200];

		off += 1;

		tmp = (tmp >> 8) ^ this->table[tmp & 0xFF];
		this->table[off + decrypt_offsets[0]] = tmp;

		tmp = (tmp >> 8) ^ this->table[tmp & 0xFF];
		this->table[off + decrypt_offsets[1]] = tmp;

		tmp = (tmp >> 8) ^ this->table[tmp & 0xFF];
		this->table[off + decrypt_offsets[2]] = tmp;

		tmp = (tmp >> 8) ^ this->table[tmp & 0xFF];
		this->table[off + decrypt_offsets[3]] = tmp;

		tmp = (tmp >> 8) ^ this->table[tmp & 0xFF];
		this->table[off + decrypt_offsets[4]] = tmp;

		tmp = (tmp >> 8) ^ this->table[tmp & 0xFF];
		this->table[off + decrypt_offsets[5]] = tmp;

		tmp = (tmp >> 8) ^ this->table[tmp & 0xFF];
		this->table[off + decrypt_offsets[6]] = tmp;
	}
}

uint32_t DynTable::get(uint32_t idx) {
	return this->table[idx];
}



#pragma endregion

#pragma region NativeHelpers

int32_t inline ubyte0(int32_t i) {
	return i & 0xFF;
}

int32_t inline ubyte1(int32_t i) {
	return (i >> 8) & 0xFF;
}

int32_t inline ubyte2(int32_t i) {
	return (i >> 16) & 0xFF;
}

int32_t inline ubyte3(int32_t i) {
	return (i >> 24) & 0xFF;
}

int32_t inline word0(int64_t l) {
	return (l >> 0) & 0xFFFF;
}

#pragma endregion

DynTable dyn_table_r;
int32_t decrypt_rotator = 0x33ABAEBE;

int32_t decrypt_offsets[] = {
	-257, -1, 255, 511, 767, 1023, 1279
};

void inline init_decryption(void) {
	dyn_table_r.generate(decrypt_rotator, decrypt_offsets);
}

uint16_t inline decrypt_p1(int64_t left, int64_t right) {
	uint16_t decrypted = right ^ ~left ^ 0xD25;
	for (auto i = 0; i < 5; i++) {
		auto idx1 = (decrypted ^ 0x4400u) >> 8;
		auto idx2 = (decrypted ^ 0x55) & 0xFF;
		decrypted = byte_table[byte_table[idx1]] | (byte_table[byte_table[idx2]] << 8);
	}
	return decrypted;
}

uint64_t inline decrypt_p20(int64_t left, int64_t right, uint64_t a, uint64_t b, uint64_t c, uint64_t d_idx) {
	auto p1 = decrypt_p1(left, right);
	auto idx2 = ubyte3(p1 ^ b);
	auto idx3 = ubyte1(p1) ^ c + 0x200;
	auto idx1 = ubyte0(p1 ^ a) + 0x300;
	auto idx4 = d_idx + 0x100; /* 2nd byte of p1 is discarded for static obfuscation key :( */

	auto dta = dyn_table_r.get(idx1);
	auto dtb = dyn_table_r.get(idx2);
	auto dtc = dyn_table_r.get(idx3);
	auto dtd = dyn_table_r.get(idx4);
	auto mixed = (dta ^ dtb ^ dtc ^ dtd);
	auto inverted = ~mixed;
	auto remainder = inverted % 0x2B;
	return remainder;
}

uint64_t inline decrypt_p21(int64_t left, int64_t right) {
	uint64_t dec = decrypt_p20(left, right, 0xBC, 0xD7AF5ABC, 0x5A, 0xAF);
	return dec;
}

uint64_t inline decrypt_p22(int64_t left, int64_t right) {
	uint64_t dec = decrypt_p20(left, right, 0xC, 0x5CE7E30Cu, 0xE3, 0xE7);
	return dec;
}

struct EncryptedBlock {
	int64_t data[48];
};

template <typename T>
T decrypt_ptr(EncryptedBlock *block) {
	auto a = block->data[decrypt_p21(block->data[44], block->data[45])];
	auto b = decrypt_p22(block->data[46], block->data[47]);
	return (T)(a ^ b);
};



class TSLEncryptedPointer
{
private:
	uint64_t encrypted_pointers[44];
	uint16_t encrypted_index1;
	uint8_t pad0[6];
	uint16_t encrypted_index2;
	uint8_t pad1[6];
	uint16_t encrypted_xor1;
	uint8_t pad2[6];
	uint16_t encrypted_xor2;
	uint8_t pad3[6];


	uint16_t DecryptWord(uint8_t* table8, uint16_t word)
	{
		for (int i = 0; i < 5; i++)
		{
			word = table8[table8[(word & 0xFF) ^ 0x55]] << 0x08 | table8[table8[(word >> 0x08) ^ 0x44]];
		}

		return  word;
	}

	uint32_t DecryptDword(uint32_t* table32, uint32_t dword)
	{
		uint32_t xor = 0;
		xor ^= table32[((dword >> 0x00) & 0xFF) + 0x300];
		xor ^= table32[((dword >> 0x08) & 0xFF) + 0x200];
		xor ^= table32[((dword >> 0x10) & 0xFF) + 0x100];
		xor ^= table32[((dword >> 0x18) & 0xFF) + 0x000];
		return ~xor;
	}

	uint64_t DecryptIndex(uint32_t* table32, uint8_t* table8)
	{
		uint32_t temp = DecryptWord(table8, ~encrypted_index1 ^ encrypted_index2 ^ 0x0D25);

		temp ^= 0xD7AF5ABC;
		temp = DecryptDword(table32, temp);

		return temp % 0x2B;
	}

	uint64_t DecryptXor(uint32_t* table32, uint8_t* table8)
	{
		uint32_t temp = DecryptWord(table8, ~encrypted_xor1 ^ encrypted_xor2 ^ 0x0D25);

		temp ^= 0x5CE7E30C;
		temp = DecryptDword(table32, temp);

		return temp % 0x2B;
	}

public:
	uint64_t Decrypt(uint32_t* table32, uint8_t* table8)
	{
		uint64_t index = DecryptIndex(table32, table8);
		uint64_t pointer = encrypted_pointers[index];
		uint64_t xor = DecryptXor(table32, table8);

		return pointer ^ xor;
	}
};
