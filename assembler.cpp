#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

std::unordered_map<std::string, std::string> registers;
enum instruction_type { r, i, l, s, b, j, n };

std::unordered_map<std::string, instruction_type> instruction_to_type;
std::unordered_map<std::string, std::string> instruction_to_binary;

class Utils {
public:
  Utils(const Utils &) = delete;
  Utils &operator=(const Utils &) = delete; // underloading = operator
  static Utils &getInstance() {
    static Utils obj;
    return obj;
  }
  void initialize_register_maps() {
    for (int i = 0; i <= 31; i++) {
      std::string s = "x" + std::to_string(i);
      registers[s] = decimal_to_binary(i).substr(27);
      // std::cout<<registers[s]<<"\n";
    }
    registers["Zero"] = registers["x0"];
    registers["ra"] = registers["x1"];
    registers["sp"] = registers["x2"];
    registers["gp"] = registers["x3"];
    registers["tp"] = registers["x4"];
    registers["t0"] = registers["x5"];
    registers["t1"] = registers["x6"];
    registers["t2"] = registers["x7"];
    registers["s0"] = registers["fp"] = registers["x8"];
    registers["s1"] = registers["x9"];
    registers["a0"] = registers["x10"];
    registers["a1"] = registers["x11"];
    registers["a2"] = registers["x12"];
    registers["a3"] = registers["x13"];
    registers["a4"] = registers["x14"];
    registers["a5"] = registers["x15"];
    registers["a6"] = registers["x16"];
    registers["a7"] = registers["x17"];
    registers["s2"] = registers["x18"];
    registers["s3"] = registers["x19"];
    registers["s4"] = registers["x20"];
    registers["s5"] = registers["x21"];
    registers["s6"] = registers["x22"];
    registers["s7"] = registers["x23"];
    registers["s8"] = registers["x24"];
    registers["s9"] = registers["x25"];
    registers["s10"] = registers["x26"];
    registers["s11"] = registers["x27"];
    registers["t3"] = registers["x28"];
    registers["t4"] = registers["x29"];
    registers["t5"] = registers["x30"];
    registers["t6"] = registers["x31"];

    instruction_to_type["ADD"] = r;
    instruction_to_type["SLT"] = r;
    instruction_to_type["SLTU"] = r;
    instruction_to_type["AND"] = r;
    instruction_to_type["OR"] = r;
    instruction_to_type["XOR"] = r;
    instruction_to_type["SLL"] = r;
    instruction_to_type["SRL"] = r;
    instruction_to_type["SUB"] = r;
    instruction_to_type["SRA"] = r;
    instruction_to_type["MUL"] = r;
    instruction_to_type["NOP"] = n;
    instruction_to_type["LW"] = l;
    instruction_to_type["LD"] = l;
    instruction_to_type["LH"] = l;
    instruction_to_type["LB"] = l;
    instruction_to_type["LWU"] = l;
    instruction_to_type["LHU"] = l;
    instruction_to_type["LBU"] = l;
    instruction_to_type["SD"] = s;
    instruction_to_type["SW"] = s;
    instruction_to_type["SHW"] = s;
    instruction_to_type["SB"] = s;
    instruction_to_type["ADDI"] = i;
    instruction_to_type["SLTI"] = i;
    instruction_to_type["SLTU"] = i;
    instruction_to_type["ANDI"] = i;
    instruction_to_type["ORI"] = i;
    instruction_to_type["XORI"] = i;
    instruction_to_type["SLLI"] = i;
    instruction_to_type["SRLI"] = i;
    instruction_to_type["SRAI"] = i;
    instruction_to_type["BEQ"] = b;
    instruction_to_type["BNE"] = b;
    instruction_to_type["BLT"] = b;
    instruction_to_type["BGT"] = b;
    instruction_to_type["BLTU"] = b;
    instruction_to_type["BGE"] = b;
    instruction_to_type["JAL"] = j;
    instruction_to_type["JALR"] = j;
    instruction_to_type["J"] = j;
    instruction_to_type["JR"] = j;

    instruction_to_binary["ADD"] = decimal_to_binary(0).substr(22);
    instruction_to_binary["SLT"] = decimal_to_binary(1).substr(22);
    instruction_to_binary["SLTU"] = decimal_to_binary(2).substr(22);
    instruction_to_binary["AND"] = decimal_to_binary(3).substr(22);
    instruction_to_binary["OR"] = decimal_to_binary(4).substr(22);
    instruction_to_binary["XOR"] = decimal_to_binary(5).substr(22);
    instruction_to_binary["SLL"] = decimal_to_binary(6).substr(22);
    instruction_to_binary["SRL"] = decimal_to_binary(7).substr(22);
    instruction_to_binary["SUB"] = decimal_to_binary(256).substr(22);
    instruction_to_binary["SRA"] = decimal_to_binary(257).substr(22);
    instruction_to_binary["MUL"] = decimal_to_binary(258).substr(22);
    instruction_to_binary["LW"] = decimal_to_binary(0).substr(29);
    instruction_to_binary["LD"] = decimal_to_binary(1).substr(29);
    instruction_to_binary["LH"] = decimal_to_binary(2).substr(29);
    instruction_to_binary["LB"] = decimal_to_binary(3).substr(29);
    instruction_to_binary["LWU"] = decimal_to_binary(4).substr(29);
    instruction_to_binary["LHU"] = decimal_to_binary(5).substr(29);
    instruction_to_binary["LBU"] = decimal_to_binary(6).substr(29);
    instruction_to_binary["SD"] = decimal_to_binary(3).substr(22);
    instruction_to_binary["SW"] = decimal_to_binary(2).substr(22);
    instruction_to_binary["SHW"] = decimal_to_binary(1).substr(22);
    instruction_to_binary["SB"] = decimal_to_binary(0).substr(22);
    instruction_to_binary["ADDI"] = decimal_to_binary(0).substr(17);
    instruction_to_binary["SLTI"] = decimal_to_binary(1).substr(17);
    instruction_to_binary["SLTU"] = decimal_to_binary(2).substr(17);
    instruction_to_binary["ANDI"] = decimal_to_binary(3).substr(17);
    instruction_to_binary["ORI"] = decimal_to_binary(4).substr(17);
    instruction_to_binary["XORI"] = decimal_to_binary(5).substr(17);
    instruction_to_binary["SLLI"] = decimal_to_binary(0).substr(17);
    instruction_to_binary["SRLI"] = decimal_to_binary(1).substr(17);
    instruction_to_binary["SRAI"] = decimal_to_binary(1 << 13).substr(17);
    instruction_to_binary["BEQ"] = decimal_to_binary(0).substr(29);
    instruction_to_binary["BNE"] = decimal_to_binary(1).substr(29);
    instruction_to_binary["BLT"] = decimal_to_binary(2).substr(29);
    instruction_to_binary["BLTU"] = decimal_to_binary(3).substr(29);
    instruction_to_binary["BGE"] = decimal_to_binary(4).substr(29);
    instruction_to_binary["BGEU"] = decimal_to_binary(5).substr(29);
    instruction_to_binary["BGT"] = decimal_to_binary(6).substr(29);
    instruction_to_binary["JALR"] = "0";
    instruction_to_binary["JR"] = "000";
  }
  std::string decimal_to_binary(int n) {
    std::string binary;
    bool is_negative = false;
    if (n < 0)
      is_negative = true;
    n = abs(n);
    while (n > 0) {
      if (n % 2) {
        binary.push_back('1');
      } else {
        binary.push_back('0');
      }
      n /= 2;
    }
    while (binary.size() < 32) {
      binary.push_back('0');
    }
    reverse(binary.begin(), binary.end());
    if (is_negative) {
      flip(binary);
      add_one(binary);
    }
    return binary;
  }
  std::vector<std::string> tokenizer(std::string &instruction) {
    std::vector<std::string> tokens;
    std::string s;
    for (auto &it : instruction) {
      if (it == ' ' || it == ',') {
        if (s.size()) {
          tokens.push_back(s);
          s = "";
        }
      } else if (it != ' ' && it != ',') {
        s.push_back(it);
      }
    }
    if (s.size())
      tokens.push_back(s);
    auto temp = s;
    bool is_present = false;
    for (auto &it : temp) {
      if (it == '(') {
        is_present = true;
        break;
      }
    }
    if (is_present) {
      std::string s1, s2;
      {
        int ind = 0;
        while (temp[ind] != '(') {
          s1.push_back(temp[ind++]);
        }
        ind++;
        while (temp[ind] != ')') {
          s2.push_back(temp[ind++]);
        }
      }
      tokens.pop_back();
      tokens.push_back(s2);
      tokens.push_back(s1);
      // std::cout<<tokens[tokens.size()-1]<<"\n";
    }
    // for(auto &it:tokens){
    //     std::cout<<it<<" ";
    // }
    // std::cout<<std::endl;
    // std::cout<<tokens.size()<<std::endl;
    return tokens;
  }

private:
  Utils() {}
  static void flip(std::string &s) {
    for (char &ch : s) {
      if (ch == '1')
        ch = '0';
      else
        ch = '1';
    }
  }
  static void add_one(std::string &s) {
    int n = s.size();
    if (s.back() == '0') {
      s[n - 1] = '1';
      return;
    }
    for (int i = n - 1; i >= 0; i--) {
      if (s[i] == '1') {
        s[i] = '0';
      } else {
        s[i] = '1';
        break;
      }
    }
    return;
  }
};
class R_Type {
public:
  std::vector<std::string> tokens;
  Utils &ut = Utils::getInstance();
  R_Type(const R_Type &) = delete;
  R_Type &operator=(const R_Type &) = delete; // underloading = operator
  static R_Type &getInstance_r(std::vector<std::string> &tokens) {
    static R_Type obj(tokens);
    return obj;
  }
  void assign(std::vector<std::string> &tok) { tokens = tok; }
  std::string convert_tokens_to_binary() {
    std::string ans = "";
    std::string func = instruction_to_binary[tokens[0]];
    std::string func7 = func.substr(0, 7);
    std::string func3 = func.substr(7);
    std::string opcode = ut.decimal_to_binary(51).substr(25);
    std::string rs2 = registers[tokens[2]];
    std::string rs1 = registers[tokens[3]];
    std::string rd = registers[tokens[1]];
    ans = func7 + " " + rs2 + " " + rs1 + " " + func3 + " " + rd + " " + opcode;
    return ans;
  }

private:
  R_Type(std::vector<std::string> &tokens) { this->tokens = tokens; }
};
class L_Type {
public:
  std::vector<std::string> tokens;
  Utils &ut = Utils::getInstance();
  L_Type(const L_Type &) = delete;
  L_Type &operator=(const L_Type &) = delete; // underloading = operator
  static L_Type &getInstance_l(std::vector<std::string> &tokens) {
    static L_Type obj(tokens);
    return obj;
  }
  void assign(std::vector<std::string> &tok) { tokens = tok; }
  std::string convert_tokens_to_binary() {
    std::string ans = "";
    std::string func3 = instruction_to_binary[tokens[0]];
    std::string opcode = ut.decimal_to_binary(3).substr(25);
    std::string rs1 = registers[tokens[2]];
    std::string rd = registers[tokens[1]];
    std::string imm = ut.decimal_to_binary(stoi(tokens[3])).substr(20);
    ans = imm + " " + rs1 + " " + func3 + " " + rd + " " + opcode;
    return ans;
  }

private:
  L_Type(std::vector<std::string> &tokens) { this->tokens = tokens; }
};
class S_Type {
public:
  std::vector<std::string> tokens;
  Utils &ut = Utils::getInstance();
  S_Type(const S_Type &) = delete;
  S_Type &operator=(const S_Type &) = delete; // underloading = operator
  static S_Type &getInstance_s(std::vector<std::string> &tokens) {
    static S_Type obj(tokens);
    return obj;
  }
  void assign(std::vector<std::string> &tok) { tokens = tok; }
  std::string convert_tokens_to_binary() {
    std::string ans = "";
    std::string func3 = instruction_to_binary[tokens[0]].substr(7);
    std::string opcode = ut.decimal_to_binary(35).substr(25);
    std::string rs1 = registers[tokens[2]];
    std::string rs2 = registers[tokens[1]];
    std::string imm = ut.decimal_to_binary(stoi(tokens[3])).substr(20);
    std::string imm1 = imm.substr(0, 7);
    std::string imm2 = imm.substr(7);
    ans =
        imm1 + " " + rs2 + " " + rs1 + " " + func3 + " " + imm2 + " " + opcode;
    return ans;
  }

private:
  S_Type(std::vector<std::string> &tokens) { this->tokens = tokens; }
};

class I_Type {
public:
  std::vector<std::string> tokens;
  Utils &ut = Utils::getInstance();
  I_Type(const I_Type &) = delete;
  I_Type &operator=(const I_Type &) = delete; // underloading = operator
  static I_Type &getInstance_i(std::vector<std::string> &tokens) {
    static I_Type obj(tokens);
    return obj;
  }

  void assign(std::vector<std::string> &tok) { tokens = tok; }

  std::string convert_tokens_to_binary() {
    std::string ans = "";
    if (tokens[0] == "SLLI" || tokens[0] == "SRLI" || tokens[0] == "SRAI") {
      std::string imm = ut.decimal_to_binary(stoi(tokens[3])).substr(25);
      std::string imm2 = ut.decimal_to_binary(stoi(tokens[3])).substr(27);
      std::string rs1 = registers[tokens[2]];
      std::string func3 = instruction_to_binary[tokens[0]].substr(12);
      std::string rd = registers[tokens[1]];
      std::string opcode = "0010011";
      ans =
          imm + " " + imm2 + " " + rs1 + " " + func3 + " " + rd + " " + opcode;
    } else {
      std::string imm = ut.decimal_to_binary(stoi(tokens[3])).substr(20);
      std::string rs1 = registers[tokens[2]];
      std::string func3 = instruction_to_binary[tokens[0]].substr(12);
      std::string rd = registers[tokens[1]];
      std::string opcode = "0010011";
      ans = imm + " " + rs1 + " " + func3 + " " + rd + " " + opcode;
    }
    return ans;
  }

private:
  I_Type(std::vector<std::string> &tokens) { this->tokens = tokens; }
};

std::unordered_map<std::string, int> labels;

class B_Type {
public:
  std::vector<std::string> tokens;
  int line_number;
  Utils &ut = Utils::getInstance();
  B_Type(const B_Type &) = delete;
  B_Type &operator=(const B_Type &) = delete; // underloading = operator
  static B_Type &getInstance_b(std::vector<std::string> &tokens, int i) {
    static B_Type obj(tokens, i);
    return obj;
  }

  void assign(std::vector<std::string> &tok, int i) {
    tokens = tok;
    line_number = i;
  }

  std::string convert_tokens_to_binary() {
    std::string ans = "";
    int label_line = labels[tokens[3]];
    std::string offset =
        ut.decimal_to_binary(label_line - line_number).substr(20);
    std::string imm1 = offset.substr(0, 7);
    std::string imm2 = offset.substr(7);
    std::string rs2 = registers[tokens[2]];
    std::string rs1 = registers[tokens[1]];
    std::string func3 = instruction_to_binary[tokens[0]];
    std::string opcode = "1100011";
    ans =
        imm1 + " " + rs2 + " " + rs1 + " " + func3 + " " + imm2 + " " + opcode;
    return ans;
  }

private:
  B_Type(std::vector<std::string> &tokens, int i) {
    this->tokens = tokens;
    line_number = i;
  }
};

class J_Type {
public:
  std::vector<std::string> tokens;
  int line_number;
  Utils &ut = Utils::getInstance();
  J_Type(const J_Type &) = delete;
  J_Type &operator=(const J_Type &) = delete; // underloading = operator
  static J_Type &getInstance_j(std::vector<std::string> &tokens, int i) {
    static J_Type obj(tokens, i);
    return obj;
  }

  void assign(std::vector<std::string> &tok, int i) {
    tokens = tok;
    line_number = i;
  }

  std::string convert_tokens_to_binary() {
    std::string ans = "";
    if (tokens[0] == "JAL") {
      std::string offset =
          ut.decimal_to_binary(labels[tokens[2]] - line_number).substr(12);
      std::string rd = registers[tokens[1]];
      std::string opcode = "1101111";
      ans = offset + " " + rd + " " + opcode;
    } else if (tokens[0] == "JALR") {
      std::string imm = ut.decimal_to_binary(stoi(tokens[3])).substr(20);
      std::string rs1 = registers[tokens[2]];
      std::string rd = registers[tokens[1]];
      std::string opcode = "1100111";
      ans = imm + " " + rs1 + " 000 " + rd + " " + opcode;
    } else if (tokens[0] == "J") {
      int label_line = labels[tokens[1]];
      std::string offset =
          ut.decimal_to_binary(label_line - line_number).substr(12);
      std::string rd = "00000";
      std::string opcode = "1101111";
      ans = offset + " " + rd + " " + opcode;
    } else if (tokens[0] == "JR") {
      std::string rs1 = registers[tokens[1]];
      ans = "000000000000 " + rs1 + " 000 00000 1101111";
    }
    return ans;
  }

private:
  J_Type(std::vector<std::string> &tokens, int i) {
    this->tokens = tokens;
    line_number = i;
  }
};

class Instruction {
public:
  std::string instruction;
  std::vector<std::string> tokens;
  std::string binary_instruction;
  int instruction_type;
  Utils &ut = Utils::getInstance();
  Instruction() {}
  std::string handle_instructions(std::string &instruction, int i) {
    tokens = ut.tokenizer(instruction);
    auto type = instruction_to_type[tokens[0]];
    if (type == (instruction_type::r)) {
      R_Type &r_obj = R_Type::getInstance_r(tokens);
      r_obj.assign(tokens);
      return r_obj.convert_tokens_to_binary();
    } else if (type == (instruction_type::n)) {
      return "00000000000000000000000000010011";
    } else if (type == (instruction_type::l)) {
      L_Type &l_obj = L_Type::getInstance_l(tokens);
      l_obj.assign(tokens);
      return l_obj.convert_tokens_to_binary();
    } else if (type == (instruction_type::s)) {
      S_Type &s_obj = S_Type::getInstance_s(tokens);
      s_obj.assign(tokens);
      return s_obj.convert_tokens_to_binary();
    } else if (type == (instruction_type::i)) {
      I_Type &i_obj = I_Type::getInstance_i(tokens);
      i_obj.assign(tokens);
      return i_obj.convert_tokens_to_binary();
    } else if (type == (instruction_type::b)) {
      B_Type &b_obj = B_Type::getInstance_b(tokens, i);
      b_obj.assign(tokens, i);
      return b_obj.convert_tokens_to_binary();
    } else if (type == (instruction_type::j)) {
      J_Type &j_obj = J_Type::getInstance_j(tokens, i);
      j_obj.assign(tokens, i);
      return j_obj.convert_tokens_to_binary();
    }
    return "";
  }
};

int main() {
  Utils &ut = Utils::getInstance();
  ut.initialize_register_maps();
  freopen("input.s", "r", stdin);
  freopen("output.txt", "w", stdout);
  std::vector<std::string> inputs;
  std::string line = "";
  while (std::getline(std::cin, line)) {
    while (!line.empty() && (line.back() == '\r' || line.back() == '\n' ||
                             line.back() == '\t')) {
      line.pop_back();
    }
    if (line.empty() || line[0] == '#')
      continue;
    if (line.back() == ':') {
      labels[line.substr(0, line.size() - 1)] = inputs.size();
    } else {
      inputs.push_back(line);
    }
  }
  Instruction instr;
  for (int i = 0; i < inputs.size(); i++) {
    if (inputs[i].back() != ':')
      std::cout << instr.handle_instructions(inputs[i], i) << "\n";
  }
  return 0;
}
