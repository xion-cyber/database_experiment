#include <cstdio>
#include <cmath>
#include <ctime>
#include "pin.H"

/**************************************
 * Cache Model Base Class
**************************************/
class CacheModel
{
public:
    // Constructor
    CacheModel(UINT32 block_num, UINT32 log_block_size)
        : m_block_num(block_num), m_blksz_log(log_block_size),
          m_rd_reqs(0), m_wr_reqs(0), m_rd_hits(0), m_wr_hits(0)
    {
        m_valids = new bool[m_block_num];//各个块的有效位
        m_tags = new UINT32[m_block_num];//每个块的标记tag
        m_replace_q = new UINT32[m_block_num];//和缓冲块数目相等的队列

        for (UINT i = 0; i < m_block_num; i++)
        {
            m_valids[i] = false;//初始化无数据
            m_replace_q[i] = i;
        }
    }

    // Destructor
    virtual ~CacheModel()
    {
        delete[] m_valids;
        delete[] m_tags;
        delete[] m_replace_q;
    }

    // Update the cache state whenever data is read
    // 收到读请求时 查询cache是否命中 同时更新读请求数目和命中数目
    void readReq(UINT32 mem_addr)
    {
        m_rd_reqs++;
        if (access(mem_addr)) m_rd_hits++;
    }

    // Update the cache state whenever data is written
    // 收到写请求时 查询cache是否命中 同时更新写请求数目和命中数目
    void writeReq(UINT32 mem_addr)
    {
        m_wr_reqs++;
        if (access(mem_addr)) m_wr_hits++;
    }

    UINT32 getRdReq() { return m_rd_reqs; }//返回当前读的总数
    UINT32 getWrReq() { return m_wr_reqs; }//返回当前写的总数

    //打印当前接收到的请求次数 命中次数和命中率
    void dumpResults()
    {
        float rdHitRate = 100 * (float)m_rd_hits/m_rd_reqs;
        float wrHitRate = 100 * (float)m_wr_hits/m_wr_reqs;
        printf("\tread req: %lu,\thit: %lu,\thit rate: %.2f%%\n", m_rd_reqs, m_rd_hits, rdHitRate);
        printf("\twrite req: %lu,\thit: %lu,\thit rate: %.2f%%\n", m_wr_reqs, m_wr_hits, wrHitRate);
    }

protected:
    UINT32 m_block_num;     // The number of cache blocks cache块的数目
    UINT32 m_blksz_log;     // cache块大小的以2为底的对数

    bool* m_valids;         // 有效位标记
    UINT32* m_tags;         // cache块标记
    UINT32* m_replace_q;    // 发生cache块替换时 待替换块组成的候选队列

    UINT64 m_rd_reqs;       // The number of read-requests
    UINT64 m_wr_reqs;       // The number of write-requests
    UINT64 m_rd_hits;       // The number of hit read-requests
    UINT64 m_wr_hits;       // The number of hit write-requests

    // Look up the cache to decide whether the access is hit or missed
    // 用当前访存地址查cache 若命中 记录块号
    virtual bool lookup(UINT32 mem_addr, UINT32& blk_id) = 0;

    // Access the cache: update m_replace_q if hit, otherwise replace a block and update m_replace_q
    // 模拟cache访问 命中时更新替换候选队列 否则进行cache块替换
    virtual bool access(UINT32 mem_addr) = 0;

    // Update m_replace_q
    // 更新cache块替换的候选队列
    virtual void updateReplaceQ(UINT32 blk_id) = 0;
};

/**************************************
 * Fully Associative Cache Class
**************************************/
class FullAssoCache : public CacheModel
{
public:
    // Constructor
    FullAssoCache(UINT32 block_num, UINT32 log_block_size)
        : CacheModel(block_num, log_block_size) {}

    // Destructor
    ~FullAssoCache() {}

private:
    UINT32 getTag(UINT32 addr)
    {
        //全相联需要遍历寻找 tag就是标记
        return addr>>m_blksz_log;
    }

    // Look up the cache to decide whether the access is hit or missed
    bool lookup(UINT32 mem_addr, UINT32& blk_id)
    {
        UINT32 tag = getTag(mem_addr);
        // TODO
        // 全相联则遍历所有块查找
        for(UINT32 i=0;i<m_block_num;++i)
        {
            if(m_tags[i]==tag && m_valids[i]==true)
            {
                //命中则记录对应块号
                blk_id=i;
                return true;
            }
        }
        return false;
    }

    // Access the cache: update m_replace_q if hit, otherwise replace a block and update m_replace_q
    bool access(UINT32 mem_addr)
    {
        UINT32 blk_id;
        if (lookup(mem_addr, blk_id))
        {
            updateReplaceQ(blk_id);     // Update m_replace_q
            return true;
        }

        // Get the to-be-replaced block id using m_replace_q
        UINT32 bid_2be_replaced = m_replace_q[0];// TODO
        // Replace the cache block
        // TODO
        // 填充块号
        if(m_valids[bid_2be_replaced]==false) m_valids[bid_2be_replaced]=true;
        m_tags[bid_2be_replaced]=getTag(mem_addr);//替换tag
        updateReplaceQ(bid_2be_replaced);

        return false;
    }

    // Update m_replace_q
    void updateReplaceQ(UINT32 blk_id)
    {
        // TODO
        UINT32 loc=0;
        //将找到的块号放到队尾
        for(UINT32 i=0;i<m_block_num;++i)
        {
            if(blk_id==m_replace_q[i])
            {
                loc=i;
                break;
            }
        }
        for(UINT32 i=loc;i<m_block_num-1;i++)
        {
            m_replace_q[i]=m_replace_q[i+1];
        }

        m_replace_q[m_block_num-1]=blk_id;
    }
};

/**************************************
 * Directly Mapped Cache Class
**************************************/
class DirectMapCache : public CacheModel
{
public:
    // Constructor
    DirectMapCache(UINT32 block_num, UINT32 log_block_size) 
    : CacheModel(block_num, log_block_size) {}

    // Destructor
    ~DirectMapCache() {}

private:

    // 得到区块号
    UINT32 getTag(UINT32 addr)
    {
        return (addr>>m_blksz_log)/m_block_num;//得到
    }
    // 得到区内号
    UINT32 getBlkid(UINT32 addr)
    {
        UINT32 tag=addr>>m_blksz_log;
        return tag & (m_block_num-1);
    }

    // Look up the cache to decide whether the access is hit or missed
    bool lookup(UINT32 mem_addr, UINT32& blk_id)
    {
        // TODO
        //得到cache块内的地址
        blk_id=getBlkid(mem_addr);
        UINT32 tag=getTag(mem_addr);
        if(m_tags[blk_id]==tag && m_valids[blk_id]==true)
        {
            return true;
        }
        return false;
    }

    // Access the cache: update m_replace_q if hit, otherwise replace a block and update m_replace_q
    bool access(UINT32 mem_addr)
    {
        UINT32 blk_id;
        // TODO
        if(lookup(mem_addr, blk_id))
        {
            return true;
        }
        //未命中时 替换对应块
        blk_id=getBlkid(mem_addr);
        if(m_valids[blk_id]==false)m_valids[blk_id]=true;
        m_tags[blk_id]=getTag(mem_addr);
        return false;
    }

    // Update m_replace_q
    void updateReplaceQ(UINT32 blk_id)
    {
        // TODO: do nothing
    }
};

/**************************************
 * Set-Associative Cache Class
**************************************/
class SetAssoCache : public CacheModel
{
public:
    // Constructor
    SetAssoCache(UINT32 block_num, UINT32 log_block_size, UINT32 group_size)
    : CacheModel(block_num, log_block_size), m_group_size(group_size)
    {
        // m_group_size=group_size;
    }
    // Destructor
    ~SetAssoCache() {}

private:
    UINT32 m_group_size;//组数

    // 获得tag
    UINT32 getTag(UINT32 addr)
    {
        return (addr>>m_blksz_log)/(m_block_num/m_group_size);
        // return addr>>13;
    }
    // 获得组号
    UINT32 getGroupid(UINT32 addr)
    {
        return (addr>>m_blksz_log) & (m_block_num/m_group_size-1);
        // return (addr>>m_blksz_log) & 127;
    }

    // Look up the cache to decide whether the access is hit or missed
    bool lookup(UINT32 mem_addr, UINT32& blk_id)
    {
        // TODO
        UINT32 tag=getTag(mem_addr);
        UINT32 group_id = getGroupid(mem_addr);
        // 对组内的所有tag和valid进行判断
        for(UINT32 i=0;i<m_group_size;++i)
        {
            if(m_tags[group_id*m_group_size+i]==tag && m_valids[group_id*m_group_size+i]==true)
            {
                blk_id=group_id*m_group_size+i;
                return true;
            }
        }
        blk_id=group_id*m_group_size;
        return false;
    }

    // Access the cache: update m_replace_q if hit, otherwise replace a block and update m_replace_q
    bool access(UINT32 mem_addr)
    {
        // TODO
        UINT32 blk_id;
        if(lookup(mem_addr, blk_id))
        {
            updateReplaceQ(blk_id);
            return true;
        }
        //取出组内队列的第一个
        UINT32 bid_2be_replaced = m_replace_q[blk_id];// TODO

        if(m_valids[bid_2be_replaced]==false) m_valids[bid_2be_replaced]=true;
        m_tags[bid_2be_replaced]=getTag(mem_addr);
        updateReplaceQ(bid_2be_replaced);

        return false;
    }

    // Update m_replace_q
    void updateReplaceQ(UINT32 blk_id)
    {
        // TODO
        UINT32 group_id=blk_id/m_group_size;

        UINT32 loc=0;
        //找出blk_id的位置
        for(UINT32 i=0;i<m_group_size;++i)
        {
            if(m_replace_q[i+group_id*m_group_size]==blk_id)
            {
                loc=i+group_id*m_group_size;
                break;
            }
        }
        for(UINT32 i=loc;i<group_id*m_group_size+m_group_size-1;++i)
        {
            m_replace_q[i]=m_replace_q[i+1];
        }
        m_replace_q[group_id*m_group_size+m_group_size-1]=blk_id;
    }
};

CacheModel* my_fa_cache;
CacheModel* my_dm_cache;
CacheModel* my_sa_cache;

double time_fa_rd = 0, time_fa_wr = 0;
double time_dm_rd = 0, time_dm_wr = 0;
double time_sa_rd = 0, time_sa_wr = 0;

// Cache reading analysis routine
void readCache(UINT32 mem_addr)
{
    mem_addr = (mem_addr >> 2) << 2;
    clock_t pt0 = clock();
    my_fa_cache->readReq(mem_addr);
    clock_t pt1 = clock();
    my_dm_cache->readReq(mem_addr);
    clock_t pt2 = clock();
    my_sa_cache->readReq(mem_addr);
    clock_t pt3 = clock();

    time_fa_rd += 1000000*(double)(pt1 - pt0) / CLOCKS_PER_SEC;
    time_dm_rd += 1000000*(double)(pt2 - pt1) / CLOCKS_PER_SEC;
    time_sa_rd += 1000000*(double)(pt3 - pt2) / CLOCKS_PER_SEC;
}

// Cache writing analysis routine
void writeCache(UINT32 mem_addr)
{
    mem_addr = (mem_addr >> 2) << 2;
    clock_t pt0 = clock();
    my_fa_cache->writeReq(mem_addr);
    clock_t pt1 = clock();
    my_dm_cache->writeReq(mem_addr);
    clock_t pt2 = clock();
    my_sa_cache->writeReq(mem_addr);
    clock_t pt3 = clock();

    time_fa_wr += 1000000*(double)(pt1 - pt0) / CLOCKS_PER_SEC;
    time_dm_wr += 1000000*(double)(pt2 - pt1) / CLOCKS_PER_SEC;
    time_sa_wr += 1000000*(double)(pt3 - pt2) / CLOCKS_PER_SEC;
}

// This knob will set the cache param m_block_num
// 设置块数目
KNOB<UINT32> KnobBlockNum(KNOB_MODE_WRITEONCE, "pintool",
        "n", "512", "specify the number of blocks in bytes");

// This knob will set the cache param m_blksz_log
// 设置容量大小 多少位
KNOB<UINT32> KnobBlockSizeLog(KNOB_MODE_WRITEONCE, "pintool",
        "b", "6", "specify the log of the block size in bytes");

// This knob will set the cache param m_sets_log
KNOB<UINT32> KnobSetsLog(KNOB_MODE_WRITEONCE, "pintool",
        "r", "7", "specify the log of the number of rows");

// This knob will set the cache param m_asso
KNOB<UINT32> KnobAssociativity(KNOB_MODE_WRITEONCE, "pintool",
        "a", "4", "specify the m_asso");

// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID *v)
{
    if (INS_IsMemoryRead(ins))
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)readCache, IARG_MEMORYREAD_EA, IARG_END);
    if (INS_IsMemoryWrite(ins))
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)writeCache, IARG_MEMORYWRITE_EA, IARG_END);
}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
    printf("\nFully Associative Cache:\n");
    printf("average read time: %.2fus\n", time_fa_rd/my_fa_cache->getRdReq());
    printf("average write time: %.2fus\n", time_fa_rd/my_fa_cache->getWrReq());
    my_fa_cache->dumpResults();
    printf("\nDirectly Mapped Cache:\n");
    printf("average read time: %.2fus\n", time_dm_rd/my_dm_cache->getRdReq());
    printf("average write time: %.2fus\n", time_dm_rd/my_dm_cache->getWrReq());
    my_dm_cache->dumpResults();
    printf("\nSet-Associative Cache:\n");
    printf("average read time: %.2fus\n", time_sa_rd/my_sa_cache->getRdReq());
    printf("average write time: %.2fus\n", time_sa_rd/my_sa_cache->getWrReq());
    my_sa_cache->dumpResults();

    delete my_fa_cache;
    delete my_dm_cache;
    delete my_sa_cache;
}

// argc, argv are the entire command line, including pin -t <toolname> -- ...
int main(int argc, char* argv[])
{
    // Initialize pin
    PIN_Init(argc, argv);

    my_fa_cache = new FullAssoCache(KnobBlockNum.Value(), KnobBlockSizeLog.Value());
    my_dm_cache = new DirectMapCache(KnobBlockNum.Value(), KnobBlockSizeLog.Value());
    my_sa_cache = new SetAssoCache(KnobBlockNum.Value(), KnobBlockSizeLog.Value(), KnobAssociativity.Value());

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);

    // Start the program, never returns
    PIN_StartProgram();

    return 0;
}
