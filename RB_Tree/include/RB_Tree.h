#ifndef RB_TREE_H
#define RB_TREE_H

#include <cstdlib>

#include <vector>
#include <fstream>
#include <string>

template <typename T>
class RB_Tree
{
  private:
	struct Node
	{
		T value;

		Node* right  = nullptr;
		Node* left   = nullptr;

		Node* parent = nullptr;

		bool is_red  = true;

		Node(const T& value):
			value(value) {}

		Node(const T& value, Node* parent):
			value(value), parent(parent) {}
	};

  private:
	Node* root_ = nullptr;
	std::vector<Node*> data_;

  private:
	static constexpr const char* blue_gray_   = "#48565D";
	static constexpr const char* coral_pink_  = "#F08080";
	static constexpr const char* navy_blue_   = "#0B0042";
	static constexpr const char* black_       = "#1B1B1C";
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
		if (node->parent != NULL)
		{
			if (node->parent->left == node)
				node->parent->left  = pivot;
			else
				node->parent->right = pivot;
		}
		else root_ = pivot;

		node->left = pivot->right;
		if (pivot->right != NULL)
			pivot->right->parent = node;

		node->parent = pivot;
		pivot->right = node;
	}

	void subtree_insert(Node* sub_root, const T& value)
	{
		if (value >= sub_root->value)
		{
			if (sub_root->right == nullptr)
			{
				Node* inserted_node = new Node(value, sub_root);
				sub_root->right = inserted_node;

				data_.push_back(inserted_node);

				fix_invariant(inserted_node);

				return;
			}

			subtree_insert(sub_root->right, value);

		}
		else
		{
			if (sub_root->left  == nullptr)
			{
				Node* inserted_node = new Node(value, sub_root);
				sub_root->left = inserted_node;

				data_.push_back(inserted_node);

				fix_invariant(inserted_node);

				return;
			}

			subtree_insert(sub_root->left,  value);
		}

		return;
	}

	void fix_invariant(Node* inserted_node)
	{

	}

  private:
	void dump_regular_nodes(Node* node, std::ostream& dump)
	{
		if (node == nullptr) return;

		dump 	<< "\t"
				<< reinterpret_cast<size_t>(node)
				<< "[shape = Mrecord, fillcolor = \""
				<< (node->is_red ? black_ : red_)
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

		if(node->right != NULL)
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

  public:
	void insert(const T& value)
	{
		if (root_ == nullptr)
		{
			root_ = new Node(value);
			data_.push_back(root_);
			root_->is_red = false;
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

};

#endif // RB_TREE_H
