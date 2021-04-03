#include <iostream>
#include <vector>
#include <array>
#include <queue>

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

}

bool safe(const mat_t &mat, int row, int col, visited_t& visited)
{
  return (row >= 0) and (row < 5) and 
	 (col >= 0) and (col < 5) and
	 (mat[row][col] and not visited[row][col]);
}

void dfs(const mat_t &mat, int row, int col, visited_t& visited) {

  std::array<int, 8> row_nbr = { -1, -1,-1,  0, 0,  1, 1, 1 };
  std::array<int, 8> col_nbr = { -1,  0, 1, -1, 1, -1, 0, 1 };

  visited[row][col] = true;

  for(auto k = 0; k < 8; ++k)
    if(safe(mat, row + row_nbr[k], col + col_nbr[k], visited))
      dfs(mat, row + row_nbr[k], col + col_nbr[k], visited);
}

void bfs(const mat_t &mat, int row, int col, visited_t& visited) {
  
  std::array<int, 8> row_nbr = { -1, -1,-1,  0, 0,  1, 1, 1 };
  std::array<int, 8> col_nbr = { -1,  0, 1, -1, 1, -1, 0, 1 };

  visited[row][col] = true;
  std::queue<std::pair<int, int>> q;

  q.push(std::make_pair(col, row));
  
  while(not q.empty()) {
    auto srow = q.front().first;
    auto scol = q.front().second;
    q.pop();

    for(auto k = 0; k < 8; ++k) {
      if(safe(mat, srow + row_nbr[k], scol + col_nbr[k], visited)) {
        visited[srow + row_nbr[k]][scol + col_nbr[k]] = true;
	q.push(std::make_pair(srow + row_nbr[k], scol + col_nbr[k]));
      }
    }
  }
}

void count_island(const mat_t& mat) {
  visited_t visited = {false};

  int count = 0;
  for(auto row = 0; row < 5; ++row) {
    for(auto col = 0; col < 5; ++col) {
      if(mat[row][col] and not visited[row][col]){
        dfs(mat, row, col, visited);
        print_mat(visited);
	++count;
      }	
    }
  }

  std::cout << "number of island: " << count << "\n";
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
