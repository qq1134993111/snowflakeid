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

	//如果当前时间小于上一次ID生成的时间戳，说明系统时钟回退过这个时候应当抛出异常
	if (timestamp < last_timestamp_)
	{
		printf("Clock moved backwards.  Refusing to generate id for %lld milliseconds \n", last_timestamp_ - timestamp);
		//无法处理不处理了,就记个日志让它错吧
	}

	//如果是同一时间生成的，则进行毫秒内序列
	if (last_timestamp_ == timestamp)
	{
		sequence_ = (sequence_ + 1) & 0xfff;//生成序列的掩码，这里为4095 (0b111111111111=0xfff=4095)
		//毫秒内序列溢出
		if (sequence_ == 0)
		{
			//阻塞到下一个毫秒,获得新的时间戳
			timestamp = TilNextMillis(last_timestamp_);
		}
	}
	//时间戳改变，毫秒内序列重置
	else
	{
		sequence_ = 0;
	}

	//上次生成ID的时间截
	last_timestamp_ = timestamp;

	//移位并通过或运算拼到一起组成64位的ID
	return ((timestamp - twepoch_) << 22) /*时间截向左移22位(5+5+12) */
		| (data_center_id_ << 17) /*数据标识id向左移17位(12+5)*/
		| (worker_id_ << 12) /*机器ID向左移12位 */
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
