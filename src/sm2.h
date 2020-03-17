/************************************************************************
* 版权所有 (C)2019, 天津通卡智能网络科技股份有限公司。
* 
* 文件名称： // sm2.h
* 文件标识： // 
* 内容摘要： // sm2算法头文件
* 其它说明： // 
* 当前版本： // 
* 作    者： // mwl
* 完成日期： // 2019/05/24
* 
* 修改记录1：// 
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容： 
* 修改记录2：…
************************************************************************/
#ifndef __XXSM2_H__
#define __XXSM2_H__

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

void BNprintf(BIGNUM* bn);

//SM2_sign_setup
int SM2_sign_setup(EC_KEY *eckey, BN_CTX *ctx_in, BIGNUM **kinvp, BIGNUM **rp);

//SM2_sign_ex
int	SM2_sign_ex(int type, const unsigned char *dgst, int dlen, unsigned char *sig, unsigned int *siglen, const BIGNUM *kinv, const BIGNUM *r, EC_KEY *eckey);

//SM2_sign
int	SM2_sign(int type, const unsigned char *dgst, int dlen, unsigned char *sig, unsigned int *siglen, EC_KEY *eckey);

//SM2_verify
int SM2_verify(int type, const unsigned char *dgst, int dgst_len,const unsigned char *sigbuf, int sig_len, EC_KEY *eckey);

//SM2 DH, comupting shared point
int SM2_DH_key(const EC_GROUP * group,const EC_POINT *b_pub_key_r, const EC_POINT *b_pub_key, const BIGNUM *a_r,EC_KEY *a_eckey,unsigned char *outkey,size_t keylen);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
#endif 
