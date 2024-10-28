#ifndef RB_TREE_H
#define RB_TREE_H

#include <cstdlib>

#include <vector>
#include <fstream>
#include <string>

#include "log.h"

template <typename KeyT>
class RB_Tree
{
  private:
	struct Node
	{
		KeyT value;

		Node* right  = nullptr;
		Node* left   = nullptr;

		Node* parent = nullptr;

		bool is_red  = true;

		Node(const KeyT& _value):
			value(_value) {}

		Node(const KeyT& _value, Node* _parent, bool _is_red = true):
			value(_value), parent(_parent), is_red(_is_red) {}

		Node* get_grandp()
		{
			if (parent != nullptr) return parent->parent;

			return nullptr;
		}

		Node* get_unc()
		{
			Node* grandparent = get_grandp();

			if (grandparent == nullptr) return nullptr;

			return grandparent->right == parent ? grandparent->left : grandparent->right;
		}

		bool is_left_child()
		{
			return parent->left == this;
		}

		bool is_right_child()
		{
			return !is_left_child();
		}

		bool is_triangle()
		{
			return	   (is_left_child()  && parent->is_right_child())
					|| (is_right_child() && parent->is_left_child());
		}

		Node* get_bro()
		{
			return is_left_child() ? parent->right : parent->left;
		}
	};

  private:
	Node* root_ = nullptr;
	std::vector<Node*> data_;

  private:
	static constexpr const char* blue_gray_   = "#48565D";
	static constexpr const char* coral_pink_  = "#F08080";
	static constexpr const char* navy_blue_   = "#0B0042";
	static constexpr const char* black_	      = "#1B1B1C";
	static constexpr const char* red_         = "#820007";

  private:
	void rotate_left(Node* node)
	{
		Node* pivot = node->right;

		pivot->parent = node->parent;

		if (node->parent != nullptr)
		{
			if (node->parent->left == node)
				node->parent->left  = pivot;
			else
				node->parent->right = pivot;
		}
		else root_ = pivot;

		node->right   = pivot->left;
		if (pivot->left != nullptr)
			pivot->left->parent = node;

		node->parent  = pivot;
		pivot->left   = node;
	}

	void rotate_right(Node* node)
	{
		Node* pivot = node->left;

		pivot->parent = node->parent;
		if (node->parent != nullptr)
		{
			if (node->parent->left == node)
				node->parent->left  = pivot;
			else
				node->parent->right = pivot;
		}
		else root_ = pivot;

		node->left = pivot->right;
		if (pivot->right != nullptr)
			pivot->right->parent = node;

		node->parent = pivot;
		pivot->right = node;
	}

	void subtree_insert(Node* sub_root, const KeyT& value)
	{
		if (value > sub_root->value)
		{
			if (sub_root->right == nullptr)
			{
				Node* inserted_node = new Node(value, sub_root);
				sub_root->right     = inserted_node;

				data_.emplace_back(inserted_node);

				fix_violation(inserted_node);

				return;
			}

			subtree_insert(sub_root->right, value);
		}
		else if (value < sub_root->value)
		{
			if (sub_root->left  == nullptr)
			{
				Node* inserted_node = new Node(value, sub_root);
				sub_root->left      = inserted_node;

				data_.emplace_back(inserted_node);

				fix_violation(inserted_node);

				return;
			}

			subtree_insert(sub_root->left,  value);
		}

		return;
	}

	void handle_black_unc(Node* cur_node)
	{
		Node* parent      = cur_node->parent;
		Node* grandparent = cur_node->get_grandp();

		if (cur_node->is_left_child() && parent->is_right_child())
		{
			rotate_right(parent);
			cur_node = cur_node->right;
		}
		else if (cur_node->is_right_child() && parent->is_left_child())
		{
			rotate_left(parent);
			cur_node = cur_node->left;
		}

		parent = cur_node->parent;
		grandparent = cur_node->get_grandp();

		if (cur_node->is_left_child()) rotate_right(grandparent);
		else rotate_left(grandparent);

		paint_black(parent);
		paint_red(grandparent);
	}

	void fix_violation(Node* cur_node)
	{
		while (cur_node != root_ && cur_node->parent->is_red)
		{
			Node* parent = cur_node->parent;
			Node* uncle  = cur_node->get_unc();

			if (uncle && uncle->is_red)
			{
				paint_black(parent);
				paint_black(uncle);

				Node* grandparent = cur_node->get_grandp();

				paint_red(grandparent);
				cur_node = grandparent;
			}
			else
			{
				handle_black_unc(cur_node);
				break;
			}
		}
		paint_black(root_);
	}

	void paint_black(Node* node)
	{
		if (node != nullptr) node->is_red = false;
	}

	void paint_red(Node* node)
	{
		node->is_red = true;
	}

  private:
	void dump_regular_nodes(Node* node, std::ostream& dump)
	{
		if (node == nullptr) return;

		dump 	<< "\t"
				<< reinterpret_cast<size_t>(node)
				<< "[shape = Mrecord, fillcolor = \""
				<< (node->is_red ? red_ : black_)
				<< "\", label =  \"{"
				<< node << " | val: " << node->value
				<< " | {L: " << node->left
				<< " R: " << node->right << "}}"
				<< "\" ];\n";

		dump_regular_nodes(node->left,  dump);

		dump_regular_nodes(node->right, dump);
	}

	void dump_connections(Node* node, std::ostream& dump)
	{
		if(node == nullptr) return;

		if(node->left != nullptr)
		{
			dump 	<< reinterpret_cast<size_t>(node)
					<< " -> "
					<< reinterpret_cast<size_t>(node->left) << '\n';

			dump_connections(node->left, dump);
		}

		if(node->right != nullptr)
		{
			dump 	<< reinterpret_cast<size_t>(node)
					<< " -> "
					<< reinterpret_cast<size_t>(node->right) << '\n';

			dump_connections(node->right, dump);
		}
	}

	void compile_dot(const std::string& file_name)
	{
		std::string cmd = "dot -Tpng " + file_name + ".dot -o " + file_name + ".png -Gdpi=100\n";

		std::system(cmd.c_str());
	}

  private:
	void free_data()
	{
		for (auto& elem : data_)
		{
			LOG("Deleting {}\n", elem->value);
			delete elem;
		}

		root_ = nullptr;
		data_.clear();
	}

	Node* create_based_on(const Node* reference, Node* parent)
	{
		if (reference == nullptr) return nullptr;

		LOG("Copying {}\n", reference->value);
		Node* created = new Node(reference->value, parent, reference->is_red);
		data_.push_back(created);

		created->left  = create_based_on(reference->left,  created);
		created->right = create_based_on(reference->right, created);

		return created;
	}

  public:
	using iterator = Node*;

	RB_Tree() = default;

	RB_Tree(const RB_Tree& other)
	{
		MSG("Copy constructor called\n");

		root_ = create_based_on(other.root_, nullptr);
	}

	RB_Tree& operator = (const RB_Tree& other)
	{
		MSG("Copy assignment called\n");

		if (this == &other) return *this;

		free_data();

		root_ = create_based_on(other.root_, nullptr);

		return *this;
	}

	RB_Tree(RB_Tree&& other) noexcept :
		  root_(std::move(other.root_))
		, data_(std::move(other.data_))
	{
		MSG("Move constructor called\n");
		other.data_.clear();
		other.root_ = nullptr;
	}

	RB_Tree& operator = (RB_Tree&& other) noexcept
	{
		MSG("Move assignment called\n");

		if (this == &other) return *this;

		free_data();

		root_ = std::move(other.root_);
		data_ = std::move(other.data_);

		other.data_.clear();
		other.root_ = nullptr;

		return *this;
	}

	void insert(const KeyT& value)
	{
		LOG("Inserting {}\n", value);

		if (root_ == nullptr)
		{
			root_ = new Node(value);
			data_.push_back(root_);
			paint_black(root_);
		}
		else
			subtree_insert(root_, value);
	}

	void dump_tree()
	{
		std::string file_name = "tree_dump";
		std::ofstream dump(file_name + ".dot");

		dump << "digraph BinaryTree {\n"
				"bgcolor = \"" << blue_gray_ << "\";\n"
				"edge[minlen = 3, penwidth = 3; color = \"black\"];\n"
				"node[shape = \"rectangle\", style = \"rounded, filled\",\n"
				"\tfillcolor = \"" << coral_pink_ << "\",\n"
				"\tfontsize = 30,\n"
				"\theight = 3,\n"
				"\tpenwidth = 5, color = \"white\", fontcolor = \"white\"];\n";

		dump << "{rank = min;\n"
				"\tlist_manager [shape = Mrecord, fillcolor = \"" << navy_blue_ << "\", "
				"label = \"{ROOT: " << root_ << "}\"];\n"
				"}\n";

		if(root_ != nullptr)
			dump_regular_nodes(root_, dump);

		dump_connections(root_, dump);

		dump << "}";

		compile_dot(file_name);
	}

	iterator lower_bound(const KeyT& key) const
	{
		iterator cur_node = root_;
		iterator answer = nullptr;

		while (true)
		{
			if (key < cur_node->value)
			{
				answer = cur_node;
				if (cur_node->left == nullptr) return answer;
				cur_node = cur_node->left;
			}
			else if (key > cur_node->value)
			{
				if (cur_node->right == nullptr) return answer;
				cur_node = cur_node->right;
			}
			else return cur_node;

		}

		return answer;
	}

	iterator upper_bound(const KeyT& key) const
	{
		iterator cur_node = root_;
		iterator answer = nullptr;

		while (true)
		{
			if (key < cur_node->value)
			{
				answer = cur_node;
				if (cur_node->left == nullptr) return answer;
				cur_node = cur_node->left;
			}
			else if (key >= cur_node->value)
			{
				if (cur_node->right == nullptr) return answer;
				cur_node = cur_node->right;
			}
		}

		return answer;
	}

	size_t distance(iterator fst, iterator snd)
	{
		size_t distance = 0;

		Node* cur_node     = fst;
		Node* last_visited = fst->left;

		while (cur_node != nullptr)
		{
			if (last_visited == cur_node->parent)
			{
				if (cur_node->left != nullptr)
				{
					last_visited = cur_node;
					cur_node = cur_node->left;
					continue;
				}
				else last_visited = nullptr;
			}

			if (last_visited == cur_node->left)
			{
				if (cur_node == snd) break;

				++distance;

				LOG("Counting {}\n", cur_node->value);


				if (cur_node->right != nullptr)
				{
					last_visited = cur_node;
					cur_node     = cur_node->right;
					continue;
				}
				else last_visited = nullptr;
			}

			if (last_visited == cur_node->right)
			{
				last_visited = cur_node;
				cur_node = cur_node->parent;
			}
		}

		return distance;
	}

	~RB_Tree()
	{
		MSG("Destructor called\n");

		free_data();
	}

};

#endif // RB_TREE_H
