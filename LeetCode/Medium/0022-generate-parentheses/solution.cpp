class Solution {
private:
    vector<string> res;
    int l = 0, r = 0, max;
    string tmp = "";
    
    void func(int i) {
        if (r == max) {
            res.push_back(tmp);
            return;
        }
        
        if (l < max) {
            tmp.push_back('(');
            l++;
            
            func(i + 1);
            
            tmp.pop_back();
            l--;
        }
        
        if (l - r > 0) {
            tmp.push_back(')');
            r++;
            
            func(i + 1);
            
            tmp.pop_back();
            r--;
        }
    }
    
public:
    vector<string> generateParenthesis(int n) {
        max = n;
        
        func(0);
        
        return res;
    }
};
