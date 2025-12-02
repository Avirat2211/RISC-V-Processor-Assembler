#include<iostream>
#include<vector>
#include<algorithm>
#include<string>
#include<fstream>
#include<climits>
#include<string.h>
#define MAX_REG_COUNT 32
#define PHY_STOP 111111
#define R_TYPE_OPCODE 51
#define I_TYPE_OPCODE 19
#define L_TYPE_OPCODE 3
#define S_TYPE_OPCODE 35
#define B_TYPE_OPCODE 99
#define JAL_TYPE_OPCODE 111
#define J_TYPE_OPCODE 111
#define JR_TYPE_OPCODE 111
#define JALR_TYPE_OPCODE 103
#define NOP_TYPE_OPCODE 19

class Utils{
public:
    Utils(const Utils&)=delete;
    Utils& operator=(const Utils&)=delete; // underloading = operator
    static Utils& getInstance(){
        static Utils obj;
        return obj;
    }
    std::string decimal_to_binary(int n){
        std::string binary;
        bool is_negative = false;
        if(n<0)is_negative = true;
        n=abs(n);
        while(n>0){
            if(n%2){
                binary.push_back('1');
            }else{
                binary.push_back('0');
            }
            n/=2;
        }
        while(binary.size()<32){
            binary.push_back('0');
        }
        reverse(binary.begin(),binary.end());
        if(is_negative){
            flip(binary);
            add_one(binary);
        }
        return binary;
    } 
    std::vector<int> read_object_file(std::string filename){
        std::ifstream inp(filename);
        if(!inp){
            std::cerr<<"Failed to open input file\n";
            return {};
        }
        std::vector<int> inputs;
        std::string temp;
        while(std::getline(inp,temp)){
            temp.erase(std::remove(temp.begin(),temp.end(),' '),temp.end());
            // std::cerr << temp.size() << '\n';
            int decimal =0;
            for(auto &it:temp){
                decimal<<=1;
                if(it=='1')decimal++;
            }
            inputs.push_back(decimal);
        }
        return inputs;
    }
    int get_bits_range(int &ins,int l,int r){
        int ans = 0;
        for(int i=r;i<=l;i++){
            int x = 1ll<<i;
            ans += x;
        }
        ans &= ins;
        ans >>= r;
        return ans;
    }
    int binary_to_decimal(std::string &s){
        int ans=0;
        for(auto &it:s){
            ans*=2;
            if(it=='1'){
                ans++;
            }
        }
        return ans;
    }
    void fflip(std::string &s){
        flip(s);
    }
    std::string sign_extend(std::string num){
        std::string ans;
        std::reverse(num.begin(),num.end());
        if(num.back()=='0'){
            for(auto &it:num)ans.push_back(it);
            while(ans.size()<32)ans.push_back('0');
            std::reverse(ans.begin(),ans.end());
        }else{
            for(auto &it:num)ans.push_back(it);
            while(ans.size()<32)ans.push_back('1');
            std::reverse(ans.begin(),ans.end());
        }
        return ans;
    }
private:
    Utils(){}
    static void flip(std::string &s){
        for(char &ch:s){
            if(ch=='1')ch='0';
            else ch='1';
        }
    }
    static void add_one(std::string &s){
        int n=s.size();
        if(s.back()=='0'){
            s[n-1]='1';
            return;
        }
        for(int i=n-1;i>=0;i--){
            if(s[i]=='1'){
                s[i]='0';
            }else{
                s[i]='1';
                break;
            }
        }
        return;
    }
};
class GPR_class {
public:
    GPR_class (const GPR_class&) = delete;
    GPR_class &operator = (const GPR_class&) = delete;
    static GPR_class& get_instance_GPR() {
        static GPR_class obj;
        return obj;
    }
    int regs[MAX_REG_COUNT];
    int busy[MAX_REG_COUNT];
    int& operator[](int index) {
        if (index < 0 || index >= MAX_REG_COUNT) {
            std::cerr << "Invalid register access\n";
            exit(0);
        }
        if (index == 0) {
            static int temp = 0;
            temp = 0;
            return temp;
        }
        return regs[index];
    }
    int operator[](int index) const {
        if (index < 0 || index >= MAX_REG_COUNT) {
            std::cerr << "Invalid register access\n";
            exit(0);
        }
        if (index == 0) return 0;
        return regs[index];
    }
private:
    GPR_class() {
        memset(busy,-1,sizeof(busy));
    }
};
class DM_class {
public:
    DM_class (const DM_class&) = delete;
    DM_class &operator = (const DM_class&) = delete;
    static DM_class& get_instance_DM() {
        static DM_class obj;
        return obj;
    }
    int mem[PHY_STOP];
    int busy[PHY_STOP];
    int& operator[](int index) {
        if (index < 0 || index >= PHY_STOP) {
            std::cerr << "Invalid memory access\n";
            exit(0);
        }
        return mem[index];
    }
    int operator[](int index) const {
        if (index < 0 || index >= PHY_STOP) {
            std::cerr << "Invalid memory access\n";
            exit(0);
        }
        return mem[index];
    }
private:
    DM_class() {
        memset(busy,-1,sizeof(busy));
    }
};

struct Controller{
    int RegRead,AluSrc,AluOp,MemWrite,MemRead,RegWrite, Mem2Reg,Branch,Jump,AluSelect;
    Controller(int opcode){
        AluSelect = 0;
        switch(opcode) {
            case R_TYPE_OPCODE:
                RegRead = 1;
                RegWrite = 1;
                AluSrc = 0;
                MemRead = 0;
                MemWrite = 0;
                Mem2Reg = 0;
                Branch = 0;
                Jump = 0;
                AluOp = 2;
                break;
            case I_TYPE_OPCODE: 
                RegRead = 1;
                RegWrite = 1;
                AluSrc = 1;
                MemRead = 0;
                MemWrite = 0;
                Mem2Reg = 0;
                Branch = 0;
                Jump = 0;
                AluOp = 2;
                break;
            case L_TYPE_OPCODE:
                RegRead = 1;
                RegWrite = 1;
                AluSrc = 1;
                MemRead = 1;
                MemWrite = 0;
                Mem2Reg = 1;
                Branch = 0;
                Jump = 0;
                AluOp = 0;
                break;
            case S_TYPE_OPCODE:
                RegRead = 1;
                RegWrite = 0;
                AluSrc = 1;
                MemRead = 0;
                MemWrite = 1;
                Mem2Reg = 0;
                Branch = 0;
                Jump = 0;
                AluOp = 0;
                break;
            case B_TYPE_OPCODE:
                RegRead = 1;
                RegWrite = 0;
                AluSrc = 0;
                MemRead = 0;
                MemWrite = 0;
                Mem2Reg = 0;
                Branch = 1;
                Jump = 0;
                AluOp = 1;
                break;
            case JAL_TYPE_OPCODE:
                RegRead = 0;
                RegWrite = 1;
                AluSrc = 0;
                MemRead = 0;
                MemWrite = 0;
                Mem2Reg = 0;
                Branch = 0;
                Jump = 1;
                AluOp = 3;
                break;
            case JALR_TYPE_OPCODE:
                RegRead = 1;
                RegWrite = 1;
                AluSrc = 0;
                MemRead = 0;
                MemWrite = 0;
                Mem2Reg = 0;
                Branch = 0;
                Jump = 1;
                AluOp = 3;
                break;
            default:
                RegRead = 0;
                RegWrite = 0;
                AluSrc = 0;
                MemRead = 0;
                MemWrite = 0;
                Mem2Reg = 0;
                Branch = 0;
                Jump = 0;
                AluOp = 0;
                break;
        }
    }
};

struct _IFID{
    int ir = 0;
    int npc = 0;
    int dpc = 0;
    int stall =0;
    int valid=0;
};

struct _IDEX {
    int ir = 0;
    int npc = 0;
    int dpc = 0;
    int jpc = 0;
    int imm = 0;
    Controller cw{0};
    int rs1 = 0;
    int rs2 = 0;
    int rsl1=0;
    int rsl2=0;
    int func3 = 0;
    int func7 = 0;
    int rdl = 0;
    int stall=0;
    int valid=0;
};

struct _EXMO {
    int bpc = 0;
    int dpc = 0;
    int jpc = 0;
    Controller cw{0};
    int aluresult = 0;
    int rs2 = 0;
    int rdl = 0;
    int stall=0;
    int valid=0;
    int ir=0;
};
struct _MOWB {
    int bpc = 0;
    int dpc = 0;
    int jpc = 0;
    Controller cw{0};
    int aluresult = 0;
    int rdl = 0;
    int ldresult  = 0;
    int valid=0;
    int stall=0;
    int ir=0;
};

class Instruction{  
    DM_class& DM = DM_class::get_instance_DM();
    GPR_class& GPR = GPR_class::get_instance_GPR();
    std::vector<int> instructions;
    int pc;
    Controller cw = Controller(0); 
    _IFID IFID = _IFID();
    _IDEX IDEX = _IDEX();
    _EXMO EXMO = _EXMO();
    _MOWB MOWB = _MOWB();
    _IFID _IFID_ = _IFID();
    _IDEX _IDEX_ = _IDEX();
    _EXMO _EXMO_ = _EXMO();
    _MOWB _MOWB_ = _MOWB();
    Utils& ut=Utils::getInstance();
    int npc,jpc,rsl1,rsl2,rdl,opcode,func3,func7,imm,rs1,rs2,AluSrc1,AluSrc2,AluResult,AluZeroFlag,bpc,tpc,LDResult,AluResultNegative;
    void AluControl(struct _IDEX& IDEX_){
        Controller& cw = IDEX_.cw;
        int func3 = IDEX_.func3; 
        int func7 = IDEX_.func7;
        if(cw.AluOp==0){
            cw.AluSelect = 2;
        }
        if(cw.AluOp==1){
            if(func3==0){
                cw.AluSelect = 6; 
            }else if(func3==2){
                cw.AluSelect = 7;
            }else if(func3==4){
                cw.AluSelect = 8;
            }
        }
        if(cw.AluOp==2){ // r and i type
            if((func7>>5)&1){
                if(func3==0){
                    cw.AluSelect=6;
                }else if(func3==1){
                    cw.AluSelect = 10;
                }else if(func3==2){
                    cw.AluSelect = 3;
                }else{
                    std::cerr<<"Invalid func3 value for r type instruction\n";
                    return;
                }
            }else{
                if(func3==0){
                    cw.AluSelect=2;
                }else if(func3==1){
                    cw.AluSelect=12;
                }else if(func3==2){
                    cw.AluSelect = 11;
                }else if(func3==3){
                    cw.AluSelect = 0;   
                }else if(func3==4){
                    cw.AluSelect = 1;
                }else if(func3==5){
                    cw.AluSelect = 4;
                }else if(func3==6){
                    cw.AluSelect = 9;
                }else if(func3==7){
                    cw.AluSelect = 10;
                }
            }
        }  
    }
    int ALU(){
        auto AluSelect = IDEX.cw.AluSelect;
        std::cerr<<"AluSelect : "<<AluSelect<<"\n";
        if(AluSelect==0) return AluSrc1&AluSrc2;
        else if(AluSelect==1)return AluSrc1|AluSrc2;
        else if(AluSelect==2)return AluSrc1 + AluSrc2;
        else if(AluSelect==3)return AluSrc1 * AluSrc2;
        else if(AluSelect==4)return AluSrc1 ^ AluSrc2;
        else if(AluSelect==5) return AluSrc1 % AluSrc2;
        else if(AluSelect==6)return AluSrc2 - AluSrc1;
        else if(AluSelect==7) return (AluSrc1 < AluSrc2)?0:1;
        else if(AluSelect==8)return (AluSrc1>=AluSrc2)?0:1;
        else if(AluSelect==9)return (AluSrc1<<AluSrc2);
        else if(AluSelect==10)return (AluSrc1>>AluSrc2);
        else if(AluSelect==11){
            unsigned int a1 = (unsigned int)AluSrc1;
            unsigned int a2 = (unsigned int)AluSrc2;
            return (a1<a2)?0:1;
        }else if(AluSelect==12)return (AluSrc1<AluSrc2)?0:1;
        else{
            std::cerr<<"Invalid AluSelect\n";
            exit(0);
        }
    }
    int OutputSelect(struct _MOWB MOWB_){
        if(MOWB_.cw.Jump)return MOWB_.dpc;
        if(MOWB_.cw.Mem2Reg)return MOWB_.ldresult;
        return MOWB_.aluresult;
    }
    
    std::pair<int,bool> genImm(int instr,int opcode_){
        if(opcode_ == I_TYPE_OPCODE || opcode_ == JALR_TYPE_OPCODE){
            return {ut.get_bits_range(instr,31,20),true};
        }
        if(opcode_ == JAL_TYPE_OPCODE){
            return {ut.get_bits_range(instr,31,12),true};
        }
        if(opcode_ == B_TYPE_OPCODE || opcode_ == S_TYPE_OPCODE){
            auto x = ut.get_bits_range(instr, 31, 25);
            auto y = ut.get_bits_range(instr, 11, 7);
            return {y+(x<<5),true};
        }
        if (opcode_ == L_TYPE_OPCODE) {
            return {ut.get_bits_range(instr,31,20),true};
        }
        return {0, true};
    }
    std::pair<int,bool> genImm(){
        if(opcode == I_TYPE_OPCODE || opcode == JALR_TYPE_OPCODE){
            return {ut.get_bits_range(instructions[pc],31,20),true};
        }
        if(opcode == JAL_TYPE_OPCODE){
            return {ut.get_bits_range(instructions[pc],31,12),true};
        }
        if(opcode == B_TYPE_OPCODE || opcode == S_TYPE_OPCODE){
            auto x = ut.get_bits_range(instructions[pc], 31, 25);
            auto y = ut.get_bits_range(instructions[pc], 11, 7);
            return {y+(x<<5),true};
        }
        if (opcode == L_TYPE_OPCODE) {
            return {ut.get_bits_range(instructions[pc],31,20),true};
        }
        return {0, true};
    }
    
    void Instruction_Fetch() {
        if(pc>=instructions.size() || IFID.stall)return;
        _IFID_.ir = instructions[pc];
        npc = pc+1;
        _IFID_.npc = npc;
        _IFID_.valid = true;
        std::cerr<<"Instruction Fetch : "<<pc<<'\n';
        pc++;
    }
    bool Instruction_Decode() {
        if(IDEX.stall || !IFID.valid)return false;
        _IDEX_.jpc = IFID.npc-1 + (ut.get_bits_range(IFID.ir, 6, 0) == JAL_TYPE_OPCODE ? ut.get_bits_range(IFID.ir, 31, 12) : ut.get_bits_range(IFID.ir, 31, 20));
        rsl1 = ut.get_bits_range(IFID.ir,19,15);
        rsl2 = ut.get_bits_range(IFID.ir,24,20);
        _IDEX_.rsl2 = rsl2;
        _IDEX_.rsl1 = rsl1;
        _IDEX_.rdl = ut.get_bits_range(IFID.ir,11,7);
        _IDEX_.func7 = ut.get_bits_range(IFID.ir,31,25);
        _IDEX_.func3 = ut.get_bits_range(IFID.ir,14,12);
        opcode = ut.get_bits_range(IFID.ir, 6, 0);
        _IDEX_.imm = genImm(IFID.ir, opcode).first;
        _IDEX_.ir = IFID.ir;
        _IDEX_.cw = Controller{opcode};
        if (_IDEX_.cw.RegRead) {
            if(rsl1!=0 && GPR.busy[rsl1]!=-1){
                if(!((GPR.busy[rsl1]==IDEX.dpc-1 && ut.get_bits_range(IDEX.ir,6,0)!=L_TYPE_OPCODE) || (GPR.busy[rsl1]==EXMO.dpc-1))){
                    IFID.stall=1;
                    return false;                
                }
            }
            if(rsl2!=0 && GPR.busy[rsl2]!=-1){
                if(!((GPR.busy[rsl2]==IDEX.dpc-1 && ut.get_bits_range(IDEX.ir,6,0)!=L_TYPE_OPCODE) || (GPR.busy[rsl2]==EXMO.dpc-1))){
                    IFID.stall=1;
                    return false;                
                }
            }
            _IDEX_.rs1 = GPR[rsl1];
            _IDEX_.rs2 = GPR[rsl2];
        }
        if(_IDEX_.rdl && _IDEX_.cw.RegWrite){
            GPR.busy[_IDEX_.rdl] = IFID.npc-1;
        }
        _IDEX_.dpc = IFID.npc;
        _IDEX_.valid = 1;
        _IFID_.valid = 0;
        IFID.stall = false;
        std::cerr<<"Instruction Decode : "<<IFID.npc-1<<'\n';
        if(_IDEX_.cw.Jump){
            pc = _IDEX_.jpc;
            _IFID_ = _IFID();
            IFID = _IFID_;
            return true;
        }return false;
    }
    bool Instruction_Execution(){
        if(!IDEX.valid || EXMO.stall)return false;
        if(IDEX.cw.AluSrc){
            AluSrc2 = IDEX.imm;
        }else{
            AluSrc2 = AluSrc2_Forwarder();
            if(AluSrc2==INT_MIN){
                return false;
            }
        }
        AluSrc1 = AluSrc1_Forwarder();
        if(AluSrc1==INT_MIN){
            return false;
        }
        AluControl(IDEX);
        _EXMO_.ir = IDEX.ir;
        _EXMO_.aluresult = ALU();
        AluZeroFlag = _EXMO_.aluresult==0;
        _EXMO_.bpc = IDEX.dpc -1  + IDEX.imm;
        _EXMO_.cw = IDEX.cw;
        _EXMO_.cw.Branch = IDEX.cw.Branch && AluZeroFlag;
        _EXMO_.rdl = IDEX.rdl;
        _EXMO_.jpc = IDEX.jpc;
        _EXMO_.dpc = IDEX.dpc;
        _IDEX_.valid = 0;
        _EXMO_.rs2 = IDEX.rs2;
        _EXMO_.valid = 1;
        IDEX.stall = 0;
        std::cerr<<"Instruction Exectution : "<<IDEX.dpc - 1<<'\n';
        if(IDEX.cw.Branch && AluZeroFlag){
            pc = _EXMO_.bpc;
            _IFID_ = _IFID();
            IFID = _IFID_;
            _IDEX_ = _IDEX();
            IDEX = _IDEX_;
            return true;
        }
        return false;
    }
    void Memory_Operation(){
        if(!EXMO.valid || MOWB.stall)return;
        if(EXMO.cw.MemRead){
            if(EXMO.aluresult<0 || EXMO.aluresult>=PHY_STOP){
                std::cerr<<"Invalid Memory Access\n";
                return;
            }
            _MOWB_.ldresult = DM[EXMO.aluresult];
        }
        if(EXMO.cw.MemWrite){
            if(EXMO.aluresult>=PHY_STOP || EXMO.aluresult<0){
                std::cerr<<"Invalid Memory Access\n";
                return;
            }
            DM[EXMO.aluresult]=EXMO.rs2;
        }
        _MOWB_.cw = EXMO.cw;
        _MOWB_.jpc = EXMO.jpc;
        _MOWB_.dpc = EXMO.dpc;
        _MOWB_.bpc = EXMO.bpc;
        _MOWB_.rdl = EXMO.rdl;
        _MOWB_.ir = EXMO.ir;
        _MOWB_.aluresult = EXMO.aluresult;
        _MOWB_.valid = 1;
        _EXMO_.valid = 0;
        EXMO.stall = 0;
        std::cerr<<"Memory Operation : "<<EXMO.dpc - 1<<'\n';
    }
    void WriteBack(){
        if(!MOWB.valid)return;
        if(MOWB.cw.RegWrite){
            GPR[MOWB.rdl] = OutputSelect(MOWB);
        }
        tpc=npc;
        if(GPR.busy[MOWB.rdl]==MOWB.dpc-1){
            GPR.busy[MOWB.rdl]=-1;
        }
        _MOWB_.valid=0;
        MOWB.stall=false;
        std::cerr<<"Write Back : "<<MOWB.dpc - 1<<'\n';
    }
    int AluSrc1_Forwarder(){
        int as1; 
        if(IDEX.rsl1==0){
            return 0;
        }
        if(EXMO.rdl == IDEX.rsl1){
            as1 = EXMO.aluresult;
        }else if(MOWB.rdl==IDEX.rsl1){
            if(MOWB.cw.Mem2Reg){
                as1 = MOWB.ldresult;
            }else{
                as1 = MOWB.aluresult;
            }
        }else if(GPR.busy[IDEX.rsl1]!=-1 && GPR.busy[IDEX.rsl1]!=IDEX.dpc-1){
            IDEX.stall = 1;
            IFID.stall=1;
            return INT_MIN;
        }else{
            as1=IDEX.rs1;
        }return as1;
    }
    int AluSrc2_Forwarder(){
        int as2;
        if(IDEX.rsl2==0)return 0;
        if(EXMO.rdl == IDEX.rsl2){
            as2 = EXMO.aluresult;
        }else if(MOWB.rdl==IDEX.rsl2){
            if(MOWB.cw.Mem2Reg){
                as2 = MOWB.ldresult;
            }else{
                as2 = MOWB.aluresult;
            }
        }else if(GPR.busy[IDEX.rsl2]!=-1 && GPR.busy[IDEX.rsl2]!=IDEX.dpc-1){
            IDEX.stall = 1;
            IFID.stall=1;
            return INT_MIN;
        }else{
            as2=IDEX.rs2;
        }return as2;
    }
public:
    Instruction(std::vector<int> &ins){
        pc=0;
        instructions = ins;
        cw = Controller(0);
    };
    void executeInstructions(){
        WriteBack();
        Memory_Operation();
        auto is_branch_taken = Instruction_Execution();
        if(is_branch_taken){
            set_buffers();
            return;
        }
        auto is_jump_taken = Instruction_Decode();
        if(is_jump_taken){
            set_buffers();
            return;
        }
        Instruction_Fetch();
        set_buffers();
    }
    void set_buffers(){
        IFID = _IFID_;
        IDEX = _IDEX_;
        MOWB = _MOWB_;
        EXMO = _EXMO_;
    }
    bool check_valid_pc(){
        if(pc>=instructions.size() || pc<0)return false;
        return true;
    }
    bool is_pipeline_active(){
        return (MOWB.valid || EXMO.valid || IFID.valid || IDEX.valid);
    }
};
void print_reg_values() {
    GPR_class& GPR = GPR_class::get_instance_GPR();
    for (int i = 12; i <= 12; i++) {
        std::cout << i << ": " << GPR[i] << "\n";
    }
}
void print_dm_values(){
    DM_class& DM = DM_class::get_instance_DM();
    for (int i = 12; i <= 12; i++) {
        std::cout << i << ": " << DM[i] << "\n";
    }
}
int main(){
    Utils& ut = Utils::getInstance();
    std::vector<int> inputs = ut.read_object_file("output.txt");
    Instruction i(inputs);
    int cycle_count = 0;
    do{
        i.executeInstructions();
        print_reg_values();
        print_dm_values();
        cycle_count++;
        std::cout << "\n";
    }while(i.check_valid_pc() || i.is_pipeline_active());
    std::cout<<"Cycle count : "<<cycle_count<<'\n';
    return 0;
}