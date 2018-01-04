#include "stdafx.h"
#include"lcp_sh_d.h"
#include"toolkit\cast\cast.h"

        namespace cast = qingpei::toolkit::cast;
#define NSIZE 22
        char crc_array[300] = { 0 };
        int calcByte(int crc, char b)
        {
            int i;
            crc = crc ^ (int)b << 8;

            for (i = 0; i < 8; i++)
            {
                if ((crc & 0x8000) == 0x8000)
                    crc = crc << 1 ^ 0x1021;
                else
                    crc = crc << 1;
            }

            return crc & 0xffff;
        }
        uint16_t CRC16(char *pBuffer, int length)
        {
            uint16_t wCRC16 = 0;
            int i;
            if ((pBuffer == 0) || (length == 0))
            {
                return 0;
            }
            for (i = 0; i < length; i++)
            {
                wCRC16 = calcByte(wCRC16, pBuffer[i]);
            }
            return wCRC16;
        }

        uint64_t controlID_to_hex(uint8_t *id, int length)
        {
            vector<uint8_t> addr;
            string hex = (char*)id;
            if (length > kMacWidth)
            {
                Hex_to_char(hex, addr);
                return cast::bytes_to_num<uint64_t>(addr.data(), kMacWidth);

            }
            /*template<class uint64_t>
            uint64_t cast::bytes_to_num();*/
            return cast::bytes_to_num<uint64_t>(id, 2 * kMacWidth);

        }
        int hexcharToInt(char c)
        {
            if (c >= '0' && c <= '9') return (c - '0');
            if (c >= 'A' && c <= 'F') return (c - 'A' + 10);
            if (c >= 'a' && c <= 'f') return (c - 'a' + 10);
            return 0;
        }
        void Hex_to_char(string hexstring, vector<uint8_t> &bytes)//16进制的转换成char
        {
            //cout<<"length is :"<<sizeof(hexstring)/sizeof(char)<<endl;
            uint8_t tmp;
            for (int i = 0; i < hexstring.size(); i += 2) {
                tmp = (uint8_t)((hexcharToInt(hexstring[i]) << 4)
                    | hexcharToInt(hexstring[i + 1]));
                bytes.push_back(tmp);
            }
        }

        string CBC_AESEncryptStr(std::string sKey, std::string sIV, const char *plainText)
        {
            std::string outstr;

            //填key
            SecByteBlock key(AES::MAX_KEYLENGTH);
            memset(key, 0x30, key.size());
            sKey.size() <= AES::MAX_KEYLENGTH ? memcpy(key, sKey.c_str(), sKey.size()) : memcpy(key, sKey.c_str(), AES::MAX_KEYLENGTH);

            //填iv
            byte iv[AES::BLOCKSIZE];
            memset(iv, 0x30, AES::BLOCKSIZE);
            sIV.size() <= AES::BLOCKSIZE ? memcpy(iv, sIV.c_str(), sIV.size()) : memcpy(iv, sIV.c_str(), AES::BLOCKSIZE);

            AES::Encryption aesEncryption((byte *)key, AES::MAX_KEYLENGTH);

            CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

            StreamTransformationFilter cbcEncryptor(cbcEncryption, new HexEncoder(new StringSink(outstr)));
            cbcEncryptor.Put((byte *)plainText, strlen(plainText));
            cbcEncryptor.MessageEnd();

            return outstr;
        }
        string CBC_AESDecryptStr(std::string sKey, std::string sIV, const char *cipherText)
        {
            std::string outstr;

            //填key
            SecByteBlock key(AES::MAX_KEYLENGTH);
            memset(key, 0x30, key.size());
            sKey.size() <= AES::MAX_KEYLENGTH ? memcpy(key, sKey.c_str(), sKey.size()) : memcpy(key, sKey.c_str(), AES::MAX_KEYLENGTH);

            //填iv
            byte iv[AES::BLOCKSIZE];
            memset(iv, 0x30, AES::BLOCKSIZE);
            sIV.size() <= AES::BLOCKSIZE ? memcpy(iv, sIV.c_str(), sIV.size()) : memcpy(iv, sIV.c_str(), AES::BLOCKSIZE);


            CBC_Mode<AES >::Decryption cbcDecryption((byte *)key, AES::MAX_KEYLENGTH, iv);

            HexDecoder decryptor(new StreamTransformationFilter(cbcDecryption, new StringSink(outstr)));
            decryptor.Put((byte *)cipherText, strlen(cipherText));
            decryptor.MessageEnd();

            return outstr;
        }


        AesCryp2str::AesCryp2str(byte *keyptr, byte  *ivptr, int sizekey):
                    Encryptor(keyptr, sizekey, ivptr),
                    Decryptor(keyptr, sizekey, ivptr)
        {

        }



        int AesCryp2str::InitAesCryp(byte *keyptr, byte  *ivptr, int sizekey)
        {
            Encryptor.SetKeyWithIV(keyptr, sizekey, ivptr);

            Decryptor.SetKeyWithIV(keyptr, sizekey, ivptr);
			return 0;
		}
        int AesCryp2str::InitAesCryp(string keyptr, string  ivptr, int sizekey)
        {   //设置密钥 与向量
            Encryptor.SetKeyWithIV((const uint8_t*)keyptr.data(), sizekey, (const uint8_t*)ivptr.data());
            Decryptor.SetKeyWithIV((const uint8_t*)keyptr.data(), sizekey, (const uint8_t*)ivptr.data());
			return 0;
        }

        int AesCryp2str::AESEncryptStr(string &info, string &Encode)
        {
            CryptoPP::StringSource(info, true,
                    new CryptoPP::StreamTransformationFilter(Encryptor,
                    new CryptoPP::StringSink(Encode)
                    ) // StreamTransformationFilter
                    );
			return 0;
        }

        int AesCryp2str::AESDecryptStr(string &info, string &Decode)
        {
            CryptoPP::StringSource(info, true,
                    new CryptoPP::StreamTransformationFilter(Decryptor,
                    new CryptoPP::StringSink(Decode)
                    ) // StreamTransformationFilter
                    );
			return 0;
        }



        DataFrameSh::DataFrameSh() : command_code(0), status_code(0),
            data_length(0), body_length(0){}

        DataFrameSh::DataFrameSh(const string recvdata)
        {

        }

        DataFrameSh::DataFrameSh(uint64_t control_addr_,
            uint16_t  command_code_, uint8_t	 status_code_,
            const uint8_t	*data_, uint32_t  data_length_,
            const char  *aes_key, const char  *aes_iv)
            : command_code(command_code_), status_code(status_code_),
            data_length(data_length_), control_addr(control_addr_)
        {
            FrameInitialize(); int i = 0;
            data_length = data_length_;
            body_length = data_length + kRealBodyOffset;
            vector<uint8_t> iid; string iddata;


            for (i = 0; i < data_length; ++i)
                data.push_back(data_[i]);

            if (aes_key != nullptr)
            {
                enCryptFrame(aes_key, aes_iv);
                sKey = aes_key;
                sIV = aes_iv;
            }

        }

        int DataFrameSh::SetAesKey(byte *aeskey,byte *aesiv)
        {//string sKey;		string sIV;
            for(int i=0 ; i< 16;++i)
            {
                sKey.push_back(aeskey[i]);
                sIV.push_back(aesiv[i]);
            }
			return 0;
        }

        //create_frame 来构造新的 数据帧 用以发送
        const uint8_t* DataFrameSh::Create_Frame(uint64_t control_addr_, uint16_t  command_code_,
            uint8_t status_code_,uint32_t data_length_,const uint8_t *data_)
        {
            FrameInitialize(); int i = 0;
            body_length = data_length_ + kRealBodyOffset;
            vector<uint8_t> iid; string iddata;

            control_addr = control_addr_;
            command_code = command_code_;
            status_code = status_code_;
            data_length = data_length_;
            for (i = 0; i < data_length_; ++i)
                data.push_back(data_[i]);
            if (!sKey.empty())
            {
                enCryptFrame(sKey.c_str(), sIV.c_str());
            }
            FrameSerialize(Vsendframe);
            return Vsendframe.data();
        }
        int DataFrameSh::CheckHead(uint8_t *Headptr,int read)//检查收到的帧头
        {

            int MaxRead = kHeaderWidth + read;
            data_buf_.clear();
            for (int i = read; i < MaxRead; ++i)
            {
                data_buf_.push_back(Headptr[i]);
            }
            body_fields_length_ = data_buf_[MaxRead - 1] + (data_buf_[MaxRead - 2] << 8)
                + (data_buf_[MaxRead - 3] << 16) + (data_buf_[MaxRead-4] << 24);
            if (body_fields_length_ < kRealBodyOffset)
                return 0;
            else
                body_length = body_fields_length_;
            return body_fields_length_;
        }
		string DataFrameSh::StrFrame()//为gtest 准备 一般用不上
		{
			string myframe;
			for (int i = 0; i < data.size(); ++i)
			{
				myframe.push_back(data[i]);
			}
			return myframe;
		}
        int DataFrameSh::Checkbody(uint8_t *bodyptr)//根据收到的帧头都得body 进行crc效验的检查
        {
            for (int i = 0; i < body_fields_length_ + kCrcAndEndWidth; ++i)
                data_buf_.push_back(bodyptr[kHeaderWidth + i]);
            vector<char> nullvect;
            int code = Analy_Frame(data_buf_);
            if (2 == code)
            {
                cout << "crc error"<<endl;
            }
            else if (1 == code)
            {
                cout << "error"<<endl;
            }
            else if (0 == code)
            {
                cout << "read end"<<endl;
                memset(bodyptr, '\0', kMaxFrameLength);
            }

            cout <<hex<< control_addr<<endl;
            data_buf_.clear();

            return code;
        }

        int DataFrameSh::CheckKey()
        {
            if (sKey.empty())
                return 0;
            else
                return 1;
        }

        int DataFrameSh::enCryptFrame(const char *aes_key,//对data进行加密
            const char *aes_iv)
        {   //
            string k=aes_key;string i = aes_iv;
            aes.InitAesCryp(k,i);
            string encode,temp;
            for(char ch:data)
            {
                temp.push_back(ch);
            }
            aes.AESEncryptStr(temp,encode);
            data.clear();
            for(uint8_t ch : encode )
            {
                data.push_back(ch);
            }

//            while (data.size() % 16 != 0)
//                data.push_back(0);
//            string data_str;
//            for (i = 0; i < data.size(); ++i)
//                data_str.push_back(data[i]); data.clear();
//            string encryptdata = CBC_AESEncryptStr(aes_key, aes_iv, data_str.data());

//            for (i = 0; i < encryptdata.size(); ++i)
//                data.push_back(encryptdata[i]);
//            sKey = aes_key; sIV = aes_iv;
            if (data.size() % 16 == 0)
            {
                body_length = data.size() + kRealBodyOffset;
                return 0;    //加密成功
            }
            else

                return 1;	//否则失败
        }

        int DataFrameSh::DeCryptFrame()
        {
            //	0表示解密成功 1表示解密失败 2表示未加密无需解密 3表示data有误
            if (data.size() % 16 != 0)
            {
                if (data.size() == data_length)
                    return 2;
                else
                    return 3;
            }
            string data_str, decryptdata;
            for (int i = 0; i < data.size(); ++i)
                data_str.push_back(data[i]);
            data.clear();
            aes.AESDecryptStr(data_str, decryptdata);
            for(uint8_t ch:decryptdata)
            {
                data.push_back(ch);
            }
//            string decryptdata = CBC_AESDecryptStr(sKey, sIV, data_str.data());

//            for (int i = 0; i < decryptdata.size(); ++i)
//                data.push_back(decryptdata[i]);

            if (decryptdata.size() != data_length)
                return 1;

            return 0;
        }
        void DataFrameSh::FrameInitialize()//对帧 进行初始化
        {
            data_length = 0;
            body_length = kRealBodyOffset;
            data.clear();
        }

        void DataFrameSh::FrameSerialize(vector<uint8_t> &sendframe)
        {
            int i = 0;
            sendframe.clear();
            //aa3738393435363132 00000010 00fe ff 00000009 01 02 39 04b2  1 1 ffe4 03 00 0000 55
            sendframe.push_back(kFrameStart);
        /*	for (; i < 8; ++i)
                sendframe.push_back(control_addr[i]);*/
            uint8_t *ptr; ptr = (uint8_t*)&control_addr;
            for (i = 0; i < kMacWidth; ++i)Num_to_byte(ptr, i, sendframe);
            ptr = (uint8_t*)&body_length;
            for (i = 0; i < 4; ++i)Num_to_byte(ptr, i, sendframe);//宏转换成网络字节序
            ptr = (uint8_t*)&command_code;
            for (i = 0; i < 2; ++i)Num_to_byte(ptr, i, sendframe);
            sendframe.push_back(status_code);
            ptr = (uint8_t*)&data_length;
            for (i = 0; i < 4; ++i)Num_to_byte(ptr, i, sendframe);
            for (i = 0; i < data.size(); ++i)sendframe.push_back(data[i]);
            ident_code = Crc_Code(sendframe, sendframe.size());
            ptr = (uint8_t*)&ident_code;
            for (i = 0; i < 2; ++i)Num_to_byte(ptr, i, sendframe);
            sendframe.push_back(kFrameEnd);

        }


        uint8_t *DataFrameSh::FrameData()
        {
            FrameSerialize(Vsendframe);
            return (uint8_t*)Vsendframe.data();//(uint8_t*)sendframe.c_str();
        }
        uint16_t DataFrameSh::Crc_Code(vector<uint8_t> crcdata, uint32_t crcsize)
        {
            char *crc_code = new char[crcsize];
            memset(crc_code, 0, crcsize);
            //strcpy_s(crc_code, sendframe.size(), sendframe.data());
            for (int i = 0; i < crcsize; ++i)
                crc_code[i] = crcdata[i];
            uint16_t code = CRC16(crc_code, crcsize);
			delete []crc_code;
			crc_code = nullptr;
            return code;
        }

        std::size_t DataFrameSh::DropBytesBeforeNextStart() {
            if (data_buf_.empty()) {//判断是否存在0xaa 没有则前移一位。。。
                return 0;
            }
            else {
                data_buf_.erase(data_buf_.begin());
            }

            while (!data_buf_.empty() &&
                data_buf_.front() != kFrameStart) {
                data_buf_.erase(data_buf_.begin());
            }

            return data_buf_.size();
        }

        std::size_t DataFrameSh::data_fields_length() const {
            return body_fields_length_;
        }

        bool DataFrameSh::set_data_fields_length(std::size_t new_length) {

            bool success = true;

            if (new_length > kMaxBodyLength) {
                    cout<< "Frame body length " << body_fields_length_ << " exceeds a soft limit of "
                    << kMaxBodyLength << " bytes."
                    << " Controller may not handle this giant frame properly.";
                body_fields_length_ = kMaxBodyLength;
                success = false;
            }

            body_fields_length_ = new_length;
            ResizeForBodyAndPostfix();
            return success;
        }
        void DataFrameSh::ResizeForBodyAndPostfix() {
            data_buf_.resize(kHeaderWidth + body_fields_length_ + kCrcAndEndWidth);
        }


        int DataFrameSh::CheckCrc()
        {
            vector<uint8_t> temp;
            FrameSerialize(temp);
            uint16_t crc_tp = Crc_Code(temp,temp.size()-kCrcAndEndWidth);
            if(crc_tp == ident_code)
                return 0;
            else
                return 1;
        }

        int DataFrameSh::Analy_Frame(const vector<uint8_t> recvdata)//数据帧有误
        {
            //0表示解析成功 1表示数据帧有误 2表示效验码出错
            int i = 0; int j = 0;
            if (recvdata.size() > 22)
            {
				uint8_t fdata[9] = {0};
                if (recvdata[0] != 0xaa && recvdata[recvdata.size() - 1] != 0x55)
                {
                    cout << "数据帧有误";
                    return 1;
                }
                FrameInitialize();
                control_addr = cast::bytes_to_num<uint64_t>(recvdata.data() + 1, kMacWidth);
                cout<<"control_aadr:"<<hex<<control_addr<<endl;
                /*		for (i = 1, j = 0; i < 9; ++i, j++)*/
                /*control_addr = (recvdata[i]<<j*8);*/
                for (i = 9, j = 0; i < 13; ++i, ++j)fdata[3 - j] = recvdata[i];
                body_length = fdata[0] + (fdata[1] << 8) + (fdata[2] << 16) + (fdata[3] << 24);
				if (body_length + kHeaderWidth + kCrcAndEndWidth != recvdata.size())
					return 1;
				//_itoa_s(operation_code, fdata, 2, 16);
                memset(fdata, 0, 9);
                for (i = 13, j = 0; i < 15; ++i, ++j)fdata[j] = recvdata[i];
                command_code = cast::bytes_to_num<uint16_t>((uint8_t*)fdata, kCommandWidth);

                status_code = recvdata[15];
                for (i = 16, j = 0; i < 20; ++i, ++j)fdata[3 - j] = recvdata[i];
                data_length = fdata[0] + (fdata[1] << 8) + (fdata[2] << 16) + (fdata[3] << 24);
				
                if(data_length + kRealBodyOffset <= body_length && !sKey.empty())
                {
                    string temp,decode;
                    for(i = 20, j = 0; i < (recvdata.size() - 3); ++i, ++j)
                    {
                        temp.push_back(recvdata[i]);;
                    }
                    aes.AESDecryptStr(temp,decode);
                    if(decode.size() != data_length) return 1;//error

                    for(char ch:decode)
                    {
                        data.push_back(ch);
                    }

                }
                else
                {
                    for (i = 20, j = 0; i < (recvdata.size() - 3); ++i, ++j)data.push_back(recvdata[i]);
                // data_pad = famedata[famedata.size() - 3];
                }
                for (i = (recvdata.size() - 3), j = 0; i < recvdata.size(); ++j, ++i)
                    fdata[j] = recvdata[i];
                ident_code = cast::bytes_to_num<uint16_t>((uint8_t*)fdata, kCrcWidth);;//= atoi(fdata);+ 0x0100
                uint16_t a = Crc_Code(recvdata, recvdata.size() - 3); //对帧是否完整进行crc效验
                if (a != ident_code && a  != ident_code + 0x0100)
                    return 2;
				
                FrameSerialize(data_buf_);
                return 0;

            }

        }

        const uint8_t *DataFrameSh::Data()//返回的指针
        {
            if (data.size() != data_length)
                int code = DeCryptFrame();
            return data.data();
        }
  