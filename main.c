#include <stdio.h>
#include <gmp.h>
#include <stdbool.h>
#include <time.h>

void mpz_rand_num(mpz_t prime, unsigned int bits){
    mpz_t rand_num;
    gmp_randstate_t state;

    mpz_init(rand_num);
    mpz_init(prime);
    gmp_randinit_default(state);

    unsigned long seed = (unsigned long)time(NULL);
    gmp_randseed_ui(state, seed);

    mpz_urandomb(rand_num, state, bits);

    mpz_nextprime(prime, rand_num);

    mpz_clear(rand_num);
    gmp_randclear(state);
}
void euclidean(mpz_t result, mpz_t p, mpz_t q){
    if(mpz_cmp_ui(p, 0)==0){
        mpz_set(result, q);
        return;
    }
    if(mpz_cmp_ui(q, 0)==0){
        mpz_set(result, p);
        return;
    }

    mpz_t a, b, remainder;
    mpz_init_set(a, p);
    mpz_init_set(b, q);
    mpz_init(remainder);
    while(mpz_cmp_ui(b,0)!=0){
        mpz_mod(remainder, a, b);
        mpz_set(a, b);
        mpz_set(b, remainder);

        // gmp_printf("b: %Zd, \n a: %Zd", b ,a);
    }
    mpz_set(result, a);

    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(remainder);
}
void carmichael(mpz_t result, mpz_t prime1, mpz_t prime2){
    /*
    Since n = pq 
    Carmichael_funciton(n) = lem(Carmichael_funciton(prime1), Carmichael_funciton(prime2))
    p & q are prime so lem(p-1,q-1)
    */
    mpz_t prime1_minus_one, prime2_minus_one, gcd;

    mpz_init(prime1_minus_one);
    mpz_init(prime2_minus_one);

    
    
    mpz_sub_ui(prime1_minus_one, prime1, 1);
    mpz_sub_ui(prime2_minus_one, prime2, 1);
    
    euclidean(gcd, prime1_minus_one, prime2_minus_one);
    mpz_mul(result, prime1_minus_one, prime2_minus_one);
    mpz_div(result, result, gcd);

    mpz_clear(prime1_minus_one);
    mpz_clear(prime2_minus_one);
}


int main() {
    mpz_t prime1, prime2, n, carmichael_n, e, e_check, d;
    
    mpz_init(prime1);
    mpz_init(prime2);
    mpz_init(n);
    mpz_init(carmichael_n);
    mpz_init(d);

    mpz_init_set_ui(e, 65537);

    mpz_rand_num(prime1, 2048);
    mpz_rand_num(prime2, 2048);

    mpz_mul(n, prime1, prime2);

    carmichael(carmichael_n, prime1, prime2);
    
    euclidean(e_check, carmichael_n, e);
    while(mpz_cmp_ui(e_check, 1)!=0){
        euclidean(e_check, carmichael_n, e);
        mpz_nextprime(e, e);
    }
    
    mpz_invert(d, e, carmichael_n);

    gmp_printf("Public key: %Zd ^ %Zd\n", n, e);
    gmp_printf("Private key: %Zd\n", d);

    mpz_clear(prime1);
    mpz_clear(prime2);
    mpz_clear(d);
    mpz_clear(n);
    mpz_clear(carmichael_n);
    mpz_clear(e);
    mpz_clear(e_check);
    return 0;
}
