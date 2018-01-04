//#if!define(_INCLUDE_LCP_SH_D_H_)
#ifndef _LCP_SH_D_H_INCLUDE_
#define _LCP_SH_D_H_INCLUDE_
#include <aes.h>
#include <Hex.h>      // StreamTransformationFilter
#include <modes.h>
#include <osrng.h>     // PRNG
#include"toolkit\string\string.h"
#include"toolkit\cast\cast.h"
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<vector>
#include<iostream>
//#define _INCLUDE_LCP_SH_D_H_

using namespace CryptoPP;
using namespace std;
namespace cast = qingpei::toolkit::cast;
#pragma comment(lib,"cryptlib.lib")
#define Num_to_byte(ptr,i,sendframe) sendframe.push_back(*(ptr+i));reverse(sendframe.end() - i, sendframe.end());

 // Frame size limit comes from hardware implementation.
 // Frame Header
 //+-------------+---------------+-------------+
 //| Frame Start | Controller ID | Data length |
 //| 1 byte      | 8 bytes (MAC) | 4 bytes     |
 //+-------------+---------------+-----^-------+
 //                                    |
 // Frame Body                         |
 //+-----------------------------------+------------------------+
 //|                    AES Encryption Fields                   |
 //+--------------+----------+------------------+---------------+
 //| Command Code | Response | Real data length | Padding bytes |
 //| 2 bytes      | 1 byte   | 4 bytes          |               |
 //+--------------+----------+------------------+---------------+
 //
 // Frame Postfix
 //+---------+-----------+
 //| CRC16   | Frame End |
 //| 2 bytes | 1 byte    |
 //+---------+-----------+
 const std::size_t SmartEleAddrLenght = 6;
 const std::size_t kMaxFrameLength = 1024;
 const std::size_t kMacWidth = 8;	//
 const std::size_t kStartEndWidth = 1;
 const std::size_t kCrcWidth = 2;
 const std::size_t kCrcAndEndWidth = kCrcWidth + kStartEndWidth;
 // Length of plaintext body without padding bytes
 const std::size_t kLengthWidth = 4;
 const std::size_t kHeaderWidth = kStartEndWidth + kMacWidth + kLengthWidth;
 const std::size_t kCommandWidth = 2;
 const std::size_t kResponseWidth = 1;
 const std::size_t kRealBodyOffset = kLengthWidth + kCommandWidth +
     kResponseWidth;
 const std::size_t kMaxBodyLength = kMaxFrameLength - kHeaderWidth -
     kCrcAndEndWidth;
 const uint8_t kFrameStart = 0xAA;
 const uint8_t kFrameEnd = 0x55;
 const std::size_t Terminal_information_Length = 46;

 uint64_t controlID_to_hex(uint8_t *id,int length=0);

 int calcByte(int crc, char b);
 uint16_t CRC16(char *pBuffer, int length);
 int hexcharToInt(char c);
 void Hex_to_char(string hexstring, vector<uint8_t> &bytes);//将2进制数  转换成char
 std::string CBC_AESEncryptStr(std::string sKey, std::string sIV, const char *plainText);
 std::string CBC_AESDecryptStr(std::string sKey, std::string sIV, const char *cipherText);

 class AesCryp2str
 {
 public:
     AesCryp2str(byte *keyptr, byte  *ivptr, int sizekey=16);
	AesCryp2str(){};

 public:
     int InitAesCryp(byte *keyptr, byte  *ivptr, int sizekey=16);
     int InitAesCryp(string keyptr, string  string, int sizekey=16);

     int AESEncryptStr(string &info, string &Encode);
     int AESDecryptStr(string &info, string &Decode);
 private:
     CBC_Mode<CryptoPP::AES>::Encryption Encryptor;
     CBC_Mode<CryptoPP::AES>::Decryption Decryptor;

     CryptoPP::AutoSeededRandomPool rng;

 };

 class DataFrameSh
 {
 private:
     //uint8_t  control_addr[8];
     uint64_t control_addr;
     //帧头**********************************************//
     uint32_t body_length;
     uint16_t command_code;
     uint8_t	 status_code;

     uint32_t data_length;
     vector<uint8_t> data;
     //body*********************************************
     uint16_t ident_code;
     //帧尾*********************************************
     string sKey;		string sIV;
     //密钥***************************密钥不为空则进行加密
     vector<uint8_t> Vsendframe;
 public:
     vector<uint8_t> data_buf_;//
     //uint8_t          m_data[kMaxFrameLength];
     std::size_t body_fields_length_;//bodylength
 public:
     DataFrameSh();
     DataFrameSh(const string recvdata);
     DataFrameSh(uint64_t control_addr_,
         uint16_t  command_code,
         uint8_t	 status_code = 0xff,
         const uint8_t	*data = nullptr, uint32_t data_length = 0,
         const char  *aes_key = nullptr,
         const char  *aes_iv = nullptr);
public:
     void FrameSerialize(vector<uint8_t> &sendframe);							//序列化  用以发送

     const uint8_t* Create_Frame(uint64_t control_addr_, uint16_t  command_code,uint8_t status_code = 0xff,
         uint32_t data_length_=0,const uint8_t *data = nullptr); //构造数据帧  ，一般由Dataprocess对象调用

     int Analy_Frame(const vector<uint8_t> recvdata);	//解析 1.数据是否完整 2.数据的解密
                     //0表示解析成功 1表示数据帧有误 2表示效验码出错
     uint8_t statuscode(){return status_code;}
     int CheckHead(uint8_t *Headptr,int read=0);	//对帧头的读取进行判断					//0表示读取失败，其他表示有效长度
     int Checkbody(uint8_t *bodyptr);			//根据帧头读取的数据继续进行读取  进行crc解密 操作
     int CheckKey(); 
     int CheckCrc();                             //crc 效验 0 1
     int Datalength(){return data_length;}
     size_t DropBytesBeforeNextStart();

 public://return frame data
     size_t bodylength(){ return body_length; }//数据帧body的长度
     int FrameLength(){ return kCrcAndEndWidth + kHeaderWidth + body_length; }//帧的长度
     uint16_t command(){ return command_code; }//帧的命令码
     uint64_t Addr(){return control_addr;}//return controller address
     const uint8_t *Frame(){return data.data();}

     //const uint8_t *SendFrame(){return Vsendframe.data();}


     std::size_t data_fields_length() const;
     bool set_data_fields_length(std::size_t new_length);
     void ResizeForBodyAndPostfix();
	string StrFrame();//为gtest 准备 一般用不上
     void Data(vector<uint8_t> &R){ for(int i=0;i<data.size();++i)R.push_back(data[i]);}
     const uint8_t *Data();	//解密后

     //const uint8_t *FrameData();帧序列化后  的uint8指针 SendFrame

     uint8_t *FrameData();//帧序列化后  的uint8指针


 public:    //加密解密
     int enCryptFrame(const char *aes_key = nullptr, const char *aes_iv = nullptr);
     int DeCryptFrame();	   //解密 0表示解密成功 1表示解密失败 2表示为加密无需解密 3表示data有误
     int SetAesKey(byte *aeskey,byte *aesiv);
     AesCryp2str aes;
 private:
     uint16_t Crc_Code(vector<uint8_t> crcdata, uint32_t crcsize);//crc效验码
     void FrameInitialize();//初始化帧数据

 };
 //typedef shared_ptr<DataFrameSh> Frame_ptr;


#endif
