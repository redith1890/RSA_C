#include <stdio.h>
#include <gmp.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint32_t u32;
typedef uint8_t u8;
typedef uint64_t u64;
#define null NULL
#define internal static

void
mpz_rand_num(mpz_t prime, u32 bits)
{
	mpz_t rand_num;
	gmp_randstate_t state;

	mpz_init(rand_num);
	gmp_randinit_default(state);

	u64 seed = (u64)time(null) ^ (u64)clock();
	gmp_randseed_ui(state, seed);

	mpz_urandomb(rand_num, state, bits);

	mpz_nextprime(prime, rand_num);

	mpz_clear(rand_num);
	gmp_randclear(state);
}

void
carmichael(mpz_t result, mpz_t prime1, mpz_t prime2)
{
	/*
    Since n = pq
    Carmichael_funciton(n) = lem(Carmichael_funciton(prime1), Carmichael_funciton(prime2))
    p & q are prime so lem(p-1,q-1)
    */
	mpz_t prime1_minus_one, prime2_minus_one, gcd;

	mpz_init(prime1_minus_one);
	mpz_init(prime2_minus_one);
	mpz_init(gcd);


	mpz_sub_ui(prime1_minus_one, prime1, 1);
	mpz_sub_ui(prime2_minus_one, prime2, 1);

	mpz_gcd(gcd, prime1_minus_one, prime2_minus_one);
	mpz_mul(result, prime1_minus_one, prime2_minus_one);
	mpz_div(result, result, gcd);

	mpz_clear(prime1_minus_one);
	mpz_clear(prime2_minus_one);
}

void
generate_keys(mpz_t keys[3])
{
	mpz_t prime1, prime2, n, carmichael_n, e, e_check, d;

	mpz_init(prime1);
	mpz_init(prime2);
	mpz_init(n);
	mpz_init(carmichael_n);
	mpz_init(d);
	mpz_init(e_check);

	mpz_init_set_ui(e, 65537);

	mpz_rand_num(prime1, 2048);
	mpz_rand_num(prime2, 2048);

	mpz_mul(n, prime1, prime2);

	carmichael(carmichael_n, prime1, prime2);

	mpz_gcd(e_check, carmichael_n, e);
	while(mpz_cmp_ui(e_check, 1)!=0)
	{
		mpz_gcd(e_check, carmichael_n, e);
		mpz_nextprime(e, e);
	}

	mpz_invert(d, e, carmichael_n);

	mpz_init_set(keys[0], n);
	mpz_init_set(keys[1], e);
	mpz_init_set(keys[2], d);

	mpz_clear(prime1);
	mpz_clear(prime2);
	mpz_clear(d);
	mpz_clear(n);
	mpz_clear(carmichael_n);
	mpz_clear(e);
	mpz_clear(e_check);
}


typedef struct RSA_Keys
{
	mpz_t n;
	mpz_t e;
	mpz_t d;
} RSA_Keys;

void
init_rsa_keys(RSA_Keys *keys)
{
	mpz_init(keys->n);
	mpz_init(keys->e);
	mpz_init(keys->d);
}

void
clear_rsa_keys(RSA_Keys *keys)
{
	mpz_clear(keys->n);
	mpz_clear(keys->e);
	mpz_clear(keys->d);
}

void
get_rsa_keys(RSA_Keys *keys)
{
	mpz_t temp_keys[3];
	mpz_init(temp_keys[0]);
	mpz_init(temp_keys[1]);
	mpz_init(temp_keys[2]);

	generate_keys(temp_keys);

	mpz_set(keys->n, temp_keys[0]);
	mpz_set(keys->e, temp_keys[1]);
	mpz_set(keys->d, temp_keys[2]);

	mpz_clear(temp_keys[0]);
	mpz_clear(temp_keys[1]);
	mpz_clear(temp_keys[2]);
}

mpz_t*
encrypt_message(const char *message, size_t len, const RSA_Keys *keys)
{
	mpz_t *encrypted = (mpz_t*)malloc(len * sizeof(mpz_t));

	for (size_t i = 0; i < len; i++)
	{
		mpz_init(encrypted[i]);
		mpz_t m;
		mpz_init_set_ui(m, (u8)message[i]);
		mpz_powm(encrypted[i], m, keys->e, keys->n);
		mpz_clear(m);
	}

	return encrypted;
}

char*
decrypt_message(mpz_t *encrypted, size_t len, const RSA_Keys *keys)
{
	char *decrypted = (char*)malloc((len + 1) * sizeof(char));

	for (size_t i = 0; i < len; i++)
	{
		mpz_t m;
		mpz_init(m);
		mpz_powm(m, encrypted[i], keys->d, keys->n);
		u64 val = mpz_get_ui(m);
		decrypted[i] = (char)val;
		mpz_clear(m);
	}
	decrypted[len] = '\0';

	return decrypted;
}

void
print_encrypted(mpz_t *encrypted, size_t len)
{
	printf("Encrypted Message:\n");
	for (size_t i = 0; i < len; i++) gmp_printf("Letter %zu: %Zd\n", i+1, encrypted[i]);
}

void
clear_encrypted(mpz_t *encrypted, size_t len)
{
	for (size_t i = 0; i < len; i++)
	{
		mpz_clear(encrypted[i]);
	}
	free(encrypted);
}

int
main(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("Error: argument needed\nUse: <rsa> <text to encrypt>\n");
		exit(0);
	}

	clock_t start = clock();

	RSA_Keys keys;
	init_rsa_keys(&keys);

	get_rsa_keys(&keys);
	clock_t end = clock();
	double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
	double t1 = time_taken;

	gmp_printf("n: %Zd\n", keys.n);
	gmp_printf("e: %Zd\n", keys.e);
	gmp_printf("d: %Zd\n", keys.d);

	size_t len = strlen(argv[1]);

	start = clock();
	mpz_t *encrypted = encrypt_message(argv[1], len, &keys);
	end = clock();
	time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
	double t2 = time_taken;

	print_encrypted(encrypted, len);

	start = clock();
	char *decrypted = decrypt_message(encrypted, len, &keys);
	end = clock();
	time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

	printf("Time for generate RSA keys: %f seconds\n", t1);
	printf("Time for encrypt the message: %f seconds\n", t2);
	printf("Time for decrypt the message: %f seconds\n", time_taken);

	printf("\nDecrypted Message: %s\n", decrypted);

	clear_encrypted(encrypted, len);
	free(decrypted);
	clear_rsa_keys(&keys);

	return 0;
}
