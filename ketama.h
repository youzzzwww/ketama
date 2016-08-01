/* writen by xiesongbo in 2016.7.28
 * not thread safe so consider lock before using in multithread
 * */
#ifndef _IMSERVER_KETAMA_H_
#define _IMSERVER_KETAMA_H_

#include <cstdint>
#include <functional>
#include <string>
#include <map>
#include <vector>
#include <unordered_map>
#include <iostream>

#include "hash.h"

#define MAX_VISUAL_NUM 100

struct VirtualNode
{
	std::string physical_node_id;
};

template <class T>
struct PhysicalNode
{
	T node;
	std::vector<std::map<uint32_t, VirtualNode>::iterator> virtual_ptrs;
};

/* T means origin class, 
 * H represent hash class provided for visual node mapping
 * static uint32_t H.hash(const std::string&) function must be defined
 * */
template <class T, class H = Murmur3>
class Ketama
{
	public:
		/* node_id must be unique, weight between in [0, 255] */
		bool InsertNode(const std::string& node_id,
				const T& node, const uint8_t weight);
		T* FindNode(const std::string& key);
		void DeleteNode(const std::string& node_id);

	public:
		std::map<uint32_t, VirtualNode> _virtual_node_map;
		std::unordered_map<std::string, PhysicalNode<T>> _physical_map;
};

template <class T, class H>
bool Ketama<T, H>::InsertNode(const std::string& node_id,
		const T& node, const uint8_t weight)
{
	// first caculate virtual node numbers
	uint32_t virtual_numbers = static_cast<uint32_t>(
			(double)weight/UINT8_MAX * MAX_VISUAL_NUM);

	PhysicalNode<T> cur_physical_node;
	cur_physical_node.node = node;
	for (uint32_t i=0; i<virtual_numbers; ++i)
	{
		// construct virtual node
		std::string cur_virtual_id = node_id + std::to_string(i);
		uint32_t cur_virtual_key = H::hash(cur_virtual_id);
		// insert virtual node to virtual map
		auto it = _virtual_node_map.insert(std::pair<uint32_t, VirtualNode>(
				cur_virtual_key, {node_id}));
		// link virtual node to its physic node
		cur_physical_node.virtual_ptrs.push_back(it.first);
	}
	// insert physical node
	_physical_map.insert(std::pair<std::string, PhysicalNode<T>&>(
				node_id, cur_physical_node));
	return true;
}

template <class T, class H>
T* Ketama<T, H>::FindNode(const std::string& key)
{
	uint32_t virtual_key = H::hash(key);
	auto next_it = _virtual_node_map.upper_bound(virtual_key);
	if (next_it == _virtual_node_map.end())
		// can't find upper node, set its ptr to first
		next_it = _virtual_node_map.begin();

	// find the physical node accordingly
	//std::cout << "to find the physical node:" << next_it->second.physical_node_id << std::endl;
	auto physical_it = _physical_map.find(next_it->second.physical_node_id);
	if (physical_it == _physical_map.end())
	{
		// physical/virtual link is wrong
		return NULL;
	}
	return &(physical_it->second.node);
}

template <class T, class H>
void Ketama<T, H>::DeleteNode(const std::string& node_id)
{
	auto physical_it = _physical_map.find(node_id);
	if (physical_it != _physical_map.end())
	{
		// erase all virtual nodes, and then the corresponding physical node
		for (auto virtual_ptr : physical_it->second.virtual_ptrs)
		{
			_virtual_node_map.erase(virtual_ptr);
		}
		_physical_map.erase(physical_it);
	}
}

#endif
