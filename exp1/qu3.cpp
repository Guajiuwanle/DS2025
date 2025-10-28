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

// ������C++98��Ƕ��ģ������"vector<vector<int> >"��ע��ո�
vector<vector<int> > generateTestHeights(int groupNum = 10) {
    vector<vector<int> > testGroups;  // ������>> �� > >
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
    cout << "==================== ����3����״ͼ��������� ====================" << endl;
    vector<vector<int> > testGroups = generateTestHeights();  // ������>> �� > >
    cout << "=== ��״ͼ������������� ===" << endl;

    for (size_t i = 0; i < testGroups.size(); ++i) {
        vector<int>& heights = testGroups[i];
        int area = largestRectangleArea(heights);
        cout << "�������� " << (i+1) << "��" << endl;
        cout << "�߶����鳤�ȣ�" << heights.size() << endl;
        cout << "ǰ10���߶ȣ�";
        for (int j = 0; j < min(10, (int)heights.size()); ++j) {
            cout << heights[j] << " ";
        }
        if (heights.size() > 10) cout << "...";
        cout << "\n��������" << area << "\n" << endl;
    }
}

int main() {
    testLargestRectangle();
    return 0;
}
