#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/ecdsa.h>
#include <openssl/ecdh.h>
#include "sm2.h"
#include "LIB_TCPS_OPENSSL.h"

typedef struct {
	int	field_type,	/* either NID_X9_62_prime_field or
				 * NID_X9_62_characteristic_two_field */
		seed_len,
		param_len;
	unsigned int cofactor;	/* promoted to BN_ULONG */
} EC_CURVE_DATAX;

static const struct { EC_CURVE_DATAX h; unsigned char data[0+32*6]; }
	_EC_SECG_PRIME_SM2 = {
	{ 1,0,32,1 },
	{							/* no seed */
	  0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,   //p
	  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	  0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	  0xFF,0xFF,
	  0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	  0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFC,   //a
	  0x28,0xE9,0xFA,0x9E,0x9D,0x9F,0x5E,0x34,0x4D,0x5A,
	  0x9E,0x4B,0xCF,0x65,0x09,0xA7,0xF3,0x97,0x89,0xF5,
	  0x15,0xAB,0x8F,0x92,0xDD,0xBC,0xBD,0x41,0x4D,0x94,
	  0x0E,0x93,  //b
	  0x32,0xC4,0xAE,0x2C,0x1F,0x19,0x81,0x19,0x5F,0x99,
	  0x04,0x46,0x6A,0x39,0xC9,0x94,0x8F,0xE3,0x0B,0xBF,
	  0xF2,0x66,0x0B,0xE1,0x71,0x5A,0x45,0x89,0x33,0x4C,
	  0x74,0xC7,	//x
	  0xBC,0x37,0x36,0xA2,0xF4,0xF6,0x77,0x9C,0x59,0xBD,
	  0xCE,0xE3,0x6B,0x69,0x21,0x53,0xD0,0xA9,0x87,0x7C,
	  0xC6,0x2A,0x47,0x40,0x02,0xDF,0x32,0xE5,0x21,0x39,
	  0xF0,0xA0,	//y
	  0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x72,0x03,0xDF,0x6B,
	  0x21,0xC6,0x05,0x2B,0x53,0xBB,0xF4,0x09,0x39,0xD5,
	  0x41,0x23 //z
	}
	};
typedef struct _ec_list_element_st {
	int	nid;
	const EC_CURVE_DATAX *data;
	const EC_METHOD *(*meth)(void);
	const char *comment;
} ec_list_elementx;

static const ec_list_elementx Xcurve_list[] = {
	{ 0, &_EC_SECG_PRIME_SM2.h, 0, "SM2 curve over a 256 bit prime field" }
};

static EC_GROUP *ec_SM2group_from_data(void)
{
	EC_GROUP *group=NULL;
	EC_POINT *P=NULL;
	BN_CTX	 *ctx=NULL;
	BIGNUM	 *p=NULL, *a=NULL, *b=NULL, *x=NULL, *y=NULL, *order=NULL;
	int	 ok=0;
	int	 seed_len,param_len;
	const EC_METHOD *meth;
	const EC_CURVE_DATAX *data;
	const unsigned char *params;

	if ((ctx = BN_CTX_new()) == NULL)
	{
		ECerr(EC_F_EC_GROUP_NEW_FROM_DATA, ERR_R_MALLOC_FAILURE);
		goto err;
	}

	data = Xcurve_list[0].data;
	seed_len  = data->seed_len;
	param_len = data->param_len;
	params	  = (const unsigned char *)(data+1);	/* skip header */
	params	 += seed_len;				/* skip seed   */

	if (!(p = BN_bin2bn(params+0*param_len, param_len, NULL))
		|| !(a = BN_bin2bn(params+1*param_len, param_len, NULL))
		|| !(b = BN_bin2bn(params+2*param_len, param_len, NULL)))
	{
		ECerr(EC_F_EC_GROUP_NEW_FROM_DATA, ERR_R_BN_LIB);
		goto err;
	}

	if (Xcurve_list[0].meth != 0)
	{
		//meth = Xcurve_list[0].meth();
		//if (((group = EC_GROUP_new(meth)) == NULL) ||(!(group->meth->group_set_curve(group, p, a, b, ctx))))
		//	{
		//	ECerr(EC_F_EC_GROUP_NEW_FROM_DATA, ERR_R_EC_LIB);
		//	goto err;
		//	}
	}
	else if (data->field_type == 1)
	{
		if ((group = EC_GROUP_new_curve_GFp(p, a, b, ctx)) == NULL)
			{
			ECerr(EC_F_EC_GROUP_NEW_FROM_DATA, ERR_R_EC_LIB);
			goto err;
			}
	}
#ifndef OPENSSL_NO_EC2M
	else	/* field_type == NID_X9_62_characteristic_two_field */
		{
		if ((group = EC_GROUP_new_curve_GF2m(p, a, b, ctx)) == NULL)
			{
			ECerr(EC_F_EC_GROUP_NEW_FROM_DATA, ERR_R_EC_LIB);
			goto err;
			}
		}
#endif

	if ((P = EC_POINT_new(group)) == NULL)
		{
		ECerr(EC_F_EC_GROUP_NEW_FROM_DATA, ERR_R_EC_LIB);
		goto err;
		}

	if (!(x = BN_bin2bn(params+3*param_len, param_len, NULL))
		|| !(y = BN_bin2bn(params+4*param_len, param_len, NULL)))
		{
		ECerr(EC_F_EC_GROUP_NEW_FROM_DATA, ERR_R_BN_LIB);
		goto err;
		}
	if (!EC_POINT_set_affine_coordinates_GFp(group, P, x, y, ctx))
		{
		ECerr(EC_F_EC_GROUP_NEW_FROM_DATA, ERR_R_EC_LIB);
		goto err;
		}
	if (!(order = BN_bin2bn(params+5*param_len, param_len, NULL))
		|| !BN_set_word(x, (BN_ULONG)data->cofactor))
		{
		ECerr(EC_F_EC_GROUP_NEW_FROM_DATA, ERR_R_BN_LIB);
		goto err;
		}
	if (!EC_GROUP_set_generator(group, P, order, x))
		{
		ECerr(EC_F_EC_GROUP_NEW_FROM_DATA, ERR_R_EC_LIB);
		goto err;
		}
	if (seed_len)
		{
		if (!EC_GROUP_set_seed(group, params-seed_len, seed_len))
			{
			ECerr(EC_F_EC_GROUP_NEW_FROM_DATA, ERR_R_EC_LIB);
			goto err;
			}
		}
	ok=1;
err:
	if (!ok)
		{
		EC_GROUP_free(group);
		group = NULL;
		}
	if (P)
		EC_POINT_free(P);
	if (ctx)
		BN_CTX_free(ctx);
	if (p)
		BN_free(p);
	if (a)
		BN_free(a);
	if (b)
		BN_free(b);
	if (order)
		BN_free(order);
	if (x)
		BN_free(x);
	if (y)
		BN_free(y);
	return group;
}




int TCPS_SM2_signature(char *prikey,unsigned char *digest,int digestlen,unsigned char *Sign,int *Signlen)
{
	BN_CTX *ctx = NULL;

	BIGNUM *p = NULL;
	BIGNUM *a = NULL;
	BIGNUM *b = NULL;

	EC_GROUP *group= NULL;

	EC_POINT *P = NULL;
	EC_POINT *Q = NULL;
	EC_POINT *R = NULL;

	BIGNUM *x = NULL;
	BIGNUM *y = NULL;
	BIGNUM *z = NULL;
	EC_KEY	*eckey = NULL;
	//unsigned char	*signature;

	//int	sig_len;
	int ret=-1;
	BIGNUM *kinv = NULL;
	BIGNUM *rp = NULL;
	BIGNUM *order = NULL;


	//EC_POINT * DHPoint = NULL;
		

	if(strlen(prikey)!=64)return -1;

	ctx = BN_CTX_new();
	if (!ctx) return -1;

	
	/* Curve SM2 (Chinese National Algorithm) */
	//http://www.oscca.gov.cn/News/201012/News_1197.htm
	p = BN_new();
	a = BN_new();
	b = BN_new();
	if (!p || !a || !b) goto builtin_err;
	group = EC_GROUP_new(EC_GFp_mont_method()); /* applications should use EC_GROUP_new_curve_GFp
 	                                             * so that the library gets to choose the EC_METHOD */
	if (!group)  goto builtin_err;
	
	if (!BN_hex2bn(&p, "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF")) goto builtin_err;
	if (1 != BN_is_prime_ex(p, BN_prime_checks, ctx, NULL)) goto builtin_err;
	if (!BN_hex2bn(&a, "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC")) goto builtin_err;
	if (!BN_hex2bn(&b, "28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93")) goto builtin_err;
	if (!EC_GROUP_set_curve_GFp(group, p, a, b, ctx)) goto builtin_err;

	P = EC_POINT_new(group);
	Q = EC_POINT_new(group);
	R = EC_POINT_new(group);
	if (!P || !Q || !R)  goto builtin_err;

	x = BN_new();
	y = BN_new();
	z = BN_new();
	if (!x || !y || !z)  goto builtin_err;


	if (!BN_hex2bn(&x, "32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7"))  goto builtin_err;
	if (!EC_POINT_set_compressed_coordinates_GFp(group, P, x, 0, ctx))  goto builtin_err;
	if (!EC_POINT_is_on_curve(group, P, ctx))  goto builtin_err;
	if (!BN_hex2bn(&z, "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123"))  goto builtin_err;
	if (!EC_GROUP_set_generator(group, P, z, BN_value_one())) goto builtin_err;
/*	
	if (!EC_POINT_get_affine_coordinates_GFp(group, P, x, y, ctx)) goto builtin_err;
	fprintf(stdout, "\nChinese sm2 algorithm test -- Generator:\n     x = 0x");
	BNPrintf(x);
	fprintf(stdout, "\n     y = 0x");
	BNPrintf( y);
	fprintf(stdout, "\n");

	if (!BN_hex2bn(&z, "BC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0")) goto builtin_err;
	if (0 != BN_cmp(y, z)) goto builtin_err;
*/	
	//fprintf(stdout, "verify degree ...");
	if (EC_GROUP_get_degree(group) != 256)  goto builtin_err;
	//fprintf(stdout, " ok\n");
	
	//fprintf(stdout, "verify group order ...");
	//fflush(stdout);
	//if (!EC_GROUP_get_order(group, z, ctx))  goto builtin_err;////get R
	//if (!EC_GROUP_precompute_mult(group, ctx))  goto builtin_err;
	//if (!EC_POINT_mul(group, Q, z, NULL, NULL, ctx))  goto builtin_err;
	//if (!EC_POINT_is_at_infinity(group, Q))  goto builtin_err;
 	//fflush(stdout);
	//fprintf(stdout, " ok\n");

	//testing ECDSA for SM2
	/* create new ecdsa key */
	if ((eckey = EC_KEY_new()) == NULL)
		goto builtin_err;
	if (EC_KEY_set_group(eckey, group) == 0)
	{
		goto builtin_err;
	}
	/* create key */

	if (!BN_hex2bn(&z, prikey)) goto builtin_err;

	EC_KEY_set_private_key(eckey,z);

	//fprintf(stdout, " key ok\n");
	/* check key */
	//if (!EC_KEY_check_key(eckey))
	//{
	//	goto builtin_err;
	//}


	/* create signature */
	//sig_len = ECDSA_size(eckey);
	//if ((signature = OPENSSL_malloc(sig_len)) == NULL)
	//	goto builtin_err;



	rp    = BN_new();
	kinv  = BN_new();
	order = BN_new();

	if (!BN_hex2bn(&z, "6CB28D99385C175C94F94E934817663FC176D925DD72B727260DBAAE1FB2F96F")) goto builtin_err;
	if (!EC_POINT_mul(group, Q, z, NULL, NULL, ctx))
	{
		goto builtin_err;
	}
	if (!EC_POINT_get_affine_coordinates_GFp(group,Q, x, y, ctx))
	{
		goto builtin_err;
	}


	EC_GROUP_get_order(group, order, ctx);
	if (!BN_nnmod(rp, x, order, ctx))
	{
		goto builtin_err;
	}
	if (!BN_copy(kinv, z ))
	{
		goto builtin_err;
	}

	if (!SM2_sign_ex(1, digest, digestlen, Sign, (unsigned int *)Signlen, kinv, rp, eckey))
	{
		goto builtin_err;
	}
	//memcpy(Sign,signature,sig_len);
	//*Signlen=sig_len;
	ret=0;

builtin_err:
		
	//if (signature)
		//OPENSSL_free(signature);
	//signature = NULL;

	if (P)
		EC_POINT_free(P);
	if (Q)
		EC_POINT_free(Q);
	if (R)
		EC_POINT_free(R);
	//if (DHPoint)
		//EC_POINT_free(DHPoint);
	if (eckey)
		EC_KEY_free(eckey);
	eckey = NULL;
	if (group)
		EC_GROUP_free(group);
	if (ctx)
		BN_CTX_free(ctx);
	if(p) {
		OPENSSL_free(p);
	}
	if(a) {
		OPENSSL_free(a);
	}
	if(b) {
		OPENSSL_free(b);
	}
	if(x) {
		OPENSSL_free(x);
	}
	if(y) {
		OPENSSL_free(y);
	}
	if(z) {
		OPENSSL_free(z);
	}

	if(rp) {
		OPENSSL_free(rp);
	}

	if(kinv) {
		OPENSSL_free(kinv);
	}
	if(order) {
		OPENSSL_free(order);
	}
	return ret;

}

int TCPS_SM2_verify(char *pubkeyX,char *pubkeyY,unsigned char *digest,int digestlen,unsigned char *Sign,int Signlen)
{
	BN_CTX *ctx = NULL;
	BIGNUM *p = NULL;
	BIGNUM *a = NULL;
	BIGNUM *b = NULL;

	EC_GROUP *group= NULL;

	EC_POINT *P = NULL;
	EC_POINT *Q = NULL;
	EC_POINT *R = NULL;

	BIGNUM *x = NULL;
	BIGNUM *y = NULL;
	BIGNUM *z = NULL;
	EC_KEY	*eckey = NULL;
	//unsigned char	*signature;

	//int	sig_len;
	int ret=-1;
	//BIGNUM *kinv, *rp,*order; 

	//EC_POINT * DHPoint = NULL;

	if(strlen(pubkeyX)!=64)return -1;
	if(strlen(pubkeyY)!=64)return -1;

	ctx = BN_CTX_new();
	if (!ctx) return -1;

	/* Curve SM2 (Chinese National Algorithm) */
	//http://www.oscca.gov.cn/News/201012/News_1197.htm
	p = BN_new();
	a = BN_new();
	b = BN_new();
	if (!p || !a || !b) goto builtin_err;
	group = EC_GROUP_new(EC_GFp_mont_method()); /* applications should use EC_GROUP_new_curve_GFp
 	                                             * so that the library gets to choose the EC_METHOD */
	if (!group) goto builtin_err;
	
	if (!BN_hex2bn(&p, "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF")) goto builtin_err;
	if (1 != BN_is_prime_ex(p, BN_prime_checks, ctx, NULL)) goto builtin_err;
	if (!BN_hex2bn(&a, "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC")) goto builtin_err;
	if (!BN_hex2bn(&b, "28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93")) goto builtin_err;
	if (!EC_GROUP_set_curve_GFp(group, p, a, b, ctx)) goto builtin_err;

	P = EC_POINT_new(group);
	Q = EC_POINT_new(group);
	R = EC_POINT_new(group);
	if (!P || !Q || !R) goto builtin_err;

	x = BN_new();
	y = BN_new();
	z = BN_new();
	if (!x || !y || !z) goto builtin_err;


	if (!BN_hex2bn(&x, "32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7")) goto builtin_err;
	if (!EC_POINT_set_compressed_coordinates_GFp(group, P, x, 0, ctx)) goto builtin_err;
	if (!EC_POINT_is_on_curve(group, P, ctx)) goto builtin_err;
	if (!BN_hex2bn(&z, "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123")) goto builtin_err;
	if (!EC_GROUP_set_generator(group, P, z, BN_value_one())) goto builtin_err;
	


	if (EC_GROUP_get_degree(group) != 256) goto builtin_err;

	//if (!EC_GROUP_get_order(group, z, ctx)) goto builtin_err;////get R
	//if (!EC_GROUP_precompute_mult(group, ctx)) goto builtin_err;
	//if (!EC_POINT_mul(group, Q, z, NULL, NULL, ctx)) goto builtin_err;
	//if (!EC_POINT_is_at_infinity(group, Q)) goto builtin_err;


	//testing ECDSA for SM2
	/* create new ecdsa key */
	if ((eckey = EC_KEY_new()) == NULL)
		goto builtin_err;
	if (EC_KEY_set_group(eckey, group) == 0)
	{
		goto builtin_err;
	}

	if (!BN_hex2bn(&x, pubkeyX)) goto builtin_err;
	if (!BN_hex2bn(&y, pubkeyY)) goto builtin_err;
	if (!EC_POINT_set_affine_coordinates_GFp(group, P, x, y, ctx)) goto builtin_err;

	EC_KEY_set_public_key(eckey, P);


	/* check key */
	if (!EC_KEY_check_key(eckey))
	{
		fprintf(stdout," failed\n");
		goto builtin_err;
	}


	/* verify signature */
	if (SM2_verify(1, digest, 32, Sign, Signlen, eckey) != 1)
	{
		fprintf(stdout, " failed\n");
		goto builtin_err;
	}

	ret=0;

builtin_err:
		
	//if (signature)
	//	OPENSSL_free(signature);
	//signature = NULL;


	if (P)
		EC_POINT_free(P);
	if (Q)
		EC_POINT_free(Q);
	if (R)

		EC_POINT_free(R);
	//if (DHPoint)
		//EC_POINT_free(DHPoint);
	if (eckey)
		EC_KEY_free(eckey);
	eckey = NULL;
	if (group)
		EC_GROUP_free(group);
	if (ctx)
		BN_CTX_free(ctx);
	if(p) {
		OPENSSL_free(p);
	}
	if(a) {
		OPENSSL_free(a);
	}
	if(b) {
		OPENSSL_free(b);
	}
	if(x) {
		OPENSSL_free(x);
	}
	if(y) {
		OPENSSL_free(y);
	}
	if(z) {
		OPENSSL_free(z);
	}


	return ret;

}



int TCPS_SM2_verify2(char *pubkey,unsigned char *digest,int digestlen,unsigned char *Sign,int Signlen)
{
	//BN_CTX *ctx = NULL;
	//BIGNUM *p = NULL;
	//BIGNUM *a = NULL;
	//BIGNUM *b = NULL;

	EC_GROUP *group= NULL;

	EC_POINT *P = NULL;
	//EC_POINT *Q = NULL;
	//EC_POINT *R = NULL;

	BIGNUM *x = NULL;
	BIGNUM *y = NULL;
	//BIGNUM *z = NULL;
	EC_KEY	*eckey = NULL;
	//unsigned char	*signature;
	//struct timeval tv;
	//int	sig_len;
	int ret=-1;
	//BIGNUM *kinv, *rp,*order; 
	char keyX[70]={0};

	//EC_POINT * DHPoint = NULL;

	//gettimeofday(&tv,NULL);
	//printf("SM2 TIME1:%lds:%ldms\n",tv.tv_sec,(tv.tv_usec/1000));

	//if(strlen(pubkeyX)!=64)return -1;
	//if(strlen(pubkeyY)!=64)return -1;
	if((strlen(pubkey)!=66)&&(strlen(pubkey)!=130))return -1;

	//ctx = BN_CTX_new();
	//if (!ctx) return -1;
#if 0
	/* Curve SM2 (Chinese National Algorithm) */
	//http://www.oscca.gov.cn/News/201012/News_1197.htm
	p = BN_new();
	a = BN_new();
	b = BN_new();
	if (!p || !a || !b) goto builtin_err;
	group = EC_GROUP_new(EC_GFp_mont_method()); /* applications should use EC_GROUP_new_curve_GFp
 	                                             * so that the library gets to choose the EC_METHOD */
	if (!group) goto builtin_err;
	
	if (!BN_hex2bn(&p, "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF")) goto builtin_err;
	if (1 != BN_is_prime_ex(p, BN_prime_checks, ctx, NULL)) goto builtin_err;
	if (!BN_hex2bn(&a, "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC")) goto builtin_err;
	if (!BN_hex2bn(&b, "28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93")) goto builtin_err;
	if (!EC_GROUP_set_curve_GFp(group, p, a, b, ctx)) goto builtin_err;

	P = EC_POINT_new(group);
	//Q = EC_POINT_new(group);
	//R = EC_POINT_new(group);
	//if (!P || !Q || !R) goto builtin_err;
	if (!P) goto builtin_err;

	x = BN_new();
	y = BN_new();
	z = BN_new();
	if (!x || !y || !z) goto builtin_err;


	if (!BN_hex2bn(&x, "32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7")) goto builtin_err;
	if (!EC_POINT_set_compressed_coordinates_GFp(group, P, x, 0, ctx)) goto builtin_err;
	////if (!EC_POINT_is_on_curve(group, P, ctx)) goto builtin_err;
	if (!BN_hex2bn(&z, "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123")) goto builtin_err;
	if (!EC_GROUP_set_generator(group, P, z, BN_value_one())) goto builtin_err;
	


	if (EC_GROUP_get_degree(group) != 256) goto builtin_err;

	////if (!EC_GROUP_get_order(group, z, ctx)) goto builtin_err;////get R
	////if (!EC_GROUP_precompute_mult(group, ctx)) goto builtin_err;
	////if (!EC_POINT_mul(group, Q, z, NULL, NULL, ctx)) goto builtin_err;
	////if (!EC_POINT_is_at_infinity(group, Q)) goto builtin_err;


	//testing ECDSA for SM2
	/* create new ecdsa key */
	if ((eckey = EC_KEY_new()) == NULL)
		goto builtin_err;
	if (EC_KEY_set_group(eckey, group) == 0)
	{
		goto builtin_err;
	}
#endif

	if ((eckey = EC_KEY_new()) == NULL)
		goto builtin_err;
	group=ec_SM2group_from_data();
	if (!group) goto builtin_err;
	if (EC_KEY_set_group(eckey, group) == 0)
	{
		goto builtin_err;

	}	

	P = EC_POINT_new(group);

	x = BN_new();
	y = BN_new();
	if (!x || !y ) goto builtin_err;

	//gettimeofday(&tv,NULL);
	//printf("SM2 TIME2:%lds:%ldms\n",tv.tv_sec,(tv.tv_usec/1000));

	if(memcmp(pubkey,"02",2)==0)
	{
		if (!BN_hex2bn(&x,pubkey+2)) goto builtin_err;
		if (!EC_POINT_set_compressed_coordinates_GFp(group, P, x, 0, NULL)) goto builtin_err;
		if (!EC_POINT_is_on_curve(group, P, NULL)) goto builtin_err;

		//if (!EC_POINT_get_affine_coordinates_GFp(group, P, x, y, ctx)) goto builtin_err;;
	
		//fprintf(stdout, "y = 0x");
		//BNPrintf(y);
		//fprintf(stdout, "\n");
	}
	else if(memcmp(pubkey,"03",2)==0)
	{
		if (!BN_hex2bn(&x,pubkey+2)) goto builtin_err;
		if (!EC_POINT_set_compressed_coordinates_GFp(group, P, x, 1, NULL)) goto builtin_err;
		if (!EC_POINT_is_on_curve(group, P, NULL)) goto builtin_err;
	}
	else if(memcmp(pubkey,"04",2)==0)
	{
		if (!BN_hex2bn(&y, pubkey+66)) goto builtin_err;
		pubkey[66]=0;
		if (!BN_hex2bn(&x, pubkey+2)) goto builtin_err;
		
		if (!EC_POINT_set_affine_coordinates_GFp(group, P, x, y, NULL)) goto builtin_err;
	}
	else
	{
		goto builtin_err;
	}

	EC_KEY_set_public_key(eckey, P);


	//gettimeofday(&tv,NULL);
	//printf("SM2 TIME3:%lds:%ldms\n",tv.tv_sec,(tv.tv_usec/1000));

	/* check key */
/*
	if (!EC_KEY_check_key(eckey))
	{
		fprintf(stdout," check key failed\n");
		goto builtin_err;
	}
*/

	/* verify signature */
	if (SM2_verify(1, digest, 32, Sign, Signlen, eckey) != 1)
	{
		fprintf(stdout, "verify failed\n");
		goto builtin_err;
	}
	
	ret=0;
	//gettimeofday(&tv,NULL);
	//printf("SM2 TIME4:%lds:%ldms\n",tv.tv_sec,(tv.tv_usec/1000));

builtin_err:
		
	//if (signature)
	//	OPENSSL_free(signature);
	//signature = NULL;


	if (P)
		EC_POINT_free(P);
	////if (Q)
		////EC_POINT_free(Q);
	////if (R)
		////EC_POINT_free(R);

	////if (DHPoint)
		////EC_POINT_free(DHPoint);
	if (eckey)
		EC_KEY_free(eckey);
	eckey = NULL;
	if (group)
		EC_GROUP_free(group);
	//if (ctx)
	//	BN_CTX_free(ctx);
	//if(p) {
	//	OPENSSL_free(p);
	//}
	//if(a) {
	//	OPENSSL_free(a);
	//}
	//if(b) {
	//	OPENSSL_free(b);
	//}
	if(x) {
		OPENSSL_free(x);
	}
	if(y) {
		OPENSSL_free(y);
	}

	//if(z) {
	//	OPENSSL_free(z);
	//}


	return ret;

}


//===========================================
//Function:	TCPS_SM2_GetY
//Description:
//Input:	*pubkey	--- 压缩公钥
//			*Y		--- 公钥y
//			len		--- 存放公钥y的buf长度
//Output:	no
//Return:	 0 成功， 非0 失败
//Author:	mwl	
//Date:		2018-4-27
//===========================================
int TCPS_SM2_GetY(char *pubkey,char *Y,int len)
{
	BN_CTX *ctx = NULL;
	EC_GROUP *group= NULL;

	EC_POINT *P = NULL;

	BIGNUM *x = NULL;
	BIGNUM *y = NULL;
	char *yp = NULL;
	EC_KEY	*eckey = NULL;

	int ret=-1;

	char keyX[70]={0};

	if(strlen(pubkey)!=66)return -1;

	if ((eckey = EC_KEY_new()) == NULL)
		goto builtin_err;
	group=ec_SM2group_from_data();
	if (!group) goto builtin_err;
	if (EC_KEY_set_group(eckey, group) == 0)
	{
		goto builtin_err;

	}	

	P = EC_POINT_new(group);

	x = BN_new();
	y = BN_new();
	if (!x || !y ) goto builtin_err;

	//gettimeofday(&tv,NULL);
	//printf("SM2 TIME2:%lds:%ldms\n",tv.tv_sec,(tv.tv_usec/1000));

	if(memcmp(pubkey,"02",2)==0)
	{
		if (!BN_hex2bn(&x,pubkey+2)) goto builtin_err;
		if (!EC_POINT_set_compressed_coordinates_GFp(group, P, x, 0, NULL)) goto builtin_err;
		if (!EC_POINT_is_on_curve(group, P, NULL)) goto builtin_err;

	}
	else if(memcmp(pubkey,"03",2)==0)
	{
		if (!BN_hex2bn(&x,pubkey+2)) goto builtin_err;
		if (!EC_POINT_set_compressed_coordinates_GFp(group, P, x, 1, NULL)) goto builtin_err;
		if (!EC_POINT_is_on_curve(group, P, NULL)) goto builtin_err;
	}
	else
	{
		goto builtin_err;
	}

	if (!EC_POINT_get_affine_coordinates_GFp(group, P, x, y, ctx)) goto builtin_err;

	yp = BN_bn2hex(y); 
 
	if(strlen(yp)>(len-2))goto builtin_err;

    sprintf(Y,"%s", yp); 
	
	ret=0;

builtin_err:
		


	if (P)
		EC_POINT_free(P);

	if (eckey)
		EC_KEY_free(eckey);
	eckey = NULL;
	if (group)
		EC_GROUP_free(group);
	if (ctx)
		BN_CTX_free(ctx);
	if(x) {
		OPENSSL_free(x);
	}
	if(y) {
		OPENSSL_free(y);
	}

	if(yp) {
		OPENSSL_free(yp);
	}
	return ret;

}




