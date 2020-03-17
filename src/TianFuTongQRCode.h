#ifndef HEADER_TianFuTong_H
#define HEADER_TianFuTong_H



#ifdef  __cplusplus
extern "C" {
#endif


#define TianFuTong_NO  0xfe0c 



/*************************************************
*  Function:       // Read_TianFuTong_PARAM
*  Description:    // 获取 参数信息
*  Input:          // 无
*                  //
*  Output:         // 无
*                  //
*  Return:         // －1：失败    0：成功
*  Others:         // 其它说明
*************************************************/
int Read_TianFuTong_PARAM(void);

/*************************************************
*  Function:       // check_TianFuTong_qrcode_demo
*  Description:    // 验证二维码
*  Input:          // *qrcode	:二维码数据
*                  // *qrcode_len	:二维码数据长度
*  Output:         // QR_INFO  二维码结果
*  Return:         // －2：二维码结构错误  －1:失败  0 : 成功
*  Others:         // 其它说明
*************************************************/
int check_TianFuTong_qrcode_demo(unsigned char* qrcode, int* qrcode_len);


#ifdef  __cplusplus
}
#endif

#endif 
