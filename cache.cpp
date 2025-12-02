#include<iostream>
#include<unistd.h>
#include<climits>
#include<cstdlib>
#include<fstream>
#include<random>
enum state{
    VALID,MISS_PENDING,INVALID,MODIFIED
};
enum read_write_signal{
    WR,RD
};
int LOG(int a){
    if(a<=1)return 0;
    return 32-__builtin_clz((unsigned)(a-1));
}
class CPUReq{
public:
    int tag,index,word_offset,character_offset,block_address,block_offset;
    int *data;
    read_write_signal T;
    CPUReq(int sz){
        tag=0;
        index=0;
        word_offset=0;
        character_offset=0;
        block_address=0;
        block_offset =0;
        data = nullptr;
        if(sz>0){
            data = new int[sz];
        }
    }
    CPUReq(){}
};
struct CacheLine{
    int tag;
    int *data;
    int state;
    CacheLine(){
        tag=0;
        data = nullptr;
        state = state;
    }
    CacheLine(int sz){
        tag=0;
        data = new int[sz];
        state = state::INVALID;
    }
};
int *memory= nullptr;
int hit=0;
class DirectMappedCache{
    int CACHE_SIZE,WORD_SIZE,BLOCK_SIZE,CACHE_ADDRESS_SIZE,MEM_SIZE;
    bool is_write_back;
    bool is_write_alloc;
    void set_cpu_req(int A,read_write_signal sig){
        int character_offset_bits = LOG(WORD_SIZE);
        cpu_req.character_offset = (A&((1<<character_offset_bits)-1));
        int word_offset_bits = LOG(BLOCK_SIZE/WORD_SIZE);
        cpu_req.word_offset = ((A>>character_offset_bits)&((1<<word_offset_bits)-1));
        int index_bits = LOG(CACHE_SIZE/BLOCK_SIZE);
        cpu_req.index = ((A/BLOCK_SIZE)&((1<<index_bits)-1));
        int tag_bits = CACHE_ADDRESS_SIZE - word_offset_bits-index_bits-character_offset_bits;
        cpu_req.tag = (((A/BLOCK_SIZE)>>index_bits)&((1<<tag_bits)-1));
        cpu_req.T = sig;
        cpu_req.block_address = A/BLOCK_SIZE;
        cpu_req.block_offset = A%BLOCK_SIZE;
    }
    CacheLine* blocks;
public:
    CPUReq cpu_req;
    DirectMappedCache(int cache_size,int block_size,int word_size,int mem_size,int cache_address_size,int _is_write_back,int _is_write_alloc){
        CACHE_SIZE = cache_size;
        BLOCK_SIZE = block_size;
        WORD_SIZE = word_size;
        MEM_SIZE = mem_size;
        CACHE_ADDRESS_SIZE = cache_address_size;
        is_write_back = _is_write_back;
        is_write_alloc = _is_write_alloc;
        blocks = new CacheLine[CACHE_SIZE/BLOCK_SIZE];
        for(int i=0;i<CACHE_SIZE/BLOCK_SIZE;i++){
            blocks[i].data = new int[BLOCK_SIZE/WORD_SIZE];
            blocks[i].tag = 0;
            blocks[i].state = INVALID;
        }
        cpu_req = CPUReq(BLOCK_SIZE/WORD_SIZE);
    }
    void evict(CacheLine& block){block.state = INVALID;}
    int process_input(int A,read_write_signal sig,int _data=0){
        set_cpu_req(A,sig);
        CacheLine& B = blocks[cpu_req.index];
        if(cpu_req.T==read_write_signal::RD){
            // Read
            if(B.tag == cpu_req.tag && (B.state==state::VALID || B.state == state::MODIFIED)){
                // Read Hit
                hit++;
                return B.data[cpu_req.word_offset];
            }else{
                // Read Miss
                if(is_write_back && B.state == MODIFIED){
                    int BA = (B.tag<<LOG(CACHE_SIZE/BLOCK_SIZE)) + cpu_req.index;
                    for(int i=0;i<BLOCK_SIZE/WORD_SIZE;i++){
                        memory[i+BA*(BLOCK_SIZE/WORD_SIZE)]=B.data[i];
                    }
                }
                evict(B);
                int BA = cpu_req.block_address;
                for(int i=0;i<BLOCK_SIZE/WORD_SIZE;i++){
                        B.data[i]=memory[i+BA*(BLOCK_SIZE/WORD_SIZE)];
                }
                B.state = VALID;
                B.tag = cpu_req.tag;
                return B.data[cpu_req.word_offset];
            }
        }else{
            // Write
            if(B.tag == cpu_req.tag && (B.state == state::VALID || B.state == state::MODIFIED)){
                // Write Hit
                hit++;
                if(is_write_back){
                    B.state = MODIFIED;
                    B.data[cpu_req.word_offset] = _data;
                }else{
                    B.data[cpu_req.word_offset] = _data;
                    int idx = cpu_req.block_address*(BLOCK_SIZE/WORD_SIZE) + cpu_req.word_offset;
                    memory[idx] = _data;
                }
                return _data;
            }else{
                // Write Miss
                if(!is_write_alloc){
                    int idx = cpu_req.block_address*(BLOCK_SIZE/WORD_SIZE) + cpu_req.word_offset;
                    memory[idx] = _data;
                    return _data;
                }else{
                    if(is_write_back && B.state==MODIFIED){
                        int BA = (B.tag<<(CACHE_SIZE/BLOCK_SIZE)) + cpu_req.index;
                        for(int i=0;i<BLOCK_SIZE/WORD_SIZE;i++){
                            memory[BA*(BLOCK_SIZE/WORD_SIZE) + i]=B.data[i];
                        }
                    }
                    int BA = cpu_req.block_address;
                    for(int i=0;i<BLOCK_SIZE/WORD_SIZE;i++){
                            B.data[i] = memory[BA*(BLOCK_SIZE/WORD_SIZE) + i];
                    }
                    B.data[cpu_req.word_offset] = _data;
                    if(is_write_back){
                        B.state = MODIFIED;
                    }else{
                        int idx = BA*BLOCK_SIZE/WORD_SIZE  + cpu_req.word_offset;
                        memory[idx] = _data;
                        B.state = VALID;
                    }
                    B.tag = cpu_req.tag;
                    return _data;
                }
            }
        }
    }
};
class AssociativeCache{
    int CACHE_SIZE,WORD_SIZE,BLOCK_SIZE,CACHE_ADDRESS_SIZE,MEM_SIZE;
    bool is_write_back;
    bool is_write_alloc;
    CacheLine* blocks;
    void set_cpu_req(int A,read_write_signal sig){
        int character_offset_bits = LOG(WORD_SIZE);
        cpu_req.character_offset = (A&((1<<character_offset_bits)-1));
        int word_offset_bits = LOG(BLOCK_SIZE/WORD_SIZE);
        cpu_req.word_offset = ((A>>character_offset_bits)&((1<<word_offset_bits)-1));
        cpu_req.index = 0;
        int tag_bits = CACHE_ADDRESS_SIZE - word_offset_bits-character_offset_bits;
        cpu_req.tag = (((A/BLOCK_SIZE))&((1<<tag_bits)-1));
        cpu_req.T = sig;
        cpu_req.block_address = A/BLOCK_SIZE;
        cpu_req.block_offset = A%BLOCK_SIZE;
    }
    int victim_index(){
        int victim=-1;
        for(int i=0;i<CACHE_SIZE/BLOCK_SIZE;i++){
            if(blocks[i].state == INVALID){
                return i;
            }
        }
        victim=rand()%(CACHE_SIZE/BLOCK_SIZE);
        return victim;
    }
public:
    CPUReq cpu_req;
    AssociativeCache(int cache_size,int block_size,int word_size,int mem_size,int cache_address_size,int _is_write_back,int _is_write_alloc){
        CACHE_SIZE = cache_size;
        BLOCK_SIZE = block_size;
        WORD_SIZE = word_size;
        MEM_SIZE = mem_size;
        CACHE_ADDRESS_SIZE = cache_address_size;
        is_write_back = _is_write_back;
        is_write_alloc = _is_write_alloc;
        blocks = new CacheLine[CACHE_SIZE/BLOCK_SIZE];
        for(int i=0;i<CACHE_SIZE/BLOCK_SIZE;i++){
            blocks[i].data = new int[BLOCK_SIZE/WORD_SIZE];
            blocks[i].tag = 0;
            blocks[i].state = INVALID;
        }
        cpu_req = CPUReq(BLOCK_SIZE/WORD_SIZE);
    }
    void evict(CacheLine& block){block.state = INVALID;}
    int process_input(int A,read_write_signal sig,int _data=0){
        set_cpu_req(A,sig);
        int index=-1;
        for(int i=0;i<CACHE_SIZE/BLOCK_SIZE;i++){
            if((blocks[i].state==VALID || blocks[i].state==MODIFIED) && blocks[i].tag == cpu_req.tag){
                index=i;
                break;
            }
        }
        if(index!=-1){
            // Hit
            CacheLine& B = blocks[index];
            if(cpu_req.T==read_write_signal::RD){
            // Read
                if(B.state==state::VALID || B.state == state::MODIFIED){
                    // Read Hit
                    hit++;
                    return B.data[cpu_req.word_offset];
                }
            }else{
                // Write hit
                hit++;
                if(!is_write_back){
                    if(B.state == VALID || B.state == MODIFIED){
                        B.data[cpu_req.word_offset]=_data;
                        int idx = cpu_req.block_address*(BLOCK_SIZE/WORD_SIZE) + cpu_req.word_offset;
                        memory[idx]=_data;
                        return _data;
                    }
                }else{
                    B.data[cpu_req.word_offset]=_data;
                    B.state = MODIFIED;
                    return _data;
                }
            }
        }else{
            // Miss
            int victim = victim_index();
            CacheLine& B = blocks[victim];
            if(is_write_back && B.state == MODIFIED){
                int BA = B.tag;
                for(int i=0;i<BLOCK_SIZE/WORD_SIZE;i++){
                    memory[BA*(BLOCK_SIZE/WORD_SIZE) + i] = B.data[i];
                }
            }
            if(cpu_req.T==read_write_signal::RD){
                // Read Miss
                int BA = cpu_req.block_address;
                for(int i=0;i<BLOCK_SIZE/WORD_SIZE;i++){
                    B.data[i] = memory[BA*(BLOCK_SIZE/WORD_SIZE)+i];
                }
                B.tag = cpu_req.tag;
                B.state=VALID;
                return B.data[cpu_req.word_offset];
            }else{
                // Write Miss
                if(!is_write_alloc){
                    int idx = cpu_req.block_address*(BLOCK_SIZE/WORD_SIZE) + cpu_req.word_offset;
                    memory[idx] = _data;
                    return _data;
                }else{
                    int BA = cpu_req.block_address;
                    for(int i=0;i<BLOCK_SIZE/WORD_SIZE;i++){
                        B.data[i] = memory[BA*(BLOCK_SIZE/WORD_SIZE)+i];
                    }
                    B.tag = cpu_req.tag;
                    if(!is_write_back){
                        B.data[cpu_req.word_offset] = _data;
                        int idx=BA*(BLOCK_SIZE/WORD_SIZE) + cpu_req.word_offset;
                        memory[idx] = _data;
                        B.state = VALID;
                        return _data;
                    }else{
                        B.data[cpu_req.word_offset] = _data;
                        B.state = MODIFIED;
                        return _data;
                    }
                }
            }
        }
    }
};
class SetAssociativeCache{
    int CACHE_SIZE,WORD_SIZE,BLOCK_SIZE,CACHE_ADDRESS_SIZE,MEM_SIZE,ASSOCIATIVITY,SETS,LINES;
    int tag_bits;
    bool is_write_back;
    bool is_write_alloc;
    CacheLine* blocks;
    void set_cpu_req(int A,read_write_signal sig){
        int character_offset_bits = LOG(WORD_SIZE);
        cpu_req.character_offset = (A&((1<<character_offset_bits)-1));
        int word_offset_bits = LOG(BLOCK_SIZE/WORD_SIZE);
        cpu_req.word_offset = ((A>>character_offset_bits)&((1<<word_offset_bits)-1));
        cpu_req.block_address = A/BLOCK_SIZE;
        cpu_req.index = cpu_req.block_address&((1<<LOG(SETS))-1);
        cpu_req.tag = ((cpu_req.block_address>>LOG(SETS))&((1<<tag_bits)-1));
        cpu_req.T = sig;
        cpu_req.block_offset = A%BLOCK_SIZE;
    }
    int victim_index(int index){
        int victim=-1;
        int base = index*ASSOCIATIVITY;
        for(int i=0;i<ASSOCIATIVITY;i++){
            if(blocks[base + i].state == INVALID){
                return i+base;
            }
        }
        victim=rand()%(base + ASSOCIATIVITY);
        return victim;
    }
public:
    CPUReq cpu_req;
    SetAssociativeCache(int cache_size,int block_size,int word_size,int mem_size,int cache_address_size,int _associativity,int _is_write_back,int _is_write_alloc){
        CACHE_SIZE = cache_size;
        BLOCK_SIZE = block_size;
        WORD_SIZE = word_size;
        MEM_SIZE = mem_size;
        CACHE_ADDRESS_SIZE = cache_address_size;
        LINES = CACHE_SIZE/BLOCK_SIZE;
        ASSOCIATIVITY = _associativity;
        SETS = LINES/ASSOCIATIVITY;
        is_write_back = _is_write_back;
        is_write_alloc = _is_write_alloc;
        blocks = new CacheLine[LINES];
        for(int i=0;i<LINES;i++){
            blocks[i].data = new int[BLOCK_SIZE/WORD_SIZE];
            blocks[i].tag = 0;
            blocks[i].state = INVALID;
        }
        cpu_req = CPUReq(BLOCK_SIZE/WORD_SIZE);
        tag_bits = CACHE_ADDRESS_SIZE - LOG(BLOCK_SIZE/WORD_SIZE)-LOG(WORD_SIZE)-LOG(SETS);
        tag_bits = std::max(0,tag_bits);
    }
    void evict(CacheLine& block){block.state = INVALID;}
    int process_input(int A,read_write_signal sig,int _data=0){
        set_cpu_req(A,sig);
        int set_idx = cpu_req.index;
        int base = set_idx*ASSOCIATIVITY;
        int index=-1;
        for(int i=0;i<ASSOCIATIVITY;i++){
            if((blocks[base+i].state==VALID || blocks[base+i].state==MODIFIED) && blocks[base+i].tag == cpu_req.tag){
                index=i+base;
                break;
            }
        }
        if(index!=-1){
            // Hit
            CacheLine& B = blocks[index];
            if(cpu_req.T==read_write_signal::RD){
            // Read
                if(B.state==state::VALID || B.state == state::MODIFIED){
                    // Read Hit
                    hit++;
                    return B.data[cpu_req.word_offset];
                }
            }else{
                // Write hit
                hit++;
                if(!is_write_back){
                    if(B.state == VALID || B.state == MODIFIED){
                        B.data[cpu_req.word_offset]=_data;
                        int idx = cpu_req.block_address*(BLOCK_SIZE/WORD_SIZE) + cpu_req.word_offset;
                        memory[idx]=_data;
                        return _data;
                    }
                }else{
                    B.data[cpu_req.word_offset]=_data;
                    B.state = MODIFIED;
                    return _data;
                }
            }
        }else{
            // Miss
            int victim = victim_index(set_idx);
            CacheLine& B = blocks[victim];
            if(is_write_back && B.state == MODIFIED){
                int BA = (B.tag<<LOG(SETS)) + set_idx;
                for(int i=0;i<BLOCK_SIZE/WORD_SIZE;i++){
                    memory[BA*(BLOCK_SIZE/WORD_SIZE) + i] = B.data[i];
                }
            }
            if(cpu_req.T==read_write_signal::RD){
                // Read Miss
                int BA = cpu_req.block_address;
                for(int i=0;i<BLOCK_SIZE/WORD_SIZE;i++){
                    B.data[i] = memory[BA*(BLOCK_SIZE/WORD_SIZE)+i];
                }
                B.tag = cpu_req.tag;
                B.state=VALID;
                return B.data[cpu_req.word_offset];
            }else{
                // Write Miss
                if(!is_write_alloc){
                    int idx = cpu_req.block_address*(BLOCK_SIZE/WORD_SIZE) + cpu_req.word_offset;
                    memory[idx] = _data;
                    return _data;
                }else{
                    int BA = cpu_req.block_address;
                    for(int i=0;i<BLOCK_SIZE/WORD_SIZE;i++){
                        B.data[i] = memory[BA*(BLOCK_SIZE/WORD_SIZE)+i];
                    }
                    B.tag = cpu_req.tag;
                    if(!is_write_back){
                        B.data[cpu_req.word_offset] = _data;
                        int idx=BA*(BLOCK_SIZE/WORD_SIZE) + cpu_req.word_offset;
                        memory[idx] = _data;
                        B.state = VALID;
                        return _data;
                    }else{
                        B.data[cpu_req.word_offset] = _data;
                        B.state = MODIFIED;
                        return _data;
                    }
                }
            }
        }
    }
};
int main(){
    std::ifstream in("config.txt");
    if(!in){
        std::cerr<<"Failed to open file\n";
        return 0;
    }
    srand(time(NULL));
    int mem_size = 0;
    int cache_size = 0;
    int associativity=1,block_size=0,word_size=0,is_write_back=0,is_write_alloc=0;
    std::string s;
    while(std::getline(in,s)){
        auto pos = s.find('=');
        auto val = s.substr(pos+1);
        s = s.substr(0,pos);
        if(s=="ASSOCIATIVITY"){
            associativity = std::stoi(val);
        }else if(s=="CACHE_SIZE"){
            cache_size = std::stoi(val);
        }else if(s=="BLOCK_SIZE"){
            block_size=std::stoi(val);
        }else if(s=="WORD_SIZE"){
            word_size = std::stoi(val);
        }else if(s=="MEM_SIZE"){
            mem_size = std::stoi(val);
        }else if(s=="WRITE_ALLOC"){
            is_write_alloc = 1;
        }else if(s=="WRITE_BACK"){
            is_write_back = 1;
        }
    }
    cache_size*=(1<<10);
    mem_size*=(1<<10);
    int block_offset_bits = LOG(block_size/word_size);
    int block_address_bits = LOG(mem_size/block_size);
    int word_index_size = LOG(word_size);
    memory = new int[mem_size/word_size];
    for(int i=0;i<mem_size/word_size;i++)memory[i]=0;
    auto cache_address_size = block_address_bits + block_offset_bits + word_index_size;
    DirectMappedCache d(cache_size,block_size,word_size,mem_size,cache_address_size,is_write_back,is_write_alloc);
    AssociativeCache a(cache_size,block_size,word_size,mem_size,cache_address_size,is_write_back,is_write_alloc);
    SetAssociativeCache sa(cache_size,block_size,word_size,mem_size,cache_address_size,associativity,is_write_back,is_write_alloc);
    int max_address = (1<<cache_address_size)-1;
    std::random_device rd;
    std::mt19937 gen(rd());
    const int turns = 1000000;
    int inputs[turns];
    std::uniform_int_distribution<> dist(1,max_address);
    for(int i=0;i<turns;i++){
        inputs[i] = std::max(dist(gen),0);
    }
    for(int i=0;i<turns;i++){
        auto x = inputs[i];
        if(x&1){
            d.process_input(x,read_write_signal::WR,i);
        }else{
            d.process_input(x,read_write_signal::RD);
        }
    }
    std::cout<<"DirctMapped Cache Performance : ";
    std::cout<<1.0*hit/(1.0*turns)<<'\n';
    hit=0;
    for(int i=0;i<turns;i++){
        auto x = inputs[i];
        if(x&1){
            a.process_input(x,read_write_signal::WR,i);
        }else{
            a.process_input(x,read_write_signal::RD);
        }
    }
    std::cout<<"Associative Cache Performance : ";
    std::cout<<1.0*hit/(1.0*turns)<<'\n';
    hit=0;for(int i=0;i<turns;i++){
        auto x = inputs[i];
        if(x&1){
            sa.process_input(x,read_write_signal::WR,i);
        }else{
            sa.process_input(x,read_write_signal::RD);
        }
    }
    std::cout<<"Set Associative Cache Performance : ";
    std::cout<<1.0*hit/(1.0*turns)<<'\n';
    return 0;
}