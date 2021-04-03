#include <iostream>
#include <vector>
#include <array>
#include <queue>
#include <algorithm>

typedef std::vector<std::vector<int>> mat_t;
typedef std::array<std::array<bool, 5>, 5> visited_t;  

void print_mat(const visited_t& mat) {

  for(auto row : mat) {
     for(auto elm : row) {
       if(elm)
         std::cout << "\033[35m"<< std::boolalpha << elm << "\033[0m\t";
       else 
         std::cout << "\033[30m" << std::boolalpha << elm << "\033[0m\t";
     }
     std::cout << std::endl;
   }
   std::cout << std::endl;
}

void print_mat(const mat_t& mat) {

  for(auto row : mat) {
     for(auto elm : row){
       std::cout << elm << "\t";
     }
     std::cout << std::endl;
   }
   std::cout << std::endl;
}

bool safe(const mat_t &mat, int row, int col, visited_t& visited, int& size)
{
  if((row >= 0) and (row < 5) and 
     (col >= 0) and (col < 5) and
     (mat[row][col] and not visited[row][col])){
       ++size;
       return true;
    }
    else 
       return false;
}

void dfs(const mat_t &mat, int row, int col, visited_t& visited, int& size) {

  std::array<int, 8> row_nbr = { -1, -1,-1,  0, 0,  1, 1, 1 };
  std::array<int, 8> col_nbr = { -1,  0, 1, -1, 1, -1, 0, 1 };

  visited[row][col] = true;
  for(auto k = 0; k < 8; ++k)
    if(safe(mat, row + row_nbr[k], col + col_nbr[k], visited, size))
      dfs(mat, row + row_nbr[k], col + col_nbr[k], visited, size);
}

void bfs(const mat_t &mat, int row, int col, visited_t& visited) {
  
  std::array<int, 8> row_nbr = { -1, -1,-1,  0, 0,  1, 1, 1 };
  std::array<int, 8> col_nbr = { -1,  0, 1, -1, 1, -1, 0, 1 };

  visited[row][col] = true;
  std::queue<std::pair<int, int>> q;
  int size = 0;
  q.push(std::make_pair(col, row));
  
  while(not q.empty()) {
    auto srow = q.front().first;
    auto scol = q.front().second;
    q.pop();

    for(auto k = 0; k < 8; ++k) {
      if(safe(mat, srow + row_nbr[k], scol + col_nbr[k], visited, size)) {
        visited[srow + row_nbr[k]][scol + col_nbr[k]] = true;
	q.push(std::make_pair(srow + row_nbr[k], scol + col_nbr[k]));
      }
    }
  }
}

void count_island(const mat_t& mat) {
  visited_t visited = {false};
  
  int size = 1;
  int count = 0;
  std::vector<int> stack_of_sizes;
  for(auto row = 0; row < 5; ++row) {
    for(auto col = 0; col < 5; ++col) {
      if(mat[row][col] and not visited[row][col]){
        dfs(mat, row, col, visited, size);
        print_mat(visited);
	++count;
	std::cout << "debug -- count_island -- pushing to stack of sizes: " << size << "\n"; 
	stack_of_sizes.push_back(size);
	size = 1;
      }	
    }
  }
  auto ret = std::max_element(stack_of_sizes.begin(), stack_of_sizes.end());
  auto pos = std::distance(stack_of_sizes.begin(), ret); 
  std::cout << "info  -- count island -- number of island: " << count << "\n";
  std::cout << "info  -- count island -- max size: "  << stack_of_sizes.at(pos) << "\n";
  return;
}


int main()
{
   std::vector<std::vector<int>> mat = {{1, 1, 0, 0, 0},
					{0, 1, 0, 0, 1},
					{1, 0, 0, 1, 1},
					{0, 0, 0, 0, 0},
					{1, 1, 1, 1, 1},
				       };
   
   count_island(mat);
   return 0;
}
