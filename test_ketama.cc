#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <functional>

#include "ketama.h"

void print_virtual_nodes(std::map<uint32_t, VirtualNode>& virtual_map)
{
	std::map<std::string, int32_t> records;

	for (auto virtual_it = virtual_map.begin();
			virtual_it!=virtual_map.end(); ++virtual_it)
	{
		auto find_it = records.find(virtual_it->second.physical_node_id);
		if (find_it != records.end())
		{
			++(find_it->second);
		}
		else 
		{
			records.insert(std::pair<std::string, int32_t>(
						virtual_it->second.physical_node_id, 1));
		}
	}
	// print
	for (auto it=records.begin(); it!=records.end(); ++it)
	{
		fprintf(stdout, "node name:%s\t node_count:%d\n", it->first.c_str(), it->second);
	}
}

bool test_base_operation(void)
{
	Ketama<std::string>* ketama1 = new Ketama<std::string>();
	ketama1->InsertNode("test1", "hello world 1", 255);
	fprintf(stdout, "the virtual map size:%ld\n", ketama1->_virtual_node_map.size());
	const std::string* node1 = ketama1->FindNode("i'm random");
	if (node1)
		std::cout << "node:" << *node1 << std::endl;
	else
		return false;
	ketama1->DeleteNode("test1");
	const std::string* node2 = ketama1->FindNode("i'm random");
	if (node2)
		return false;
	return true;
}

bool test_weight_distribution(void)
{
	const uint32_t test_num = 40000;
	std::vector<uint8_t> weight{100, 180, 255};
	Ketama<uint32_t>* ketama_test = new Ketama<uint32_t>();
	ketama_test->InsertNode("test1", 0, weight[0]);
	ketama_test->InsertNode("test2", 0, weight[1]);
	ketama_test->InsertNode("test3", 0, weight[2]);
	//print_virtual_nodes(ketama_test->_virtual_node_map);

	// generate random number and hash them in ketama
	std::default_random_engine generator;
	std::uniform_int_distribution<uint32_t> distribution;
	for (uint32_t i=0; i<test_num; ++i)
	{
		uint32_t* node = ketama_test->FindNode(std::to_string(
					distribution(generator) ));
		*node += 1;
	}

	// print statistic information
	const uint32_t weight_sum = 
		std::accumulate(weight.begin(), weight.end(), 0);
	for (auto weight_it=weight.begin(); weight_it!=weight.end(); ++weight_it)
	{
		fprintf(stdout, "%f\t", (double)*weight_it/weight_sum);
	}
	fprintf(stdout, "\n");
	fprintf(stdout, "physical_node\t real_shoot\n");
	for (auto it=ketama_test->_physical_map.begin();
			it!=ketama_test->_physical_map.end(); ++it)
	{
		fprintf(stdout, "%s\t %f\n", it->first.c_str(), (double)it->second.node/test_num);
	}
	return true;
}

bool test_add_node()
{
	const uint32_t test_num = 40000;
	Ketama<std::string>* ketama_test = new Ketama<std::string>();
	ketama_test->InsertNode("test1", "test1", 255);
	ketama_test->InsertNode("test2", "test2", 255);
	ketama_test->InsertNode("test3", "test3", 255);
	ketama_test->InsertNode("test4", "test4", 255);
	//print_virtual_nodes(ketama_test->_virtual_node_map);

	std::fstream ftest("test.txt", 
			std::fstream::in|std::fstream::out|std::fstream::trunc);
	// generate random number and hash them in ketama
	std::default_random_engine generator;
	std::uniform_int_distribution<uint32_t> distribution;
	for (uint32_t i=0; i<test_num; ++i)
	{
		uint32_t i_rand = distribution(generator);
		std::string* node = ketama_test->FindNode(std::to_string(i_rand));
		ftest << i_rand << " " << *node << std::endl;
	}

	// add new node, check value in file
	ketama_test->InsertNode("test5", "test5", 100);
	ftest.seekg(0, ftest.beg);
	uint32_t missed = 0;
	std::string origin_rand, origin_node_id;
	while (ftest >> origin_rand >> origin_node_id)
	{
		std::string* cur_node = ketama_test->FindNode(origin_rand);
		if (cur_node->compare(origin_node_id) != 0)
			++missed;
	}

	// print result
	fprintf(stdout, "missed:%f\n", (double)missed/test_num);
	return true;
}

typedef bool (*TEST_FUNC)(void);
struct testcase {
    const char *description;
    TEST_FUNC function;
};

struct testcase testcases[] = {
    { "base_operation", test_base_operation },
    { "weight_distribution", test_weight_distribution },
    { "add_node", test_add_node },
    { NULL, NULL }
};

int main(int argc, char **argv)
{
	int exitcode = 0;
	int ii = 0, num_cases = 0;

	for (num_cases = 0; testcases[num_cases].description; num_cases++) {
		/* Just counting */
	}

	printf("1..%d\n", num_cases);

	for (ii = 0; testcases[ii].description != NULL; ++ii) {
		fflush(stdout);
		bool ret = testcases[ii].function();
		if (ret) {
			fprintf(stdout, "ok %d - %s\n", ii + 1, testcases[ii].description);
		} else {
			fprintf(stdout, "not ok %d - %s\n", ii + 1, testcases[ii].description);
			exitcode = 1;
		}
		fprintf(stdout, "\n");
		fflush(stdout);
	}

	return exitcode;
}
