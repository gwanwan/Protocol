#ifndef _PROTOCOL_H_INCLUDED
#define _PROTOCOL_H_INCLUDED
#include<string.h>
#include<vector>
#include <stdlib.h>
#include <sstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include<aes.h>
#endif
using namespace std;


#define GET_FAME_REAL_LEN(FAME, fame_real_length) fame_real_length = strlen(FAME.data)+24//?????????????????
#define PUSHZERO(ptr,i,str)  if(*(ptr+i)=='\0'){str.push_back('\0');}else{str.push_back(*(ptr+i));}
//fame_real_length = 23 + strlen(fame.data) + strlen(fame.data_pad);
#define DATA_LENGTH(in,length)  while (*(in + length++))//?????????????
#if!define
#define Reset_coord              0x0006 //???????
#define REGIST_COM				 0x0008	//???????
#define RE_KEY_COM				 0x0009	//??????????
#define HEARTBEAT_COM			 0x000A	//??????????
#define RE_CONTROL_COM			 0x000B	//??????????????
#define TIMESYNC_CONTROL		 0x000C	//????????????????
#define SET_CONTROL_LIGHT_MODE	 0x000D	//??????????????????????
#define SET_CONTROL_LIGHT_PLAN	 0x000E	//???????????????????????
#define EMPTY_CONTROL_LIGHT_PLAN 0x000F //??????????????????????
#define PHONE_NUM_DOWLOAD		 0x0010	//???????????
#define PHONE_NUM_UPDATA		 0x0011	//??????????
#define PHONE_NUM_DELETE		 0x0012	//??????????
#define CONTROL_MSGNUM_QUERY	 0x0013	//?????????????????????
#define CONTROL_MSG_UPDATA		 0x0014	//?????????????????????
#define CONTROL_MSG_DELETE		 0x0015	//?????????????????????
#define CONTROL_ATTR_QUERY		 0x0080	//????????????????
#define CONTROL_LIGHT_PLAN_QUERT 0x0081 //??????????????????
#define CONTROL_LOG_REPORT		 0x0082	//????????????????????
#define CONTROL_FIRMWARE_UPGRADE 0x0100 //???????????????????????
#define CONTROL_FIRMWARE_UPDATE  0x0101	//?????????????????????
#define CONTROL_EXIT_UPGRDE      0x0102	//???????????????????
#define TERMINAL_INF_INITIALIZE  0x1000	//??????????????
#define TERMINAL_INF_DELETE		 0x1001	//????????????
#define TERMINAL_LIGHT_DIM		 0x1002	//??????????
#define TERMIANL_SET_LIGHT_GRADU 0x1003	//???????????????????
#define TERMIANL_SET_ELECTRICAL  0x1004	//??????????????????
#define TERMIANL_SET_GROUP		 0x1005	//????????????
#define TERMIANL_GROUP_DELETE	 0x1006	//???????????
#define TERMIANL_SCENE_SET		 0x1007	//??????????????
#define TERMIANL_SCENE_DELETE	 0x1008	//?????????????
#define TERMIANL_AUTO_LIGHT_SET	 0x1009	//????????????????????
#define TERMIANL_AUTO_LIGHT_DEL  0x100A	//???????????????????
#define TERMIANL_MAXLIGHT_SET	 0x100B	//?????????????????????
#define TERMIANL_MINLIGHT_SET	 0x100C	//???????????????????
#define TERMIANL_DEFAUL_FAIL_SET 0x100D	//?????????????????
#define TERMIANL_RESET_DEFAUL	 0x100E	//?????????????????
#define	TERMIANL_INFO_REPORT	 0x100F	//????????????????????
#define TERMIANL_ALARM_CONITION	 0x1010	//???????????????????
#define TERMIANL_LIGHT_COEF_SET	 0x1011	//????????????????????????
#define TERMIANL_CRITERIA_QUERY	 0x1080	//?????????????
#define TERMIANL_AUTO_LIGHT_QUER 0x1081	//???????????????????

#define TERMIANL_GROUP_QUER		 0x1082	//????????????
#define TERMIANL_SCENE_QUER		 0x1083	//?????????????
#define TERMIANL_ALARM_REPORT	 0x1100	//??????????????????
#define	CONTROL_IN_UPGRADE_MODE	 0x1200	//?????????????????????????????
#define DOWN_DEVICE_TO_CONTROL	 0x1201	//????????????????????????????
#define	DOWN_UP_PACK_TO_CONTROL  0x1202	//???????????????????????????????
#define STRAT_UPPACK_TO_CONTROL	 0x1203	//???????????????????????
#define CONTROL_EXIT_UP_MODE	 0x1204	//????????????????????????????
#define DISTRI_BOX_STATUR_QUERY	 0x8000	//?????????????????
#define	SET_DISBOX_ALM_CONITION	 0x8100	//????????????????????
#define CONTROL_DISBOX_ALM_MSG	 0x8200	//????????????????????????
#define CONTROL_MSG_SEND		 0x00FE	//???????????????????????
#define CONTROL_TERM_ADDRE_QUERY 0x00FD	//?????????????????????????
#define SET_PECE_CONTROL_CUR_OP  0x00FC	//?????????????????????????
#define SET_CONTROL_INFRARED_COM 0x00FB	//?????????????????????????
#define SET_SENCE_DATA_MODE		 0x00FA	//??????????????
#define RE_BACK_SENCE_DATA		 0x00F9	//?????????????
#define ACTIVATE_SENCE_DATA		 0x00F8	//????????????
#define	ADD_SMART_ELE_METER		 0xF000	//???????????
#define ELE_METER_DATA_QUERY	 0xF001	//??????????????
#define ELE_METER_DATA_DELETE	 0xF002	//??????????????????
#define SET_TERMINAL_TYPE		 0x1050	//??????????????
#define TERMINAL_LIGHT_UP		 0x1051	//???????????
#define LOOP_DEVICE_SWITCH		 0x1052	//???????????
#define SCONTOL_FLEXO_LIGHT_PLAN 0x1053//???????????????????????????
#define TERMINAL_FLEXO_LIGHT_HM  0x1054	//?????????????????

#endif // !0



short CRC16_1(char* pchMsg, short wDataLen);
short CRC16_1(string str, short wDataLen);
struct Terminal_address		//8byte +1byte
{
    char term[8];			//??? ??? ???
    char coe_or_confirm;	//??????? ?? ????

    friend class boost::serialization::access;
    //use this function to serialize and deserialize object
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & term & coe_or_confirm;
    }
public:
    static std::string Serialize(const Terminal_address& msg)
    {
        std::ostringstream archiveStream;
        boost::archive::text_oarchive archive(archiveStream);
        archive << msg;
        return archiveStream.str();
    }
    static void DeSerialize(const std::string& message, Terminal_address &msg)
    {
        std::istringstream archiveStream(message);
        boost::archive::text_iarchive archive(archiveStream);
        archive >> msg;
        return;
    }
};

class Coord_debug
{
    uint8_t  head ;
    uint8_t  type;
    string	 data;
    uint16_t crc_code;
    uint8_t  end ;

    friend class boost::serialization::access; // ???????????????
    //use this function to serialize and deserialize object
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & head & type & data & crc_code & end;
    }

    string create(string mydata);


//aa31323334353637381e00000006000007000000 3200aa55c0ff33 8615c055
public:
    void Analy(vector<uint8_t> str, Coord_debug &msg);

    string transform(string f_data);

    string Detransform(string mydata);

public:
    static std::string Serialize(const Coord_debug& msg)
    {
        std::ostringstream archiveStream;
        boost::archive::text_oarchive archive(archiveStream);
        archive << msg;
        return archiveStream.str();
    }
    static void DeSerialize(const std::string& message, Coord_debug &msg)
    {
        std::istringstream archiveStream(message);
        boost::archive::text_iarchive archive(archiveStream);
        archive >> msg;
        return;
    }
};


//struct FameMsg
//{
//	char	head;
//	char	control_ID[8];
//	int		fame_length;
//	short	operation_code;
//	unsigned char	status_code;
//	int		real_data_len;	//??????????
//	string	data;			//????
//	char	data_pad;		//????????????
//	short	ident_code;
//	char	fame_end;
//
//	friend class boost::serialization::access;
//
//	//use this function to serialize and deserialize object
//	template<class Archive>
//	void serialize(Archive & ar, const unsigned int version)
//	{
//		ar & head & control_ID & fame_length & operation_code
//			& status_code & real_data_len & data & ident_code
//			& data_pad & fame_end;
//	}
//
//};
//
//class Serialization
//{
//public:
//	static std::string Serialize(const FameMsg& msg)
//	{
//		std::ostringstream archiveStream;
//		boost::archive::text_oarchive archive(archiveStream);
//		archive << msg;
//		return archiveStream.str();
//	}
//	static FameMsg DeSerialize(const std::string& message)
//	{
//		FameMsg msg;
//		std::istringstream archiveStream(message);
//		boost::archive::text_iarchive archive(archiveStream);
//		archive >> msg;
//		return msg;
//	}
//};
void Hex_to_char(string myframe, vector<uint8_t> &v_data);
class protocols_head
{
public:
    uint8_t head_fame;//????????
    char controller_ID[9];//?????????????
public:
    protocols_head(char control_id[]) :head_fame(0xAA){ strcpy_s(controller_ID, control_id); }
    protocols_head(){}
};
class protocols_end
{
public:
    uint16_t ident_code;//??????
    uint8_t  end_fame;//???
public:
    protocols_end(uint16_t code) :ident_code(code), end_fame(0x55){}
    protocols_end(){}
};
class Data_fame :public protocols_head, public protocols_end
{
private:
    uint32_t		data_len;		//????????
    uint16_t		operation_code;	//?????? ???????????????
    uint8_t     	status_code;	//?????
    uint32_t		real_data_len;	//??????????
    string			data;			//????
    uint8_t         data_pad;		//????????????
    uint32_t		fame_real_length;
    AES             MyAes;


    //char *data_crc;
public:

    Data_fame(char *control_id,
        uint16_t code, uint16_t op_code, uint8_t st_code,
        const char *dat, uint8_t pad_d, string aeskey, string keyiv);//?????????

    Data_fame(const string F_data);
    void Data_fameSet(const string F_data);
    Data_fame()
    {
//		char *data = new char[35];
//		memset(data, 0, 35);
        //char *data_crc = new char[35]; memset(data_crc, 0, 35);;
    }

    Data_fame(const Data_fame  &fame){
        /*printf("%s\n", strlen((fame.data)));
        GET_FAME_REAL_LEN(fame, fame_real_length);*/
        //char *data_crc = new char[35]; memset(data_crc, 0, 35);
        if (fame.data_len == fame_real_length && fame.end_fame != NULL &&
            fame.head_fame != NULL)
        {
            data_len = fame.data_len;
            operation_code = fame.operation_code; status_code = fame.status_code;
            real_data_len = fame.real_data_len;
            //strcpy_s(data, real_data_len, fame.data);
            data = fame.data;
            //strcpy_s(data_pad, fame.data_pad);
        }
        else
        {
            printf("?????????");
        }
    }

    //????????
    void myserialize(vector<char> &myfame);

    friend class boost::serialization::access; // ????????
    //use this function to serialize and deserialize object
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & head_fame & controller_ID & data_len & operation_code
            & status_code & real_data_len & data & data_pad
            & ident_code & end_fame;
    }

public:
    static std::string Serialize(const Data_fame& msg)
    {
        std::ostringstream archiveStream;
        boost::archive::text_oarchive archive(archiveStream);
        archive << msg;
        return archiveStream.str();
    }
    static void DeSerialize(const std::string& message, Data_fame &msg)
    {
        std::istringstream archiveStream(message);
        boost::archive::text_iarchive archive(archiveStream);
        archive >> msg;
        return;
    }


    friend class boost::serialization::access; // ??????? ??????????
    //use this function to serialize and deserialize object
    template<class Archive>
    void identserialize(Archive & ar, const unsigned int version)
    {
        ar & head_fame & controller_ID & data_len & operation_code
            & status_code & real_data_len & data & data_pad;

    }

public:
    static std::string identSerialize(const Data_fame& msg)
    {
        std::ostringstream archiveStream;
        boost::archive::text_oarchive archive(archiveStream);
        archive << msg;
        return archiveStream.str();
    }
    static void identDeSerialize(const std::string& message, Data_fame &msg)
    {
        std::istringstream archiveStream(message);
        boost::archive::text_iarchive archive(archiveStream);
        archive >> msg;
        return;
    }

    ~Data_fame(){}// }//delete[]data_crc;printf("???\n");
    operator string()const;
    string outstring(){ return data;}
    void setcontrolID(char *id)
    {for(int i=0;i<8;++i)
        controller_ID[i]=id[i];
    }
    void Fame_cout();//int length


    uint16_t insertHexnum(string num);
    void Create_Fame(const vector<char> v_data, const Data_fame &C_fame);//??????????
    void Create_Fame(const vector<char> v_data, short type);
    void Create_Fame_Regist_Command(char A_Data[], int length);
    string Create_Fame_Reset_coord(string famedata,uint32_t length);      //new
    string Create_Fame_Send_NetWork(string famedata,uint32_t length);
    string Create_Fame_Search_Network(string famedata,uint32_t length);
    void Create_Fame_RE_key_Command(string A_Data);
    void Create_Fame_TIMESYNC_CONTROL(char A_Data[]);
    void Create_Fame_SET_CONTROL_LIGHT_MODE(char A_Data[]);
    void Create_Fame_SET_CONTROL_LIGHT_PLAN(char A_Data[]);
    void Create_Fame_EMPTY_CONTROL_LIGHT_PLAN(char A_Data[]);
    void Create_Fame_PHONE_NUM_DOWLOAD(char A_Data[]);
    void Create_Fame_PHONE_NUM_UPDATA(char A_Data[]);
    void Create_Fame_PHONE_NUM_DELETE(char A_Data[]);
    void Create_Fame_CONTROL_MSGNUM_QUERY(char A_Data[]);
    string Create_Fame_CONTROL_MSG_SEND(int state);
    void Create_Fame_CONTROL_MSG_UPDATA(char A_Data[]);
    void Create_Fame_CONTROL_MSG_DELETE(char A_Data[]);
    void Create_Fame_CONTROL_ATTR_QUERY(char A_Data[]);
    void Create_Fame_CONTROL_LIGHT_PLAN_QUERT(char A_Data[]);
    void Create_Fame_CONTROL_LOG_REPORT(char A_Data[]);
    void Create_Fame_CONTROL_FIRMWARE_UPGRADE(char A_Data[]);
    void Create_Fame_CONTROL_FIRMWARE_UPDATE(char A_Data[]);
    void Create_Fame_CONTROL_EXIT_UPGRDE(const char A_Data[]);
    void Create_Fame_TERMINAL_INF_INITIALIZE(const char A_Data[]);
    void Create_Fame_TERMINAL_INF_DELETE(const char A_Data[]);
    void Create_Fame_TERMINAL_LIGHT_DIM(const char A_Data[]);
    void Create_Fame_TERMIANL_SET_LIGHT_GRADU(const char A_Data[]);
    void Create_Fame_TERMIANL_SET_ELECTRICAL(const char A_Data[]);
    void Create_Fame_TERMIANL_SET_GROUP(const char A_Data[]);
    void Create_Fame_TERMIANL_GROUP_DELETE(const char A_Data[]);
    void Create_Fame_TERMIANL_SCENE_SET(const char A_Data[]);
    void Create_Fame_TERMIANL_SCENE_DELETE(const char A_Data[]);
    void Create_Fame_TERMIANL_AUTO_LIGHT_SET(const char A_Data[]);
    void Create_Fame_TERMIANL_AUTO_LIGHT_DEL(const char A_Data[]);
    void Create_Fame_TERMIANL_MAXLIGHT_SET(const char A_Data[]);
    void Create_Fame_TERMIANL_MINLIGHT_SET(const char A_Data[]);
    void Create_Fame_TERMIANL_DEFAUL_FAIL_SET(const char A_Data[]);
    void Create_Fame_TERMIANL_RESET_DEFAUL(const char A_Data[]);
    void Create_Fame_TERMIANL_INFO_REPORT(const char A_Data[]);
    void Create_Fame_TERMIANL_ALARM_CONITION(const char A_Data[]);
    void Create_Fame_TERMIANL_LIGHT_COEF_SET(const char A_Data[]);
    void Create_Fame_TERMIANL_CRITERIA_QUERY(const char A_Data[]);
    void Create_Fame_TERMIANL_AUTO_LIGHT_QUER(const char A_Data[]);

    int Analy_Fame(string myframe, vector<uint8_t> &v_data);//?????????
    void Analy_Fame_Regist_Command(const Data_fame & A_Fame, unsigned char A_type, vector<char> &v_data, string aeskey, string keyiv);
    void Analy_Fame_RE_key_Command(const Data_fame & A_Fame, vector<char> &v_data, string aeskey, string keyiv);
    void Analy_Fame_TIMESYNC_CONTROL(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_SET_CONTROL_LIGHT_MODE(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_SET_CONTROL_LIGHT_PLAN(const Data_fame & A_Fame, vector<char> &v_data);
    /*???????	????????	???		??		??		???????	??????	?????
    2Byte	1Byte	1Byte	1Byte	1Byte	8Byte	1Byte	1Byte  ?????????*/
    void Analy_Fame_EMPTY_CONTROL_LIGHT_PLAN(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_PHONE_NUM_DOWLOAD(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_PHONE_NUM_UPDATA(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_PHONE_NUM_DELETE(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_CONTROL_MSGNUM_QUERY(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_CONTROL_MSG_UPDATA(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_CONTROL_MSG_DELETE(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_CONTROL_ATTR_QUERY(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_CONTROL_LIGHT_PLAN_QUERT(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_CONTROL_LOG_REPORT(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_CONTROL_FIRMWARE_UPGRADE(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_CONTROL_FIRMWARE_UPDATE(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_CONTROL_EXIT_UPGRDE(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMINAL_INF_INITIALIZE(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMINAL_INF_DELETE(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMINAL_LIGHT_DIM(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMIANL_SET_LIGHT_GRADU(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMIANL_SET_ELECTRICAL(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMIANL_SET_GROUP(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMIANL_GROUP_DELETE(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMIANL_SCENE_SET(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMIANL_SCENE_DELETE(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMIANL_AUTO_LIGHT_SET(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMIANL_AUTO_LIGHT_DEL(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMIANL_MAXLIGHT_SET(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMIANL_MINLIGHT_SET(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMIANL_DEFAUL_FAIL_SET(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMIANL_RESET_DEFAUL(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMIANL_INFO_REPORT(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMIANL_ALARM_CONITION(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMIANL_LIGHT_COEF_SET(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMIANL_CRITERIA_QUERY(const Data_fame & A_Fame, vector<char> &v_data);
    void Analy_Fame_TERMIANL_AUTO_LIGHT_QUER(const Data_fame & A_Fame, vector<char> &v_data);



};
