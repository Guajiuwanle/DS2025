#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstddef>
#include <cctype>
#include <map>
using namespace std;

// 位图类 Bitmap
class Bitmap {
private:
    unsigned char* M;
    size_t N;       // 字节数组长度
    size_t _sz;     // 有效位个数

public:
    typedef size_t Rank;

    // 初始化
    void init(Rank n) {
        N = (n + 7) / 8;
        M = new unsigned char[N];
        memset(M, 0, N);
        _sz = 0;
    }

    // 构造函数
    Bitmap(Rank n = 8) { init(n); }
    
    // 拷贝构造函数
    Bitmap(const Bitmap& other) {
        init(other.N * 8);
        memcpy(M, other.M, N);
        _sz = other._sz;
    }
    
    // 赋值运算符
    Bitmap& operator=(const Bitmap& other) {
        if (this != &other) {
            delete[] M;
            init(other.N * 8);
            memcpy(M, other.M, N);
            _sz = other._sz;
        }
        return *this;
    }
    
    // 析构函数
    ~Bitmap() {
        delete[] M;
        M = NULL;
        _sz = 0;
    }

    // 返回有效位个数
    Rank size() const { return _sz; }

    // 设置第k位
    void set(Rank k) {
        expand(k);
        if (!test(k)) {
            _sz++;
            M[k >> 3] |= (0x80 >> (k & 0x07));
        }
    }

    // 清除第k位
    void clear(Rank k) {
        expand(k);
        if (test(k)) {
            _sz--;
            M[k >> 3] &= ~(0x80 >> (k & 0x07));
        }
    }

    // 测试第k位
    bool test(Rank k) const {
        if (k >= 8 * N) return false;
        return (M[k >> 3] & (0x80 >> (k & 0x07))) != 0;
    }

    // 转换为字符串
    char* bits2string(Rank n) const {
        char* s = new char[n + 1];
        s[n] = '\0';
        for (Rank i = 0; i < n; i++) {
            s[i] = test(i) ? '1' : '0';
        }
        return s;
    }

    // 扩容
    void expand(Rank k) {
        if (k < 8 * N) return;
        
        Rank newN = (k + 8) / 8;
        unsigned char* newM = new unsigned char[newN];
        memset(newM, 0, newN);
        
        if (M) {
            memcpy(newM, M, N);
            delete[] M;
        }
        
        M = newM;
        N = newN;
    }
};

// 二叉树节点
template <typename T>
struct BinNode {
    T data;
    BinNode<T>* parent;
    BinNode<T>* lc;  // 左孩子
    BinNode<T>* rc;  // 右孩子
    
    BinNode() : parent(NULL), lc(NULL), rc(NULL) {}
    BinNode(T e, BinNode<T>* p = NULL, BinNode<T>* l = NULL, BinNode<T>* r = NULL)
        : data(e), parent(p), lc(l), rc(r) {}
    
    // 判断是否为叶子
    bool isLeaf() const { return !lc && !rc; }
};

// 二叉树类 BinTree
template <typename T>
class BinTree {
protected:
    BinNode<T>* _root;
    size_t _size;

    // 递归删除子树
    void removeAt(BinNode<T>* x) {
        if (!x) return;
        removeAt(x->lc);
        removeAt(x->rc);
        delete x;
        _size--;
    }

public:
    typedef size_t Rank;

    BinTree() : _root(NULL), _size(0) {}
    BinTree(BinNode<T>* root) : _root(root), _size(1) {}
    
    ~BinTree() {
        if (_size > 0) removeAt(_root);
    }

    BinNode<T>* root() const { return _root; }
    Rank size() const { return _size; }
    bool empty() const { return !_root; }
};

// Huffman节点数据
struct HuffChar {
    char ch;
    int weight;
    
    HuffChar(char c = '^', int w = 0) : ch(c), weight(w) {}
};

// 用于优先队列的比较器
struct HuffNodeCompare {
    bool operator()(BinNode<HuffChar>* a, BinNode<HuffChar>* b) const {
        return a->data.weight > b->data.weight;
    }
};

// Huffman树，继承自BinTree
class HuffTree : public BinTree<HuffChar> {
public:
    // 根据频率表构建Huffman树
    HuffTree(const vector<pair<char, int> >& freqTable) : BinTree<HuffChar>() {
        // 使用优先队列（小顶堆）
        priority_queue<BinNode<HuffChar>*, vector<BinNode<HuffChar>*>, HuffNodeCompare> minHeap;

        // 创建叶子节点
        for (size_t i = 0; i < freqTable.size(); i++) {
            const pair<char, int>& item = freqTable[i];
            if (item.second > 0) {
                BinNode<HuffChar>* leaf = new BinNode<HuffChar>(HuffChar(item.first, item.second));
                minHeap.push(leaf);
                _size++;
            }
        }

        // 如果只有一个节点，直接返回
        if (minHeap.size() == 1) {
            _root = minHeap.top();
            return;
        }

        // 合并构建Huffman树
        while (minHeap.size() > 1) {
            BinNode<HuffChar>* x = minHeap.top(); minHeap.pop();
            BinNode<HuffChar>* y = minHeap.top(); minHeap.pop();
            
            // 创建合并节点
            BinNode<HuffChar>* parent = new BinNode<HuffChar>(
                HuffChar('^', x->data.weight + y->data.weight)
            );
            parent->lc = x; 
            parent->rc = y;
            x->parent = parent;
            y->parent = parent;
            
            minHeap.push(parent);
            _size++;
        }
        
        // 设置根节点
        if (!minHeap.empty()) {
            _root = minHeap.top();
        }
    }
};

// Huffman编码类，基于Bitmap
class HuffCode {
private:
    Bitmap _code;
    size_t _length;

public:
    typedef size_t Rank;

    HuffCode() : _length(0) {}
    
    // 拷贝构造函数
    HuffCode(const HuffCode& other) : _code(other._code), _length(other._length) {}
    
    // 赋值运算符
    HuffCode& operator=(const HuffCode& other) {
        if (this != &other) {
            _code = other._code;
            _length = other._length;
        }
        return *this;
    }
    
    // 添加一个比特
    void append(bool bit) {
        if (bit) {
            _code.set(_length);
        } else {
            _code.clear(_length);
        }
        _length++;
    }
    
    // 获取长度
    Rank size() const { return _length; }
    
    // 转换为字符串
    string toString() const {
        char* str = _code.bits2string(_length);
        string result(str);
        delete[] str;
        return result;
    }
};

// Huffman编码算法实现
class Huffman {
private:
    HuffTree* _tree;
    map<char, HuffCode> _codeMap;

    // 递归生成编码
    void _generateCodes(BinNode<HuffChar>* node, HuffCode code) {
        if (!node) return;
        
        if (node->isLeaf()) {
            _codeMap[node->data.ch] = code;
            return;
        }
        
        if (node->lc) {
            HuffCode leftCode = code;
            leftCode.append(false);  // 左分支为0
            _generateCodes(node->lc, leftCode);
        }
        
        if (node->rc) {
            HuffCode rightCode = code;
            rightCode.append(true);   // 右分支为1
            _generateCodes(node->rc, rightCode);
        }
    }

public:
    Huffman(const string& text) : _tree(NULL) {
        // 统计26个字母频率（不分大小写）
        vector<pair<char, int> > freqTable(26);
        for (int i = 0; i < 26; i++) {
            freqTable[i] = make_pair('a' + i, 0);
        }
        
        // 统计频率
        for (size_t i = 0; i < text.size(); i++) {
            char c = text[i];
            if (isalpha(c)) {
                char lowerC = tolower(c);
                freqTable[lowerC - 'a'].second++;
            }
        }
        
        // 构建Huffman树
        _tree = new HuffTree(freqTable);
        
        // 生成编码表 - 使用递归方法
        if (_tree->root()) {
            _generateCodes(_tree->root(), HuffCode());
        }
    }
    
    ~Huffman() {
        delete _tree;
    }
    
    // 编码单个字符
    string encode(char c) const {
        if (!isalpha(c)) return "";
        char lowerC = tolower(c);
        map<char, HuffCode>::const_iterator it = _codeMap.find(lowerC);
        if (it != _codeMap.end()) {
            return it->second.toString();
        }
        return "";
    }
    
    // 编码单词
    string encodeWord(const string& word) const {
        string result;
        for (size_t i = 0; i < word.size(); i++) {
            char c = word[i];
            if (isalpha(c)) {
                result += encode(c);
            }
        }
        return result;
    }
    
    // 打印编码表
    void printCodeTable() const {
        cout << "Huffman编码表：" << endl;
        for (char c = 'a'; c <= 'z'; c++) {
            map<char, HuffCode>::const_iterator it = _codeMap.find(c);
            if (it != _codeMap.end()) {
                cout << "'" << c << "' : " << it->second.toString() << endl;
            }
        }
    }
};

// 简化测试文本
const string I_HAVE_A_DREAM_TEXT = 
    "i have a dream that one day this nation will rise up";

int main() {
    cout << "=== Huffman编码实现 ===" << endl;
    cout << "文本: Martin Luther King《I Have a Dream》片段" << endl << endl;
    
    try {
        // 创建Huffman编码器
        Huffman huffman(I_HAVE_A_DREAM_TEXT);
        
        // 打印编码表
        huffman.printCodeTable();
        
        cout << "\n=== 单词编码测试 ===" << endl;
        vector<string> testWords;
        testWords.push_back("dream");
        testWords.push_back("nation");
        testWords.push_back("rise");
        for (size_t i = 0; i < testWords.size(); i++) {
            string code = huffman.encodeWord(testWords[i]);
            cout << testWords[i] << " : " << code << endl;
        }
    } catch (const exception& e) {
        cerr << "错误: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
