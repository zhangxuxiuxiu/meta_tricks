#include <iostream>
#include <iterator>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>

std::pair<bool, int> find_slop(int* arr, int start, int end){
	if(end-start < 2){
		return {true, end};
	}

	auto idx = start+1;
	while(idx<end && arr[idx-1]<=arr[idx] ) ++idx;
	if(idx-start>1){
		return {true, idx};
	}

	while(idx<end && arr[idx-1]>=arr[idx] ) ++idx;
	return {false, idx};
}

void print(int* arr, int len){
	std::copy(arr, arr+len, std::ostream_iterator<int>(std::cout,"\t"));
	std::cout << '\n';
}

// move [start, mid) to [end-mid+start, end)
int circle_move_block(int* arr, int pos, int start, int mid, int end){
	auto prev = pos, next = 0, cnt = 0;
	auto x = arr[pos];
	do {
		// arr[1] = arr[5]
		// arr[5] = arr[3]
		// arr[3] = arr[1] //
		if (prev - start < end-mid){
			next = prev + (mid - start);	
		} else {
			next = prev - (end - mid);
		}
		arr[prev] = (next == pos ? x : arr[next]);
		prev = next;
		++cnt;
	} while(next!=pos);
	return cnt;
}

// move [mid, end) to [start, start+end-mid)
// move [start, mid) to [end-mid+start, end)
void move_block(int* arr, int start, int mid, int end){
//	std::cout << "move block:[" << start << ',' << mid << ',' << end << ")\n";
	auto cnt = 0, total_cnt = end - start, pos = start;
	//0,	2	3	4	8,	0	2 {1,5,7}
	while (cnt < total_cnt){
		cnt += circle_move_block(arr, pos, start, mid, end);
		++pos;
	}
}

void merge(int* arr, int start, int mid, int end){
	while(start < mid && mid < end){
		if (arr[start]<= arr[mid]){
			++start;
		} else {
			auto pos = mid + 1, v = arr[start];
			while( pos < end && arr[pos] <= v) ++pos;
			move_block(arr, start, mid, pos);
			start += pos - mid;
			mid = pos;
		}
//		std::cout << "merge start:" << start << ",mid:" <<  mid <<'\n';
//		print(arr, end);
	}
}

void sort(int* arr, int n){
	auto pos = 0;
	while(pos < n){
		auto slop = find_slop(arr, pos, n);
		if (!slop.first){
			auto l = pos, r = slop.second - 1;
			while(l < r){
				std::swap(arr[l], arr[r]);
				++l; --r;
			}
		}
		pos = slop.second;
	}
	
//	print( arr, n);
	pos = 0;
	while(true){
		auto mid = find_slop(arr, pos, n).second;
		if (mid == n){
			if (pos == 0){
				break; 
			}
			pos = 0;
//			print(arr, n);
			continue;
		}
		auto end = find_slop(arr, mid, n).second;

//		std::cout << "merge arr[" << pos << ',' <<  mid << ',' << end << ")\n";
		merge(arr, pos, mid, end);
		pos = end;
		if (pos == n){
			pos = 0;
		//	print(arr, n);
		}
	}
}

template<class F>
void benchmark(std::string scene, F&& f){
	auto now = std::chrono::steady_clock::now();	

	for(auto k=16; k<18; ++k){
		std::vector<int> vec;
		int n = pow(2,k);
		vec.reserve(n);
		for(auto idx =0; idx < n ; ++idx){
			vec.push_back(rand());
		}
//		print(&vec[0], n);
		f(&vec[0], &vec[0]+n);
	}

//	 std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
	std::cout << scene << " consumed:"  << std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - now).count() << "ns\n";

}

int main(){
//	int vec[] = {0,      2,      3,      4,      8,      0,      2,      3,      4,      9,      89};
//	print(vec, sizeof(vec)/sizeof(int));
//	merge(vec, 0, 5, 11);
//	print(vec, sizeof(vec)/sizeof(int));
//
//	std::cout << '\n';
//	int arr[] = {2,3,4,0,8,2,0,3,9,4,89,23,94,8,29,8,37,49,82,7};
//	print(arr, sizeof(arr)/sizeof(int));
//	sort(arr, sizeof(arr)/sizeof(int));
//	print(arr, sizeof(arr)/sizeof(int));

	benchmark("std::sort", [](int* l, int* r){ std::sort(l, r);});
	benchmark("merge sort", [](int* l, int* r){ sort(l, r-l);});
	return 0;
}
