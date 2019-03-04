//-------------- CPU ---------------------------
#define		UserCardCommand		0x03	//;用户卡命令码
#define		UserCardLength		39	//;用户卡长度
#define		InitCardType		0x01	//;初始化卡类型码
#define		UserCardType		0x02	//;普通用户卡类型码
#define		RecuitCardType		0x03	//;补卡类型码

#define		UserOpenCardType		0x01	//;开户卡类型码
#define		UserchargeCardType	0x02	//;普通用户卡类型码
#define		UserRecuitCardType	0x03	//;补卡类型码
#define		UserTestCardType		0x04	//;测试卡类型码
#define		UserKeyCardType		  0x05	//;密钥卡类型码



#define		PresetCardCommand	0x04	//;预制卡命令码
#define		PresetCardLength	26	//;预制卡长度

#define		OK_CARD_PROC		0	//卡片操作成功
#define		IC_VLOW_ERR			1	//表计电压过低176V(80%Un)
#define		IC_ESAM_RDWR_ERR	2	//操作ESAM错误
#define		IC_ESAM_RST_ERR		3	//ESAM复位错误
#define		IC_IC_RST_ERR		4	//卡片复位错误:反插卡或插铁片
#define		IC_AUTH_ERR			5	//身份认证错误(通信成功但密文不匹配)
#define		IC_EXAUTH_ERR		6	//外部身份认证错误(通信成功但认证不通过)
#define		IC_NOFX_ERR			7	//未发行的卡片(读卡片时返回6B00)
#define		IC_TYPE_ERR			8	//卡类型错误
#define		IC_KER_ERR			9	//卡片操作未授权：密钥状态不为公钥是插参数预制卡
#define		IC_MAC_ERR			10	//重复开户 
#define		IC_ID_ERR			11	//表号不一致
#define		IC_HUHAO_ERR		12	//客户编号不一致
#define		IC_SERIAL_ERR		13	//卡序号不一致
#define		IC_FORMAT_ERR		14	//卡片文件格式不合法
#define		IC_NOOPEN_ERR		15	//购电卡插入未开户电表
#define		IC_NOOPEN2_ERR		16	//补卡插入未开户电表
#define		IC_TIMES_ERR		17	//购电次数错误
#define		IC_NONULL_ERR		18	//用户卡返写信息文件不为空
#define		IC_NOIN_ERR			19	//操作卡片通讯错误(读写卡片失败)
#define		IC_PREIC_ERR		20	//提前拔卡
#define		IC_OVERMAX_ERR		21	//剩余金额超囤积
#define		IC_SELECTION_ERR			22	//表号不一致


//-------------- CPU end -----------------------
