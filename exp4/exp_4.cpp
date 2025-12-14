#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <cstring>

// 边界框结构体（位置、大小、置信度）
struct BoundingBox {
    float x1;    // 左上角x
    float y1;    // 左上角y
    float x2;    // 右下角x
    float y2;    // 右下角y
    float score; // 置信度
};

// 1. 冒泡排序（降序，按置信度）
void bubbleSort(std::vector<BoundingBox>& boxes) {
    int n = boxes.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            // 修改：降序排列，便于NMS处理
            if (boxes[j].score < boxes[j + 1].score) {
                std::swap(boxes[j], boxes[j + 1]);
            }
        }
    }
}

// 2. 选择排序（降序，按置信度）
void selectionSort(std::vector<BoundingBox>& boxes) {
    int n = boxes.size();
    for (int i = 0; i < n - 1; ++i) {
        int maxIdx = i;
        for (int j = i + 1; j < n; ++j) {
            // 修改：找最大置信度
            if (boxes[j].score > boxes[maxIdx].score) {
                maxIdx = j;
            }
        }
        std::swap(boxes[i], boxes[maxIdx]);
    }
}

// 3. 插入排序（降序，按置信度）
void insertionSort(std::vector<BoundingBox>& boxes) {
    int n = boxes.size();
    for (int i = 1; i < n; ++i) {
        BoundingBox key = boxes[i];
        int j = i - 1;
        // 修改：降序排列
        while (j >= 0 && boxes[j].score < key.score) {
            boxes[j + 1] = boxes[j];
            --j;
        }
        boxes[j + 1] = key;
    }
}

// 4. 快速排序辅助函数（分区，降序）
int partition(std::vector<BoundingBox>& boxes, int low, int high) {
    float pivot = boxes[high].score;
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        // 修改：降序排列
        if (boxes[j].score >= pivot) {
            ++i;
            std::swap(boxes[i], boxes[j]);
        }
    }
    std::swap(boxes[i + 1], boxes[high]);
    return i + 1;
}

// 4. 快速排序（降序，按置信度）
void quickSort(std::vector<BoundingBox>& boxes, int low, int high) {
    if (low < high) {
        int pi = partition(boxes, low, high);
        quickSort(boxes, low, pi - 1);
        quickSort(boxes, pi + 1, high);
    }
}

// 快速排序的包装函数（与其他排序算法接口一致）
void quickSortWrapper(std::vector<BoundingBox>& boxes) {
    if (!boxes.empty()) {
        quickSort(boxes, 0, static_cast<int>(boxes.size()) - 1);
    }
}

// 生成随机分布的边界框（100~10000个）
std::vector<BoundingBox> generateRandomBoxes(int count) {
    std::vector<BoundingBox> boxes;
    boxes.reserve(count);
    srand(static_cast<unsigned int>(time(NULL)));
    
    for (int i = 0; i < count; ++i) {
        BoundingBox box;
        // 位置：0~900（避免边界溢出）
        box.x1 = static_cast<float>(rand() % 901);
        box.y1 = static_cast<float>(rand() % 901);
        // 大小：50~100（合理尺寸）
        float width = 50.0f + static_cast<float>(rand() % 51);
        float height = 50.0f + static_cast<float>(rand() % 51);
        box.x2 = box.x1 + width;
        box.y2 = box.y1 + height;
        // 置信度：0~1
        box.score = static_cast<float>(rand()) / RAND_MAX;
        boxes.push_back(box);
    }
    return boxes;
}

// 生成聚集分布的边界框（100~10000个）
std::vector<BoundingBox> generateClusteredBoxes(int count) {
    std::vector<BoundingBox> boxes;
    boxes.reserve(count);
    srand(static_cast<unsigned int>(time(NULL)));

    // 聚集中心（4个聚集区域，避免边界溢出）
    const float centers[4][2] = {{200, 200}, {200, 700}, {700, 200}, {700, 700}};

    for (int i = 0; i < count; ++i) {
        BoundingBox box;
        // 随机选择一个聚集中心
        int centerIdx = rand() % 4;
        float cx = centers[centerIdx][0];
        float cy = centers[centerIdx][1];
        // 位置：中心附近±50（保证坐标为正）
        box.x1 = cx - 50.0f + static_cast<float>(rand() % 101);
        box.y1 = cy - 50.0f + static_cast<float>(rand() % 101);
        // 大小：50~100（与随机分布一致）
        float width = 50.0f + static_cast<float>(rand() % 51);
        float height = 50.0f + static_cast<float>(rand() % 51);
        box.x2 = box.x1 + width;
        box.y2 = box.y1 + height;
        // 置信度：0~1
        box.score = static_cast<float>(rand()) / RAND_MAX;
        boxes.push_back(box);
    }
    return boxes;
}

// 计算IoU（交并比）
float calculateIoU(const BoundingBox& a, const BoundingBox& b) {
    float x1 = std::max(a.x1, b.x1);
    float y1 = std::max(a.y1, b.y1);
    float x2 = std::min(a.x2, b.x2);
    float y2 = std::min(a.y2, b.y2);

    // 计算交集面积（避免负数）
    float interWidth = std::max(0.0f, x2 - x1);
    float interHeight = std::max(0.0f, y2 - y1);
    float interArea = interWidth * interHeight;
    
    if (interArea <= 0.0f) return 0.0f;

    // 计算并集面积
    float areaA = (a.x2 - a.x1) * (a.y2 - a.y1);
    float areaB = (b.x2 - b.x1) * (b.y2 - b.y1);
    float unionArea = areaA + areaB - interArea;

    return interArea / unionArea;
}

// 基础NMS算法（使用指定的排序算法）
std::vector<BoundingBox> basicNMS(std::vector<BoundingBox> boxes, float iouThreshold, 
                                  void (*sortFunc)(std::vector<BoundingBox>&)) {
    std::vector<BoundingBox> result;
    if (boxes.empty()) return result;

    // 使用指定的排序算法按置信度降序排序
    if (sortFunc != NULL) {
        sortFunc(boxes);
    } else {
        // 默认使用快速排序
        quickSortWrapper(boxes);
    }

    while (!boxes.empty()) {
        // 取置信度最高的框
        BoundingBox topBox = boxes[0];
        result.push_back(topBox);

        // 移除与topBox IoU超过阈值的框
        std::vector<BoundingBox> temp;
        for (size_t i = 1; i < boxes.size(); ++i) {
            if (calculateIoU(topBox, boxes[i]) < iouThreshold) {
                temp.push_back(boxes[i]);
            }
        }
        boxes = temp;
    }
    return result;
}

// 性能测试函数（返回运行时间，单位：毫秒）
double testSortPerformance(void (*sortFunc)(std::vector<BoundingBox>&), 
                          std::vector<BoundingBox> boxes, 
                          const char* sortName, 
                          int dataSize, 
                          const char* dataDist) {
    // 复制原始数据（避免修改原数组）
    std::vector<BoundingBox> testBoxes = boxes;

    // 计时开始
    clock_t start = clock();

    // 执行排序
    if (sortFunc != NULL) {
        sortFunc(testBoxes);
    }

    // 计时结束
    clock_t end = clock();
    double elapsed = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000.0;

    // 输出结果
    std::cout << std::left << std::setw(15) << sortName
              << std::setw(12) << dataSize
              << std::setw(15) << dataDist
              << std::fixed << std::setprecision(3) << elapsed << " ms" << std::endl;

    return elapsed;
}

// NMS性能测试函数
double testNMSPerformance(void (*sortFunc)(std::vector<BoundingBox>&), 
                         std::vector<BoundingBox> boxes, 
                         const char* sortName, 
                         int dataSize, 
                         const char* dataDist,
                         float iouThreshold) {
    // 复制原始数据
    std::vector<BoundingBox> testBoxes = boxes;

    // 计时开始
    clock_t start = clock();

    // 执行NMS
    std::vector<BoundingBox> result = basicNMS(testBoxes, iouThreshold, sortFunc);

    // 计时结束
    clock_t end = clock();
    double elapsed = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000.0;

    // 输出结果
    std::cout << std::left << std::setw(15) << sortName
              << std::setw(12) << dataSize
              << std::setw(15) << dataDist
              << std::setw(8) << result.size()
              << std::fixed << std::setprecision(3) << elapsed << " ms" << std::endl;

    return elapsed;
}

int main() {
    // 实验配置
    const int dataSizes[] = {100, 1000, 5000, 10000}; // 数据规模
    const char* dataDists[] = {"Random", "Clustered"}; // 数据分布
    const float iouThreshold = 0.5f; // NMS IoU阈值

    // 排序算法数组
    typedef void (*SortFunc)(std::vector<BoundingBox>&);
    SortFunc sortFuncs[] = {bubbleSort, selectionSort, insertionSort, quickSortWrapper};
    const char* sortNames[] = {"BubbleSort", "SelectionSort", "InsertionSort", "QuickSort"};

    std::cout << "=== 排序算法性能测试 ===" << std::endl;
    std::cout << std::left << std::setw(15) << "排序算法"
              << std::setw(12) << "数据规模"
              << std::setw(15) << "数据分布"
              << "运行时间" << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    // 遍历所有数据规模和分布
    for (int sizeIdx = 0; sizeIdx < 4; ++sizeIdx) {
        int dataSize = dataSizes[sizeIdx];
        for (int distIdx = 0; distIdx < 2; ++distIdx) {
            const char* dataDist = dataDists[distIdx];
            std::vector<BoundingBox> boxes;

            // 生成对应分布的边界框
            if (distIdx == 0) {
                boxes = generateRandomBoxes(dataSize);
            } else {
                boxes = generateClusteredBoxes(dataSize);
            }

            // 测试所有排序算法
            for (int i = 0; i < 4; ++i) {
                testSortPerformance(sortFuncs[i], boxes, sortNames[i], dataSize, dataDist);
            }
        }
    }

    // NMS算法测试（使用不同排序算法）
    std::cout << "\n=== NMS算法性能测试（IoU阈值=" << iouThreshold << "）===" << std::endl;
    std::cout << std::left << std::setw(15) << "排序算法"
              << std::setw(12) << "数据规模"
              << std::setw(15) << "数据分布"
              << std::setw(8) << "结果数"
              << "运行时间" << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    // 测试NMS在不同数据规模和分布下的性能
    for (int sizeIdx = 0; sizeIdx < 4; ++sizeIdx) {
        int dataSize = dataSizes[sizeIdx];
        for (int distIdx = 0; distIdx < 2; ++distIdx) {
            const char* dataDist = dataDists[distIdx];
            std::vector<BoundingBox> boxes;

            // 生成对应分布的边界框
            if (distIdx == 0) {
                boxes = generateRandomBoxes(dataSize);
            } else {
                boxes = generateClusteredBoxes(dataSize);
            }

            // 测试每种排序算法在NMS中的性能
            for (int i = 0; i < 4; ++i) {
                testNMSPerformance(sortFuncs[i], boxes, sortNames[i], dataSize, dataDist, iouThreshold);
            }
        }
    }

    // 附加测试：验证NMS算法的正确性
    std::cout << "\n=== NMS算法正确性验证 ===" << std::endl;
    std::vector<BoundingBox> testBoxes = generateRandomBoxes(100);
    std::vector<BoundingBox> nmsResult = basicNMS(testBoxes, iouThreshold, quickSortWrapper);
    
    std::cout << "原始边界框数量: " << testBoxes.size() << std::endl;
    std::cout << "NMS后边界框数量: " << nmsResult.size() << std::endl;
    
    // 验证结果中没有重叠度过高的边界框
    bool valid = true;
    for (size_t i = 0; i < nmsResult.size() && valid; ++i) {
        for (size_t j = i + 1; j < nmsResult.size() && valid; ++j) {
            if (calculateIoU(nmsResult[i], nmsResult[j]) > iouThreshold) {
                std::cout << "错误: 检测到IoU超过阈值的边界框!" << std::endl;
                valid = false;
            }
        }
    }
    if (valid) {
        std::cout << "验证通过: 所有边界框IoU均低于阈值" << std::endl;
    }

    return 0;
}
