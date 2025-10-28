#include <iostream>
#include <vector>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

int largestRectangleArea(vector<int>& heights) {
    stack<int> st;
    int maxArea = 0;
    int n = heights.size();

    for (int i = 0; i <= n; ++i) {
        while (!st.empty() && (i == n || heights[i] < heights[st.top()])) {
            int height = heights[st.top()];
            st.pop();
            int width = st.empty() ? i : (i - st.top() - 1);
            maxArea = max(maxArea, height * width);
        }
        st.push(i);
    }

    return maxArea;
}

// 修正：C++98中嵌套模板需用"vector<vector<int> >"（注意空格）
vector<vector<int> > generateTestHeights(int groupNum = 10) {
    vector<vector<int> > testGroups;  // 修正：>> → > >
    srand(time(0));

    for (int g = 0; g < groupNum; ++g) {
        int size = rand() % 100000 + 1;
        vector<int> heights;
        for (int i = 0; i < size; ++i) {
            heights.push_back(rand() % 10001);
        }
        testGroups.push_back(heights);
    }

    vector<int> example1;
    example1.push_back(2);
    example1.push_back(1);
    example1.push_back(5);
    example1.push_back(6);
    example1.push_back(2);
    example1.push_back(3);
    testGroups.push_back(example1);

    vector<int> example2;
    example2.push_back(2);
    example2.push_back(4);
    testGroups.push_back(example2);

    return testGroups;
}

void testLargestRectangle() {
    cout << "==================== 任务3：柱状图最大矩形面积 ====================" << endl;
    vector<vector<int> > testGroups = generateTestHeights();  // 修正：>> → > >
    cout << "=== 柱状图最大矩形面积测试 ===" << endl;

    for (size_t i = 0; i < testGroups.size(); ++i) {
        vector<int>& heights = testGroups[i];
        int area = largestRectangleArea(heights);
        cout << "测试用例 " << (i+1) << "：" << endl;
        cout << "高度数组长度：" << heights.size() << endl;
        cout << "前10个高度：";
        for (int j = 0; j < min(10, (int)heights.size()); ++j) {
            cout << heights[j] << " ";
        }
        if (heights.size() > 10) cout << "...";
        cout << "\n最大面积：" << area << "\n" << endl;
    }
}

int main() {
    testLargestRectangle();
    return 0;
}
