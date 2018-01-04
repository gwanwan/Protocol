// protocols.cpp : ???????????¨®??????????
//
#define _CRT_SECURE_NO_DEPRECATE 1

#define _CRT_NONSTDC_NO_DEPRECATE 1

#include"aes.h"
#include"protocols.h"
#include<iostream>
#include<stdio.h>
#include <stdlib.h>

using namespace std;


#define NSIZE 22
char crc_array[300]={0};//????CRC§µ?????????????????

int calcByte(int crc, char b)
{
     int i;
 crc = crc ^ (int)b << 8;

    for ( i = 0; i < 8; i++)
    {
        if ((crc & 0x8000) == 0x8000)
            crc = crc << 1 ^ 0x1021;
        else
            crc = crc << 1;
    }

    return crc & 0xffff;
}
uint16_t CRC16a(char *pBuffer, int length)
{
 uint16_t wCRC16=0;
 int i;
 if (( pBuffer==0 )||( length==0 ))
 {
  return 0;
 }
 for ( i = 0; i < length; i++)
 {
  wCRC16 = calcByte(wCRC16, pBuffer[i]);
 }
 return wCRC16;
}
string currenttime()
{
    time_t t = time(NULL);
    struct std::tm *now = localtime(&t);
    uint16_t year = static_cast<uint16_t>(now->tm_year + 1900);
    uint8_t month = static_cast<uint8_t>(now->tm_mon + 1);
    uint8_t day = static_cast<uint8_t>(now->tm_mday);
    uint8_t dayofweek = static_cast<uint8_t>(now->tm_wday);
    uint8_t hour = static_cast<uint8_t>(now->tm_hour);
    uint8_t minute = static_cast<uint8_t>(now->tm_min);
    uint8_t second = static_cast<uint8_t>(now->tm_sec);
    if (dayofweek == 0) {
        dayofweek = 7;  // Sunday == 7 in this protocol
    }
    std::ostringstream ss;
    char *timedata = new char[8];
    _itoa_s(year, timedata, 8, 10);
    ss << static_cast<int>(year);
    ss << static_cast<int>(month);
    ss << static_cast<int>(day);
    ss << static_cast<int>(dayofweek);
    ss << static_cast<int>(hour) ;
    ss << static_cast<int>(minute);
    ss << static_cast<int>(second);

    delete[]timedata;
    std::string time_string = ss.str();
    return time_string;
}


string Coord_debug::create(string mydata)
{//create fame
    int j=0;
    head = 0x02;
    end  = 0x03;
                                              //if(mydata.size()<3)data.push_back('@');
    data.append(transform(mydata));
    string crc;
    crc.push_back(0x01);

    crc.push_back(0x02);
    crc.push_back(type);
    for (int i = 0; i < data.size(); ++i)
    {
        crc.push_back(data[i]);
    }
    char *crc_co = new char[crc.size()-2];
    memset(crc_co, 0, crc.size()-2);
    for(int i=0;i<crc.size()-2;++i)crc_co[i]=crc[i+2];
    //ident_code = CRC16_1(crc_code, myfame.size());
    crc_code = CRC16a(crc_co,crc.size()-2);
    char a='0';
    string ident;
    char *ptr;ptr = (char*)&crc_code;

    for (int i = 0; i < 2; ++i)PUSHZERO(ptr,i,ident);
    reverse(ident.end()-2,ident.end());
    string aftranident=transform(ident);    //to exchanged 0x02 0x03

    crc.append(aftranident);
    crc.push_back(0x03);
    //crc.push_back(0x03);
    return crc;
}

//aa31323334353637381e00000006000007000000 3200aa55c0ff33 8615c055

void Coord_debug::Analy(vector<uint8_t> str, Coord_debug &msg)
{
    //DeSerialize(str, msg);
    int k=0;
    //if(str[1] == 0x20)k=1;
    head=str[1];
    type=str[2];
    string detransf;
    for(int i=3+k;i<str.size()-1;i++)detransf.push_back(str[i]);
    detransf = Detransform(detransf);
    if(detransf.size()>1 && type == 0x20 && str[3]==0x3a)
    {
        data.push_back(detransf[0]);
        data.push_back(detransf[2]);
        data.push_back(detransf[1]);
        data.push_back(detransf[3]);
        crc_code = (detransf[4])+(detransf[4]<<5);
    }
    else if( type == 0x20)
    {
        data.push_back(detransf[0]);
        data.push_back(detransf[1]);
    }
    else
         data.push_back(detransf[0]);

    //decrc.pop_back();
    char *crc_co = new char[str.size()];memset(crc_co, 0, str.size());
    for(int i=1;i<str.size()-3;i++)
        crc_co[i]=str[i];
    //ident_code = CRC16_1(crc_code, myfame.size());
    uint16_t code  = CRC16a(crc_co,str.size());
    if (crc_code == code){
        cout << "data complete" << endl;
    }

}
//analy frame
string Coord_debug::transform(string f_data)
{
    int i = 0;
    string sedata;
    if(!f_data.empty())
    {
        for (; i < f_data.size() - 1; ++i)
    {
        if (f_data[i] == 0x02){ sedata.push_back(0x1B); sedata.push_back(0xE7); }
        else if (f_data[i] == 0x03){ sedata.push_back(0x1B); sedata.push_back(0xE8); }
        else if (f_data[i] == 0x1B){ sedata.push_back(0x1B); sedata.push_back(0x00); }
        else
        {
            sedata.push_back(f_data[i]);
        }
    }
         sedata.push_back(f_data[i]);
    }
    return sedata;
}
string Coord_debug::Detransform(string mydata)
{
    string f_data;int i = 0;
    string sedata = mydata;
    uint8_t a,b;
    for (; i < sedata.size() - 1; ++i)
    {
        a=sedata[i];b=sedata[i+1];
        if ( a == 0x1b &&  b == 0xe7){ f_data.push_back(0x02); i += 1; }
        else if (  a == 0x1b && b == 0xe8){ f_data.push_back(0x03); i += 1; }
        else if (  a == 0x1b && b == 0x00){ f_data.push_back(0x1B); i += 1; }
        else
        {
            f_data.push_back(sedata[i]);
        }
    }
    f_data.push_back(sedata[i]);
    //sedata = f_data;
    return f_data;
}

//                   length    op  st datalen         data              ident end
//aa0000000016000000 00000010 00fe ff 00000009 01 02 39 c20831311414 03 ff00 55
//aa00000000b0ffe51e 0000000e 00fe ff0 0000007 01 02 39 0bc100       03 0030 55
//aa0000000000000000 00000010 00fe ff 00000009 01 02 39 432117 ff99 03 0014 55
uint16_t Data_fame::insertHexnum(string bnum)
{
    uint16_t renum;
    //data_len = fdata[0]+(fdata[1]<<8)+(fdata[2]<<16)+(fdata[3]<<24);
    string ptr;int i;
    //number.data();
    char num;
    for(i=bnum.size();i<4;i++)ptr.push_back(0x0);
    for(i=0;i<bnum.size();++i)
    {
        num = bnum[i];
        if(num == '1')ptr.push_back(0x01);else if(num == '2')ptr.push_back(0x02);
        else if(num == '0')ptr.push_back(0x00);else if(num == '3')ptr.push_back(0x03);
        else if(num == '4')ptr.push_back(0x04); else if(num == '5')ptr.push_back(0x05);
        else if(num == '6')ptr.push_back(0x06); else if(num == '7')ptr.push_back(0x07);
        else if(num == '8')ptr.push_back(0x08);else if(num == '9')ptr.push_back(0x09);
        else if(num == 'a'||num == 'A')ptr.push_back(0x0a); else if(num == 'b'||num == 'B')ptr.push_back(0x0b);
        else if(num == 'c'||num == 'C')ptr.push_back(0x0c);else if(num == 'd'||num == 'D')ptr.push_back(0x0d);
        else if(num == 'f'||num == 'F')ptr.push_back(0x0f);else if(num == 'e'||num == 'E')ptr.push_back(0x0e);

    }

    renum = ptr[3]+(ptr[2]<<4)+(ptr[1]<<8)+(ptr[0]<<12);
    return renum;
}

Data_fame::Data_fame(char *control_id,uint16_t code, uint16_t op_code, uint8_t st_code,
const char *dat, uint8_t pad_d, string aeskey, string keyiv) :protocols_head(control_id)
    , protocols_end(code), operation_code(op_code), status_code(st_code),MyAes(aeskey,keyiv)
{
    real_data_len = strlen(dat);
    int i;//?????? ????????data????
    string cbcen ;
    char *aesdat = new char[real_data_len];
    strcpy(aesdat,dat);
    if (aeskey.size() != 0)
    {
        //AES cry(aeskey,keyiv);
        cbcen=MyAes.Cipher(aesdat);
        i = cbcen.size();
    }

    data_pad = cbcen.size() - real_data_len;       //????
    data_len = cbcen.size() + 25;
    for (i = 0; i < cbcen.size(); ++i)data.push_back(cbcen.c_str()[i]);
    string crc =identSerialize(*this);
    char *data_crc = new char[crc.size()];
    memset(data_crc, 0,crc.size());
    strcpy(data_crc,crc.c_str());
    //ident_code = CRC16_1(crc_code, myfame.size());
    ident_code  = CRC16a(data_crc,crc.size());
}

Data_fame::Data_fame(const string F_data)//deserialize
{
    int i = 0;
    int j = 0;
    if(F_data.size()>23)
    {
        string famedata = F_data;
        char *fdata = new char[9];
        head_fame = famedata[i];
        for (i = 1; i < 9; ++i, j++)
        controller_ID[j] = famedata[i];
        string crcdata;//= fdata

        for (i = 9, j = 0; i < 13; ++i, ++j)fdata[3-j] = famedata[i];//????????????
       // data_len = atoi(fdata);
         data_len = fdata[0]+(fdata[1]<<8)+(fdata[2]<<16)+(fdata[3]<<24);
        //data = new char[data_len - 7];
        memset(fdata,0,9);
        //_itoa_s(operation_code, fdata, 2, 16);
        for (i = 13, j = 0; i < 15; ++i, ++j)fdata[1-j] = famedata[i];
        operation_code = fdata[0]+(fdata[1]<<8);// atoi(fdata);
        status_code = famedata[15];

        for (i = 16, j = 0; i < 20; ++i, ++j)fdata[3-j] = famedata[i];//????????????
        real_data_len = fdata[0]+(fdata[1]<<8)+(fdata[2]<<16)+(fdata[3]<<24);// atoi(fdata);
        for (i = 20, j = 0; i < (famedata.size() - 2); ++i, ++j)data.push_back(famedata[i]);//

       // data_pad = famedata[famedata.size() - 3];
        for (i = (famedata.size() - 3), j = 0; i < famedata.size(); ++j, ++i)
            fdata[1-j] = famedata[i];
        ident_code = fdata[0]+(fdata[1]<<8);//= atoi(fdata);
        end_fame = famedata[i];
    }
}

void Data_fame::Data_fameSet(const string F_data)
{
    int i = 0;
    int j = 0;
    if(F_data.size()>23)
    {
        string famedata = F_data;
        char *fdata = new char[9];
        head_fame = famedata[i];
        for (i = 1; i < 9; ++i, j++)
        controller_ID[j] = famedata[i];
        string crcdata;//= fdata

        for (i = 9, j = 0; i < 13; ++i, ++j)fdata[4-j] = famedata[i];//????????????
        data_len = atoi(fdata);
        data = new char[data_len - 7];

        //_itoa_s(operation_code, fdata, 2, 16);
        for (i = 13, j = 0; i < 15; ++i, ++j)fdata[2-j] = famedata[i];
        operation_code = atoi(fdata);
        status_code = famedata[15];

        for (i = 16, j = 0; i < 20; ++i, ++j)fdata[4-j] = famedata[i];//????????????
        real_data_len = atoi(fdata);
        for (i = 20, j = 0; i < (famedata.size() - 4); ++i, ++j)data[j] = famedata[i];//

        data_pad = famedata[famedata.size() - 3];
        for (i = (famedata.size() - 3), j = 0; i < (famedata.size() - 1); ++j, ++i)
            fdata[2-j] = famedata[i];
        ident_code = atoi(fdata);
        end_fame = famedata[famedata.size()];
    }
}

void Data_fame::Fame_cout()
{
    //cout << data << endl;int length
    for (int ddd = 0; ddd < data.size(); ++ddd)
        cout << data[ddd];
    cout << endl;
}

void Data_fame::myserialize(vector<char> &myfame)
{
    int i = 0;
    //aa3738393435363132 00000010 00fe ff 00000009 01 02 39 04b2  1 1 ffe4 03 00 0000 55
    //aa3738393435363132 00000010 00fe ff 00000009 01 02 39 08ae 3131 ffce 03 00 0000 55
    myfame.push_back(head_fame);
    for (; i < 8; ++i)
    {
        myfame.push_back(controller_ID[i]);//myfame.push_back(static_cast<int>controller_ID[i]);
    }char *ptr;ptr = (char*)&data_len;
    for (i = 0; i < 4; ++i)PUSHZERO(ptr,i,myfame);reverse(myfame.end()-i,myfame.end());
    ptr = (char*)&operation_code;
    for (i = 0; i < 2; ++i)PUSHZERO(ptr,i,myfame);reverse(myfame.end()-i,myfame.end());
    myfame.push_back(status_code);
    ptr = (char*)&real_data_len;
    for (i = 0; i < 4; ++i)PUSHZERO(ptr,i,myfame);reverse(myfame.end()-i,myfame.end());
    for (i = 0; i < data.size(); ++i)myfame.push_back(data[i]);/**/
   // crcdata.push_back(data[i]);
   // myfame.push_back((char)data_pad);
    char *crc_code = new char[myfame.size()];
    memset(crc_code, 0, myfame.size());
    for(i=0;i<myfame.size();++i)crc_code[i]=myfame[i];
    //ident_code = CRC16_1(crc_code, myfame.size());
    ident_code = CRC16a(crc_code,myfame.size());
    ptr = (char*)&ident_code;
    for (i = 0; i < 2; ++i)
        PUSHZERO(ptr,i,myfame);
    reverse(myfame.end()-i,myfame.end());
    myfame.push_back(end_fame);
}

Data_fame::operator string()const
{
    int i = 0;
    char *fdata = new char[9];
    string crcdata;//= fdata
    crcdata.push_back(head_fame);
    for (; i < 8; ++i)
        crcdata.push_back(controller_ID[i]);
    _itoa_s(data_len, fdata, 4, 16);
    for (i = 0; i < 4; ++i)crcdata.push_back(fdata[i]);
    _itoa_s(operation_code, fdata, 2, 16);
    for (i = 0; i < 2; ++i)crcdata.push_back(fdata[i]);
    crcdata.push_back(status_code);
    _itoa_s(real_data_len, fdata, 4, 16);
    for (i = 0; i < 4; ++i)crcdata.push_back(fdata[i]);
    //crcdata.push_back();
    /*for (i = 0; i < strlen(data); ++i)
        crcdata.push_back(data[i])*/;
    crcdata.push_back(data[i]);
    _itoa_s(ident_code, fdata, 9, 16);
    for (i = 0; i < 8; ++i)
        crcdata.push_back(fdata[i]);
    crcdata.push_back(end_fame);

    delete[]fdata;
    return crcdata;
}

void Data_fame::Create_Fame(const vector<char> v_data, short op_code)
{
    int length = v_data.size();
    cout << endl;
    //	cout << "???????????????????????0x0008??"<< endl;
    char *fame_data = new char[length];

    for (int i = 0; i < length; ++i){ fame_data[i] = v_data[i]; }
    //	short int op_code;
    //cin >> hex>>op_code ;
    operation_code = op_code;
    /***********************???????????????data??*******************************************/
    if (op_code == REGIST_COM){ Create_Fame_Regist_Command(fame_data, length); }
    else if (op_code == RE_KEY_COM){ Create_Fame_Regist_Command(fame_data, length); }
    else if (op_code == HEARTBEAT_COM){ Create_Fame_Regist_Command(fame_data, length); }
    else if (op_code == RE_CONTROL_COM){ Create_Fame_Regist_Command(fame_data, length); }
    else if (op_code == TIMESYNC_CONTROL){ Create_Fame_Regist_Command(fame_data, length); }
    else if (op_code == SET_CONTROL_LIGHT_MODE){ Create_Fame_Regist_Command(fame_data, length); }
    else if (op_code == SET_CONTROL_LIGHT_PLAN){ Create_Fame_Regist_Command(fame_data, length); }
    else if (op_code == EMPTY_CONTROL_LIGHT_PLAN){}
    else if (op_code == PHONE_NUM_DOWLOAD){}
    else if (op_code == PHONE_NUM_UPDATA){}
    else if (op_code == PHONE_NUM_DELETE){}
    else if (op_code == CONTROL_MSGNUM_QUERY){}
    else if (op_code == CONTROL_MSG_UPDATA){}
    else if (op_code == CONTROL_MSG_DELETE){}
    else if (op_code == CONTROL_ATTR_QUERY){}
    else if (op_code == CONTROL_LIGHT_PLAN_QUERT){}
    else if (op_code == CONTROL_LOG_REPORT){}
    else if (op_code == CONTROL_FIRMWARE_UPGRADE){}
    else if (op_code == CONTROL_FIRMWARE_UPDATE){}
    else if (op_code == CONTROL_EXIT_UPGRDE){}
    else if (op_code == TERMINAL_INF_INITIALIZE){}
    else if (op_code == TERMINAL_INF_DELETE){}
    else if (op_code == TERMINAL_LIGHT_DIM){}
    else if (op_code == TERMIANL_SET_LIGHT_GRADU){}
    else if (op_code == TERMIANL_SET_ELECTRICAL){}
    else if (op_code == TERMIANL_SET_GROUP){}
    else if (op_code == TERMIANL_GROUP_DELETE){}
    else if (op_code == TERMIANL_SCENE_SET){}
    else if (op_code == TERMIANL_SCENE_DELETE){}
    else if (op_code == TERMIANL_AUTO_LIGHT_SET){}
    else if (op_code == TERMIANL_AUTO_LIGHT_DEL){}
    else if (op_code == TERMIANL_MAXLIGHT_SET){}
    else if (op_code == TERMIANL_MINLIGHT_SET){}
    else if (op_code == TERMIANL_DEFAUL_FAIL_SET){}
    else if (op_code == TERMIANL_RESET_DEFAUL){}
    else if (op_code == TERMIANL_INFO_REPORT){}
    else if (op_code == TERMIANL_ALARM_CONITION){}
    else if (op_code == TERMIANL_LIGHT_COEF_SET){}
    else if (op_code == TERMIANL_CRITERIA_QUERY){}
    else if (op_code == TERMIANL_AUTO_LIGHT_QUER){}

    delete[]fame_data;
    fame_data = NULL;
}

void Data_fame::Create_Fame_Regist_Command(char A_Data[], int length)
{
    data_len = 23 + length;
    real_data_len = length;
    data.clear();
//	for (int i = 0; i < length; i++)
//	{
//		data[i] = A_Data[i];
//	}
    data = A_Data;


}
void Data_fame::Create_Fame_RE_key_Command(string A_Data)
{
    data_len = 23 + A_Data.size();
    real_data_len = A_Data.size();
    data.clear();
    data = A_Data;
}

void Data_fame::Create_Fame_Data_NULL()//????????
{
    data_len = 7; real_data_len = 0; data_pad = 0;
    status_code = 0xff;
    head_fame = 0xAA;
    end_fame = 0x55;
    data.clear();
}


string Data_fame::Create_Fame_TERMINAL_INF_DELETE(
    vector<Terminal> term_address)
{
    Create_Fame_Data_NULL(); data_len += 8 * term_address.size();
    operation_code = 0x1001; real_data_len = 8 * term_address.size();
    for (int i = 0; i < term_address.size(); ++i)
        for (int j = 0; i < 8; ++j)
            data.push_back(term_address[i].term[j]);

    vector<char> serializedata;
    myserialize(serializedata);
    string writedata;
    for (int i = 0; i<serializedata.size(); ++i)
        writedata.push_back((uint8_t)serializedata[i]);
    return writedata;
}
string Data_fame::Create_Fame_SET_CONTROL_LIGHT_MODE(uint8_t mode)//??????
{
    Create_Fame_Data_NULL(); operation_code = 0x000d;
    data.push_back(mode); data_len += data.size();
    real_data_len = data.size();
    vector<char> serializedata;
    myserialize(serializedata);
    string writedata;
    for (int i = 0; i<serializedata.size(); ++i)
        writedata.push_back((uint8_t)serializedata[i]);
    return writedata;
}
string Data_fame::Create_Fame_Re_Control_COM()
{
    Create_Fame_Data_NULL();
    operation_code = 0x000b;
    vector<char> serializedata;
    myserialize(serializedata);
    string writedata;
    for (int i = 0; i<serializedata.size(); ++i)
        writedata.push_back((uint8_t)serializedata[i]);
    return writedata;
}
string Data_fame::Create_Fame_Hearthbeat_COM()
{
    Create_Fame_Data_NULL();
    operation_code = 0x000a;
    vector<char> serializedata;
    myserialize(serializedata);
    string writedata;
    for (int i = 0; i<serializedata.size(); ++i)
        writedata.push_back((uint8_t)serializedata[i]);
    return writedata;
}
string Data_fame::Create_Fame_TERMINAL_INF_INITIALIZE(
    vector<Terminal> term_address)
{
    Create_Fame_Data_NULL(); data_len += 8*term_address.size();
    operation_code = 0x1000; real_data_len = 8*term_address.size();
    for (int i = 0; i < term_address.size();++i)
        for (int j = 0; i < 8; ++j)
            data.push_back(term_address[i].term[j]);

    vector<char> serializedata;
    myserialize(serializedata);
    string writedata;
    for (int i = 0; i<serializedata.size(); ++i)
        writedata.push_back((uint8_t)serializedata[i]);
    return writedata;
}
string Data_fame::Create_Fame_TERMINAL_LIGHT_DIM(
    vector<Terminal_address> Light_mid)//????????????
{
    Create_Fame_Data_NULL(); data_len += 8 * Light_mid.size();
    operation_code = 0x1002; real_data_len = 8 * Light_mid.size();
    for (int i = 0; i < Light_mid.size(); ++i)
    {
        for (int j = 0; i < 9; ++j)
        {
            data.push_back(Light_mid[i].term[j]);
        }
        data.push_back(Light_mid[i].coe_or_confirm);
    }
    vector<char> serializedata;
    myserialize(serializedata);
    string writedata;
    for (int i = 0; i<serializedata.size(); ++i)
        writedata.push_back((uint8_t)serializedata[i]);
    return writedata;
}

string Data_fame::Create_Fame_Reset_coord(string famedata,uint32_t length)
{
    data_len = 7 + famedata.size();
    real_data_len = length;
    operation_code = 0xFE;status_code = 0xff;

    data.clear();
    data.append(famedata);
    data_pad =10;
    head_fame = 0xAA;
    end_fame = 0x55;
    vector<char> serializedata;
    myserialize(serializedata);
    string writedata;
    for(int i=0;i<serializedata.size();++i)
        writedata.push_back((uint8_t)serializedata[i]);
    return writedata;

}
//aa3132333435363738 0000000e 0600 00 07000000    02 06 55aa c840 03 6b 0000 55
//aa0000000016000000 0000000d 00fe ff 00000006 01 02 3940ff800300d40255
//aa00ababababababab 00000009 00fe ff 00000002 00 00  001455
//aa3132333435363738 0000000d 00fe ff 00000005 01 01 02 3a b12403ffc055
//aa3132333435363738 0000000d 00fe ff 00000005 01 02 3a b008 03 0039 55
//aacccccccccccccccc 00000010 000c ff 00000009 32303136081e0e0605 ff80 55
string Data_fame::Create_Fame_Send_NetWork(string famedata,uint32_t length)
{
    data_len = 7 + famedata.size();
    real_data_len = length;
    operation_code = 0xfe;status_code = 0xFF;
    data_pad = 0;
    data.clear();
    data.append(famedata);
    head_fame = 0xAA;
    end_fame = 0x55;
    vector<char> serializedata;
    myserialize(serializedata);
    string writedata;
    for(int i=0;i<serializedata.size();++i)
        writedata.push_back((uint8_t)serializedata[i]);
    return writedata;

}

string Data_fame::Create_Fame_Search_Network(string famedata,uint32_t length)
{
    data_len = 7 + famedata.size();
    real_data_len =famedata.size();
    operation_code = 0xfe;status_code = 0xFF;
    head_fame=0xaa;end_fame=0x55;
    data_pad =0;
    //data to ident_code
    data.clear();
    data.append(famedata);

    vector<char> serializedata;
    myserialize(serializedata);
    string writedata;
    for(int i=0;i<serializedata.size();++i)
        writedata.push_back((uint8_t)serializedata[i]);
    return writedata;
}
string Data_fame::Create_Fame_CONTROL_MSG_SEND(int state)
{
    data_len = 8;
    real_data_len = 1;
    operation_code = 0xfe;status_code = 0xFF;
    head_fame=0xaa;end_fame=0x55;
    //data to ident_code
    data.clear();
    if(state == 1)
        data.push_back(0x00);
    else if(state == 0)
        data.push_back(0x02);
    //data.push_back(0x00);
    //    Coord_debug rsetcd;string trait;
    //    rsetcd.type = 0x3A;
    //    string rscd = rsetcd.create(trait);
    vector<char> serializedata;
    myserialize(serializedata);
    string writedata;
    for(int i=0;i<serializedata.size();++i)
        writedata.push_back((uint8_t)serializedata[i]);
    return writedata;
}
//
//void Create_Fame_TIMESYNC_CONTROL(char A_Data[]);
//void Create_Fame_SET_CONTROL_LIGHT_MODE(char A_Data[]);
//void Create_Fame_SET_CONTROL_LIGHT_PLAN(char A_Data[]);
//void Create_Fame_EMPTY_CONTROL_LIGHT_PLAN(char A_Data[]);
//void Create_Fame_PHONE_NUM_DOWLOAD(char A_Data[]);
//void Create_Fame_PHONE_NUM_UPDATA(char A_Data[]);
//void Create_Fame_PHONE_NUM_DELETE(char A_Data[]);
//void Create_Fame_CONTROL_MSGNUM_QUERY(char A_Data[]);
//void Create_Fame_CONTROL_MSG_UPDATA(char A_Data[]);
//void Create_Fame_CONTROL_MSG_DELETE(char A_Data[]);
//void Create_Fame_CONTROL_ATTR_QUERY(char A_Data[]);
//void Create_Fame_CONTROL_LIGHT_PLAN_QUERT(char A_Data[]);
//void Create_Fame_CONTROL_LOG_REPORT(char A_Data[]);
//void Create_Fame_CONTROL_FIRMWARE_UPGRADE(char A_Data[]);
//void Create_Fame_CONTROL_FIRMWARE_UPDATE(char A_Data[]);
//void Create_Fame_CONTROL_EXIT_UPGRDE(const char A_Data[]);
//void Create_Fame_TERMINAL_INF_INITIALIZE(const char A_Data[]);
//void Create_Fame_TERMINAL_INF_DELETE(const char A_Data[]);
//void Create_Fame_TERMINAL_LIGHT_DIM(const char A_Data[]);
//void Create_Fame_TERMIANL_SET_LIGHT_GRADU(const char A_Data[]);
//void Create_Fame_TERMIANL_SET_ELECTRICAL(const char A_Data[]);
//void Create_Fame_TERMIANL_SET_GROUP(const char A_Data[]);
//void Create_Fame_TERMIANL_GROUP_DELETE(const char A_Data[]);
//void Create_Fame_TERMIANL_SCENE_SET(const char A_Data[]);
//void Create_Fame_TERMIANL_SCENE_DELETE(const char A_Data[]);
//void Create_Fame_TERMIANL_AUTO_LIGHT_SET(const char A_Data[]);
//void Create_Fame_TERMIANL_AUTO_LIGHT_DEL(const char A_Data[]);
//void Create_Fame_TERMIANL_MAXLIGHT_SET(const char A_Data[]);
//void Create_Fame_TERMIANL_MINLIGHT_SET(const char A_Data[]);
//void Create_Fame_TERMIANL_DEFAUL_FAIL_SET(const char A_Data[]);
//void Create_Fame_TERMIANL_RESET_DEFAUL(const char A_Data[]);
//void Create_Fame_TERMIANL_INFO_REPORT(const char A_Data[]);
//void Create_Fame_TERMIANL_ALARM_CONITION(const char A_Data[]);
//void Create_Fame_TERMIANL_LIGHT_COEF_SET(const char A_Data[]);
//void Create_Fame_TERMIANL_CRITERIA_QUERY(const char A_Data[]);
//void Create_Fame_TERMIANL_AUTO_LIGHT_QUER(const char A_Data[]);

/******************************************????????********************************************/
/**********************************????????????????????????***********************************/

int hexcharToInt(char c)
{
       if (c >= '0' && c <= '9') return (c - '0');
       if (c >= 'A' && c <= 'F') return (c - 'A' + 10);
       if (c >= 'a' && c <= 'f') return (c - 'a' + 10);
  return 0;
}

void Hex_to_char(string hexstring,vector<uint8_t> &bytes)
{
  //cout<<"length is :"<<sizeof(hexstring)/sizeof(char)<<endl;
    uint8_t tmp;
       for (int i=0 ; i <hexstring.size() ; i+=2) {
           tmp = (uint8_t) ((hexcharToInt(hexstring[i]) << 4)
                               | hexcharToInt(hexstring[i+1]));
           bytes.push_back(tmp);
       }
}

int Data_fame::Analy_Fame(string myframe, vector<uint8_t> &v_data)
{
    int i = 0;
//    Hex_to_char(myframe,v_data);
//    string createdata ;
//    for(i=0;i<v_data.size();++i)createdata.push_back(v_data[i]);
    Data_fame frame(myframe);//createdata
   // Data_fameSet(createdata);
    if (  frame.head_fame == 0xAA)//frame.end_fame == 0x55 &&
    {
        char *crcdata = new char[myframe.size()-3];
        memset(crcdata, 0, myframe.size()-3);
        for(i=0;i<myframe.size()-3;++i)crcdata[i]=myframe[i];
        ident_code = CRC16a(crcdata, myframe.size()-3);
        v_data.clear();
        for(int i=0;i<frame.data.size()-1;++i)
            v_data.push_back(frame.data[i]);
        if (frame.ident_code == ident_code)
        {
            return 0;//complete
        }//crc16§Ή??
    }
    else
    {
        return 1;
    }

    if (frame.operation_code == REGIST_COM){ /*Analy_Fame_Regist_Command(A_Fame, A_type, v_data, aeskey, keyiv);*/ }
    else if (frame.operation_code == RE_KEY_COM){ /*Analy_Fame_RE_key_Command(A_Fame, v_data, aeskey, keyiv);*/ }
    else if (frame.operation_code == HEARTBEAT_COM){ return 0; }
    else if (frame.operation_code == RE_CONTROL_COM){ return 0; }
    else if (frame.operation_code == TIMESYNC_CONTROL){}
    else if (frame.operation_code == SET_CONTROL_LIGHT_MODE){}
    else if (frame.operation_code == SET_CONTROL_LIGHT_PLAN){}
    else if (frame.operation_code == EMPTY_CONTROL_LIGHT_PLAN){}
    else if (frame.operation_code == PHONE_NUM_DOWLOAD){}
    else if (frame.operation_code == PHONE_NUM_UPDATA){}
    else if (frame.operation_code == PHONE_NUM_DELETE){}
    else if (frame.operation_code == CONTROL_MSGNUM_QUERY){}
    else if (frame.operation_code == CONTROL_MSG_UPDATA){}
    else if (frame.operation_code == CONTROL_MSG_DELETE){}
    else if (frame.operation_code == CONTROL_ATTR_QUERY){}
    else if (frame.operation_code == CONTROL_LIGHT_PLAN_QUERT){}
    else if (frame.operation_code == CONTROL_LOG_REPORT){}
    else if (frame.operation_code == CONTROL_FIRMWARE_UPGRADE){}
    else if (frame.operation_code == CONTROL_FIRMWARE_UPDATE){}
    else if (frame.operation_code == CONTROL_EXIT_UPGRDE){}
    else if (frame.operation_code == TERMINAL_INF_INITIALIZE){}
    else if (frame.operation_code == TERMINAL_INF_DELETE){}
    else if (frame.operation_code == TERMINAL_LIGHT_DIM){}
    else if (frame.operation_code == TERMIANL_SET_LIGHT_GRADU){}
    else if (frame.operation_code == TERMIANL_SET_ELECTRICAL){}
    else if (frame.operation_code == TERMIANL_SET_GROUP){}
    else if (frame.operation_code == TERMIANL_GROUP_DELETE){}
    else if (frame.operation_code == TERMIANL_SCENE_SET){}
    else if (frame.operation_code == TERMIANL_SCENE_DELETE){}
    else if (frame.operation_code == TERMIANL_AUTO_LIGHT_SET){}
    else if (frame.operation_code == TERMIANL_AUTO_LIGHT_DEL){}
    else if (frame.operation_code == TERMIANL_MAXLIGHT_SET){}
    else if (frame.operation_code == TERMIANL_MINLIGHT_SET){}
    else if (frame.operation_code == TERMIANL_DEFAUL_FAIL_SET){}
    else if (frame.operation_code == TERMIANL_RESET_DEFAUL){}
    else if (frame.operation_code == TERMIANL_INFO_REPORT){}
    else if (frame.operation_code == TERMIANL_ALARM_CONITION){}
    else if (frame.operation_code == TERMIANL_LIGHT_COEF_SET){}
    else if (frame.operation_code == TERMIANL_CRITERIA_QUERY){}
    else if (frame.operation_code == TERMIANL_AUTO_LIGHT_QUER){}
    else if (frame.operation_code == Reset_coord){Coord_debug coordata;
        coordata.Analy(v_data,coordata);}

    return 0;//
}
void Data_fame::Analy_Fame_Regist_Command(const Data_fame & A_Fame, unsigned char A_type, vector<char> &v_data, string aeskey, string keyiv)
{
    //int maxsize = A_Fame.data_len - 24;
    if (A_Fame.status_code == 0xFF)
    {
        char *aesdat = new char[data_len - 23];
        strcpy(aesdat,A_Fame.data.c_str());
        string dat = MyAes.InvCipher(aesdat);
        //string dat = CBC_AESDecryptStr1(aeskey, keyiv, A_Fame.data.c_str());
        //printf("copy %s\n",A_Data);
        for (int i = 0; i < dat.size(); ++i){ v_data.push_back(dat[i]); }
        printf("%s\n", dat.c_str());

    }
    else if (A_Fame.status_code == 0x00)
    {
        printf("??????");
    }
    return;
}
void Data_fame::Analy_Fame_RE_key_Command(const Data_fame & A_Fame, vector<char> &v_data, string aeskey, string keyiv)
{
    if (A_Fame.status_code == 0xFF)
    {
        for (unsigned int i = 0; i < v_data.size(); ++i)
        {
            v_data.push_back(A_Fame.data[i]);
        }

    }
    else if (A_Fame.status_code == 0x00)
    {
        printf("??????");
    }
    return;
}


//void Data_fame::Analy_Fame_TIMESYNC_CONTROL(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size() ; ++i){ v_data.push_back(A_Fame.data[i]); }
//		printf("%d%d", A_Fame.data[0], A_Fame.data[1]);
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		printf("??????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_SET_CONTROL_LIGHT_MODE(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i){ v_data.push_back(A_Fame.data[i]); }
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		printf("????????");
//	}
//	else if (A_Fame.status_code == 0x03)
//	{
//		printf("???????????????????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_SET_CONTROL_LIGHT_PLAN(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i){ v_data.push_back(A_Fame.data[i]); }
//		printf("???????	????????	§³?		??		??		???????	??????	?????"); //?????????
//		printf("2Byte	1Byte	1Byte	1Byte	1Byte	8Byte	1Byte	1Byte");
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		printf("???¨®??");
//	}
//	return;
//}//a
//void Data_fame::Analy_Fame_EMPTY_CONTROL_LIGHT_PLAN(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{}
//	else if (A_Fame.status_code == 0x00)
//	{
//		printf("??????");
//	}
//	return;
//}//a
//void Data_fame::Analy_Fame_PHONE_NUM_DOWLOAD(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i){ v_data.push_back(A_Fame.data[i]); }
//		/*??????	????	????????	??????unicode??	???
//			1Byte	NByte	1Byte	NByte	1Byte*/
//		printf("??????	????	????????	??????unicode??	???");
//		printf("1Byte	12Byte	1Byte	16Byte	1Byte");//32
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		printf("??????");
//	}
//	return;
//}//a
//void Data_fame::Analy_Fame_PHONE_NUM_UPDATA(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		printf("??????");
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i){ v_data.push_back(A_Fame.data[i]); }
//		printf("??????????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_PHONE_NUM_DELETE(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		printf("???????????");//??????
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		printf("??????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_CONTROL_MSGNUM_QUERY(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		printf("???????????????");
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i){ v_data.push_back(A_Fame.data[i]); }
//		printf("????????");//
//	}
//	return;
//}
//void Data_fame::Analy_Fame_CONTROL_MSG_UPDATA(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i){ v_data.push_back(A_Fame.data[i]); }
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i){ v_data.push_back(A_Fame.data[i]); }
//		printf("??????");//??????????? ????
//	}
//	return;
//}
//void Data_fame::Analy_Fame_CONTROL_MSG_DELETE(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i){ v_data.push_back(A_Fame.data[i]); }
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		printf("??????");//
//	}
//	return;
//}
//void Data_fame::Analy_Fame_CONTROL_ATTR_QUERY(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)								//???????
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{ v_data.push_back(A_Fame.data[i]); }		/*????	????????	????????	????????ID N
//														2*N		2Byte		2Byte		2Byte*/
//	}															//?????????
//	else if (A_Fame.status_code == 0x00)						/*p18*/
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		};
//		printf("??????");						//???????????????????p18
//	}
//	return;
//}
//void Data_fame::Analy_Fame_CONTROL_LIGHT_PLAN_QUERT(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("??????");//???????§Ϋ?????
//	}
//	return;
//}
//void Data_fame::Analy_Fame_CONTROL_LOG_REPORT(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("???????+???????");//???????§Ϋ?????
//	}
//	return;
//}
//void Data_fame::Analy_Fame_CONTROL_FIRMWARE_UPGRADE(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		printf("???????");
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		printf("???????");//???????§Ϋ?????
//	}
//	return;
//}
//void Data_fame::Analy_Fame_CONTROL_FIRMWARE_UPDATE(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("????????");//?????????????????
//	}
//	return;
//}
//void Data_fame::Analy_Fame_CONTROL_EXIT_UPGRDE(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		printf("???????");//
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMINAL_INF_INITIALIZE(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		/*????υτ????	????υτ?????	????υτ????		???????		????υτ???????
//			??Byte			1Byte			4Byte			1Byte		32Byte*/
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("????????");//?????????????????
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMINAL_INF_DELETE(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		printf("?υτ???????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMINAL_LIGHT_DIM(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		/*????υτ???	?????
//			 8Byte	    1Byte*/
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("?????????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMIANL_SET_LIGHT_GRADU(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		/*????υτ???	?????????
//			8Byte	    1Byte*/
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("?????????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMIANL_SET_ELECTRICAL(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		/*????υτ???	?????
//		8Byte	    1Byte*/
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("???????????????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMIANL_SET_GROUP(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		/*????υτ???	?????
//		8Byte	    1Byte*/
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("???????????????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMIANL_GROUP_DELETE(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		/*????υτ???	?????
//		8Byte	    1Byte*/
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("??????????????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMIANL_SCENE_SET(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		/*????υτ???	?????
//		8Byte	    1Byte*/
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("?????????????????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMIANL_SCENE_DELETE(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("???????????????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMIANL_AUTO_LIGHT_SET(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		/*????υτ???	???????????	§³?		??		?????	????????
//			8Byte			N		1Byte	1Byte	1Byte	??????..*/
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("????????????????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMIANL_AUTO_LIGHT_DEL(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		/*????υτ???
//			8Byte*/
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("???????????????????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMIANL_MAXLIGHT_SET(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		/*????υτ???
//		8Byte*/
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("??????????????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMIANL_MINLIGHT_SET(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		/*????υτ???
//		8Byte*/
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		};
//		printf("??§³??????????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMIANL_DEFAUL_FAIL_SET(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		/*????υτ???
//		8Byte*/
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("?????????????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMIANL_RESET_DEFAUL(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		/*????υτ???
//		8Byte*/
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("????????????????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMIANL_INFO_REPORT(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		/*????υτ???
//		8Byte*/
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("???????????????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMIANL_ALARM_CONITION(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("????????????????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMIANL_LIGHT_COEF_SET(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("???????????????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMIANL_CRITERIA_QUERY(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("???????????????????????");
//	}
//	return;
//}
//void Data_fame::Analy_Fame_TERMIANL_AUTO_LIGHT_QUER(const Data_fame & A_Fame,  vector<char> &v_data)
//{
//	if (A_Fame.status_code == 0xFF)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		};
//	}
//	else if (A_Fame.status_code == 0x00)
//	{
//		for (unsigned int i = 0; i < v_data.size(); ++i)
//		{
//			v_data.push_back(A_Fame.data[i]);
//		}
//		printf("????????????????");
//	}
//	return;
//}




