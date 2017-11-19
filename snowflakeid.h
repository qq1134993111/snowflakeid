#pragma once
#include <stdint.h>
#include <thread>
#include <atomic>
/*
Twitter的分布式自增ID算法Snowflake的C++移植
参考
http://www.cnblogs.com/relucent/p/4955340.html
https://github.com/tian-yuan/ServiceFramework/blob/master/baselib/base/snowflake_id_util.h
*/

/**
* Twitter_Snowflake
* SnowFlake的结构如下(每部分用-分开):
* 0 - 0000000000 0000000000 0000000000 0000000000 0 - 00000 - 00000 - 000000000000
* 1位标识，由于long基本类型在Java中是带符号的，最高位是符号位，正数是0，负数是1，所以id一般是正数，最高位是0
* 41位时间截(毫秒级)，注意，41位时间截不是存储当前时间的时间截，而是存储时间截的差值（当前时间截 - 开始时间截)
* 得到的值），这里的的开始时间截，一般是我们的id生成器开始使用的时间，由我们程序来指定的（如下下面程序IdWorker类的startTime属性）。41位的时间截，可以使用69年，年T = (1L << 41) / (1000L * 60 * 60 * 24 * 365) = 69
* 10位的数据机器位，可以部署在1024个节点，包括5位datacenterId和5位workerId
* 12位序列，毫秒内的计数，12位的计数顺序号支持每个节点每毫秒(同一机器，同一时间截)产生4096个ID序号
* 加起来刚好64位。
* SnowFlake的优点是，整体上按照时间自增排序，并且整个分布式系统内不会产生ID碰撞(由数据中心ID和机器ID作区分)，并且效率较高。
*/

class SnowflakeIdWorker
{
public:
	SnowflakeIdWorker(uint8_t worker_id = 0, uint8_t data_center_id = 0);
	~SnowflakeIdWorker();

	uint8_t GetWorkerID() { return worker_id_; }
	uint8_t GetDataCenterID() { return data_center_id_; }
	bool Init(uint8_t worker_id = 0, uint8_t data_center_id = 0);
	uint64_t GenerateID();

	uint64_t  twepoch_ = 0;//开始时间截，默认为0,从1970年1月1日算起
private:
	uint8_t worker_id_; //工作机器ID(0~31)
	uint8_t data_center_id_;//数据中心ID(0~31)

	uint16_t sequence_ = 0;//毫秒内序列(0~4095)
	uint64_t last_timestamp_ = 0;//上一次生成ID的时间戳

	//返回以毫秒为单位的当前时间
	uint64_t TimeGen();

	//阻塞到下一个毫秒，直到获得新的时间戳
	//lastTimestamp 次生成ID的时间截
	//返回当前毫秒时间戳
	uint64_t TilNextMillis(uint64_t last_timestamp);

};
//
//inline uint64_t GenerateIDSafe()
//{
//	static std::atomic<uint16_t> s_thread_count = 0;
//
//	static thread_local std::atomic<uint16_t> s_tl_number = ++s_thread_count;
//	static thread_local SnowflakeIdWorker s_tl_snowflakeid(s_tl_number %32, s_tl_number /32);
//
//
//	return s_tl_snowflakeid.GenerateID();
//}





