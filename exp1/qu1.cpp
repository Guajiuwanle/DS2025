#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <cstdlib>

using namespace std;

class Complex {
private:
    double real;   // 实部
    double imag;   // 虚部
public:
    Complex(double r = 0, double i = 0) : real(r), imag(i) {}

    double getReal() const { return real; }
    double getImag() const { return imag; }
    double getMod() const { return sqrt(real * real + imag * imag); }

    bool operator==(const Complex& c) const {
        return fabs(real - c.real) < 1e-6 && fabs(imag - c.imag) < 1e-6;
    }

    friend ostream& operator<<(ostream& os, const Complex& c);
};

ostream& operator<<(ostream& os, const Complex& c) {
    os << c.real << (c.imag >= 0 ? "+" : "") << c.imag << "i";
    return os;
}

bool cmpComplex(const Complex& a, const Complex& b) {
    if (fabs(a.getMod() - b.getMod()) > 1e-6) {
        return a.getMod() < b.getMod();
    }
    return a.getReal() < b.getReal();
}

vector<Complex> generateRandomComplexVec(int size = 20) {
    vector<Complex> vec;
    srand(time(0));
    for (int j = 0; j < size; ++j) {  
        double r = rand() % 21 - 10;
        double i = rand() % 21 - 10;
        vec.push_back(Complex(r, i));
    }
    return vec;
}

void shuffleVec(vector<Complex>& vec) {
    random_shuffle(vec.begin(), vec.end());
}

int findComplex(const vector<Complex>& vec, const Complex& target) {
    for (size_t i = 0; i < vec.size(); ++i) {
        if (vec[i] == target) return (int)i;
    }
    return -1;
}

void insertComplex(vector<Complex>& vec, int idx, const Complex& c) {
    if (idx >= 0 && idx <= (int)vec.size()) {
        vec.insert(vec.begin() + idx, c);
    }
}

void deleteComplex(vector<Complex>& vec, int idx) {
    if (idx >= 0 && idx < (int)vec.size()) {
        vec.erase(vec.begin() + idx);
    }
}

void uniqueVec(vector<Complex>& vec) {
    vector<Complex> temp;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (findComplex(temp, vec[i]) == -1) {
            temp.push_back(vec[i]);
        }
    }
    vec = temp;
}

void bubbleSort(vector<Complex>& vec) {
    int n = vec.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - 1 - i; ++j) {
            if (!cmpComplex(vec[j], vec[j + 1])) {
                swap(vec[j], vec[j + 1]);
            }
        }
    }
}

void merge(vector<Complex>& vec, int l, int mid, int r) {
    vector<Complex> temp(r - l + 1);
    int i = l, j = mid + 1, k = 0;
    while (i <= mid && j <= r) {
        temp[k++] = cmpComplex(vec[i], vec[j]) ? vec[i++] : vec[j++];
    }
    while (i <= mid) temp[k++] = vec[i++];
    while (j <= r) temp[k++] = vec[j++];
    for (int p = 0; p < (int)temp.size(); ++p) {
        vec[l + p] = temp[p];
    }
}

void mergeSort(vector<Complex>& vec, int l, int r) {
    if (l >= r) return;
    int mid = (l + r) / 2;
    mergeSort(vec, l, mid);
    mergeSort(vec, mid + 1, r);
    merge(vec, l, mid, r);
}

vector<Complex> rangeFind(const vector<Complex>& vec, double m1, double m2) {
    vector<Complex> res;
    for (size_t i = 0; i < vec.size(); ++i) {
        double mod = vec[i].getMod();
        if (mod >= m1 && mod < m2) {
            res.push_back(vec[i]);
        }
    }
    return res;
}

void testSortEfficiency() {
    int size = 10000;
    vector<Complex> vec = generateRandomComplexVec(size);
    vector<Complex> vec1 = vec, vec2 = vec, vec3 = vec;

    sort(vec1.begin(), vec1.end(), cmpComplex);
    vector<Complex> vecReverse;
    for (size_t i = 0; i < vec1.size(); ++i) {
        vecReverse.push_back(vec1[vec1.size() - 1 - i]);
    }

    clock_t start, end;
    double bubbleTime, mergeTime;

    start = clock();
    bubbleSort(vec2);
    end = clock();
    bubbleTime = (double)(end - start) / CLOCKS_PER_SEC;

    start = clock();
    mergeSort(vec3, 0, vec3.size() - 1);
    end = clock();
    mergeTime = (double)(end - start) / CLOCKS_PER_SEC;

    cout << "乱序序列（size=" << size << "）：" << endl;
    cout << "起泡排序耗时：" << bubbleTime << "s" << endl;
    cout << "归并排序耗时：" << mergeTime << "s" << endl;

    vec2 = vec1; vec3 = vec1;
    start = clock();
    bubbleSort(vec2);
    end = clock();
    bubbleTime = (double)(end - start) / CLOCKS_PER_SEC;

    start = clock();
    mergeSort(vec3, 0, vec3.size() - 1);
    end = clock();
    mergeTime = (double)(end - start) / CLOCKS_PER_SEC;

    cout << "\n顺序序列（size=" << size << "）：" << endl;
    cout << "起泡排序耗时：" << bubbleTime << "s" << endl;
    cout << "归并排序耗时：" << mergeTime << "s" << endl;

    vec2 = vecReverse; vec3 = vecReverse;
    start = clock();
    bubbleSort(vec2);
    end = clock();
    bubbleTime = (double)(end - start) / CLOCKS_PER_SEC;

    start = clock();
    mergeSort(vec3, 0, vec3.size() - 1);
    end = clock();
    mergeTime = (double)(end - start) / CLOCKS_PER_SEC;

    cout << "\n逆序序列（size=" << size << "）：" << endl;
    cout << "起泡排序耗时：" << bubbleTime << "s" << endl;
    cout << "归并排序耗时：" << mergeTime << "s" << endl;
}

void testComplexTask() {
    vector<Complex> vec = generateRandomComplexVec(10);
    cout << "==================== 任务1：复数类操作 ====================" << endl;
    cout << "初始乱序向量：" << endl;
    for (size_t i = 0; i < vec.size(); ++i) {
        cout << vec[i] << " ";
    }
    cout << endl;

    shuffleVec(vec);
    cout << "置乱后：" << endl;
    for (size_t i = 0; i < vec.size(); ++i) {
        cout << vec[i] << " ";
    }
    cout << endl;

    Complex target = vec[3];
    int idx = findComplex(vec, target);
    cout << "查找 " << target << "，索引：" << idx << endl;

    Complex insertC(5, 5);
    insertComplex(vec, 2, insertC);
    cout << "插入 " << insertC << " 后：" << endl;
    for (size_t i = 0; i < vec.size(); ++i) {
        cout << vec[i] << " ";
    }
    cout << endl;

    deleteComplex(vec, 2);
    cout << "删除索引2后：" << endl;
    for (size_t i = 0; i < vec.size(); ++i) {
        cout << vec[i] << " ";
    }
    cout << endl;

    uniqueVec(vec);
    cout << "唯一化后：" << endl;
    for (size_t i = 0; i < vec.size(); ++i) {
        cout << vec[i] << " ";
    }
    cout << endl;

    sort(vec.begin(), vec.end(), cmpComplex);
    cout << "排序后（模→实部）：" << endl;
    for (size_t i = 0; i < vec.size(); ++i) {
        cout << vec[i] << "（模：" << vec[i].getMod() << "） ";
    }
    cout << endl;

    vector<Complex> rangeVec = rangeFind(vec, 3, 8);
    cout << "模介于 [3,8) 的元素：" << endl;
    for (size_t i = 0; i < rangeVec.size(); ++i) {
        cout << rangeVec[i] << "（模：" << rangeVec[i].getMod() << "） ";
    }
    cout << endl;

    cout << "\n=== 排序效率测试 ===" << endl;
    testSortEfficiency();
}

int main() {
    testComplexTask();
    return 0;
}
