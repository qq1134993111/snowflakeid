#pragma once
#include <stdint.h>
#include <thread>
#include <atomic>
/*
Twitter�ķֲ�ʽ����ID�㷨Snowflake��C++��ֲ
�ο�
http://www.cnblogs.com/relucent/p/4955340.html
https://github.com/tian-yuan/ServiceFramework/blob/master/baselib/base/snowflake_id_util.h
*/

/**
* Twitter_Snowflake
* SnowFlake�Ľṹ����(ÿ������-�ֿ�):
* 0 - 0000000000 0000000000 0000000000 0000000000 0 - 00000 - 00000 - 000000000000
* 1λ��ʶ������long����������Java���Ǵ����ŵģ����λ�Ƿ���λ��������0��������1������idһ�������������λ��0
* 41λʱ���(���뼶)��ע�⣬41λʱ��ز��Ǵ洢��ǰʱ���ʱ��أ����Ǵ洢ʱ��صĲ�ֵ����ǰʱ��� - ��ʼʱ���)
* �õ���ֵ��������ĵĿ�ʼʱ��أ�һ�������ǵ�id��������ʼʹ�õ�ʱ�䣬�����ǳ�����ָ���ģ������������IdWorker���startTime���ԣ���41λ��ʱ��أ�����ʹ��69�꣬��T = (1L << 41) / (1000L * 60 * 60 * 24 * 365) = 69
* 10λ�����ݻ���λ�����Բ�����1024���ڵ㣬����5λdatacenterId��5λworkerId
* 12λ���У������ڵļ�����12λ�ļ���˳���֧��ÿ���ڵ�ÿ����(ͬһ������ͬһʱ���)����4096��ID���
* �������պ�64λ��
* SnowFlake���ŵ��ǣ������ϰ���ʱ���������򣬲��������ֲ�ʽϵͳ�ڲ������ID��ײ(����������ID�ͻ���ID������)������Ч�ʽϸߡ�
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

	uint64_t  twepoch_ = 0;//��ʼʱ��أ�Ĭ��Ϊ0,��1970��1��1������
private:
	uint8_t worker_id_; //��������ID(0~31)
	uint8_t data_center_id_;//��������ID(0~31)

	uint16_t sequence_ = 0;//����������(0~4095)
	uint64_t last_timestamp_ = 0;//��һ������ID��ʱ���

	//�����Ժ���Ϊ��λ�ĵ�ǰʱ��
	uint64_t TimeGen();

	//��������һ�����룬ֱ������µ�ʱ���
	//lastTimestamp ������ID��ʱ���
	//���ص�ǰ����ʱ���
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





