#include "stdafx.h"
#include "Handler.h"

int Dispose::ReadFrameAccordinglength(uint8_t * data, int Readlength)
{

	PushData(data,Readlength);
	if (FrameBuff.size() > LengthSite+IntLength)
	{
		int FrameLen = FrameLength();
        if (FrameLen > 4096)//FrameLen < LengthSite + IntLength ||
		{
			FindHead(Head);//在数据内继续查找用find
			if (FrameBuff.size() > LengthSite + IntLength)
				FrameLen = FrameLength();
		}
		

		auto tempframe = FrameBuff.begin();
		if (FrameBuff.size() >= FrameLen + LengthSite + IntLength + supplement)
		{
			int i = 1;
			for (; FrameLen + LengthSite + IntLength + supplement > i; ++i)
				tempframe++;
			if(*tempframe == End)
			{
				GetFrame(FrameLen + LengthSite + IntLength + supplement);
				return 1;
			}
			else
			{
				FindHead(Head);
			}
		}
	}

	return 0;
}

int Dispose::ReadFrameHeadEnd(uint8_t * data, int Readlength)
{
	PushData(data, Readlength);
    auto temp = FrameBuff.begin();
	int i = 0;
	for (; i < FrameBuff.size(); ++i)
	{
        if (*temp == End)
            return GetFrame(i+1);//0读取结束
        ++temp;
	}
	return 1; //表示还未读取完毕
}

int Dispose::FindHead(uint8_t Head)
{
	auto it = FrameBuff.begin();
	auto it0 = FrameBuff.begin();
	it0++;
	for (;it0 != FrameBuff.end();++it0)
	{
		if (*it0 == Head)
			break;
	}
	FrameBuff.erase(it, it0);
	return 0;
}

int Dispose::CheckEnd(uint8_t End)
{
	return 0;
}

int Dispose::PushData(uint8_t * data, int Readlength)
{
	if (FrameBuff.empty())
	{
		int i = 0;
		for (; i < Readlength; ++i)
		{
			if (data[i] == Head)
				break;
		}
		for (; i < Readlength; ++i)
		{
			FrameBuff.push_back(data[i]);
		}
	}
	else
	{
		for (int i = 0; i < Readlength; ++i)
			FrameBuff.push_back(data[i]);
	}
	return 0;
}

int Dispose::FrameLength()
{
	uint8_t temp[4];
	auto tempframe = FrameBuff.begin();
	for (int i = 0; i < LengthSite; i++)
		tempframe++;
	for (int i = 0; i < IntLength; i++)
	{
		temp[i] = *tempframe;
		tempframe++;
	}
    int FrameLength = qingpei::toolkit::cast::bytes_to_num<uint16_t>(temp, IntLength);
	return FrameLength;
}

int Dispose::GetFrame(int framelength)
{
	frame.clear();
	auto temp = FrameBuff.begin();
    for(int i=0;i< framelength;++i)
	{
		frame.push_back(*temp);
		temp++;
	}
    FrameBuff.clear();
	return 0;
}

int Dispose::GetFrame()
{
	frame.clear();
	for (auto temp : FrameBuff)
	{
		frame.push_back(temp);
		if (temp == End)
			break;
	}
	return 0;
}
