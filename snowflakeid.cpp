#include "snowflakeid.h"

#include<chrono>
#include<stdio.h>

SnowflakeIdWorker::SnowflakeIdWorker(uint8_t worker_id, uint8_t data_center_id)
{
	worker_id_ = worker_id & 0x1F;
	data_center_id_ = data_center_id & 0x1F;
}

SnowflakeIdWorker ::~SnowflakeIdWorker()
{
}

bool SnowflakeIdWorker::Init(uint8_t worker_id, uint8_t data_center_id)
{
	if (worker_id <= 0x1F && data_center_id <= 0x1F)
	{
		worker_id_ = worker_id;
		data_center_id_ = data_center_id;

		return true;
	}

	return false;
}

uint64_t SnowflakeIdWorker::GenerateID()
{

	uint64_t timestamp = TimeGen();

	//�����ǰʱ��С����һ��ID���ɵ�ʱ�����˵��ϵͳʱ�ӻ��˹����ʱ��Ӧ���׳��쳣
	if (timestamp < last_timestamp_)
	{
		printf("Clock moved backwards.  Refusing to generate id for %lld milliseconds \n", last_timestamp_ - timestamp);
		//�޷�����������,�ͼǸ���־�������
	}

	//�����ͬһʱ�����ɵģ�����к���������
	if (last_timestamp_ == timestamp)
	{
		sequence_ = (sequence_ + 1) & 0xfff;//�������е����룬����Ϊ4095 (0b111111111111=0xfff=4095)
		//�������������
		if (sequence_ == 0)
		{
			//��������һ������,����µ�ʱ���
			timestamp = TilNextMillis(last_timestamp_);
		}
	}
	//ʱ����ı䣬��������������
	else
	{
		sequence_ = 0;
	}

	//�ϴ�����ID��ʱ���
	last_timestamp_ = timestamp;

	//��λ��ͨ��������ƴ��һ�����64λ��ID
	return ((timestamp - twepoch_) << 22) /*ʱ���������22λ(5+5+12) */
		| (data_center_id_ << 17) /*���ݱ�ʶid������17λ(12+5)*/
		| (worker_id_ << 12) /*����ID������12λ */
		| sequence_;

}

uint64_t SnowflakeIdWorker::TimeGen()
{
	auto time_now = std::chrono::system_clock::now();
	auto duration_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_now.time_since_epoch());
	return duration_in_ms.count();
}

uint64_t SnowflakeIdWorker::TilNextMillis(uint64_t last_timestamp)
{
	uint64_t timestamp = TimeGen();
	while (timestamp <= last_timestamp_)
	{
		timestamp = TimeGen();
	}
	return timestamp;

}
