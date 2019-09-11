/**
 * 
 * Given a linked list and a value x, partition it such that all nodes less than x come before nodes greater than
 * or equal to x.
 * 
 * You should preserve the original relative order of the nodes in each of the two partitions.
 * 
 * Example:
 * 
 * Input: head = 1->4->3->2->5->2, x = 3
 * Output: 1->2->2->4->3->5
*/


/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
class Solution {
public:
    ListNode* partition(ListNode* head, int x) {
        	ListNode* dummyHead1 = new ListNode(-1);
		ListNode* dummyHead2 = new ListNode(-1);
		ListNode *subNode1 = dummyHead1, *subNode2 = dummyHead2;

		while (head) {

			if (head->val < x) {
				subNode1->next = head;
				subNode1 = subNode1->next;
			}
			else {
				subNode2->next = head;
				subNode2 = subNode2->next;
			}

			head = head->next;
		}

		subNode2->next = NULL;
		subNode1->next = dummyHead2->next;
		return dummyHead1->next;
	
    }
};




/**
 * Given an array of integers, return indices of the two numbers such that they add up to a specific target.
 * 
 * You may assume that each input would have exactly one solution, and you may not use the same element twice.
 * 
 * Example:
 * 
 * Given nums = [2, 7, 11, 15], target = 9,
 * 
 * Because nums[0] + nums[1] = 2 + 7 = 9,
 * return [0, 1].
*/


class Solution {
public:
    vector<int> twoSum(vector<int>& numbers, int target) {
      //Key is the number and value is its index in the vector.
      unordered_map<int, int> hash;
      vector<int> result;
      for (int i = 0; i < numbers.size(); i++) {
        int numberToFind = target - numbers[i];

        //if numberToFind is found in map, return them
        if (hash.find(numberToFind) != hash.end()) {
          result.push_back(hash[numberToFind]);
          result.push_back(i);			
          return result;
        }

        //number was not found. Put it in the map.
        hash[numbers[i]] = i;
      }
      return result;
    }    
};


/**
 * Given a non-empty array of integers, every element appears twice except for one. Find that single one.
 * 
 * Note:
 * 
 * Your algorithm should have a linear runtime complexity. Could you implement it without using extra memory?
 * 
 * Example 1:
 * 
 * Input: [2,2,1]
 * Output: 1
 */


class Solution {
public:
    int singleNumber(vector<int>& nums) {
      unordered_map<int,int> hash;
      
      for(int i = 0; i < nums.size(); ++i) {        
        ++hash[nums[i]];        
      }
      
    for (auto elem : hash) {
      if(elem.second == 1) {
        return elem.first;
      }
    }
      
      return 0;
    }
};
